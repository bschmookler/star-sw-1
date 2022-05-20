// \class StFcsTrackMatchMaker
// \author Akio Ogawa
//
//  $Id: StFcsTrackMatchMaker.cxx,v 1.1 2021/03/30 13:34:15 akio Exp $
//  $Log: StFcsTrackMatchMaker.cxx,v $

#include "StFcsTrackMatchMaker.h"
#include "StEnumerations.h"
#include "StMessMgr.h"
//#include "StEventTypes.h"
#include "StEvent/StEvent.h"
#include "StEvent/StFcsCollection.h"
#include "StEvent/StFcsCluster.h"
#include "StEvent/StFwdTrackCollection.h"
#include "StEvent/StFwdTrack.h"

#include "StThreeVectorF.hh"
#include "StFcsDbMaker/StFcsDb.h"
#include "StRoot/StEpdUtil/StEpdGeom.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

ClassImp(StFcsTrackMatchMaker)

StFcsTrackMatchMaker::StFcsTrackMatchMaker(const char* name): StMaker(name) {}

StFcsTrackMatchMaker::~StFcsTrackMatchMaker(){}

int StFcsTrackMatchMaker::Init(){  
  mFcsDb=static_cast<StFcsDb*>(GetDataSet("fcsDb"));  
  if(!mFcsDb){
    LOG_ERROR  << "StFcsTrackMatchMaker::InitRun Failed to get StFcsDb" << endm;
    return kStFatal;
  }
  mEpdgeo=new StEpdGeom;
  if(mFilename){
    LOG_INFO << "Opening " << mFilename << endm;
    mFile = new TFile(mFilename,"RECREATE");
    mHdx[0] = new TH1F("dx_EcalTrk","dx Ecal-Track",100,-50,50);
    mHdy[0] = new TH1F("dy_EcalTrk","dy Ecal-Track",100,-50,50);
    mHdr[0] = new TH1F("dr_EcalTrk","dr Ecal-Track",100,0,20);
    mNtrk[0]= new TH1F("NTrk_Ecal","NTrk_Ecal",10,0.0,10.0);
    mNclu[0]= new TH1F("NEcalClu_Trk","NEcalClu_Trk",10,0.0,10.0);
    mHdx[1] = new TH1F("dx_HcalTrk","dx Hcal-Track",100,-50,50);
    mHdy[1] = new TH1F("dy_HcalTrk","dy Hcal-Track",100,-50,50);	
    mHdr[1] = new TH1F("dr_HcalTrk","dr Ecal-Track",100,0,20);
    mNtrk[1]= new TH1F("NTrk_Hcal","NTrk_Hcal",10,0.0,10.0);
    mNclu[1]= new TH1F("NHcalClu_Trk","NHcalClu_Trk",10,0.0,10.0);
    mPtEt[0] =new TH1F("ETovPT_E","ETovPT_E",50,0.0,2.0);
    mPtEt[1] =new TH1F("ETovPT_EH","ETovPT_E+H",50,0.0,2.0);
    mPtEt2[0]=new TH2F("ETPT_E", "ETovPT_E; ET(Ecal); TrkPT",50,0.0,8.0,50,0.0,8.0);
    mPtEt2[1]=new TH2F("ETPT_EH","ETovPT_E+H ET(E+H); TrkPT",50,0.0,8.0,50,0.0,8.0);
    mCharge[0]=new TH1F("Charge_E","Charge_E",3,-1.0,2.0);
    mCharge[1]=new TH1F("Charge_EH","Charge_E+H",3,-1.0,2.0);
  }
  return kStOK;
}

int StFcsTrackMatchMaker::Finish(){
  if(mFile){
    LOG_INFO << "Closing "<<mFilename<<endm;
    mFile->Write();
    mFile->Close();
  }
  return kStOK;
}

