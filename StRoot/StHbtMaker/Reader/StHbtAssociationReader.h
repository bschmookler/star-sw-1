#ifndef StHbtAssociationReader_hh
#define StHbtAssociationReader_hh

#include <ctime>
#include "StMaker.h"
#include "StHbtMaker/Base/StHbtEventReader.hh"
#include "StV0MiniDstMaker/StV0MiniDstMaker.h"
#include "StHbtMaker/Base/StHbtEventCut.hh"
#include "StHbtMaker/Base/StHbtTrackCut.hh"

class StHbtAssociationReader : public StHbtEventReader{
  
 private:
  // pointers to other mkers
  StMaker* mTheEventMaker;        //! this is the chain where the StEventReaderMaker is
  StMaker* mTheMcEventMaker;      //! this is the chain where the StMcEventReaderMaker is
  StMaker* mTheAssociationMaker;  //! this is the chain where the StAssociationEventReaderMaker is
  StV0MiniDstMaker* mTheV0Maker;  //! this is the chain where the StV0MiniDstMaker is

  // pointers to front-loaded cuts
  StHbtEventCut* mEventCut;       //!
  StHbtTrackCut* mTrackCut; //!

  // some monitor histograms
  StHbt1DHisto* mDiffCurrent;   //! momenta diff distribution
  StHbt1DHisto* mDiff;          //! momenta diff distribution
  StHbt1DHisto* mDiffMean;      //! mean of momenta diff distribution
  StHbt1DHisto* mDiffRMS;       //! sigma of momenta diff distribution
  StHbt2DHisto* mDiffEvents;    //! sigma of momenta diff distribution

  long              mV0;        //! Number of v0s looked at to date
  int eventNumber;
  time_t timeStamp;
  
 protected:
  TOrdCollection *mCollection; //!

 public:

  StHbtAssociationReader();
  ~StHbtAssociationReader();
  
  virtual StHbtEvent* ReturnHbtEvent();
  virtual StHbtString Report();

  // sets and gets for the other makers
  void SetTheEventMaker(StMaker*);
  void SetTheMcEventMaker(StMaker*);
  void SetTheAssociationMaker(StMaker*);
  void SetTheV0Maker(StV0MiniDstMaker*);
  StMaker* TheEventMaker();
  StMaker* TheMcEventMaker();
  StMaker* TheAssociationMaker();
  StV0MiniDstMaker* TheV0Maker();

  // sets and gets for the front-loaded cuts
  void SetEventCut(StHbtEventCut*);          // use these methods to do
  void SetTrackCut(StHbtTrackCut*);    // "front-loaded" cuts

  ClassDef(StHbtAssociationReader, 0)
};
    
inline void StHbtAssociationReader::SetTheEventMaker(StMaker* maker){mTheEventMaker=maker;}
inline void StHbtAssociationReader::SetTheMcEventMaker(StMaker* mcMaker){mTheMcEventMaker=mcMaker;}
inline void StHbtAssociationReader::SetTheAssociationMaker(StMaker* associationMaker){mTheAssociationMaker=associationMaker;}
inline StMaker* StHbtAssociationReader::TheEventMaker(){return mTheEventMaker;}
inline StMaker* StHbtAssociationReader::TheMcEventMaker(){return mTheMcEventMaker;}
inline StMaker* StHbtAssociationReader::TheAssociationMaker(){return mTheAssociationMaker;}
inline StV0MiniDstMaker* StHbtAssociationReader::TheV0Maker(){return mTheV0Maker;}

inline void StHbtAssociationReader::SetEventCut(StHbtEventCut* ecut){mEventCut=ecut;}
inline void StHbtAssociationReader::SetTrackCut(StHbtTrackCut* pcut){mTrackCut=pcut;}
 
#endif
