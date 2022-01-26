/***************************************************************************
 * $id: StFttDb.cxx,v 1.22 2020/12/17 21:01:04 jdb Exp $
 * \author: jdb
 ***************************************************************************
 *
 * Description: This interface between FCS and the STAR database
 *
 ***************************************************************************/

#include "StFttDb.h"
#include "StMaker.h"
#include "StMessMgr.h"
#include "StEvent/StFttRawHit.h"
#include "StEvent/StFttCluster.h"
#include "StEvent/StFttPoint.h"
#include <math.h>


ClassImp(StFttDb)


double StFttDb::stripPitch = 3.2; // mm
double StFttDb::rowLength = 180; // mm
vector<string> StFttDb::orientationLabels = { "Horizontal", "Vertical", "DiagonalH", "DiagonalV", "Unknown" };


StFttDb::StFttDb(const char *name) : TDataSet(name) {}; 

StFttDb::~StFttDb() {}

int StFttDb::Init(){

  return kStOK;
}

void StFttDb::setDbAccess(int v) {mDbAccess =  v;}
void StFttDb::setRun(int run) {mRun = run;}

int StFttDb::InitRun(int runNumber) {
    LOG_INFO << "StFttDb::InitRun - run = " << runNumber << endm;
    mRun=runNumber;
    return kStOK;
}


size_t StFttDb::uuid( StFttRawHit * h, bool includeStrip ) {
    // this UUID is not really universally unique
    // it is unique up to the hardware location 
    // at the give precision
    // NOT including strip level is useful for cluster 
    // calculations that combine all strips from given 
    // plane, quad, row, orientation

    
    size_t _uuid = (size_t)h->orientation() + (nStripOrientations) * ( h->row() + nRowsPerQuad * ( h->quadrant() + nQuadPerPlane * h->plane() ) );
    
    if ( includeStrip ){
        _uuid = (size_t) h->strip() * maxStripPerRow *( h->orientation() + (nStripOrientations) * ( h->row() + nRowsPerQuad * ( h->quadrant() + nQuadPerPlane * h->plane() ) ) );
    } 

    return _uuid;
}

size_t StFttDb::uuid( StFttCluster * c ) {
    // this UUID is not really universally unique
    // it is unique up to the hardware location 

    size_t _uuid = (size_t)c->orientation() + (nStripOrientations) * ( c->row() + nRowsPerQuad * ( c->quadrant() + nQuadPerPlane * c->plane() ) );
    return _uuid;
}


uint16_t StFttDb::packKey( int feb, int vmm, int ch ) const{
    // feb = [1 - 6] = 3 bits
    // vmm = [1 - 4] = 3 bits
    // ch  = [1 - 64] = 7 bits
    return feb + (vmm << 3) + (ch << 6);
}
void StFttDb::unpackKey( int key, int &feb, int &vmm, int &ch ) const{
    feb = key & 0b111;
    vmm = (key >> 3) & 0b111;
    ch  = (key >> 6) & 0b1111111;
    return;
}
uint16_t StFttDb::packVal( int row, int strip ) const{
    // row = [1 - 4] = 3 bits
    // strip = [1 - 152] = 8 bits
    return row + ( strip << 3 );
}
void StFttDb::unpackVal( int val, int &row, int &strip ) const{
    row = val & 0b111; // 3 bits
    strip = (val >> 3) & 0b11111111; // 8 bit
    return;
}

void StFttDb::loadHardwareMapFromFile( std::string fn ){
    std::ifstream inf;
    inf.open( fn.c_str() );

    mMap.clear();
    if ( !inf ) {
        LOG_WARN << "sTGC Hardware map file not found" << endm;
        return;
    }

    mMap.clear();
    string hs0, hs1, hs2, hs3, hs4;
    // HEADER:
    // Row_num    FEB_num    VMM_num    VMM_ch         strip_ch
    inf >> hs0 >> hs1 >> hs2 >> hs3 >> hs4;
    
    if ( mDebug ){
        printf( "Map Header: %s, %s, %s, %s, %s", hs0.c_str(), hs1.c_str(), hs2.c_str(), hs3.c_str(), hs4.c_str() );
        puts("");
    }
    
    uint16_t row, feb, vmm, ch, strip;
    while( inf >> row >> feb >> vmm >> ch >> strip ){
        // pack the key (feb, vmm, ch)
        uint16_t key = packKey( feb, vmm, ch );
        uint16_t val = packVal( row, strip );
        mMap[ key ] = val;
        rMap[ val ] = key;
        if ( mDebug ){
            printf( "key=%d", key );
            printf( "in=(feb=%d, vmm=%d, ch=%d)\n", feb, vmm, ch );
            int ufeb, uvmm, uch;
            unpackKey( key, ufeb, uvmm, uch );
            printf( "key=(feb=%d, vmm=%d, ch=%d)\n", ufeb, uvmm, uch ); puts("");
            assert( feb == ufeb && vmm == uvmm && ch == uch );
            int urow, ustrip;
            printf( "val=%d", val );
            unpackVal( val, urow, ustrip );
            assert( row == urow && strip == ustrip );
            printf( "(row=%d, strip=%d)\n", row, strip );
        }
    }
    inf.close();
}