int StFcsTrackMatchMaker::Make(){
  StEvent* event = (StEvent*)GetInputDS("StEvent");
  if(!event) {
    LOG_ERROR << "StFcsTrackMatchMaker::Make did not find StEvent"<<endm; 
    return kStErr;
  }
  mFcsColl = event->fcsCollection();
  if(!mFcsColl) {
    LOG_ERROR << "StFcsTrackMatchMaker::Make did not find StEvent->StFcsCollection"<<endm; 
    return kStErr;
  }
  mFwdTrkColl = event->fwdTrackCollection();
  if(!mFwdTrkColl) {
    LOG_ERROR << "StFcsTrackMatchMaker::Make did not find StEvent->fwdTrackCollection"<<endm; 
    return kStErr;
  }
    
  int ntrk=mFwdTrkColl->numberOfTracks();
  int nMatch[2]={0,0};
  for(int itrk=0; itrk<ntrk; itrk++){
    StFwdTrack* trk=mFwdTrkColl->tracks()[itrk];
    if(Debug()){
      LOG_INFO << Form("Proj0 : %6.2f %6.2f %6.2f",trk->mProjections[0].XYZ.x(),trk->mProjections[0].XYZ.y(),trk->mProjections[0].XYZ.z())<<endm;
      LOG_INFO << Form("Proj1 : %6.2f %6.2f %6.2f",trk->mProjections[1].XYZ.x(),trk->mProjections[1].XYZ.y(),trk->mProjections[1].XYZ.z())<<endm;
      LOG_INFO << Form("Proj2 : %6.2f %6.2f %6.2f",trk->mProjections[2].XYZ.x(),trk->mProjections[2].XYZ.y(),trk->mProjections[2].XYZ.z())<<endm;
    }
    //North or south from track
    int ns=0;
    if(trk->mProjections[1].XYZ.x()>0.0) ns=1;
    //Look for a Ecal & Hcal match for a track
    for(int ehp=0; ehp<2; ehp++){
      StThreeVectorD proj = trk->mProjections[ehp].XYZ;
      int det = ns*2+ehp;
      int nclu  = mFcsColl->numberOfClusters(det);
      for(int iclu=0; iclu<nclu; iclu++){
	StFcsCluster* clu=mFcsColl->clusters(det)[iclu];
	float energy=clu->energy();
	if(energy>mMinEnergy){
	  StThreeVectorD xyz=mFcsDb->getStarXYZfromColumnRow(det,clu->x(),clu->y());
	  double dx = xyz.x() - proj.x();
	  double dy = xyz.y() - proj.y();
	  double dr = sqrt(dx*dx + dy*dy);
	  if(Debug()) LOG_INFO << Form("EHP=%1d dx = %6.2f - %6.2f  = %6.2f dy = %6.2f - %6.2f  = %6.2f dr=%6.2f",
				       ehp,xyz.x(),proj.x(),dx,xyz.y(),proj.y(),dy,dr) << endm;
	  if(mFile){
	    if(fabs(dy)<mMaxDistance) mHdx[ehp]->Fill(dx);
	    if(fabs(dx)<mMaxDistance) mHdy[ehp]->Fill(dy);
	    mHdr[ehp]->Fill(dr);
	  }
	  if(dr<mMaxDistance){
	    if(ehp==0){trk->addEcalCluster(clu);}
	    if(ehp==1){trk->addHcalCluster(clu);}
	    clu->addTrack(trk);
	    nMatch[ehp]++;
	  }
	}
      }
    }
  }
  
  //Sort by ET/PT 
  for(int itrk=0; itrk<ntrk; itrk++){
    StFwdTrack* trk=mFwdTrkColl->tracks()[itrk];
    trk->sortEcalClusterByET();
    trk->sortHcalClusterByET();
    if(Debug()){
      LOG_INFO << Form("TRK pT=%6.2f Cg=%1d NEcal=%1d NHcal=%1d",
		       trk->momentum().perp(),trk->charge(),
		       trk->ecalClusters().size(),
		       trk->hcalClusters().size())<<endm;
    }
  }
  for(int det=0; det<4; det++){
    int nclu  = mFcsColl->numberOfClusters(det);
    for(int iclu=0; iclu<nclu; iclu++){
      StFcsCluster* clu=mFcsColl->clusters(det)[iclu];
      clu->sortTrackByPT();
      if(Debug()){
	if(clu->energy() > mMinEnergy){
	  LOG_INFO << Form("FCS DET=%d ET=%6.2f NTrk=%1d",
			   clu->detectorId(), clu->fourMomentum().perp(), clu->tracks().size()) << endm;
	}
      }
    }
  }
  
  //Filling hitograms if file is specified
  if(mFile){
    for(int itrk=0; itrk<ntrk; itrk++){
      StFwdTrack* trk=mFwdTrkColl->tracks()[itrk];
      double pt=trk->momentum().perp();
      int ne=trk->ecalClusters().size();
      int nh=trk->hcalClusters().size();
      mNclu[0]->Fill(double(ne));
      mNclu[1]->Fill(double(nh));
      double ete=0;
      if(ne>0){
	StFcsCluster* eclu=trk->ecalClusters()[0]; //Take top ET ones 
	ete=eclu->fourMomentum().perp();
	mPtEt[0]->Fill(ete/pt);
	mPtEt2[0]->Fill(ete,pt);
	mCharge[0]->Fill(trk->charge());
      }
      if(nh>0){
	StFcsCluster* hclu=trk->hcalClusters()[0]; //Take top ET ones 
	double eth=hclu->fourMomentum().perp();
	mPtEt[0]->Fill((ete+eth)/pt);
	mPtEt2[0]->Fill(ete+eth,pt);
	mCharge[1]->Fill(trk->charge());
      }
    }
    for(int det=0; det<4; det++){
      int ehp=det/2;
      int nclu  = mFcsColl->numberOfClusters(det);
      for(int iclu=0; iclu<nclu; iclu++){
	StFcsCluster* clu=mFcsColl->clusters(det)[iclu];
	if(clu->energy() > mMinEnergy){
	  mNtrk[ehp]->Fill(clu->tracks().size());
	}
      }
    }
    LOG_INFO << Form("NTrack=%3d NEcalMatch=%3d NHcalMatch=%3d",ntrk,nMatch[0],nMatch[1])<<endm;
  }    
  return kStOK;
}
