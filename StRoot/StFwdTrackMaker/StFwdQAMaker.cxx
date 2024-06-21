#include "StFwdTrackMaker/StFwdQAMaker.h"
#include "StFwdQAMaker.h"
#include "St_base/StMessMgr.h"
#include "StBFChain/StBFChain.h"
#include "StFwdTrackMaker/StFwdTrackMaker.h"

#include "StFwdTrackMaker/include/Tracker/FwdTracker.h"
#include "StFwdTrackMaker/include/Tracker/ObjExporter.h"
// StEvent includes
#include "StEvent/StBTofCollection.h"
#include "StEvent/StBTofHeader.h"
#include "StEvent/StEvent.h"
#include "StEvent/StFttCluster.h"
#include "StEvent/StFttCollection.h"
#include "StEvent/StFcsCluster.h"
#include "StEvent/StFcsCollection.h"
#include "StFcsDbMaker/StFcsDb.h"
#include "StRoot/StEpdUtil/StEpdGeom.h"
#include "StEvent/StFwdTrackCollection.h"
#include "StEvent/StFwdTrack.h"


#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuFstCollection.h"
#include "StMuDSTMaker/COMMON/StMuFstHit.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuFwdTrack.h"
#include "StMuDSTMaker/COMMON/StMuFwdTrackCollection.h"
#include "StMuDSTMaker/COMMON/StMuFcsCollection.h"
#include "StMuDSTMaker/COMMON/StMuFcsCluster.h"
#include "StMuDSTMaker/COMMON/StMuFcsHit.h"


/** Clear the FwdTreeData from one event to next */
void FwdTreeData::clear(){
    header.clear();
    ftt.reset();
    fttClusters.reset();
    fst.reset();
    reco.reset();
    seeds.reset();
    wcal.reset();
    hcal.reset();
}

/** @brief Sets the Tree cluster from StFcsCluster 
*  @param clu - StFcsCluster
*  @param fcsDb - StFcsDb
* @return void
*/
void FwdTreeFcsCluster::set( StFcsCluster *clu, StFcsDb * fcsDb ) {
    if (!clu) {
        mId = -1;
        return;
    }
    if ( fcsDb ){
        StThreeVectorD xyz = fcsDb->getStarXYZfromColumnRow(clu->detectorId(),clu->x(),clu->y());
        pos.SetXYZ( xyz.x(), xyz.y(), xyz.z() );
    } else {
        pos.SetXYZ( 0, 0, 0 );
    }

    mDetectorId = clu->detectorId();
    
    mEnergy = clu->energy();
    mNTowers = clu->nTowers();
    mId = clu->id();
    mCategory = clu->category();
    mX = clu->x();
    mY = clu->y();
    mSigmaMin = clu->sigmaMin();
    mSigmaMax = clu->sigmaMax();
    mTheta = clu->theta();
    mChi2Ndf1Photon = clu->chi2Ndf1Photon();
    mChi2Ndf2Photon = clu->chi2Ndf2Photon();
}

void FwdTreeFcsCluster::set( StMuFcsCluster *clu, StFcsDb * fcsDb ) {
    if (!clu) {
        mId = -1;
        return;
    }
    if ( fcsDb ){
        StThreeVectorD xyz = fcsDb->getStarXYZfromColumnRow(clu->detectorId(),clu->x(),clu->y());
        pos.SetXYZ( xyz.x(), xyz.y(), xyz.z() );
    } else {
        pos.SetXYZ( 0, 0, 0 );
    }

    mDetectorId = clu->detectorId();
    mEnergy = clu->energy();
    mNTowers = clu->nTowers();
    mId = clu->id();
    mCategory = clu->category();
    mX = clu->x();
    mY = clu->y();
    mSigmaMin = clu->sigmaMin();
    mSigmaMax = clu->sigmaMax();
    mTheta = clu->theta();
    mChi2Ndf1Photon = clu->chi2Ndf1Photon();
    mChi2Ndf2Photon = clu->chi2Ndf2Photon();
}

