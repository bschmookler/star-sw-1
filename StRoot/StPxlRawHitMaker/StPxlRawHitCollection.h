/*!
 * \class StPxlRawHitCollection 
 * \author Qiu Hao, March 2013
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id: StPxlRawHitCollection.h,v 1.1 2014/01/23 01:05:02 qiuh Exp $
 *
 * Author: Qiu Hao, March 2013
 ***************************************************************************
 *
 * Description:
 * pxl raw hit collection
 * More information at
 * https://www.star.bnl.gov/protected/heavy/qiuh/HFT/software/PXL_software.pdf
 *
 ***************************************************************************
 *
 * $Log: StPxlRawHitCollection.h,v $
 * Revision 1.1  2014/01/23 01:05:02  qiuh
 * *** empty log message ***
 *
 *
 **************************************************************************/
#ifndef StPxlRawHitCollection_hh
#define StPxlRawHitCollection_hh

#include "StObject.h"
#include "StPxlRawHit.h"
#include "StPxlUtil/StPxlConstants.h"

class StPxlRawHitCollection : public StObject {
public:
    StPxlRawHitCollection();
    ~StPxlRawHitCollection();
    void addRawHit(StPxlRawHit* rawHit); ///< add a raw hit to the collection
    Int_t numberOfRawHits(Int_t sector, Int_t ladder, Int_t sensor); ///< number of raw hits in a sensor
    StPxlRawHit* rawHit(Int_t sector, Int_t ladder, Int_t sensor, Int_t rawHitIndex); ///< pointer to a rawHit
    virtual const char *GetCVS() const {
        static const char cvs[]="Tag $Name:  $ $Id: StPxlRawHitCollection.h,v 1.1 2014/01/23 01:05:02 qiuh Exp $ built "__DATE__" "__TIME__ ;
        return cvs;
    }

protected:
    vector<StPxlRawHit*> mPxlRawHitVec[nPxlSectors][nPxlLaddersPerSector][nPxlSensorsPerLadder];  ///< vectors to store raw hit pointers

    ClassDef(StPxlRawHitCollection,1)
};

#endif
