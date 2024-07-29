#ifndef FWD_TRACKER_H
#define FWD_TRACKER_H

#include "Fit/Fitter.h"
#include "TFile.h"
#include "TGraph2D.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "StFwdTrackMaker/include/Tracker/FwdHit.h"
#include "StFwdTrackMaker/include/Tracker/FwdDataSource.h"
#include "StFwdTrackMaker/include/Tracker/TrackFitter.h"

#include "Criteria/Criteria.h"
#include "Criteria/ICriterion.h"
#include "KiTrack/Automaton.h"
#include "KiTrack/SegmentBuilder.h"
#include "KiTrack/SubsetHopfieldNN.h"

#include "StFwdTrackMaker/include/Tracker/CriteriaKeeper.h"

#include "GenFit/FitStatus.h"
#include "GenFit/GFRaveVertexFactory.h"

#include "StFwdTrackMaker/FwdTrackerConfig.h"
#include "StFwdTrackMaker/Common.h"

// Utility class for evaluating ID and QA truth
struct MCTruthUtils {

    static int dominantContribution(Seed_t hits, double &qa) {

        // track_id, hits on track
        std::unordered_map<int,int> truth;
        for ( auto hit : hits ) {
            FwdHit* fhit = dynamic_cast<FwdHit*>(hit);
            truth[ fhit->_tid ]++;
        }

        if ( truth.size() == 0 ){
            return -1;
        }

        using namespace std;
        using P = decltype(truth)::value_type;
        auto dom = max_element(begin(truth), end(truth), [](P a, P b){ return a.second < b.second; });

        // QA represents the percentage of hits which
        // vote the same way on the track
        if ( hits.size() > 0 )
            qa = double(dom->second) / double(hits.size()) ;
        else
            qa = 0;

        return dom->first;
    };
};

class EventStats {
    public:
    TString classname() const { return "EventStats"; }
    void reset(){
        mNumSeeds = 0;
        mAttemptedFits = 0;
        mFailedFits = 0;
        mGoodFits = 0;
        mGoodCardinals = 0;

        mAttemptedReFits = 0;
        mFailedReFits = 0;
        mGoodReFits = 0;
        mPossibleReFit = 0;
        mStep1Duration.clear();
        mStep2Duration.clear();
        mStep3Duration.clear();
        mStep4Duration.clear();
        mFitDuration.clear();
    }
    int mNumSeeds = 0;
    int mAttemptedFits = 0;
    int mFailedFits = 0;
    int mGoodFits = 0;
    int mGoodCardinals = 0;

    int mAttemptedReFits = 0;
    int mFailedReFits = 0;
    int mGoodReFits = 0;
    int mPossibleReFit = 0;
    vector<float> mStep1Duration;
    vector<float> mSeedFindingDuration;
    vector<float> mStep2Duration;
    vector<float> mStep3Duration;
    vector<float> mStep4Duration;
    vector<float> mFitDuration;
};

class GenfitTrackResult {
public:
    GenfitTrackResult(){}
    GenfitTrackResult(  Seed_t &seed, std::shared_ptr<genfit::Track> track ) {
        set( seed, track );
    }
    ~GenfitTrackResult(){
        // Clear();
    }
    void Clear() {
        if ( mTrack ){
            mTrack->Clear();
        }
    }
    void set(   Seed_t &seeds, std::shared_ptr<genfit::Track> track ){
        setSeeds( seeds );
        setTrack( track );
    }
    void setSeeds( Seed_t &seed ){
        mSeed = seed;
    }
    void setTrack( std::shared_ptr<genfit::Track> track ){
        try {
            // this->track = new genfit::Track(*track);
            mTrack      = track;
            mStatus     = mTrack->getFitStatus();
            mTrackRep   = mTrack->getCardinalRep();

            mIsFitConverged          = mStatus->isFitConverged();
            mIsFitConvergedFully     = mStatus->isFitConvergedFully();
            mIsFitConvergedPartially = mStatus->isFitConvergedPartially();
            mNFailedPoints           = mStatus->getNFailedPoints();
            mCharge                  = mStatus->getCharge();
            mChi2                    = mStatus->getChi2();

            if ( mIsFitConverged ){
                mMomentum = mTrackRep->getMom( mTrack->getFittedState(0, mTrackRep) );
            }
            LOG_DEBUG << "GenfitTrackResult::set Track successful" << endm;
        } catch ( genfit::Exception &e ) {
            LOG_ERROR << "Unable to set track -> GenfitException: " << e.what() << endm;
            this->mTrack                    = nullptr;
            this->mTrackRep                 = nullptr;

            this->mIsFitConverged           = false;
            this->mIsFitConvergedFully      = false;
            this->mIsFitConvergedPartially  = false;
            this->mNFailedPoints            = 99;
            this->mCharge                   = 0;
            this->mChi2                     = -1;
        }
    }

    /** @brief Set the DCA and primary vertex for the event
     * 
     */
    void setDCA( TVector3 pv ){
        mPV = pv;
        if ( mTrack ){
            try {
                auto dcaState = mTrack->getFittedState( 0 );
                this->mTrackRep->extrapolateToPoint( dcaState, mPV );
                this->mDCA = dcaState.getPos();
            } catch ( genfit::Exception &e ) {
                LOG_ERROR << "CANNOT GET DCA : GenfitException: " << e.what() << endm;
                this->mDCA = TVector3(99,99,99);
            }
            
        }
    }
    size_t numFtt() const { 
        size_t n = 0;
        for ( auto hit : mSeed ){
            if ( dynamic_cast<FwdHit*>(hit)->_detid == kFttId ){
                n++;
            }
        }
        return n;
    }
    size_t numFst() const { 
        size_t n = 0;
        for ( auto hit : mSeed ){
            if ( dynamic_cast<FwdHit*>(hit)->_detid == kFstId ){
                n++;
            }
        }
        return n;
    }
    size_t numPV() const {
        size_t n = 0;
        for ( auto hit : mSeed ){
            if ( dynamic_cast<FwdHit*>(hit)->_detid == kTpcId ){
                n++;
            }
        }
        return n;
    }

    void mergeSeeds( GenfitTrackResult &other ){
        // combine the unique Ftt and Fst seeds
        for ( auto hit : other.mSeed ){
            if ( std::find( mSeed.begin(), mSeed.end(), hit ) == mSeed.end() ){
                mSeed.push_back( hit );
            }
        }
    }

    Seed_t                          mSeed;
    TVector3                        mPV;
    TVector3                        mMomentum;
    TVector3                        mMomentumAtFirstMeasurement; // momentum at first measurement (not PV)
    float                           mCharge = 0;
    float                           mChi2 = -1;
    genfit::FitStatus               *mStatus = nullptr;
    genfit::AbsTrackRep             *mTrackRep = nullptr;
    std::shared_ptr<genfit::Track>  mTrack = nullptr;
    bool                            mIsFitConverged = false;
    bool                            mIsFitConvergedFully = false;
    bool                            mIsFitConvergedPartially = false;
    size_t                          mNFailedPoints = 0;
    TVector3                        mDCA;
}; // GenfitTrackResult