void FwdTreeRecoTrack::set( StMuFwdTrack *muFwd ){
    if ( !muFwd ){
        LOG_INFO << "Invalid muFwdTrack found, skipping FwdTreeRecoTrack::set" << endm;
        return;
    }

    if (muFwd->didFitConverge()) status = 2;
    if (muFwd->didFitConvergeFully()) status = 1;
    nFailedPoints = muFwd->numberOfFailedPoints();
    q = muFwd->charge();
    mom = muFwd->momentum();
    projs.clear();
    seeds.clear();
    for ( auto p : muFwd->mProjections ){
        LOG_INFO << "Adding Track Projection to Reco Track" << endm;
        FwdTreeTrackProjection tp;
        tp.set( p.mDetId, p.mXYZ, p.mMom, p.mCov );
        projs.push_back( tp );
    }
    for ( auto s : muFwd->mFSTPoints ){
        LOG_INFO << "Adding Track Seed to Reco Track" << endm;
        FwdTreeHit h;
        h.set( s.mXYZ.X(), s.mXYZ.Y(), s.mXYZ.Z(), 1, 1 );
        seeds.push_back( h );
    }
    for ( auto s : muFwd->mFTTPoints ){
        LOG_INFO << "Adding Track Seed to Reco Track" << endm;
        FwdTreeHit h;
        h.set( s.mXYZ.X(), s.mXYZ.Y(), s.mXYZ.Z(), 1, 1 );
        seeds.push_back( h );
    }
}

void FwdTreeRecoTrack::set( StFwdTrack *fwd ){
    if ( !fwd ){
        LOG_INFO << "Invalid fwdTrack found, skipping FwdTreeRecoTrack::set" << endm;
        return;
    }

    if (fwd->didFitConverge()) status = 2;
    if (fwd->didFitConvergeFully()) status = 1;
    nFailedPoints = fwd->numberOfFailedPoints();
    mChi2 = fwd->chi2();
    q = fwd->charge();
    mom.SetXYZ( fwd->momentum().x(), fwd->momentum().y(), fwd->momentum().z() );
    projs.clear();
    seeds.clear();
    for ( auto p : fwd->mProjections ){
        LOG_INFO << "Adding Track Projection to Reco Track" << endm;
        FwdTreeTrackProjection tp;
        tp.set( p.mDetId, TVector3(p.mXYZ.x(), p.mXYZ.y(), p.mXYZ.z()), TVector3(p.mMom.x(), p.mMom.y(), p.mMom.z()), p.mCov );
        projs.push_back( tp );
    }
    for ( auto s : fwd->mFSTPoints ){
        LOG_INFO << "Adding Track Seed to Reco Track" << endm;
        FwdTreeHit h;
        h.set( s.mXYZ.x(), s.mXYZ.y(), s.mXYZ.z(), 1, 1 );
        seeds.push_back( h );
    }
    for ( auto s : fwd->mFTTPoints ){
        LOG_INFO << "Adding Track Seed to Reco Track" << endm;
        FwdTreeHit h;
        h.set( s.mXYZ.x(), s.mXYZ.y(), s.mXYZ.z(), 1, 1 );
        seeds.push_back( h );
    }
}


StFwdQAMaker::StFwdQAMaker() : StMaker("fwdQAMaker"), mTreeFile(nullptr), mTree(nullptr) {

}

