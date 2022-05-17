/***************************************************************************
 *
 * $Id: StFwdTrack.h,v 2.1 2021/01/11 20:25:37 ullrich Exp $
 *
 * Author: jdb, Feb 2022
 ***************************************************************************
 *
 * Description: StFwdTrack stores the Forward tracks built from Fst and Ftt
 *
 ***************************************************************************
 *
 * $Log: StFwdTrack.h,v $
 * Revision 2.1  2021/01/11 20:25:37  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StFwdTrack_hh
#define StFwdTrack_hh

#include "Stiostream.h"
#include "StObject.h"
#include <vector>
#include "StThreeVectorD.hh"
#include "StEvent/StContainers.h"


namespace genfit {
    class Track;
}

class StFcsCluster;

struct StFwdTrackProjection {
    StFwdTrackProjection() {}
    StFwdTrackProjection( StThreeVectorD xyz, float c[9] ) {
        XYZ = xyz;
        memcpy( cov, c, sizeof(cov) );
    }
    StThreeVectorD XYZ;
    float cov[9];

    float dx(){
        return sqrt( cov[0] );
    }
    float dy(){
        return sqrt( cov[4] );
    }
    float dz(){
        return sqrt( cov[8] );
    }
};

class StFwdTrack : public StObject {

public:
    StFwdTrack( genfit::Track * );
    vector<StFwdTrackProjection> mProjections;
    genfit::Track *mGenfitTrack;

    // momentum at the primary vertex
    const unsigned int   numberOfFitPoints() const;
    const StThreeVectorD momentum() const;
    const StThreeVectorD momentumAt(int _id = 1) const;
    const char charge() const;

    StPtrVecFcsCluster& ecalClusters();
    const StPtrVecFcsCluster& ecalClusters() const;
    void addEcalCluster(StFcsCluster* p);

    StPtrVecFcsCluster& hcalClusters();
    const StPtrVecFcsCluster& hcalClusters() const;
    void addHcalCluster(StFcsCluster* p);

protected:
    StPtrVecFcsCluster mEcalClusters;
    StPtrVecFcsCluster mHcalClusters;

    ClassDef(StFwdTrack,1)

};

#endif