class ForwardTrackMaker {
  public:
    ForwardTrackMaker() : mConfigFile("config.xml"), mEventVertex(-999, -999, -999) {
        // noop
    }

    const std::vector<GenfitTrackResult> &getTrackResults() const { return mTrackResults; }
    const std::vector<Seed_t> &getTrackSeeds() const { return mTrackSeeds; }
    const EventStats &getEventStats() const { return mEventStats; }

    void Clear(){
        for ( auto gtr : mTrackResults ){
            gtr.Clear();
        }
        mTrackResults.clear();
    }

    /**
     * @brief Set the Config File object
     *
     * @param cf : config filename
     */
    void setConfigFile(std::string cf) {
        mConfigFile = cf;
    }

    /**
     * @brief Set the Save Criteria Values object
     *
     * @param save : true to save crit values
     */
    void setSaveCriteriaValues(bool save) {
        mSaveCriteriaValues = save;
    }

    // Adopt external configuration file
    void setConfig(FwdTrackerConfig cfg) { mConfig = cfg; }
    // Adopt external hit loader
    void setData(std::shared_ptr<FwdDataSource>data) { mDataSource = data; }

    /**
     * @brief Initialize FwdTracker
     *
     * @param geoCache : name of cached geometry file
     * @param genHistograms : generate histograms
     */
    virtual void initialize( TString geoCache, bool genHistograms) {
        mGeoCache = geoCache;
        mDoTrackFitting = mConfig.get<bool>("TrackFitter:active", true);

        if (!mConfig.exists("TrackFitter"))
            mDoTrackFitting = false;
    } //initialize

    /**
     * @brief Loads Criteria from XML configuration.
     * Utility function for loading criteria from XML config.
     * @param path : path in config to load
     * @return vector of ICriterion pointers
    */
    std::vector<KiTrack::ICriterion *> loadCriteria(string path) {

        std::vector<KiTrack::ICriterion *> crits;
        auto paths = mConfig.childrenOf(path);

        for (string p : paths) {
            string name = mConfig.get<string>(p + ":name", "");
            bool active = mConfig.get<bool>(p + ":active", true);

            if (false == active) {
                continue;
            }

            float vmin = mConfig.get<float>(p + ":min", 0);
            float vmax = mConfig.get<float>(p + ":max", 1);

            KiTrack::ICriterion * crit = nullptr;
            if ( name == "Crit2_BDT" ){
                // crit = new BDTCrit2( vmin, vmax );
                LOG_WARN << "BDT Criteria not implemented/out of date" << endm;
            } else {
                crit = KiTrack::Criteria::createCriterion(name, vmin, vmax);
            }

            crit->setSaveValues(mSaveCriteriaValues);

            if (mSaveCriteriaValues)
                crits.push_back(new CriteriaKeeper(crit)); // CriteriaKeeper intercepts values and saves them
            else
                crits.push_back(crit);

        }

        return crits;
    } // loadCriteria

    /**
     * @brief Clear the loaded criteria
     * @param crits : vector of ICriterion pointers to properly clear
     */
    void clearCriteria( std::vector<KiTrack::ICriterion *> &crits ){
        for ( size_t i = 0; i < crits.size(); i++ ){
            if ( crits[i] ){
                delete crits[i];
                crits[i] = nullptr;
            }
        }
        crits.clear();
    }

    /**
     * @brief Get the Criteria Values object
     *
     * @param crit_name : Criteria to get
     * @return std::vector<float> : list of values
     */
    std::vector<float> getCriteriaValues(std::string crit_name) {
        std::vector<float> em;
        if (mSaveCriteriaValues != true) {
            return em;
        }

        for (auto crit : mTwoHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getValues();
            }
        }