int StFwdQAMaker::Init() {

    mTreeFile = new TFile("fwdtree.root", "RECREATE");
    mTree = new TTree("fwd", "fwd tracking tree");

    mTree->Branch("header",           &mTreeData. header, 3200, 99 );
    mTreeData.fst.createBranch(mTree, "fst");
    mTreeData.ftt.createBranch(mTree, "ftt");
    mTreeData.fttClusters.createBranch(mTree, "fttClusters");
    mTreeData.wcal.createBranch(mTree, "wcalClusters");
    mTreeData.hcal.createBranch(mTree, "hcalClusters");

    mTreeData.reco.createBranch(mTree, "reco");
    mTreeData.seeds.createBranch(mTree, "seeds");
    return kStOk;
}
int StFwdQAMaker::Finish() {

    if ( mTreeFile && mTree ){
        mTreeFile->cd();
        mTree->Write();
        mTreeFile->Write();
        LOG_INFO << "StFwdQA File written" << endm;
    }
    return kStOk;
}
int StFwdQAMaker::Make() {
    LOG_INFO << "SETUP START" << endm;
    // setup the datasets / makers
    mMuDstMaker = (StMuDstMaker *)GetMaker("MuDst");
    if(mMuDstMaker) {
        mMuDst = mMuDstMaker->muDst();
        mMuForwardTrackCollection = mMuDst->muFwdTrackCollection();
        mMuFcsCollection = mMuDst->muFcsCollection();
        if (mMuForwardTrackCollection){
            LOG_INFO << "Number of StMuFwdTracks: " << mMuForwardTrackCollection->numberOfFwdTracks() << endm;
            }
    } else {
        LOG_INFO << "No StMuDstMaker found: " << mMuDstMaker << endm;
    }
    mFcsDb = static_cast<StFcsDb *>(GetDataSet("fcsDb"));

    mFwdTrackMaker = (StFwdTrackMaker*) GetMaker( "fwdTrack" );
    if (!mFwdTrackMaker) {
        LOG_WARN << "No StFwdTrackMaker found, skipping StFwdQAMaker" << endm;
        return kStOk;
    }

    // Event header info from stevent
    StEvent *mStEvent = static_cast<StEvent *>(GetInputDS("StEvent"));
    LOG_INFO << "SETUP COMPLETE" << endm;
    // Get the primary vertex used by the FWD Tracker
    auto eventPV = mFwdTrackMaker->GetEventPrimaryVertex();
    mTreeData.header.set( 0, 0, 0, eventPV  );

    // Fill Header from StEvent
    if ( mMuDstMaker ){

    }
    else if ( mStEvent ){
        StBTofCollection *btofC = mStEvent->btofCollection();
        if (btofC) {
            StBTofHeader * btofHeader = btofC->tofHeader();
            if (btofHeader){

                int nEast = btofHeader->numberOfVpdHits( east );
                int nWest = btofHeader->numberOfVpdHits( west );
                int nTof = btofC->tofHits().size();
                float vpdVz = btofHeader->vpdVz();

                mTreeData.header.set( 1, GetEventNumber(), nTof, eventPV  );
                mTreeData.header.vpdVz = vpdVz;
            }
        } // btofC != nullptr
    }
    LOG_INFO << "HEADER COMPLETE" << endm;

    FwdTreeHit fh;

    auto fttHits = mFwdTrackMaker -> GetFttHits();
    for ( auto h : fttHits ){
        fh.set( h.getX(), h.getY(), h.getZ(), 1, 2 );
        mTreeData.ftt.add( fh );
    }
    LOG_INFO << "FTT COMPLETE" << endm;
    auto fstHits = mFwdTrackMaker -> GetFstHits();
    for ( auto h : fstHits ){
        fh.set( h.getX(), h.getY(), h.getZ(), 1, 1 );
        mTreeData.fst.add( fh );
    }
    LOG_INFO << "FST COMPLETE" << endm;
    auto seeds = mFwdTrackMaker -> getTrackSeeds();
    size_t iSeed = 0;
    for ( auto s : seeds ){
        for ( auto h : s ){
            fh.set( h->getX(), h->getY(), h->getZ(), 1, 1 );
            fh.trackId = iSeed;
            mTreeData.seeds.add( fh );
        }
        iSeed++;
    } // for s in seeds
    LOG_INFO << "SEEDS COMPLETE" << endm;
    
    size_t iTrack = 0;
    FwdTreeRecoTrack frt;
    FwdTreeHit fth;
    if ( mMuForwardTrackCollection ){
        LOG_INFO << "Adding " << mMuForwardTrackCollection->numberOfFwdTracks() << " FwdTracks (MuDst)" << endm;
        for ( size_t iTrack = 0; iTrack < mMuForwardTrackCollection->numberOfFwdTracks(); iTrack++ ){
            auto muTrack = mMuForwardTrackCollection->getFwdTrack(iTrack);
            frt.set( muTrack );
            mTreeData.reco.add( frt );
        }
    }

    if ( mStEvent && mStEvent->fwdTrackCollection() ){
        auto fwdTracks = mStEvent->fwdTrackCollection()->tracks();
        LOG_INFO << "Adding " << fwdTracks.size() << " FwdTracks (StEvent)" << endm;
        for ( auto ft : fwdTracks ){
            frt.set( ft );
            mTreeData.reco.add( frt );
        }
    }
    LOG_INFO << "TRACKS COMPLETE" << endm;

    /** MonteCarlo Tracks */
    // Get geant tracks
    // St_g2t_track *g2t_track = (St_g2t_track *)GetDataSet("geant/g2t_track");

    // if (g2t_track){
    //     LOG_DEBUG << g2t_track->GetNRows() << " mc tracks in geant/g2t_track " << endm;
    //     for (int irow = 0; irow < g2t_track->GetNRows(); irow++) {
    //         g2t_track_st *track = (g2t_track_st *)g2t_track->At(irow);

    //         if (nullptr == track)
    //             continue;

    //         int track_id = track->id;
    //         float pt2 = track->p[0] * track->p[0] + track->p[1] * track->p[1];
    //         float pt = std::sqrt(pt2);
    //         float eta = track->eta;
    //         float phi = std::atan2(track->p[1], track->p[0]); //track->phi;
    //         int q = track->charge;
    //         // TODO add MC tracks to tree
    //     } // loop on track (irow)
    // }

    FillFttClusters();
    if ( mMuDstMaker ){
        FillFcsStMuDst();
    } else {
        FillFcsStEvent();
    }

    mTree->Fill();
    return kStOk;
}
void StFwdQAMaker::Clear(const Option_t *opts) {
    mTreeData.clear();
    return;
}

