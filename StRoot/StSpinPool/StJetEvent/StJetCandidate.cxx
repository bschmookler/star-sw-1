//
// Pibero Djawotho <pibero@tamu.edu>
// Texas A&M University
// 3 September 2009
//

#include "StJetTrack.h"
#include "StJetTower.h"
#include "StJetCandidate.h"

ClassImp(StJetCandidate);

StJetCandidate::StJetCandidate(const TVector3& vertex, double pt, double eta, double phi, double E)
  : mPt(pt)
  , mEta(eta)
  , mPhi(phi)
  , mE(E)
{
  // Front plate of BEMC towers or BPRS layer (See StEmcGeom/geometry/StEmcGeom.cxx)
  // This only works for BEMC jets.
  static const double BEMC_RADIUS = 225.405;

  TVector3 pos = momentum();
  pos.SetPerp(BEMC_RADIUS);
  pos += vertex;
  mDetEta = pos.Eta();
}

StJetTrack* StJetCandidate::leadingChargedParticle() const
{
  StJetTrack* lcp = 0;
  for (int i = 0; i < numberOfTracks(); ++i) {
    StJetTrack* t = track(i);
    if (!lcp || t->pt() > lcp->pt()) lcp = t;
  }
  return lcp;
}

double StJetCandidate::sumTrackPt() const
{
  double sumPt = 0;
  for (int i = 0; i < numberOfTracks(); ++i) sumPt += track(i)->pt();
  return sumPt;
}

double StJetCandidate::sumTowerPt() const
{
  double sumPt = 0;
  for (int i = 0; i < numberOfTowers(); ++i) sumPt += tower(i)->pt();
  return sumPt;
}