// same for all planes
// we have quadrants like:
// 
// D | A
// ------
// C | B
// Row 3 and 4 are always diagonal
// odd (even) FOB are horizontal (vertical) for A and C (B and D)
// even (odd) FOB are vertical (horizontal) for A and C (B and D)
UChar_t StFttDb::getOrientation( int rob, int feb, int vmm, int row ) const {
    if ( mDebug ) {
        printf( "getOrientation( %d, %d, %d, %d )", rob, feb, vmm, row ); puts("");
    }

    if ( rob % 2 == 0 ){ // even rob
        if ( feb % 2 != 0 ) { // odd
            // row 3 and 4 are always diagonal
            if ( 3 == row || 4 == row )
                return kFttDiagonalH;    
            return kFttHorizontal;
        }
        // row 3 and 4 are always diagonal
        if ( 3 == row || 4 == row )
            return kFttDiagonalV;
        // even
        return kFttVertical;
    } else { // odd rob
        
        if ( feb % 2 != 0 ) { // odd
            // row 3 and 4 are always diagonal
            if ( 3 == row || 4 == row )
                return kFttDiagonalV;
            return kFttVertical;
        }
        // row 3 and 4 are always diagonal
        if ( 3 == row || 4 == row )
            return kFttDiagonalH;
        // even
        return kFttHorizontal;
    }
    // should never get here!
    return kFttUnknownOrientation;
}

/* get 
 * returns the mapping for a given input
 * 
 * input:
 *      rob: 1 - 16
 *      feb: 1 - 6
 *      vmm: 1 - 4
 *      ch : 0 - 63
 *
 * output:
 *      row: 0 - 4
 *      strip: 0 - 162
 *      orientation: one of {Horizontal, Vertical, Diagonal, Unknown}
 *
 */
bool StFttDb::hardwareMap( int rob, int feb, int vmm, int ch, int &row, int &strip, UChar_t &orientation ) const{
    uint16_t key = packKey( feb, vmm, ch );
    if ( mMap.count( key ) ){
        uint16_t val = mMap.at( key );
        unpackVal( val, row, strip );
        orientation = getOrientation( rob, feb, vmm, row );
        return true;
    }
    return false;
}

bool StFttDb::hardwareMap( StFttRawHit * hit ) const{
    // LOG_INFO << "hardwareMap: " << *hit << endm;
    uint16_t key = packKey( hit->feb()+1, hit->vmm()+1, hit->channel() );
    // LOG_INFO << "\tkey: " << key << endm;
    if ( mMap.count( key ) ){
        uint16_t val = mMap.at( key );
        int row=-1, strip=-1;
        unpackVal( val, row, strip );
        // LOG_INFO << " --> row=" << row << ", strip=" << strip << endm;
        
        u_char iPlane = hit->sector() - 1;
        u_char iQuad = hit->rdo() - 1;
        int rob = iQuad + ( iPlane *nQuadPerPlane ) + 1;
        // LOG_INFO << "rob=" << rob << endm;

        UChar_t orientation = getOrientation( rob, hit->feb()+1, hit->vmm()+1, row );
        // LOG_INFO << "dir=" << (int)orientation << endm;
        hit->setMapping( iPlane, iQuad, row, strip, orientation );
        return true;
    }
    return false;
}


UChar_t StFttDb::plane( StFttRawHit * hit ){
    if ( hit->plane() < nPlane )
        return hit->plane();
    return hit->sector() - 1;
}

UChar_t StFttDb::quadrant( StFttRawHit * hit ){
    if ( hit->quadrant() < nQuad )
        return hit->quadrant();
    return hit->rdo() - 1;
}

UChar_t StFttDb::rob( StFttRawHit * hit ){
    return quadrant(hit) + ( plane(hit) * nQuadPerPlane ) + 1;
}

UChar_t StFttDb::rob( StFttCluster * clu ){
    return clu->quadrant() + ( clu->plane() * StFttDb::nQuadPerPlane );
}

UChar_t StFttDb::fob( StFttRawHit * hit ){
    return hit->feb() + ( quadrant( hit ) * nFobPerQuad ) + ( plane(hit) * nFobPerPlane ) + 1;
}

UChar_t StFttDb::orientation( StFttRawHit * hit ){
    if ( hit->orientation() < kFttUnknownOrientation ){
        return hit->orientation();
    }
    return kFttUnknownOrientation;
}