void StFwdQAMaker::FillFcsStMuDst( ) {

    if ( !mMuDst ){
        LOG_INFO << "No mMuDst found, skipping StFwdQAMaker::FillFcsStEvent" << endm;
        return;
    }
    StMuFcsCollection* fcs = mMuDst->muFcsCollection();
    if ( !fcs ){
        LOG_INFO << "No muFcsCollection found, skipping StFwdQAMaker::FillFcsStEvent" << endm;
        return;
    }

    StEpdGeom epdgeo;
    FwdTreeFcsCluster fcsclu;
    // LOAD ECAL / HCAL CLUSTERS
    for( size_t i = 0; i < fcs->numberOfClusters(); i++){
        StMuFcsCluster * clu = fcs->getCluster(i);
        LOG_INFO << "FCS CLUSTERS: " << fcs->numberOfClusters() << endm;
        fcsclu.set( clu, mFcsDb );

        if ( clu->detectorId() == kFcsEcalNorthDetId || clu->detectorId() == kFcsEcalSouthDetId ){
            LOG_INFO << "Adding WCAL Cluster to FwdTree" << endm;
            mTreeData.wcal.add( fcsclu );
        } else if ( clu->detectorId() == kFcsHcalNorthDetId || clu->detectorId() == kFcsHcalSouthDetId ){
            LOG_INFO << "Adding HCAL Cluster to FwdTree" << endm;
            mTreeData.hcal.add( fcsclu );
        }
    }
}

