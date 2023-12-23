#ifndef ST_FWD_TRACK_MAKER_H
#define ST_FWD_TRACK_MAKER_H

#include "StChain/StMaker.h"

#ifndef __CINT__
#include "GenFit/Track.h"
#endif

#include "FwdTrackerConfig.h"
#include "TVector3.h"

namespace KiTrack {
class IHit;
};

namespace genfit {
  class Track;
  class GFRaveVertex;
}

class ForwardTracker;
class ForwardTrackMaker;
class FwdDataSource;
class FwdHit;
class StarFieldAdaptor;

class StGlobalTrack;
class StRnDHitCollection;
class StTrack;
class StTrackDetectorInfo;
class SiRasterizer;
class McTrack;

// ROOT includes
#include "TNtuple.h"
#include "TTree.h"
// STL includes
#include <vector>
#include <memory>


// 877-369-6347
class StFwdTrack;
class GenfitTrackResult;



const size_t MAX_TREE_ELEMENTS = 4000;
struct FwdTreeData {
  
    /**@brief Ftt hit related info*/
    int fttN;
    vector<float> fttX, fttY, fttZ;
    vector<int> fttVolumeId;
    // Note: Below are only avalaible for hits if MC
    vector<float> fttPt;
    vector<int> fttTrackId, fttVertexId;

    /**@brief Fst hit related info*/
    int fstN;
    vector<float> fstX, fstY, fstZ;
    vector<int> fstTrackId;

    /**@brief Fcs hit related info*/
    int fcsN;
    vector<float> fcsX, fcsY, fcsZ, fcsE;
    vector<int> fcsDet;

    /**@brief RC track related info*/
    int rcN;
    vector<float> rcPt, rcEta, rcPhi, rcQuality;
    vector<int> rcTrackId, rcNumFST, rcCharge, rcNumFTT, rcNumPV;

    /**@brief MC Track related info*/
    int mcN;
    vector<float> mcPt, mcEta, mcPhi;
    vector<int> mcVertexId, mcCharge;
    vector<int> mcNumFtt, mcNumFst;

    /**@brief MC Vertex related info*/
    int vmcN;
    vector<float> vmcX, vmcY, vmcZ;

    /**@brief Track Projection related info*/
    int tprojN;
    vector<float> tprojX, tprojY, tprojZ;
    vector<float> tprojPx, tprojPy, tprojPz;
    vector<int> tprojIdD, tprojIdT;

    /**@brief RAVE RC Vertex related info*/
    int vrcN;
    vector<float> vrcX, vrcY, vrcZ;

    /**@brief Track-to-hit delta related info*/
    int thdN;
    vector<float> thdX, thdY, thdP, thdR, thaX, thaY, thaZ;

    /**@brief Seed finding Criteria related info*/
    bool saveCrit = false;
    std::map<string, std::vector<float>> Crits;
    std::map<string, std::vector<int>> CritTrackIds;

};

class StFwdTrackMaker : public StMaker {

    ClassDef(StFwdTrackMaker, 0);

  public:
    StFwdTrackMaker();
    ~StFwdTrackMaker(){/* nada */};

    int Init();
    int Finish();
    int Make();
    void Clear(const Option_t *opts = "");

    enum { kInnerGeometry,
           kOuterGeometry };

    void SetConfigFile(std::string n) {
        mConfigFile = n;
    }
    void SetGenerateHistograms( bool _genHisto ){ mGenHistograms = _genHisto; }
    void SetGenerateTree(bool _genTree) { mGenTree = _genTree; }
    void SetVisualize( bool _viz ) { mVisualize = _viz; }

    vector<StFwdTrack*> mFwdTracks;

  private:
  protected:

    // Track Seed typdef 
    typedef std::vector<KiTrack::IHit *> Seed_t;

    
    // for Wavefront OBJ export
    size_t eventIndex = 0; // counts up for processed events
    size_t mEventNum = 0; // global event num (index)

    bool mGenHistograms = false;
    bool mGenTree = false;
    std::string mConfigFile;


    std::map<std::string, TH1 *> mHistograms;
    TFile *mTreeFile = nullptr;
    TTree *mTree     = nullptr;
    FwdTreeData mTreeData;

    bool mVisualize = false;
    vector<TVector3> mFttHits;
    vector<TVector3> mFstHits;
    vector<TVector3> mFcsClusters;
    vector<float> mFcsClusterEnergy;
    vector<TVector3> mFcsPreHits;

    std::vector< genfit::GFRaveVertex * > mRaveVertices;

    void ProcessFwdTracks();
    void FillEvent();
    void FillTrackDeltas();

    StFwdTrack * makeStFwdTrack( GenfitTrackResult &gtr, size_t indexTrack );

    // I could not get the library generation to succeed with these.
    // so I have removed them
    #ifndef __CINT__
        std::shared_ptr<SiRasterizer> mSiRasterizer;
        FwdTrackerConfig mFwdConfig;
        std::shared_ptr<ForwardTracker> mForwardTracker;
        std::shared_ptr<FwdDataSource> mForwardData;
        
        size_t loadMcTracks( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap );
        void loadFcs();
        void loadFttHits( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFttHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFttHitsFromGEANT( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );

        void loadFstHits( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromMuDst( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromGEANT( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromStEvent( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
        void loadFstHitsFromStEventFastSim( std::map<int, std::shared_ptr<McTrack>> &mcTrackMap, std::map<int, std::vector<KiTrack::IHit *>> &hitMap, int count = 0 );
    #endif

    void FillTTree(); // if debugging ttree is turned on (mGenTree)
    void FitVertex();

};

#endif
