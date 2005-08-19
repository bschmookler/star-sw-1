#ifndef __StMuPrimaryVertex_hh__
#define __StMuPrimaryVertex_hh__
/*
 * Simple class to store primary vertices. Data members are a mainly a copy of 
 * StPrimaryVertex
 *
 * $Id: StMuPrimaryVertex.h,v 1.2 2005/08/19 19:46:05 mvl Exp $ 
 */

#include "TObject.h"
#include "StThreeVectorF.hh"
#include "StEnumerations.h"

class StPrimaryVertex;

class StMuPrimaryVertex : public TObject {

 public:
  StMuPrimaryVertex(): mPosition(-999,-999,-999), mPosError(-999,-999,-999), mVertexFinderId(undefinedVertexFinder), mRanking(999),mNTracksUsed(0), mNCTBMatch(0), mNBEMCMatch(0), mNEEMCMatch(0), mNCrossCentralMembrane(0), mSumTrackPt(-999),mRefMultNeg(0), mRefMultPos(0), mRefMultFtpcWest(0), mRefMultFtpcEast(0) {;}
  StMuPrimaryVertex(const StPrimaryVertex*& vertex);
  ~StMuPrimaryVertex() {;}

   StThreeVectorF   position() const        { return mPosition; }
   StThreeVectorF   posError() const        { return mPosError; }
   StVertexFinderId vertexFinderId()  const { return mVertexFinderId; } 
   Float_t          ranking()  const        { return mRanking; }
   UShort_t         nTracksUsed() const     { return mNTracksUsed; }
   UShort_t         nCTBMatch() const       { return mNCTBMatch; }
   UShort_t         nBEMCMatch() const      { return mNBEMCMatch; }
   UShort_t         nEEMCMatch() const      { return mNEEMCMatch; }
   UShort_t         nCrossCentralMembrane() const  { return mNCrossCentralMembrane; }
   Float_t          sumTrackPt() const      { return mSumTrackPt; }
   
   UShort_t         refMultPos() const      { return mRefMultPos; }
   UShort_t         refMultNeg() const      { return mRefMultNeg; }
   UShort_t         refMult()    const      { return refMultPos() + refMultNeg(); }
   UShort_t         refMultFtpcEast() const { return mRefMultFtpcEast; }
   UShort_t         refMultFtpcWest() const { return mRefMultFtpcWest; }
   UShort_t         refMultFtpc()     const { return refMultFtpcEast() + refMultFtpcWest(); }
   void setPosition(const StThreeVectorF &pos)     { mPosition = pos; }
   void setPosError(const StThreeVectorF &pos_err) { mPosError = pos_err; }


  ClassDef(StMuPrimaryVertex,2)
    
    private:
  StThreeVectorF   mPosition;
  StThreeVectorF   mPosError;
  StVertexFinderId mVertexFinderId;
  Float_t          mRanking;
  UShort_t         mNTracksUsed;
  UShort_t         mNCTBMatch;
  UShort_t         mNBEMCMatch;
  UShort_t         mNEEMCMatch;
  UShort_t         mNCrossCentralMembrane;
  Float_t          mSumTrackPt;

  // RefMult fields
  UShort_t         mRefMultNeg;
  UShort_t         mRefMultPos;
  UShort_t         mRefMultFtpcWest;
  UShort_t         mRefMultFtpcEast;
};
#endif
/*
 * $Log: StMuPrimaryVertex.h,v $
 * Revision 1.2  2005/08/19 19:46:05  mvl
 * Further updates for multiple vertices. The main changes are:
 * 1) StMudst::primaryTracks() now returns a list (TObjArray*) of tracks
 *    belonging to the 'current' primary vertex. The index number of the
 *    'current' vertex can be set using StMuDst::setCurrentVertex().
 *    This also affects StMuDst::primaryTracks(int i) and
 *    StMuDst::numberOfprimaryTracks().
 * 2) refMult is now stored for all vertices, in StMuPrimaryVertex. The
 *    obvious way to access these numbers is from the StMuprimaryVertex structures,
 *    but for ebakcward compatibility a function is provided in StMuEvent as well
 *    (this is the only function taht works for existing MuDst)
 *
 * As an aside, I've also changes the internals of StMuDst::createStEvent and
 * StMuDst::fixTrackIndices() to be able to deal with a larger range of index numbers for tracks as generated by Ittf.
 *
 * BIG FAT WARNING: StMudst2StEventMaker and StMuDstFilterMaker
 * do not fully support the multiple vertex functionality yet.
 *
 * Revision 1.1  2005/07/15 21:45:09  mvl
 * Added support for multiple primary vertices (StMuPrimaryVertex). Track Dcas are now calculated with repect to the first vertex in the list (highest rank), but another vertex number can be specified. Tarcks also store the index of the vertex they belong to (StMuTrack::vertexIndex())
 *
 */
