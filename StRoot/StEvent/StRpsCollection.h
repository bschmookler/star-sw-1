/***************************************************************************
 *
 * $Id: StRpsCollection.h,v 2.1 2009/11/23 22:18:25 ullrich Exp $
 *
 * Author: Thomas Ullrich, Nov 2009
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StRpsCollection.h,v $
 * Revision 2.1  2009/11/23 22:18:25  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StRpsCollection_hh
#define StRpsCollection_hh

#include "StObject.h"
#include "StContainers.h"
#include "StRpsRomanPot.h"

class StRpsCollection : public StObject {
public: 
    StRpsCollection();
    ~StRpsCollection();

    unsigned int numberOfRomanPots() const;
    
    const StRpsRomanPot* romanPot(unsigned int) const;
    StRpsRomanPot* romanPot(unsigned int);
 
    StPtrVecRpsCluster clusters() const;

private:
    enum {mNumberOfRomanPots = 8};
    StRpsRomanPot mRomanPots[mNumberOfRomanPots];
  
    ClassDef(StRpsCollection, 1)
};

#endif
