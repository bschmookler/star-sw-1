/***************************************************************************
 *
 * $Id: StTrsDigitalSector.cc,v 1.12 2005/09/09 22:12:49 perev Exp $
 *
 * Author: bl 
 ***************************************************************************
 *
 * Description: 
 *
 ***************************************************************************
 *
 * $Log: StTrsDigitalSector.cc,v $
 * Revision 1.12  2005/09/09 22:12:49  perev
 * Bug fix + IdTruth added
 *
 * Revision 1.10  2005/07/19 22:23:05  perev
 * Bug fix
 *
 * Revision 1.9  2003/12/24 13:44:53  fisyak
 * Add (GEANT) track Id information in Trs; propagate it via St_tpcdaq_Maker; account interface change in StTrsZeroSuppressedReaded in StMixerMaker
 *
 * Revision 1.8  2000/03/15 23:33:55  calderon
 * Remove extra messages
 *
 * Revision 1.7  1999/11/09 19:33:39  calderon
 * Message of "removed row" replaced with total number of rows removed.
 *
 * Revision 1.6  1999/10/22 00:00:13  calderon
 * -added macro to use Erf instead of erf if we have HP and Root together.
 * -constructor with char* for StTrsDedx so solaris doesn't complain
 * -remove mZeros from StTrsDigitalSector.  This causes several files to
 *  be modified to conform to the new data format, so only mData remains,
 *  access functions change and digitization procedure is different.
 *
 * Revision 1.5  1999/10/11 23:55:22  calderon
 * Version with Database Access and persistent file.
 * Not fully tested due to problems with cons, it
 * doesn't find the local files at compile time.
 * Yuri suggests forcing commit to work directly with
 * files in repository.
 *
 * Revision 1.4  1999/02/10 04:25:43  lasiuk
 * remove debug
 *
 * Revision 1.3  1999/01/22 08:06:21  lasiuk
 * use unsigned char for compatibilty with interface.
 * requires use of two arrays...ugly but fine for now.
 * use of pair<>; values returned by pointer
 *
 * Revision 1.2  1999/01/18 21:02:47  lasiuk
 * comment diagnostics
 *
 * Revision 1.1  1999/01/18 10:23:51  lasiuk
 * initial Revision
 *
 *
 **************************************************************************/
#include "StTrsDigitalSector.hh"

StTrsDigitalSector::StTrsDigitalSector(StTpcGeometry* geoDb)
{
    digitalTimeBins  timeBins;
    digitalPadRow    padRow;
    int     irow;

    for(irow=0; irow< geoDb->numberOfRows(); irow++) {
	//cout << " NumberOfPadsAtRow(" << irow << "): " << geoDb->numberOfPadsAtRow(irow+1) << endl;
	//padRow.assign(geoDb->numberOfPadsAtRow(irow+1), timeBins);
	padRow.resize(geoDb->numberOfPadsAtRow(irow+1), timeBins);
	mData.push_back(padRow);
    }
    // tmp
    // check size at creation?
    //cout << "  NumberOfRows in Data Sector: " << mData.size() << endl;
//     for(int ii=0; ii<mSector.size(); ii++) {
//  	cout << "  Data  PadsInRow(" << ii << "): " << mData[ii].size()  << endl;
//     }
}

StTrsDigitalSector::~StTrsDigitalSector() {/* nopt */}

void StTrsDigitalSector::clear() // clears only the time bins
{
    cout << "StTrsDigitalSector::clear()" << endl;
    for(unsigned int irow=0; irow<mData.size(); irow++) {
	for(unsigned int ipad=0; ipad<mData[irow].size(); ipad++) {
	    mData[irow][ipad].clear();
	}
    }
}

// Caution: rowN specifies rowNumber 1..45
// Below, rowIndex specifies index 0..44
#if 0
void StTrsDigitalSector::assignTimeBins(int rowN, int padN, digitalPadData* tbins)
{
assert(0); // Method can not work with such input
  mData[(rowN-1)][(padN-1)].clear();
  int n = tbins->size();
  for (int i=0;i<n;i++){
    mData[(rowN-1)][(padN-1)].push_back(digitalPair((*tbins)[i],0));}
}
#endif
//________________________________________________________________________________
void StTrsDigitalSector::assignTimeBins(int rowN, int padN, digitalTimeBins* tbins)
{
  mData[(rowN-1)][(padN-1)].clear();
  mData[(rowN-1)][(padN-1)].swap(*tbins);
}

//________________________________________________________________________________
void StTrsDigitalSector::assignTimeBins(StTpcPadCoordinate& coord, digitalTimeBins* tbins)
{
    assignTimeBins(coord.row(), coord.pad(), tbins);
}
//________________________________________________________________________________
int StTrsDigitalSector::cleanup()
{
    unsigned int numberOfEmptyRows=0;
    for (unsigned int iRow=0; iRow<mData.size(); iRow++) {
	unsigned int numberOfEmptyPads=0;
	for (unsigned int iPad=0; iPad<mData[iRow].size(); iPad++) {
	    if (mData[iRow][iPad].size()<7) {
		mData[iRow][iPad].clear();
		numberOfEmptyPads++;
	    }
	} // Pads are now clean
	if (numberOfEmptyPads==mData[iRow].size()) {
	    mData[iRow].clear();
	    numberOfEmptyRows++;
	}
    } // Rows are now clean
    //cout << "This sector had " << numberOfEmptyRows << " empty rows." << endl;
    if (numberOfEmptyRows==mData.size()) return 1;
    else return 0;
}    