void StFwdQAMaker::FillFcsStEvent( ) {
    if ( !mStEvent ){
        LOG_INFO << "No StEvent found, skipping StFwdQAMaker::FillFcsStEvent" << endm;
        return;
    }
    StFcsCollection* fcsCol = mStEvent->fcsCollection();
    if ( !fcsCol ){
        LOG_INFO << "No StFcsCollection found, skipping StFwdQAMaker::FillFcsStEvent" << endm;
        return;
    }

    StEpdGeom epdgeo;

    FwdTreeFcsCluster fcsclu;
    // LOAD ECAL / HCAL CLUSTERS
    for ( int idet = 0; idet  < 4; idet++ ){
        StSPtrVecFcsCluster& clusters = fcsCol->clusters(idet);
        LOG_INFO << "FCS CLUSTERS [" << idet << "]: " << clusters.size() << endm;
        int nc=fcsCol->numberOfClusters(idet);
        for ( int i = 0; i < nc; i++ ){
            StFcsCluster* clu = clusters[i];
            
            fcsclu.set( clu, mFcsDb );

            if ( clu->detectorId() == kFcsEcalNorthDetId || clu->detectorId() == kFcsEcalSouthDetId ){
                LOG_INFO << "Adding WCAL Cluster to FwdTree" << endm;
                mTreeData.wcal.add( fcsclu );
            } else if ( clu->detectorId() == kFcsHcalNorthDetId || clu->detectorId() == kFcsHcalSouthDetId ){
                LOG_INFO << "Adding HCAL Cluster to FwdTree" << endm;
                mTreeData.hcal.add( fcsclu );
            }

        } // i
    } // idet

}

void StFwdQAMaker::FillFttClusters(){
    float pz[] = {280.90499, 303.70498, 326.60501, 349.40499};
    float SCALE = 1.0;
    TVector3 cp;
    FwdTreeHit fh;
    FwdTreeFttCluster ftc;
    StEvent *event = static_cast<StEvent *>(GetInputDS("StEvent"));
    if ( !event->fttCollection() ) return;

    LOG_INFO << "FTT RawHits: " << event->fttCollection()->numberOfRawHits() << endm;
    LOG_INFO << "FTT Clusters: " << event->fttCollection()->numberOfClusters() << endm;

    for ( size_t i = 0; i < event->fttCollection()->numberOfClusters(); i++ ){
        StFttCluster* c = event->fttCollection()->clusters()[i];
        if ( c->nStrips() < 1 ) continue;
        float dw = 0.05, dlh = 60.0, dlv = 60.0;
        float mx = 0.0, my = 0.0;
        float sx = 1.0, sy = 1.0;


        if ( c->quadrant() == kFttQuadrantA ){
            mx = 0; my = 0;
            sx = 1.0; sy = 1.0;
        } else if ( c->quadrant() == kFttQuadrantB ){
            mx = 10.16*SCALE; my = 0.0*SCALE;
            sy = -1;
            dlv = -dlv;

        } else if ( c->quadrant() == kFttQuadrantC ){
            mx = -10.16*SCALE ; my = -00.0*SCALE;
            sx = -1.0; sy = -1.0;
            dlh = -dlh; dlv = -dlv;

        } else if ( c->quadrant() == kFttQuadrantD ){
            sx = -1;
            dlh = -dlh;
        }

        cp.SetZ( -pz[ c->plane() ] * SCALE );
        if ( c->orientation() == kFttHorizontal ){
            cp.SetY( my + sy * c->x()/10.0 * SCALE );
            cp.SetX( mx );
        } else if ( c->orientation() == kFttVertical ){
            cp.SetX( mx + sx * c->x()/10.0 * SCALE );
            cp.SetY( my );
        }
        // fh.set( cp.X(), cp.Y(), cp.Z(), c->nStrips(), c->quadrant() );
        ftc.pos.SetXYZ( cp.X(), cp.Y(), cp.Z() );
        ftc.mQuadrant = c->quadrant();
        ftc.mRow = c->row();
        ftc.mOrientation = c->orientation();
        ftc.mPlane = c->plane();
        ftc.mId = c->id();
        ftc.mNStrips = c->nStrips();
        ftc.mX = c->x();
        ftc.mSumAdc = c->sumAdc();

        mTreeData.fttClusters.add( ftc );
    }
}