        for (auto crit : mThreeHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getValues();
            }
        }

        return em;
    } //getCriteriaValues

    /**
     * @brief Get the Criteria All Values object
     *
     * @param crit_name : Criteria values to get
     * @return std::vector<std::map < std::string , float >> : map of values
     */
    std::vector<std::map < std::string , float >> getCriteriaAllValues(std::string crit_name) {
        std::vector<std::map < std::string , float >> em;
        if (mSaveCriteriaValues != true) {
            return em;
        }

        for (auto crit : mTwoHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getAllValues();
            }
        }

        for (auto crit : mThreeHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getAllValues();
            }
        }

        return em;
    } // getCriteriaAllValues

    /**
     * @brief Get the Criteria Track Ids object
     *
     * @param crit_name : Name of criteria to get track ids for
     * @return std::vector<int> : list of track ids
     */
    std::vector<int> getCriteriaTrackIds(std::string crit_name) {
        std::vector<int> em;
        if (mSaveCriteriaValues != true) {
            return em;
        }

        for (auto crit : mTwoHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getTrackIds();
            }
        }

        for (auto crit : mThreeHitCrit) {
            if (crit_name == crit->getName()) {
                auto critKeeper = static_cast<CriteriaKeeper *>(crit);
                return critKeeper->getTrackIds();
            }
        }

        return em;
    } //getCriteriaTrackIds

    /**
     * @brief Clear the saved values for two hit and three hit criteria
     *
     */
    void clearSavedCriteriaValues() {
        if (mSaveCriteriaValues != true) {
            return;
        }

        for (auto crit : mTwoHitCrit) {
            auto critKeeper = static_cast<CriteriaKeeper *>(crit);
            critKeeper->clear();
        }

        for (auto crit : mThreeHitCrit) {
            auto critKeeper = static_cast<CriteriaKeeper *>(crit);
            critKeeper->clear();
        }
    } // clearSavedCriteria

    /**
     * @brief Determine the total num of hits in the hitmap
     *
     * @param hitmap : hitmap to consider
     * @return size_t : total num of hits
     */
    size_t nHitsInHitMap(FwdDataSource::HitMap_t &hitmap) {
        size_t n = 0;
        for (auto kv : hitmap) {
            n += kv.second.size();
        }
        return n;
    }

    /**
     * @brief Remove used hits from the hit map
     *
     * @param hitmap : hitmap with hits used this round
     * @param tracks : tracks formed from hits
     */
    void removeHits(FwdDataSource::HitMap_t &hitmap, std::vector<Seed_t> &tracks) {

        for (auto track : tracks) {
            for (auto hit : track) {
                int sector = hit->getSector();

                auto hitit = std::find(hitmap[sector].begin(), hitmap[sector].end(), hit);

                if (hitit != hitmap[sector].end()) {
                    hitmap[sector].erase(hitit);
                    mTotalHitsRemoved++;
                }

            } // loop on hits in track
        }         // loop on track
    } // removeHits


    /** @brief merges the FST and FTT hitmaps into a single hitmap
     *  The FTT hits are shifted by the number of FST sectors
     *  @param hitmap1: FST hitmap
     *  @param hitmap2: FTT hitmap
     *  @return void
    */
    void mergeHitmaps( FwdDataSource::HitMap_t &hitmap1, FwdDataSource::HitMap_t &hitmap2 ){
        static const int numFstSectors = 3;
        for ( auto kv : hitmap2 ){
            for ( auto hit : kv.second ){
                dynamic_cast<FwdHit*>( hit )->setSector( hit->getSector() + numFstSectors );
                hitmap1[kv.first + numFstSectors].push_back( hit );
            }
        }
    } // mergeHitmaps

    /** @brief cleanup the event-wise data structures
     * 
     */
    void cleanup() {
        /************** Cleanup ****************************************/
        // Moved cleanup to the start of doEvent, so that the fit results
        // persist after the call
        mTrackSeeds.clear();
        mTrackResults.clear();
        mEventStats.reset();
        mTotalHitsRemoved = 0;
        /************** Cleanup **************************/
    }

    /**
     * @brief Perform the track finding
     * Creates a list of track seeds from the hitmaps
     * Retrieve them using `getTrackSeeds()`
     */
    void findTrackSeeds() {
        cleanup();
        /*************************************************************/
        // Get the hitmaps 
        /*************************************************************/
        long long itStart = FwdTrackerUtils::nowNanoSecond();
        FwdDataSource::HitMap_t &fttHitmap = mDataSource->getFttHits();
        FwdDataSource::HitMap_t &fstHitmap = mDataSource->getFstHits();

        /*************************************************************/
        // Determine seed finding mode
        /*************************************************************/
        string hitmapSource = mConfig.get<string>("TrackFinder:source", "ftt");
        mSeedSource = kSeqSeed; // default to FST
        if (hitmapSource == "fst")
            mSeedSource = kFstSeed;
        else if (hitmapSource == "ftt")
            mSeedSource = kFttSeed;
        else if (hitmapSource == "seq")
            mSeedSource = kSeqSeed;
        else if (hitmapSource == "sim")
            mSeedSource = kSimSeed;
        LOG_DEBUG << "Performing Fwd Seed finding with mode: " << mConfig.get<string>("TrackFinder:source", "ftt") << " = " << mSeedSource << endm;

        LOG_DEBUG << "FTT Hitmap has:" << fttHitmap.size() << " hits to consider" << endm;
        for (auto hp : fttHitmap){
            LOG_DEBUG << "HITMAP [" << hp.first << ", { ";
            for ( auto h: hp.second ){
                LOG_DEBUG << "z=" << h->getZ() << " ";
            }
            LOG_DEBUG << " }" << endm;
        }
        
        LOG_DEBUG << "Forward Track seed finding using FST" << endm;
        LOG_DEBUG << "FST Hitmap has:" << fstHitmap.size() << " hits to consider" << endm;
        for (auto hp : fstHitmap){
            LOG_DEBUG << "HITMAP [" << hp.first << ", { ";
            for ( auto h: hp.second ){
                LOG_DEBUG << "z=" << h->getZ() << " s=" << h->getSector() << " ";
            }
            LOG_DEBUG << " }" << endm;
        }

        FwdDataSource::McTrackMap_t &mcTrackMap = mDataSource->getMcTracks();

        long long duration = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        
        mEventStats.mStep1Duration.push_back( duration );

        /*************************************************************/
        // Determine if we should use MC seed finding
        /*************************************************************/
        bool mcTrackFinding = true;
        if (mConfig.exists("TrackFinder")){
            mcTrackFinding = false;
        }
        if (mConfig.exists("TrackFinder") && mConfig.get<bool>( "TrackFinder:mc", false ) == false ){
            mcTrackFinding = false;
        }
        if (mConfig.exists("TrackFinder") && mConfig.get<bool>( "TrackFinder:active", true ) == false){
            mcTrackFinding = true;
        }

        /*************************************************************/
        // DO MC Track Finding (if set to do so)
        if (mcTrackFinding) {
            LOG_DEBUG << "MC TRACK FINDING " << endm;
            doMcTrackFinding(mcTrackMap, mSeedSource);
            /***********************************************/
            // REFIT with FST or FTT hits accordingly
            // if (mConfig.get<bool>("TrackFitter:refit", false)) {
            //     if (mSeedSource == kFstSeed || mSeedSource == kSeqSeed)
            //         addFstHitsMc();
            //     if (mSeedSource != kFstSeed )
            //         addFttHitsMc();
            // } else {
            //     LOG_DEBUG << "Skipping Refit (MC tracking)" << endm;
            // }
            /***********************************************/
            long long duration2 = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
            mEventStats.mSeedFindingDuration.push_back( duration2 );
            return;
        } else {
            LOG_DEBUG << "Performing Standard Track Finding" << endm;
        }
        /*************************************************************/


        /*************************************************************/
        // Standard Track Finding
        size_t nIterations = mConfig.get<size_t>("TrackFinder:nIterations", 0);
        for (size_t iIteration = 0; iIteration < nIterations; iIteration++) {
            if ( mSeedSource == kSimSeed){
                mergeHitmaps( fstHitmap, fttHitmap );
            } else {
                if ( mSeedSource == kFstSeed || mSeedSource == kSeqSeed ){
                    doSeedFindingIteration(iIteration, fstHitmap);
                }
                if ( mSeedSource == kFttSeed || mSeedSource == kSeqSeed){
                    doSeedFindingIteration(iIteration, fttHitmap);
                }
            } 
        } // iIteration
        /*************************************************************/
        mEventStats.mNumSeeds = mTrackSeeds.size();
        long long duration2 = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        mEventStats.mSeedFindingDuration.push_back( duration2 );        
    } // FindTrackSeeds
    
    /**
     * @brief Perform a single fit from seed points
     *
     * @param seed : seed points from either FTT or FST
     * @param includeVertex : include the primary vertex in the fit or not
     * @return GenfitTrackResult : result of the fit
     */
    GenfitTrackResult fitTrack(Seed_t &seed, bool includeVertex = false) {
        // We will build this up as we go
        GenfitTrackResult gtr;

        // First, set the seed information
        gtr.setSeeds( seed );

        mEventStats.mAttemptedFits++;
        double vertex[3] = { mEventVertex.X(), mEventVertex.Y(), mEventVertex.Z() };

        double * pVertex = 0;
        // check if provided vertex is valid
        const bool isVertexValid = (fabs(mEventVertex.X()) < 150);
        if ( includeVertex && isVertexValid ) {
            pVertex = vertex; // only use it if it has been set from default
        }

        bool useMcSeed = mConfig.get<bool>("TrackFitter:mcSeed", false);
        if (true == useMcSeed) {
            /*******************************************************/
            // Only for Simulation
            // Calculate the MC info first and check filters
            int idt = 0;
            double qual = 0;
            //  Get the quality and MC truth id
            idt = MCTruthUtils::dominantContribution(seed, qual);

            TVector3 mcSeedMom;
            auto mctm = mDataSource->getMcTracks();
            // get the MC track momentum if we can (may be used for state seed)
            if (mctm.count(idt)) {
                auto mct = mctm[idt];
                mcSeedMom.SetPtEtaPhi(mct->mPt, mct->mEta, mct->mPhi);
            } else {
                LOG_WARN << "Requested McSeed, but couldnt find mct for idt: " << idt << ", qual: " << qual << endm;
            }
            /*******************************************************/

            // use the MC pt, eta, phi as the seed for fitting
            mTrackFitter->fitTrack(seed, &mcSeedMom);
        } else {
            // Normal case, real data
            mTrackFitter->fitTrack(seed );
        }

        /*******************************************************/
        // Get the track from the fitter
        // and set the track in the GenfitTrackResult
        if (mTrackFitter->getTrack() != nullptr ){    
            gtr.set( seed, mTrackFitter->getTrack() );

            if (gtr.mStatus && gtr.mStatus->isFitConvergedFully()) {
                mEventStats.mGoodFits++;
            } else {
                mEventStats.mFailedFits++;
            }
        } else {
            mEventStats.mFailedFits++;
        }
        LOG_DEBUG << "FwdTracker::fitTrack complete" << endm;
        return gtr;
    } // fitTrack

    /**
     * @brief Loop on track seeds and fit each one
     *
     * Track fitting proceeds in 3 possible iterations
     * 1. Fit seed points (without PV)
     * 2. Look for additional hits in the other tracking detector
     * 3. Refit the track with the additional hits
     * 4. Refit the track with the primary vertex
     * 5. look again and refit any additional hits
     * 
     * @param trackSeeds : Track seeds
     */
    void doTrackFitting( const std::vector<Seed_t> &trackSeeds) {
        if (!mDoTrackFitting)
            return;
        long long itStart = FwdTrackerUtils::nowNanoSecond();

        bool doRefit = mConfig.get<bool>("TrackFitter:refit", false);

        
        LOG_DEBUG << "Starting Track fitting loop on " << trackSeeds.size() << " track seeds" << endm;
        size_t index = 0;
        for (auto t : trackSeeds) {
            LOG_DEBUG << "Track seed fit #" << index << endm;
            
            /** Tracking Step 1 */
            // Fit each accepted track seed
            GenfitTrackResult gtr = fitTrack(t);

            /** Tracking Step 2 */
            // Look for additional hits in the other tracking detector
            // and refit the track with the additional hits

            if (mSeedSource != kFttSeed){
                for ( int i = 0; i < FwdSystem::sNFttLayers; i++ ){
                    addFttHits( gtr, i );
                }
            }
            if (mSeedSource != kFstSeed ){
                for ( int i = 0; i < FwdSystem::sNFstLayers; i++ ){
                    addFstHits( gtr, i );
                }
            }
            // TODO: NOW I NEED TO FIT AGAIN, how to handle GTR

            mTrackResults.push_back( gtr );
            index++;
        }

        long long duration1 = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        mEventStats.mFitDuration.push_back( duration1 );
        float perGood = (float) mEventStats.mGoodFits / (float) mEventStats.mAttemptedFits;
        float perFailed = (float) mEventStats.mFailedFits / (float) mEventStats.mAttemptedFits;
        float perCardinals = (float) mEventStats.mGoodCardinals / (float) mEventStats.mGoodFits;
        LOG_DEBUG << "Track Fitting Results:" <<
                    TString::Format( 
                        "Attempts = %lu, Good = %lu (%f%), Failed = %lu (%f%), GoodCardinals = %lu (%f%)", 
                        mEventStats.mAttemptedFits, 
                        mEventStats.mGoodFits, 
                        perGood, 
                        mEventStats.mFailedFits, 
                        perFailed, 
                        mEventStats.mGoodCardinals, 
                        perCardinals ) << endm;
        

        // Now try refitting every track with the primary vertex
        for (auto &gtr : mTrackResults) {
            
        }

        long long duration2 = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        LOG_DEBUG << "Track fitting took " << duration2 << "ms" << endm;
    } // doTrackFitting

    /**
     * @brief MC track finding builds track seeds from available hits using MC association
     *
     * @param mcTrackMap : Mc tracks
     * @param useFttAsSource : Use FTT for seeds or (false) use Fst
     */
    void doMcTrackFinding(FwdDataSource::McTrackMap_t &mcTrackMap, int seedSource) {
        LOG_INFO << "Running MC Seed Finding, mode: " << seedSource << endm;

        // If we want sequential MC track finding then do them each individually
        if ( seedSource == kSeqSeed ){
            doMcTrackFinding( mcTrackMap, kFstSeed );
            doMcTrackFinding( mcTrackMap, kFttSeed );
            return;
        }

        mTrackSeedsThisIteration.clear();
        // we will build reco tracks from each McTrack
        for (auto kv : mcTrackMap) {

            auto mc_track = kv.second;
            LOG_DEBUG << "McTrack: nFtt=" << mc_track->mFttHits.size() << ", nFst=" << mc_track->mFstHits.size() << endm;

            if (seedSource == kFttSeed && mc_track->mFttHits.size() < 2){ // require min 4 FTT hits on track
                continue;
            }

            if (seedSource == kFstSeed && mc_track->mFstHits.size() < 2 ) { // require min 3 FST hits on track
                LOG_DEBUG << "Skipping McSeedFinding bc FST hits < 2" << endm;
                continue;
            }

            if ( seedSource == kSimSeed && mc_track->mFstHits.size() < 2 && mc_track->mFttHits.size() < 2 ){
                continue;
            }

            std::set<size_t> uvid;
            Seed_t track;

            if ( seedSource != kFstSeed ){ // FTT is used unless we are ONLY considering FST
                for (auto h : mc_track->mFttHits) {
                    track.push_back(h);
                    uvid.insert(static_cast<FwdHit *>(h)->_vid);
                }
            }
            if (seedSource != kFttSeed ) { // FST
                for (auto h : mc_track->mFstHits) {
                    track.push_back(h);
                    uvid.insert(static_cast<FwdHit *>(h)->_vid);
                }
            }

            if (uvid.size() == track.size()) { // only add tracks that have one hit per volume
                mTrackSeedsThisIteration.push_back(track);
                int idt = 0;
                double qual = 0;
                idt = MCTruthUtils::dominantContribution(track, qual);
            } else {
                //Skipping track that doesnt have hits on all layers
            }
        }

        LOG_DEBUG << "McTrackFinding Found: " << mTrackSeedsThisIteration.size() << " tracks" << endm;
        doTrackFitting(mTrackSeedsThisIteration);

        // Now save to the main reco track list
        mTrackSeeds.insert( mTrackSeeds.end(), mTrackSeedsThisIteration.begin(), mTrackSeedsThisIteration.end() );
    } //doMcTrackFinding

    /** sliceHitMapInPhi
    * @brief Slices a hitmap into a phi section
    *
    * @param inputMap INPUT hitmap to process
    * @param outputMap OUTPUT hitmap, will be cleared and filled with only the hits from inputMap that are within phi region
    * @param phi_min The minimum phi to accept
    * @param phi_max The maximum Phi to accept
    *
    * @returns The number of hits in the outputMap
    */
    size_t sliceHitMapInPhi( FwdDataSource::HitMap_t &inputMap, FwdDataSource::HitMap_t &outputMap, float phi_min, float phi_max ){
        size_t n_hits_kept = 0;

        outputMap.clear(); // child STL containers will get cleared too
        for ( auto kv : inputMap ){
            for ( KiTrack::IHit* hit : kv.second ){
                TVector3 vec(hit->getX(), hit->getY(), hit->getZ() );
                if ( vec.Phi() < phi_min || vec.Phi() > phi_max ) continue;

                // now add the hits to the sliced map
                outputMap[kv.first].push_back( hit );
                n_hits_kept ++;
            } // loop on hits
        } // loop on map
        return n_hits_kept;
    } // sliceHitMapInPhi

    /** doSeedFindingOnHitmapSubset
     * @brief Does track finding steps on a subset of hits (phi slice)
     * @param iIteration: tracking iteration (for determining params)
     * @param hitmap: the hitmap to use, should already be subset of original
     * @returns a list of track seeds
     */
    vector<Seed_t> doSeedFindingOnHitmapSubset( size_t iIteration, FwdDataSource::HitMap_t &hitmap  ) {
        long long itStart = FwdTrackerUtils::nowNanoSecond();

        std::vector<Seed_t> acceptedTrackSeeds;
        /*************************************************************/
        // Step 2
        // build 2-hit segments (setup parent child relationships)
        /*************************************************************/
        // Initialize the segment builder with sorted hits
        KiTrack::SegmentBuilder builder(hitmap);

        // Load the criteria used for 2-hit segments
        // This loads from XML config if available
        std::string criteriaPath = "TrackFinder.Iteration[" + std::to_string(iIteration) + "].SegmentBuilder";

        if (false == mConfig.exists(criteriaPath)) {
            // Use the default for all iterations if it is given.
            // If not then no criteria will be applied
            criteriaPath = "TrackFinder.SegmentBuilder";
        }

        clearCriteria( mTwoHitCrit );
        mTwoHitCrit = loadCriteria(criteriaPath);
        builder.addCriteria(mTwoHitCrit);

        // Setup the connector (this tells it how to connect hits together into segments)
        std::string connPath = "TrackFinder.Iteration[" + std::to_string(iIteration) + "].Connector";

        if (false == mConfig.exists(connPath))
            connPath = "TrackFinder.Connector";

        unsigned int distance = mConfig.get<unsigned int>(connPath + ":distance", 1);
        if (mSeedSource == kFttSeed){
            distance = 2; // set distance to 2 for FTT
        }

        FwdConnector connector(distance);
        builder.addSectorConnector(&connector);
        LOG_DEBUG << "Connector added: " << endm;
        // Get the segments and return an automaton object for further work

        KiTrack::Automaton automaton = builder.get1SegAutomaton();
        LOG_DEBUG << TString::Format( "nSegments=%lu", automaton.getSegments().size() ).Data() << endm;
        LOG_DEBUG << TString::Format( "nConnections=%u", automaton.getNumberOfConnections() ).Data() << endm;

        if (automaton.getNumberOfConnections() > 9000 ){
            LOG_ERROR << "Got too many connections, bailing out of tracking" << endm;
            return acceptedTrackSeeds;
        }

        // at any point we can get a list of tracks out like this:
        // std::vector < std::vector< KiTrack::IHit* > > tracks = automaton.getTracks();
        // we can apply an optional parameter <nHits> to only get tracks with >=nHits in them

        long long duration = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        mEventStats.mStep2Duration.push_back(duration);
        itStart = FwdTrackerUtils::nowNanoSecond();

        /*************************************************************/
        // Step 3
        // build 3-hit segments from the 2-hit segments
        /*************************************************************/
        automaton.clearCriteria();
        automaton.resetStates();
        criteriaPath = "TrackFinder.Iteration[" + std::to_string(iIteration) + "].ThreeHitSegments";

        if (false == mConfig.exists(criteriaPath))
            criteriaPath = "TrackFinder.ThreeHitSegments";

        clearCriteria( mThreeHitCrit );
        mThreeHitCrit = loadCriteria(criteriaPath);
        automaton.addCriteria(mThreeHitCrit);

        duration = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        mEventStats.mStep3Duration.push_back( duration );
        if (duration > 2000 || automaton.getNumberOfConnections() > 9000){
            LOG_WARN << "The Three Hit Criteria took more than 200ms to process, duration: " << duration << " ms" << endm;
            LOG_WARN << "bailing out (skipping subset HNN)" << endm;
            
            std::string subsetPath = "TrackFinder.Iteration[" + std::to_string(iIteration) + "].SubsetNN";
            size_t minHitsOnTrack = mConfig.get<size_t>(subsetPath + ":min-hits-on-track", FwdSystem::sNFttLayers);
            acceptedTrackSeeds = automaton.getTracks(minHitsOnTrack);
            return acceptedTrackSeeds;
        }
        itStart = FwdTrackerUtils::nowNanoSecond();

        LOG_DEBUG << TString::Format( "nSegments=%lu", automaton.getSegments().size() ).Data() << endm;
        LOG_DEBUG << TString::Format( "nConnections=%u", automaton.getNumberOfConnections() ).Data() << endm;

        /*************************************************************/
        // Step 4
        // Get the tracks from the possible tracks that are the best subset
        /*************************************************************/
        std::string subsetPath = "TrackFinder.Iteration[" + std::to_string(iIteration) + "].SubsetNN";

        if (false == mConfig.exists(subsetPath))
            subsetPath = "TrackFinder.SubsetNN";

        //  only for debug really
        bool findSubsets = mConfig.get<bool>(subsetPath + ":active", true);

        if (findSubsets) {
            size_t minHitsOnTrack = mConfig.get<size_t>(subsetPath + ":min-hits-on-track", 7);
            // Getting all tracks with at least minHitsOnTrack hits on them
            std::vector<Seed_t> tracks = automaton.getTracks(minHitsOnTrack);

            float omega = mConfig.get<float>(subsetPath + ".Omega", 0.75);
            float stableThreshold = mConfig.get<float>(subsetPath + ".StableThreshold", 0.1);
            float Ti = mConfig.get<float>(subsetPath + ".InitialTemp", 2.1);
            float Tf = mConfig.get<float>(subsetPath + ".InfTemp", 0.1);

            KiTrack::SubsetHopfieldNN<Seed_t> subset;
            subset.add(tracks);
            subset.setOmega(omega);
            subset.setLimitForStable(stableThreshold);
            subset.setTStart(Ti);
            subset.setTInf(Tf);

            SeedCompare comparer;
            SeedQual quality;

            subset.calculateBestSet(comparer, quality);

            acceptedTrackSeeds = subset.getAccepted();

            // this call takes a long time due to possible huge combinatorics.
            // rejectedTracks = subset.getRejected();
            // LOG_DEBUG << "We had " << tracks.size() << " tracks. Accepted = " << acceptedTrackSeeds.size() << ", Rejected = " << rejectedTracks.size() << endm;

        } else { // the subset and hit removal
            size_t minHitsOnTrack = mConfig.get<size_t>(subsetPath + ":min-hits-on-track", FwdSystem::sNFttLayers);
            acceptedTrackSeeds = automaton.getTracks(minHitsOnTrack);
        }// subset off

        duration = (FwdTrackerUtils::nowNanoSecond() - itStart) * 1e-6; // milliseconds
        mEventStats.mStep4Duration.push_back( duration );
        if (duration > 500){
            LOG_WARN << "The took more than 500ms to process, duration: " << duration << " ms" << endm;
            LOG_WARN << "We got " << acceptedTrackSeeds.size() << " tracks this round" << endm;
        }
        LOG_DEBUG << "We got " << acceptedTrackSeeds.size() << " tracks this round" << endm;
        return acceptedTrackSeeds;
    } // doSeedFindingOnHitmapSubset

    /**
     * @brief Main tracking procedure
     *
     * @param iIteration : The track iteration
     * @param hitmap : the hitmap of available hits per plane
     */
    void doSeedFindingIteration(size_t iIteration, FwdDataSource::HitMap_t &hitmap) {

        // empty the list of reco tracks for the iteration
        mTrackSeedsThisIteration.clear();

        // check to see if we have hits!
        size_t nHitsThisIteration = nHitsInHitMap(hitmap);

        const int minHitsToConsider = 3;
        if (nHitsThisIteration < minHitsToConsider) {
            // No hits left in the hitmap! Skipping this iteration
            LOG_INFO << "No hits to consider in this iteration, skipping" << endm;
            return;
        }

        std::string pslPath = "TrackFinder.Iteration["+ std::to_string(iIteration) + "]:nPhiSlices";
        if ( false == mConfig.exists( pslPath ) ) pslPath = "TrackFinder:nPhiSlices";
        size_t phi_slice_count = mConfig.get<size_t>( pslPath, 1 );

        if ( phi_slice_count <= 1 || phi_slice_count > 100 ) { // no phi slicing!
            LOG_DEBUG << "Tracking without phi slices" << endm;
            /*************************************************************/
            // Steps 2 - 4 here
            /*************************************************************/
            auto acceptedTracks = doSeedFindingOnHitmapSubset( iIteration, hitmap );
            mTrackSeedsThisIteration.insert( mTrackSeedsThisIteration.end(), acceptedTracks.begin(), acceptedTracks.end() );
        } else {

            FwdDataSource::HitMap_t slicedHitMap;

            if ( phi_slice_count == 0 || phi_slice_count > 100 ){
                LOG_WARN << "Invalid phi_slice_count = " << phi_slice_count << ", resetting to 1" << endm;
                phi_slice_count= 1;
            }
            float phi_slice = 2 * TMath::Pi() / (float) phi_slice_count;
            for ( size_t phi_slice_index = 0; phi_slice_index < phi_slice_count; phi_slice_index++ ){

                float phi_min = phi_slice_index * phi_slice - TMath::Pi();
                float phi_max = (phi_slice_index + 1) * phi_slice - TMath::Pi();
                LOG_INFO << TString::Format( "phi slice = (%f, %f)", phi_min, phi_max ) << endm;

                /*************************************************************/
                // Step 1
                // Slice the hitmap into a phi section if needed
                // If we do that, check again that we arent wasting time on empty sections
                /*************************************************************/
                size_t nHitsThisSlice = 0;
                if ( phi_slice_count > 1 ){
                    nHitsThisSlice = sliceHitMapInPhi( hitmap, slicedHitMap, phi_min, phi_max );
                    if ( nHitsThisSlice < minHitsToConsider ) {
                        continue;
                    }
                } else { // no need to slice
                    // I think this incurs a copy, maybe we can find a way to avoid.
                    slicedHitMap = hitmap;
                }

                /*************************************************************/
                // Steps 2 - 4 here
                /*************************************************************/
                auto acceptedTracks = doSeedFindingOnHitmapSubset( iIteration, slicedHitMap );
                mTrackSeedsThisIteration.insert( mTrackSeedsThisIteration.end(), acceptedTracks.begin(), acceptedTracks.end() );
            } //loop on phi slices
        }// if loop on phi slices

        /*************************************************************/
        // Step 5
        // Remove the hits from any track that was found
        /*************************************************************/
        std::string hrmPath = "TrackFinder.Iteration["+ std::to_string(iIteration) + "].HitRemover";
        if ( false == mConfig.exists( hrmPath ) ) hrmPath = "TrackFinder.HitRemover";

        if ( true == mConfig.get<bool>( hrmPath + ":active", true ) ){
            removeHits( hitmap, mTrackSeedsThisIteration );
        }

        LOG_DEBUG << " Found " << mTrackSeedsThisIteration.size() << " seed tracks this iteration" << endm;
        // Add the set of all accepted tracks (this iteration) to our collection of found tracks from all iterations
        mTrackSeeds.insert( mTrackSeeds.end(), mTrackSeedsThisIteration.begin(), mTrackSeedsThisIteration.end() );
    } // doSeedFindingIteration

    /**
     * @brief Adds compatible FST hits to tracks seeded with FTT
     *
     */
    void addFstHitsMc() {
        FwdDataSource::HitMap_t hitmap = mDataSource->getFstHits();

        for (size_t i = 0; i < mTrackResults.size(); i++) {
            GenfitTrackResult &gtr = mTrackResults[i];

            if ( gtr.mStatus->isFitConverged() == false || gtr.mMomentum.Perp() < 1e-3) {
                LOG_DEBUG << "Skipping addFstHitsMc, fit failed" << endm;
                return;
            }
            mEventStats.mPossibleReFit++;
            
            Seed_t fst_hits_for_this_track(3, nullptr);
            Seed_t fst_hits_for_this_track_nnull;

            for (size_t j = 0; j < 3; j++) {
                for (auto h0 : hitmap[j]) {
                    if (dynamic_cast<FwdHit *>(h0)->_tid == gtr.mTrack->getMcTrackId()) {
                        fst_hits_for_this_track[j] = h0;
                        fst_hits_for_this_track_nnull.push_back(h0);
                        break;
                    }
                } // loop on hits in this layer of hitmap
            }     // loop on hitmap layers

            size_t nFstHitsFound = 0;
            if ( fst_hits_for_this_track[0] != nullptr ) nFstHitsFound++;
            if ( fst_hits_for_this_track[1] != nullptr ) nFstHitsFound++;
            if ( fst_hits_for_this_track[2] != nullptr ) nFstHitsFound++;
            LOG_DEBUG << "Found " << nFstHitsFound << " FST Hits on this track (MC lookup)" << endm;

            if (nFstHitsFound >= 1) {
                mEventStats.mAttemptedReFits++;

                Seed_t combinedSeed;
                LOG_DEBUG << "Found " << gtr.mSeed.size() << " existing seed points" << endm;
                LOG_DEBUG << "Adding " << fst_hits_for_this_track_nnull.size() << " new FST seed points" << endm;
                combinedSeed.insert( combinedSeed.begin(), fst_hits_for_this_track_nnull.begin(), fst_hits_for_this_track_nnull.end() );
                combinedSeed.insert( combinedSeed.end(), gtr.mSeed.begin(), gtr.mSeed.end() );

                double vertex[3] = { mEventVertex.X(), mEventVertex.Y(), mEventVertex.Z() };

                if ( fabs(gtr.mMomentum.Z()) > 10000 ){
                    // this seems to help some fits perform better
                    gtr.mMomentum.SetXYZ( gtr.mMomentum.X(), gtr.mMomentum.Y(), 1000 );
                }
                LOG_DEBUG << "Using previous fit momentum as the seed: " << TString::Format( "(pX=%f, pY=%f, pZ=%f)", gtr.mMomentum.X(), gtr.mMomentum.Y(), gtr.mMomentum.Z() ) << endm;

                // mTrackFitter->fitTrack(combinedSeed, vertex, &(gtr.mMomentum) );
            } // we have 3 Si hits to refit with
        } // loop on the tracks
    } // addFstHitsMc

    /**
     * @brief Adds compatible FTT hits to tracks seeded with FST
     * 
     * @param gtr : The GenfitTrackResult to add FTT hits to
     * @param disk : The FTT disk number
     * @return Seed_t : The combined seed points
     */
    void addFttHits( GenfitTrackResult &gtr, size_t disk ) {
        FwdDataSource::HitMap_t hitmap = mDataSource->getFttHits();
        if ( disk > 3 ) {
            LOG_WARN << "Invalid FTT disk number: " << disk << ", cannot add Ftt points to track" << endm;
            return;
        }
        if (gtr.mIsFitConverged != true) 
            return;
        mEventStats.mPossibleReFit++;

        Seed_t hits_near_plane;
        try {
            auto msp = mTrackFitter->projectToFtt(disk, gtr.mTrack);

            // now look for Ftt hits near the specified state 
            hits_near_plane = findFttHitsNearProjectedState(hitmap[disk], msp);
            LOG_DEBUG << " Found #FTT hits on plane #" << disk << TString::Format( " = [%ld]", hits_near_plane.size() ) << endm;
        } catch (genfit::Exception &e) {
            // Failed to project
            LOG_WARN << "Unable to get Ftt projections: " << e.what() << endm;
        }

        LOG_DEBUG << "Found " << gtr.mSeed.size() << " existing seed points" << endm;

        if ( hits_near_plane.size() > 0 ){
            mEventStats.mAttemptedReFits++;
            LOG_DEBUG << "Adding " << hits_near_plane.size() << " new FTT seed points" << endm;
            gtr.mSeed.insert( gtr.mSeed.end(), hits_near_plane.begin(), hits_near_plane.end() );
        }
        return;
    } // addFttHits

    /**
     * @brief Adds compatible FTT hits using MC info
     *
     */
    void addFttHitsMc() {
        FwdDataSource::HitMap_t hitmap = mDataSource->getFttHits();


        for (size_t i = 0; i < mTrackResults.size(); i++) {
            GenfitTrackResult &gtr = mTrackResults[i];

            if ( gtr.mStatus->isFitConverged() == false || gtr.mMomentum.Perp() < 1e-6) {
                LOG_DEBUG << "Skipping addFttHitsMc on this track, fit failed" << endm;
                return;
            }

            mEventStats.mPossibleReFit++;
            

            Seed_t fttHitsForThisTrack;

            size_t nFttHitsFound = 0;
            for (size_t j = 0; j < 4; j++) {
                for (auto h0 : hitmap[j]) {
                    if (dynamic_cast<FwdHit *>(h0)->_tid == gtr.mTrack->getMcTrackId()) {
                        fttHitsForThisTrack.push_back( h0 );
                        nFttHitsFound++;
                        break;
                    }
                } // loop on hits in this layer of hitmap
            } // loop on hitmap layers

            LOG_DEBUG << "Found " << nFttHitsFound << " FTT Hits on this track (MC lookup)" << endm;

            if (nFttHitsFound >= 1) {
                mEventStats.mAttemptedReFits++;

                // Seed_t combinedSeed;
                // LOG_DEBUG << "Found " << gtr.fstSeed.size() << " existing FST seed points" << endm;
                // combinedSeed.insert( combinedSeed.begin(), gtr.fstSeed.begin(), gtr.fstSeed.end() ); // this is goofed but will fix
                // LOG_DEBUG << "Adding " << fttHitsForThisTrack.size() << " new FTT seed points" << endm;
                // combinedSeed.insert( combinedSeed.end(), fttHitsForThisTrack.begin(), fttHitsForThisTrack.end() );

                // double vertex[3] = { mEventVertex.X(), mEventVertex.Y(), mEventVertex.Z() };
                // LOG_DEBUG << "Using previous fit momentum as the seed: " << endm;
                // LOG_DEBUG << TString::Format( "(px=%f, py=%f, pz=%f)", gtr.mMomentum.Px(), gtr.mMomentum.Py(), gtr.mMomentum.Pz() ) << endm;
                // // this prevents an exception when momentum is outrageous
                // if ( fabs(gtr.mMomentum.Pz()) > 100000 )
                //     gtr.mMomentum.SetZ( 10000 );
                // LOG_DEBUG << TString::Format( "(pt=%f, eta=%f, phi=%f)", gtr.mMomentum.Pt(), gtr.mMomentum.Eta(), gtr.mMomentum.Phi() ) << endm;

                // mTrackFitter->fitTrack(combinedSeed, vertex, &(gtr.mMomentum) );

                // auto status = mTrackFitter->getTrack()->getFitStatus();
                // if ( status && status->isFitConvergedFully() ){
                //     LOG_DEBUG << "Track Refit with FTT points converged" << endm;
                //     gtr.set( fttHitsForThisTrack, mTrackFitter->getTrack() );
                //     LOG_DEBUG << "Track Refit with " << gtr.mTrack->getNumPoints() << " points" << endm;
                //     mEventStats.mGoodReFits++;
                // } else {
                //     mEventStats.mFailedReFits++;
                //     LOG_DEBUG << "Track Refit with FTT points FAILED" << endm;
                // }

            } // we have at least one Fst hit to refit with
        } // loop on the global tracks
    } // add Ftt hits via MC associations

    /**
     * @brief Adds compatible FST hits to a track
     * 
     * @param gtr : The GenfitTrackResult to add FST hits to
     * @param disk : The FST disk number
     */
    void addFstHits( GenfitTrackResult &gtr, size_t disk ) {
        FwdDataSource::HitMap_t hitmap = mDataSource->getFstHits();
        if (gtr.mIsFitConverged == false) {
            // Original Track fit did not converge, skipping
            return;
        }
        if ( disk > 2 ){
            LOG_ERROR << "Invalid FST disk number: " << disk << endm;
            return;
        }
        mEventStats.mPossibleReFit++;

        Seed_t nearby_hits;
        try {
            // get measured state on plane at specified disk
            auto msp = mTrackFitter->projectToFst(disk, gtr.mTrack);
            // now look for Si hits near this state
            nearby_hits = findFstHitsNearProjectedState(hitmap[disk], msp);
        } catch (genfit::Exception &e) {
            LOG_WARN << "Unable to get projections: " << e.what() << endm;
        }
        LOG_DEBUG << "Track already has " << gtr.mSeed.size() << " existing seed points" << endm;
        
        if ( nearby_hits.size() > 0 ){
            mEventStats.mAttemptedReFits++;
            LOG_DEBUG << "Adding " << nearby_hits.size() << " new FST seed points from disk " << disk << endm;
            gtr.mSeed.insert( gtr.mSeed.end(), nearby_hits.begin(), nearby_hits.end() );
        }
        return;
    } // addFstHits

    /**
     * @brief Finds FST hits near projected state
     *
     * @param available_hits : FST hits to consider
     * @param msp : measured state on plabe from existing track projection
     * @param dphi : search distance in phi
     * @param dr : search distance in r
     * @return Seed_t : compatible FST hits
     */
    Seed_t findFstHitsNearProjectedState(Seed_t &available_hits, genfit::MeasuredStateOnPlane &msp, double dphi = 0.004 * 20.5, double dr = 2.75 * 2) {
        double probe_phi = TMath::ATan2(msp.getPos().Y(), msp.getPos().X());
        double probe_r = sqrt(pow(msp.getPos().X(), 2) + pow(msp.getPos().Y(), 2));

        Seed_t found_hits;

        for (auto h : available_hits) {
            double h_phi = TMath::ATan2(h->getY(), h->getX());
            double h_r = sqrt(pow(h->getX(), 2) + pow(h->getY(), 2));
            double mdphi = fabs(h_phi - probe_phi);
            if (mdphi > 2*3.1415926)
                mdphi = mdphi - 2*3.1415926;

            if ( mdphi < dphi && fabs( h_r - probe_r ) < dr) { // handle 2pi edge
                found_hits.push_back(h);
            }
        }

        return found_hits;
    } // findFstHitsNearProjectedState

    /**
     * @brief Finds FTT hits near projected state
     *
     * @param available_hits : FTT hits to consider
     * @param msp : measured state on plane from existing track fit projection
     * @param dx : search distance in x
     * @param dy : search distance in y
     *
     * @return compatible FTT hits
    */
    Seed_t findFttHitsNearProjectedState(Seed_t &available_hits, genfit::MeasuredStateOnPlane &msp, double dx = 1.5, double dy = 1.5) {

        Seed_t found_hits;
        double probe_phi = TMath::ATan2(msp.getPos().Y(), msp.getPos().X());
        double probe_r = sqrt(pow(msp.getPos().X(), 2) + pow(msp.getPos().Y(), 2));

        TLorentzVector lv1, lv2;
        lv1.SetPxPyPzE( msp.getPos().X(), msp.getPos().Y(), 0, 1 );

        double mindx = 99;
        double mindy = 99;
        double mindr = 99;
        double mindp = 99;
        KiTrack::IHit *closest = nullptr;

        for (auto h : available_hits) {

            lv2.SetPxPyPzE( h->getX(), h->getY(), 0, 1 );
            double sr = lv1.Pt() - lv2.Pt();
            double sp =  lv1.DeltaPhi( lv2 );
            double sx = h->getX() - msp.getPos().X();
            double sy = h->getY() - msp.getPos().Y();

            if ( fabs(sr) < fabs(mindr) )
                mindr = sr;
            if ( fabs(sp) < fabs(mindp) ){
                mindp = sp;
                closest = h;
            }
            if ( fabs(sx) < fabs(mindx) )
                mindx = sx;
            if ( fabs(sy) < fabs(mindy) )
                mindy = sy;

        } // loop h

        if (  fabs(mindp) < 0.04*5 && fabs(mindr) < 9 ) {
            found_hits.push_back(closest);
        }

        return found_hits;
    } // findFttHitsNearProjectedState

    bool getSaveCriteriaValues() { return mSaveCriteriaValues; }
    std::vector<KiTrack::ICriterion *> getTwoHitCriteria() { return mTwoHitCrit; }
    std::vector<KiTrack::ICriterion *> getThreeHitCriteria() { return mThreeHitCrit; }

    TrackFitter *getTrackFitter() { return mTrackFitter; }
    void setEventVertex( TVector3 v ) { mEventVertex = v; }
    TVector3 getEventVertex() { return mEventVertex; }

  protected:
    unsigned long long int nEvents;

    bool mDoTrackFitting = true;
    bool mSaveCriteriaValues = false;
    enum SeedSource { kFstSeed = 0, kFttSeed, kSimSeed, kSeqSeed };
    int mSeedSource = 0; // 0 = FST, 1 = FTT, 2 = FST+FTT simultaneous, 3 = FST+FTT sequential

    FwdTrackerConfig mConfig;
    std::string mConfigFile;
    size_t mTotalHitsRemoved;

    std::vector<GenfitTrackResult> mTrackResults;

    std::vector<Seed_t> mTrackSeeds; // the tracks recod from all iterations
    std::vector<Seed_t> mTrackSeedsThisIteration;

    // Metrics about the event
    EventStats mEventStats;

    // Set to the Primary vertex for the event
    TVector3 mEventVertex;


    std::shared_ptr<FwdDataSource> mDataSource;

    TrackFitter *mTrackFitter = nullptr;

    std::vector<KiTrack::ICriterion *> mTwoHitCrit;
    std::vector<KiTrack::ICriterion *> mThreeHitCrit;

    // histograms of the raw input data
    TString mGeoCache;
};

#endif
