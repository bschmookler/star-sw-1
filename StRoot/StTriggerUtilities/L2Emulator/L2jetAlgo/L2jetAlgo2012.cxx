#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fakeRtsLog.h>

/***********************************************************
 * $Id: L2jetAlgo2012.cxx,v 1.4 2012/03/21 18:18:03 jml Exp $
 * \author Jan Balewski, IUCF, 2006 
 ***********************************************************
 * Descripion:
  Reco of mono- & di-jets in L2 using BTOW+ETOW
  depends on L2-DB class 
 ***********************************************************
 * version updated 2/17/2006 W.W. Jacobs
 * 2008 to 2009 change by B.S. Page 11/12/2008
 */



#ifdef  IS_REAL_L2  //in l2-ana  environment
  #include "../L2algoUtil/L2EmcDb2012.h"
  #include "../L2algoUtil/L2Histo.h"
#else
  #include "StTriggerUtilities/L2Emulator/L2algoUtil/L2EmcDb2012.h"
  #include "StTriggerUtilities/L2Emulator/L2algoUtil/L2Histo.h"
#endif

#include "L2jetAlgo2012.h"
#include "L2jetResults2012.h" 
#include "Map_DeltaPhiJets.h"

//=================================================
//=================================================
L2jetAlgo2012::L2jetAlgo2012(const char* name, const char *uid, L2EmcDb2012* db, char* outDir, int resOff, bool writeHighResult) 
  :  L2VirtualAlgo2012( name, uid,  db,  outDir, true, true, resOff) { 
  /* called one per days
     all memory allocation must be done here
  */
  namechar='L';
  if (writeHighResult) namechar='H';

  createHisto();
  run_number=-1;
  LOG(DBG,"L2jetAlgo2012 instantiated, logPath='%s'\n",mOutDir1.c_str());
  eve_Jet[0]=new L2Jet;
  eve_Jet[1]=new L2Jet;
}

/*========================================
  ======================================== */
bool
L2jetAlgo2012::paramsChanged( int *rc_ints, float *rc_floats) {
  int i;
  for(i=0;i<5;i++)
    if(rc_ints[i]!=raw_ints[i]) goto  foundProblem;

  for(i=0;i<5;i++)
    if(fabs(rc_floats[i]-raw_floats[i])>0.00001)  goto  foundProblem;

  return false;

 foundProblem:
      if (mLogFile) fprintf(mLogFile,"L2jet-ghost initRun - inconsistent params, ABORT initialization\n");
  return true;
}

/*========================================
  ======================================== */
int 
L2jetAlgo2012::initRunUser( int runNo, int *rc_ints, float *rc_floats) {

  // clear content of all histograms
  int i;
  for (i=0; i<mxHA;i++) if(hA[i])hA[i]->reset();

  /* .... clear content, set threshold @ max as default */
  memset(db_btowL2PhiBin,  0,  sizeof(db_btowL2PhiBin));
  memset(db_btowL2PatchBin,0,  sizeof(db_btowL2PatchBin));

  memset(db_etowL2PhiBin,  0  ,sizeof(db_etowL2PhiBin));
  memset(db_etowL2PatchBin,0  ,sizeof(db_etowL2PatchBin));

  run_startUnix=time(0);
  run_number  =runNo;  // serves as a flag this run is initialized
  raw_ints    =rc_ints;
  raw_floats  =rc_floats;
  run_nEventOneJet=run_nEventDiJet= run_nEventRnd=0;

  mEventsInRun=0;

  if( mLogFile==0) LOG(ERR," L2jetAlgo2012() UNABLE to open run summary log file, continue anyhow\n");

  // unpack params from run control GUI
  par_useBtowEast= (rc_ints[0]&1 )>0;
  par_useBtowWest= (rc_ints[0]&2)>0;
  par_useEndcap  =  rc_ints[1]&1;
  int par_adcThr =  rc_ints[2]; // needed only in initRun()
  par_dbg = rc_ints[3];
  par_RndAcceptPrescale = rc_ints[4];
  par_minPhiBinDiff= 7; //for 1X1 JP was 5 for .6x.6 JP BP

  par_oneJetThr   =  rc_floats[0];
  par_diJetThrHigh=  rc_floats[1];
  par_diJetThrLow =  rc_floats[2];
  par_diJetThr_2 =   rc_floats[3];  // wwj 2/11
  par_diJetThr_3 =   rc_floats[4];  // wwj 2/11
  par_diJetThr_4 =   rc_floats[5];  // wwj 2/11
  par_diJetThr_5 =   rc_floats[6];  // wwj 2/11
  par_diJetEtaHigh =  rc_floats[7];  // wwj 2/11
  par_diJetEtaLow = rc_floats[8];  // wwj 2/11

  // set other hardcoded or calculated  params

  // to monitor hot towers  in E+B Emc
  float monTwEtThr=2.0; // (GeV) Et threshold,WARN,  edit histo title by hand
  par_hotTwEtThr = monTwEtThr;

  if(par_dbg) LOG(DBG,"Brian version is running!\n");  

  if (mLogFile) { 
    fprintf(mLogFile,"L2jet algorithm initRun(%d), compiled: %s , %s\n params:\n",run_number,__DATE__,__TIME__);
    fprintf(mLogFile," - use BTOW: East=%d West=%d, Endcap=%d  L2ResOffset=%d\n", par_useBtowEast, par_useBtowWest,par_useEndcap ,mResultOffset);
    fprintf(mLogFile," - threshold: ADC-ped> %d \n", par_adcThr);
    fprintf(mLogFile," - min phi opening angle Jet1<->Jet2: %d in L2phiBins\n",par_minPhiBinDiff);   
    fprintf(mLogFile," - diJet  Et thrHigh= %.2f (GeV)   thrLow= %.2f  (GeV)\n", par_diJetThrHigh, par_diJetThrLow); 
    fprintf(mLogFile," - new diJet Et thr_2= %.2f (GeV) ; thr_3= %.2f (Gev) ; thr_4= %.2f (GeV) ; thr_5= %.2f (GeV)\n",par_diJetThr_2, par_diJetThr_3, par_diJetThr_4, par_diJetThr_5); // wwj 2/10 
    fprintf(mLogFile," - oneJet Et thr = %.2f (GeV) ; rndAccPrescale=%d \n",par_oneJetThr,par_RndAcceptPrescale);
    fprintf(mLogFile," - new diJet Eta_low= %.2f ; diJet Eta_High= %.2f \n", par_diJetEtaLow, par_diJetEtaHigh); // wwj 2/10
    fprintf(mLogFile," - debug=%d, hot tower threshold: Et> %.1f GeV ( only monitoring)\n",par_dbg, monTwEtThr);
  }

  // verify consistency of input params
  int kBad=0;
  kBad+=0x0001 * ( !par_useBtowEast & !par_useBtowWest & !par_useEndcap);
  // par_pedOff no longer exists:  kBad+=0x0002 * (par_adcThr<par_pedOff); 
  kBad+=0x0004 * (par_adcThr>16); 
  kBad+=0x0008 * (par_minPhiBinDiff<5);
  kBad+=0x0010 * (par_minPhiBinDiff>=15);
  kBad+=0x0020 * (par_oneJetThr<3.);
  kBad+=0x0040 * (par_oneJetThr>12.);
  kBad+=0x0080 * (par_diJetThrLow<1.9);  //     LOOK: wwj 2/11  using lowest threshold 
  kBad+=0x0100 * (par_diJetThrHigh<par_diJetThrLow); 
  kBad+=0x0200 * (par_diJetThrHigh>12.); 
  //0x0400 not used
  kBad+=0x0800 * ( par_RndAcceptPrescale<0 );
  kBad+=0x1000 * (par_diJetThr_2<par_diJetThrLow || par_diJetThr_5>par_diJetThrHigh); // wwj 2/16 
  kBad+=0x2000 * (par_diJetThr_2>par_diJetThr_3 || par_diJetThr_3>par_diJetThr_4 || par_diJetThr_4>par_diJetThr_5); // wwj 2/16
  kBad+=0x4000 * (par_diJetEtaLow>par_diJetEtaHigh); // wwj 2/16
  if (mLogFile) {
    fprintf(mLogFile,"L2jet initRun() params checked for consistency, Error flag=0x%04x\n",kBad);
    if(kBad)   fprintf(mLogFile,"L2jet initRun()  ABORT\n");
  }

  if(kBad) {
    run_number=-66;
    if (mLogFile) { 
      fprintf(mLogFile,"L2jet algorithm init: crashB in internal logic\n");
      fclose(mLogFile);
      return kBad;
    }
  }

  char tit[100];
  sprintf(tit,"# BTOW towers>ped+%d (input); x: # of towers/event",par_adcThr);
  hA[47]->setTitle(tit);
  
  sprintf(tit,"# ETOW towers>ped+%d (input); x: # of towers/event",par_adcThr);
  hA[48]->setTitle(tit);

 
  const int mxEtaBinsE=12,mxEtaBinsB=40;

  // rebuild local lookup tables
  
  int etowEtaBin2Patch[mxEtaBinsE]={14,14,13,13,12,12,11,11,11,10,10,10};

  int nB=0, nE=0; /* counts # of unmasekd towers */ 
  int nBg=0, nEg=0; /* counts # of reasonable calibrated towers */ 

  for(i=0; i<EmcDbIndexMax; i++) {
    const L2EmcDb2012::EmcCDbItem *x=mDb->getByIndex(i);
    if(mDb->isEmpty(x)) continue;  /* dropped not mapped  channels */
    if(x->fail) continue; /* dropped masked channels */
    if(x->gain<=0) continue; /* dropped uncalibrated towers , tmp */
    /* if(x->sec!=1) continue;   tmp, to test patch mapping */

    /* WARN, calculate index to match RDO order in the ADC data banks */
    int ietaP, iphiP;
    if (mDb->isBTOW(x) ) {
      /*....... B A R R E L  .................*/
      nB++;   
      if(x->eta<0 || x->eta>mxEtaBinsB) goto crashIt_1;
      if(!par_useBtowEast && x->eta<=20) continue;
      if(!par_useBtowWest && x->eta>=21) continue;
      ietaP= (x->eta-1)/4; /* correct */
      int iphiTw=(x->sec-1)*10 + x->sub-'a';
      // allign in phi  TP @ L2 w/ L0 
      iphiTw--;
      if(iphiTw<0) iphiTw=119;
      // now cr0x1e, mod1, subm2, is beginning of the first BTOW TP
      iphiP= iphiTw/4 ; /* correct */
      //      if(ietaP==0 && iphiP==5) 
      db_btowL2PhiBin[x->rdo]=iphiP;
      db_btowL2PatchBin[x->rdo]=ietaP+ iphiP*cl2jetMaxEtaBins;
      nBg++;
    } else if(mDb->isETOW(x) &&  par_useEndcap) {
      /*....... E N D C A P ........................*/
      nE++;   
      int iphiTw= (x->sec-1)*5 + x->sub-'A';
      // allign in phi  TP @ L2 w/ L0 
      iphiTw--;
      if(iphiTw<0) iphiTw=59;
      // now subsector 01TB is beginning of the first(==0) ETOW TP in phi
      iphiP= iphiTw/2 ; /* correct */
      if(x->eta<0 || x->eta>mxEtaBinsE) goto crashIt_1;
      ietaP=etowEtaBin2Patch[x->eta-1];
      db_etowL2PhiBin[x->rdo]=iphiP;
      db_etowL2PatchBin[x->rdo]=ietaP+ iphiP*cl2jetMaxEtaBins;
      nEg++;
    }
    
  }

  if (mLogFile) {
    fprintf(mLogFile,"L2jet algorithm: found  working/calibrated: %d/%d=ETOW  & %d/%d=BTOW, based on ASCII DB\n",nE,nEg,nB,nBg);
  }

  return 0; //OK
  
 crashIt_1: /* fatal initialization error */
  run_number=-55;
  if (mLogFile) { 
    fprintf(mLogFile,"L2jet algorithm init: crashC in internal logic\n");
    fclose(mLogFile);
  }
 return -6;

}               

/*========================================
  ======================================== */
void L2jetAlgo2012::computeUser(int token){
  //put in to satisfy l2virtualalgo2012
}

/*========================================
  ======================================== */
bool
L2jetAlgo2012::decisionUser(int token, int *myL2Result){
  /* STRICT TIME BUDGET  START ....*/
  
  //Ross says:  Eventually you should take these out - timing is done in the virtual algo class as well.
  unsigned long mEveTimeStart,mEveTimeStop, mEveTimeDiff;
rdtscl_macro(mEveTimeStart);

  //Ross says:  I will check how the following line is done in L2bemcGamma2012
  //following if statement was eve_ID!=inpEveId BP
  if(1) {//UUUU
    //.................... this event has NOT been processed

  /*
    Chris doesn't want us to write anything out 
    during event processing ...
  */

    bool bemcIn = true;
    bool eemcIn = true;
  
    //eve_ID=inpEveId; // every events is processed only once
    mEventsInRun++;
    clearEvent(); /* price=13 kTicks */  
    int runTimeSec=time(0)- run_startUnix;
    hA[10]->fill(0);
    hA[12]->fill(runTimeSec);
    
    if(par_dbg>1) LOG(DBG,"\n......... in  L2Jet_doEvent(ID=%d)... bIn=%d eIn=%d\n",eve_ID,bemcIn,eemcIn);//bemcIn and eemcIn were passed to the old doEvent
  

    //Ross asks: will the jet code run on just barrel or just endcap?
    //  if that's the case, will it save time to move those sections of the
    //  code to computeUser() rather than decisionUser()? 
    if (bemcIn || eemcIn){//VVVV this algo has nothing to do w/o any Ecal data 
      
  //===== step 1: unpack raw data blocks ==============
   
  int nBtowTw=0, nEtowTw=0;
  //Ross says:  I've changed projectAdc to use the L2eventStream2012 class
  /*......... BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB */

  //we can move this to compute.
  if(bemcIn==1 && (par_useBtowEast||par_useBtowWest) ) {

    nBtowTw=projectAdc( mEveStream_btow[token].get_hits(), mEveStream_btow[token].get_hitSize(),
			db_btowL2PhiBin, db_btowL2PatchBin,
			hA[20] );
  }

  /*........... EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE */
  //and this as well.
  if(eemcIn==1 && par_useEndcap ) {
    nEtowTw=projectAdc(  mEveStream_etow[token].get_hits(), mEveStream_etow[token].get_hitSize(),
			db_etowL2PhiBin, db_etowL2PatchBin,
			hA[30] );
  }

  //============Do 2D scan==========
  float totEneGeV=true2Dscan();
  int itotEneGeV=(int)totEneGeV;
  int iK;
  for(iK=0; iK<mxJ; iK++)
    {
      weightedEtaPhi(iK);
      L2Jet *K=eve_Jet[iK];
      K->eneGeV=K->iene;
      K->phiRad=0.21*(6.0-K->fphiBin);
      while(K->phiRad<0) K->phiRad+=6.2832;
      while(K->phiRad>6.2832) K->phiRad-=6.2832;
    }

  if(eve_Jet[0]->eneGeV <eve_Jet[1]->eneGeV) {// swap jets fo E1>E2
    L2Jet *Jx=eve_Jet[0];
    eve_Jet[0]=eve_Jet[1];
    eve_Jet[1]=Jx;
  }

  if(par_dbg>2) LOG(DBG,"doEvent iphiBin1=%d iene1=%f , iphiBin2=%d iene2=%f  rms1PhiBin=%f \n",eve_Jet[0]->iphiBin,eve_Jet[0]->iene,eve_Jet[1]->iphiBin,eve_Jet[1]->iene,eve_Jet[0]->rmsPhiBin);
  
  //====== step 4: make trigger decisions====
  
  // now put in eta vs. Jet Threshold "contour" cuts ... this vesion cuts on eta_1 + eta_2 .. wwj 2/16 
  // looks like fetaBin goes 0.0->15.0 for eta -1. -> +2. ... so "eta" = fetaBin*0.2 -1.0 ... wwj 2/10

  float rjetEta_0=eve_Jet[0]->fetaBin*0.2 -1.0; //  wwj 2/10 
  float rjetEta_1=eve_Jet[1]->fetaBin*0.2 -1.0; //  wwj 2/10 

  bool acceptDiJet_EE=((rjetEta_0+rjetEta_1) > par_diJetEtaHigh && eve_Jet[0]->eneGeV > par_diJetThr_4 && eve_Jet[1]->eneGeV > par_diJetThrLow); // wwj 2/10

  bool acceptDiJet_EB=((rjetEta_0+rjetEta_1) > par_diJetEtaLow && eve_Jet[0]->eneGeV > par_diJetThr_5 && eve_Jet[1]->eneGeV > par_diJetThr_2); // wwj 2/10
  
  
  // bool acceptDiJet=( eve_Jet[0]->eneGeV > par_diJetThrHigh) && ( eve_Jet[1]->eneGeV > par_diJetThrLow);
  bool acceptDiJet_BB=(eve_Jet[0]->eneGeV > par_diJetThrHigh && eve_Jet[1]->eneGeV > par_diJetThr_3); // wwj 2/10 

  bool acceptDiJet=acceptDiJet_EE || acceptDiJet_EB || acceptDiJet_BB; // wwj 2/10

  bool acceptOneJet=( eve_Jet[0]->eneGeV > par_oneJetThr) ;

  bool acceptRnd=mRandomAccept>0; // provided by Virtual algo
  mAccept=acceptDiJet || acceptOneJet || acceptRnd;
 
  //====== step 5: update various monitorig histos

  // histogramming reco Et1-Et2, no cuts
  int iet1 =(int)eve_Jet[0]->eneGeV;
  int iet2 =(int)eve_Jet[1]->eneGeV;
  int ieta1=(int)eve_Jet[0]->fetaBin;
  int ieta2=(int)eve_Jet[1]->fetaBin;
  int iphi1=(int)eve_Jet[0]->fphiBin;
  int iphi2=(int)eve_Jet[1]->fphiBin;

  hA[40]->fill(iet1,iet2);

  hA[41]->fill(ieta1,iphi1);
  hA[42]->fill(ieta2,iphi2);
  hA[43]->fill(iphi1,iphi2);
  hA[44]->fill(iet1);
  hA[45]->fill(iet2);
  hA[46]->fill(itotEneGeV);
  hA[47]->fill(nBtowTw);
  hA[48]->fill(nEtowTw);

  // sivers delta zeta, the map is still worng
  int kphi1=int(eve_Jet[0]->phiRad*10.);
  int kphi2=int(eve_Jet[1]->phiRad*10.);
  int idelZeta=map_DelPhiJets[kphi1*MxPhiRad10 + kphi2];

  if( mAccept)  hA[10]->fill(8);

  if(acceptOneJet ){ 
    hA[10]->fill(4);
    run_nEventOneJet++;
    hA[13]->fill(runTimeSec);
    hA[50]->fill(iet1);
    hA[51]->fill(ieta1,iphi1);
    hA[52]->fill(ieta1);
    hA[53]->fill(iphi1);
  }
  
  if(acceptDiJet  ){
    hA[10]->fill(5);
    run_nEventDiJet++;
    hA[14]->fill(runTimeSec);
    hA[60]->fill(iet1,iet2);
    hA[61]->fill(ieta1,iphi1);
    hA[62]->fill(ieta2,iphi2);
    hA[63]->fill(iphi1,iphi2);
    hA[64]->fill(iet1);
    hA[65]->fill(iet2);
    hA[66]->fill(ieta1);
    hA[67]->fill(ieta2);
    hA[68]->fill(iphi1);
    hA[69]->fill(iphi2);
    hA[70]->fill(idelZeta);
    hA[71]->fill(ieta1,idelZeta);
    hA[72]->fill(ieta1,ieta2);
    hA[73]->fill((iphi1+iphi2)/2,idelZeta);
    hA[74]->fill(itotEneGeV);
   }

  if(acceptDiJet_EE  ){
    run_nEventDiJet++;        // all these histos #80-#94 added ... wwj 2/10
    hA[80]->fill(iet1,iet2);
    hA[81]->fill(ieta1,iphi1);
    hA[82]->fill(ieta2,iphi2);
    hA[83]->fill(iphi1,iphi2);
    hA[84]->fill(iet1);
    hA[85]->fill(iet2);
    hA[86]->fill(ieta1);
    hA[87]->fill(ieta2);
    hA[88]->fill(iphi1);
    hA[89]->fill(iphi2);
    hA[90]->fill(idelZeta);
    hA[91]->fill(ieta1,idelZeta);
    hA[92]->fill(ieta1,ieta2);
    hA[93]->fill((iphi1+iphi2)/2,idelZeta);
    hA[94]->fill(itotEneGeV);
   }

  if(acceptDiJet_EB  ){
    run_nEventDiJet++;        // all these histos #100-#114 added ... wwj 2/10
    hA[100]->fill(iet1,iet2);
    hA[101]->fill(ieta1,iphi1);
    hA[102]->fill(ieta2,iphi2);
    hA[103]->fill(iphi1,iphi2);
    hA[104]->fill(iet1);
    hA[105]->fill(iet2);
    hA[106]->fill(ieta1);
    hA[107]->fill(ieta2);
    hA[108]->fill(iphi1);
    hA[109]->fill(iphi2);
    hA[110]->fill(idelZeta);
    hA[111]->fill(ieta1,idelZeta);
    hA[112]->fill(ieta1,ieta2);
    hA[113]->fill((iphi1+iphi2)/2,idelZeta);
    hA[114]->fill(itotEneGeV);
   }

  if(acceptDiJet_BB  ){
    run_nEventDiJet++;        // all these histos #120-#134 added ... wwj 2/10
    hA[120]->fill(iet1,iet2);
    hA[121]->fill(ieta1,iphi1);
    hA[122]->fill(ieta2,iphi2);
    hA[123]->fill(iphi1,iphi2);
    hA[124]->fill(iet1);
    hA[125]->fill(iet2);
    hA[126]->fill(ieta1);
    hA[127]->fill(ieta2);
    hA[128]->fill(iphi1);
    hA[129]->fill(iphi2);
    hA[130]->fill(idelZeta);
    hA[131]->fill(ieta1,idelZeta);
    hA[132]->fill(ieta1,ieta2);
    hA[133]->fill((iphi1+iphi2)/2,idelZeta);
    hA[134]->fill(itotEneGeV);
   }
 
  if(mRandomAccept){  
    hA[10]->fill(6);
    run_nEventRnd++;
    hA[15]->fill(runTimeSec);
  }
  
  
  //====== step 6:  fill L2Result  (except time)
  L2jetResults2012 out; // all output bits lump together
  memset(&out,0,sizeof(out)); // clear content
  
  
  // add acceptDiJet_EE, acceptDiJet_EB and acceptDiJet_BB to below ... wwj 2/10
  out.int0.version=L2JET_RESULTS_VERSION;
  out.int0.decision=
    ( par_useBtowEast <<0 ) +
    ( par_useBtowWest <<1 ) +
    ( par_useEndcap   <<2 ) +
    ( (bemcIn>0)      <<3 ) + 
    ( (eemcIn>0)      <<4 ) + 
    (  mRandomAccept  <<5 ) +
    (  acceptOneJet   <<6 ) +
    (  acceptDiJet    <<7 ) +
    ( acceptDiJet_EE  <<8 ) +
    ( acceptDiJet_EB  <<9 ) +
    ( acceptDiJet_BB <<10 ) ;  // e.g., added last three lines here  ... wwj 2/10
  out.int0.dumm=namechar;

  out.int1.iTotEne=(unsigned short)(totEneGeV*100.); // now 1=10 MeV
  out.int2.nBtowTw=nBtowTw;
  out.int2.nEtowTw=nEtowTw;
  
  out.jet1.jPhi=(int)(eve_Jet[0]->phiRad*28.65);  //so phi/deg=2*jPhi
  out.jet1.jEta=(int)(eve_Jet[0]->fetaBin*10.);
  out.jet1.iEne=(unsigned short)(eve_Jet[0]->eneGeV*100.); // now 1=10 MeV
  
  out.jet2.jPhi=(int)(eve_Jet[1]->phiRad*28.65);
  out.jet2.jEta=(int)(eve_Jet[1]->fetaBin*10.);
  out.jet2.iEne=(unsigned short)(eve_Jet[1]->eneGeV*100.); // now 1=10 MeV

  out.jet1.rmsEta=(unsigned short)(eve_Jet[0]->rmsEtaBin*20.); //Fix: 1=0.01 physical eta
  out.jet1.rmsPhi=(unsigned short)(eve_Jet[0]->rmsPhiBin*120.); //1=0.1 deg

  out.jet2.rmsEta=(unsigned short)(eve_Jet[1]->rmsEtaBin*20.); //Fix: 1=0.01 physical eta
  out.jet2.rmsPhi=(unsigned short)(eve_Jet[1]->rmsPhiBin*120.);// 1=0.1 deg

  
  rdtscl_macro(mEveTimeStop);
  mEveTimeDiff=mEveTimeStop-mEveTimeStart;
  int  kTick=mEveTimeDiff/1000;

  //mhT->fill(kTick); taken out 12/11/08 BP

  out.int0.kTick=  kTick>255 ? 255 : kTick;
 
  //calculate and match the check sum
  out.int1.checkSum=-L2jetResults2012_doCheckSum(&out);

  

  //===== step 7: write L2Result
  memcpy(myL2Result,&out,sizeof(L2jetResults2012));

  // dirty tests, clean it up before real use
  
  if(par_dbg){//WWWW
    L2jetResults2012_print(&out);
    LOG(DBG," phiRad1=%f  phiRad2=%f \n",eve_Jet[0]->phiRad,eve_Jet[1]->phiRad);
    LOG(DBG,"idelZeta=%d  delZeta/deg=%.1f \n\n",idelZeta,idelZeta/31.416*180);
    
    //tmp printouts of errors:
    if( out.jet1.iEne+out.jet2.iEne > out.int1.iTotEne) {
    }
    if(iphi1==iphi2) {
      LOG(ERR,"L2jet-fatal error,neveId=%d, phi1,2=%d,%d\n",mEventsInRun,iphi1,iphi2);
      dumpPatchEneA();    
    }
    
    if( L2jetResults2012_doCheckSum(&out)) {
      LOG(ERR,"L2jet-fatal error, wrong cSum=%d\n", L2jetResults2012_doCheckSum(&out));
      L2jetResults2012_print(&out);
    }
    
  } // end of WWWW
  
    }// end of VVVV (etow or btow has some data)
  }// end of UUUU event processing
  
  
  return   mAccept;
}


/*========================================
  ======================================== */
void 
L2jetAlgo2012::finishRunUser() {  /* called once at the end of the run */
  if(run_number<0) return; // already finished

  if (mLogFile) {
    fprintf(mLogFile,"L2-jet algorithm finishRun(%d)\n",run_number);
    fprintf(mLogFile," - %d events seen by L2 di-jet\n",mEventsInRun);
    fprintf(mLogFile," - accepted: rnd=%d  oneJet=%d diJet=%d \n", run_nEventRnd,  run_nEventOneJet, run_nEventDiJet);

    // print few basic histos
    hA[10]->printCSV(mLogFile); // event accumulated

  }
  finishRunHisto(); // still needs current DB
  if( mHistFile==0) {
    LOG(ERR," L2jetAlgo2012: finishRun() UNABLE to open run summary log file, continue anyhow\n");
    if (mLogFile)
      fprintf(mLogFile,"L2 di-jet histos NOT saved, I/O error\n");
  } else { // save histos  
    int j;
    int nh=0;
    for(j=0;j<mxHA;j++) {
      if(hA[j]==0) continue;
      hA[j]->write(mHistFile);
      nh++;
    }    
  }
  
  run_number=-2; // clear run #
 
}

//=======================================
//=======================================
void 
L2jetAlgo2012::createHisto() {
  memset(hA,0,sizeof(hA));

  hA[10]=new   L2Histo(10,"total event counter; x=cases",9);
  hA[11]=new   L2Histo(11,"L2 time used per input event;  x: time (CPU kTics), range=100muSec; y: events ",160);

  int mxRunDration=2500;
  hA[12]=new   L2Histo(12,"rate of input events; x: time in this run (seconds); y: rate (Hz)", mxRunDration);
  
  hA[13]=new   L2Histo(13,"rate of  accepted one-Jet; x: time in this run (seconds); y: rate (Hz)", mxRunDration);
  hA[14]=new   L2Histo(14,"rate of  accepted di-Jet ; x: time in this run (seconds); y: rate (Hz)", mxRunDration);
  hA[15]=new   L2Histo(15,"rate of  random accepted  ; x: time in this run (seconds); y: rate (Hz)", mxRunDration);

  // BTOW  raw spectra
  hA[20]=new   L2Histo(20,"BTOW tower, Et>2.0 GeV (input); x: BTOW RDO index=chan*30+fiber; y: counts", 4800);
  hA[21]=new   L2Histo(21,"BTOW tower, Et>2.0 GeV (input); x: BTOW softID", 4800);
  hA[22]=new   L2Histo(22,"BTOW tower, Et>2.0 GeV (input); x: eta bin, [-1,+1];  y: phi bin ~sector",40,120);
  
  // ETOW  raw spectra
  hA[30]=new   L2Histo(30,"ETOW tower, Et>2.0 GeV (input); x: ETOW RDO index=chan*6+fiber; y: counts", 720 );
  hA[31]=new   L2Histo(31,"ETOW tower, Et>2.0 GeV (input); x: i=chan+128*crate", 768);
  hA[32]=new   L2Histo(32,"ETOW tower, Et>2.0 GeV (input); x: 12 - Endcap etaBin ,[+1,+2];  y: phi bin ~sector",12,60);
  
 // Di-Jet raw yields
 hA[40]=new   L2Histo(40,"Et Jet1-Jet2 (input); x: Jet1 Et/GeV ; Jet2 Et/GeV",30,30);
 hA[41]=new   L2Histo(41,"diJet1 eta-phi (input); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[42]=new   L2Histo(42,"diJet2 eta-phi (input); x: iEta [-1,+2]  ; y: iPhi ~sector",15,30);

 hA[43]=new  L2Histo(43,"diJet phi1-phi2 (input); x: iPhi1 ~sector ; y: iPhi2 ~sector ",30,30);

 hA[44]=new  L2Histo(44,"Jet1 Et (input); x: Et (GeV)", 60);
 hA[45]=new  L2Histo(45,"Jet2 Et (input); x: Et (GeV)", 60);
 hA[46]=new  L2Histo(46,"total Et (input); x: Et (GeV)", 100);
 hA[47]=new  L2Histo(47,"# BTOW towers>thrXX (input); x: # of towers/event", 200);
 hA[48]=new  L2Histo(48,"# ETOW towers>thrXX (input); x: # of towers/event", 100);

 // ........accepted one-jet events
 hA[50]=new  L2Histo(50,"one-Jet Et (accepted); x: jet Et (GeV)", 60);
 hA[51]=new  L2Histo(51,"one-Jet eta-phi (accepted); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[52]=new  L2Histo(52,"one-Jet eta (accepted); x: iEta [-1,+2]", 15);
 hA[53]=new  L2Histo(53,"one-Jet phi (accepted); x: iPhi ~sector", 30);

 // Di-Jet accepted
 hA[60]=new   L2Histo(60,"Et of Jet1 vs. Jet2  (accepted); x: Jet1/GeV ; Jet2/GeV",30,30);
 hA[61]=new   L2Histo(61,"diJet1 eta-phi   (accepted); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[62]=new   L2Histo(62,"diJet2 eta-phi   (accepted); x: iEta [-1,+2]  ; y: iPhi ~sector",15,30);

 hA[63]=new  L2Histo(63,"diJet phi1-phi2   (accepted); x: iPhi1 ~sector ; y: iPhi2 ~sector ",30,30);

 hA[64]=new  L2Histo(64,"diJet1 Et  (accepted); x: Et (GeV)", 60);
 hA[65]=new  L2Histo(65,"diJet2 Et   (accepted); x: Et (GeV)", 60);

 hA[66]=new  L2Histo(66,"diJet1  eta (accepted); x: i Eta [-1,+2]", 15);
 hA[67]=new  L2Histo(67,"diJet2  eta (accepted); x: i Eta [-1,+2]", 15);
 hA[68]=new  L2Histo(68,"diJet1 phi (accepted); x: iPhi ~sector", 30);
 hA[69]=new  L2Histo(69,"diJet2 phi (accepted); x: iPhi ~sector", 30);
 hA[70]=new  L2Histo(70,"diJet delZeta  (accepted); x: delta zeta  (rad*10)", MxPhiRad10);
 hA[71]=new  L2Histo(71,"diJet delZeta vs. eta1 (accepted); x: iEta1 [-1,+2] ; y: delta zeta  (rad*10)",15, MxPhiRad10);
 hA[72]=new  L2Histo(72,"diJet eta2 vs. eta1  (accepted); x: iEta1 [-1,+2] ;x: iEta2 [-1,+2] ",15,15);
 hA[73]=new  L2Histo(73,"diJet   delZeta vs. avrPhi (accepted); x: (iphi1+iphi2)/2  (12 deg/bin); y: delta zeta  (rad*10)",30, MxPhiRad10);
 hA[74]=new  L2Histo(74,"total Et diJet (accepted); x: Et (GeV)", 100);

 // Di-Jet_EE accepted  ... all these histos #80-#94 added  ... wwj 2/10
 hA[80]=new   L2Histo(80,"Et of Jet1 vs. Jet2  (accepted_EE); x: Jet1/GeV ; Jet2/GeV",30,30);
 hA[81]=new   L2Histo(81,"diJet1 eta-phi   (accepted_EE); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[82]=new   L2Histo(82,"diJet2 eta-phi   (accepted_EE); x: iEta [-1,+2]  ; y: iPhi ~sector",15,30);

 hA[83]=new  L2Histo(83,"diJet phi1-phi2   (accepted_EE); x: iPhi1 ~sector ; y: iPhi2 ~sector ",30,30);

 hA[84]=new  L2Histo(84,"diJet1 Et  (accepted_EE); x: Et (GeV)", 60);
 hA[85]=new  L2Histo(85,"diJet2 Et   (accepted_EE); x: Et (GeV)", 60);

 hA[86]=new  L2Histo(86,"diJet1  eta (accepted_EE); x: i Eta [-1,+2]", 15);
 hA[87]=new  L2Histo(87,"diJet2  eta (accepted_EE); x: i Eta [-1,+2]", 15);
 hA[88]=new  L2Histo(88,"diJet1 phi (accepted_EE); x: iPhi ~sector", 30);
 hA[89]=new  L2Histo(89,"diJet2 phi (accepted_EE); x: iPhi ~sector", 30);
 hA[90]=new  L2Histo(90,"diJet delZeta  (accepted_EE); x: delta zeta  (rad*10)", MxPhiRad10);
 hA[91]=new  L2Histo(91,"diJet delZeta vs. eta1 (accepted_EE); x: iEta1 [-1,+2] ; y: delta zeta  (rad*10)",15, MxPhiRad10);
 hA[92]=new  L2Histo(92,"diJet eta2 vs. eta1  (accepted_EE); x: iEta1 [-1,+2] ;x: iEta2 [-1,+2] ",15,15);
 hA[93]=new  L2Histo(93,"diJet   delZeta vs. avrPhi (accepted_EE); x: (iphi1+iphi2)/2  (12 deg/bin); y: delta zeta  (rad*10)",30, MxPhiRad10);
 hA[94]=new  L2Histo(94,"total Et diJet (accepted_EE); x: Et (GeV)", 60);

// Di-Jet_EB accepted ... all these histos #100-#114 added  ... wwj 2/10
 hA[100]=new   L2Histo(100,"Et of Jet1 vs. Jet2  (accepted_EB); x: Jet1/GeV ; Jet2/GeV",30,30);
 hA[101]=new   L2Histo(101,"diJet1 eta-phi   (accepted_EB); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[102]=new   L2Histo(102,"diJet2 eta-phi   (accepted_EB); x: iEta [-1,+2]  ; y: iPhi ~sector",15,30);

 hA[103]=new  L2Histo(103,"diJet phi1-phi2   (accepted_EB); x: iPhi1 ~sector ; y: iPhi2 ~sector ",30,30);

 hA[104]=new  L2Histo(104,"diJet1 Et  (accepted_EB); x: Et (GeV)", 60);
 hA[105]=new  L2Histo(105,"diJet2 Et   (accepted_EB); x: Et (GeV)", 60);

 hA[106]=new  L2Histo(106,"diJet1  eta (accepted_EB); x: i Eta [-1,+2]", 15);
 hA[107]=new  L2Histo(107,"diJet2  eta (accepted_EB); x: i Eta [-1,+2]", 15);
 hA[108]=new  L2Histo(108,"diJet1 phi (accepted_EB); x: iPhi ~sector", 30);
 hA[109]=new  L2Histo(109,"diJet2 phi (accepted_EB); x: iPhi ~sector", 30);
 hA[110]=new  L2Histo(110,"diJet delZeta  (accepted_EB); x: delta zeta  (rad*10)", MxPhiRad10);
 hA[111]=new  L2Histo(111,"diJet delZeta vs. eta1 (accepted_EB); x: iEta1 [-1,+2] ; y: delta zeta  (rad*10)",15, MxPhiRad10);
 hA[112]=new  L2Histo(112,"diJet eta2 vs. eta1  (accepted_EB); x: iEta1 [-1,+2] ;x: iEta2 [-1,+2] ",15,15);
 hA[113]=new  L2Histo(113,"diJet   delZeta vs. avrPhi (accepted_EB); x: (iphi1+iphi2)/2  (12 deg/bin); y: delta zeta  (rad*10)",30, MxPhiRad10);
 hA[114]=new  L2Histo(114,"total Et diJet (accepted_EB); x: Et (GeV)", 60);

// Di-Jet_BB accepted ... all these histos #120-#134 added ... wwj 2/10 
 hA[120]=new   L2Histo(120,"Et of Jet1 vs. Jet2  (accepted_BB); x: Jet1/GeV ; Jet2/GeV",30,30);
 hA[121]=new   L2Histo(121,"diJet1 eta-phi   (accepted_BB); x: iEta [-1,+2] ; y: iPhi ~sector ",15,30);
 hA[122]=new   L2Histo(122,"diJet2 eta-phi   (accepted_BB); x: iEta [-1,+2]  ; y: iPhi ~sector",15,30);

 hA[123]=new  L2Histo(123,"diJet phi1-phi2   (accepted_BB); x: iPhi1 ~sector ; y: iPhi2 ~sector ",30,30);

 hA[124]=new  L2Histo(124,"diJet1 Et  (accepted_BB); x: Et (GeV)", 60);
 hA[125]=new  L2Histo(125,"diJet2 Et   (accepted_BB); x: Et (GeV)", 60);

 hA[126]=new  L2Histo(126,"diJet1  eta (accepted_BB); x: i Eta [-1,+2]", 15);
 hA[127]=new  L2Histo(127,"diJet2  eta (accepted_BB); x: i Eta [-1,+2]", 15);
 hA[128]=new  L2Histo(128,"diJet1 phi (accepted_BB); x: iPhi ~sector", 30);
 hA[129]=new  L2Histo(129,"diJet2 phi (accepted_BB); x: iPhi ~sector", 30);
 hA[130]=new  L2Histo(130,"diJet delZeta  (accepted_BB); x: delta zeta  (rad*10)", MxPhiRad10);
 hA[131]=new  L2Histo(131,"diJet delZeta vs. eta1 (accepted_BB); x: iEta1 [-1,+2] ; y: delta zeta  (rad*10)",15, MxPhiRad10);
 hA[132]=new  L2Histo(132,"diJet eta2 vs. eta1  (accepted_BB); x: iEta1 [-1,+2] ;x: iEta2 [-1,+2] ",15,15);
 hA[133]=new  L2Histo(133,"diJet   delZeta vs. avrPhi (accepted_BB); x: (iphi1+iphi2)/2  (12 deg/bin); y: delta zeta  (rad*10)",30, MxPhiRad10);
 hA[134]=new  L2Histo(134,"total Et diJet (accepted_BB); x: Et (GeV)", 100);

}

//=======================================
//=======================================
void 
L2jetAlgo2012::clearEvent(){
  mAccept=false;
  memset(eve_patchEne,0,sizeof(eve_patchEne));
  memset(eve_phiEne,0,sizeof(eve_phiEne));
  eve_Jet[0]->clear();
  eve_Jet[1]->clear();
}



//=======================================
//=======================================
int 
L2jetAlgo2012::projectAdc(const HitTower1 *hit,const int hitSize,
	     unsigned short *phiBin, unsigned short *patchBin,
	     L2Histo *hHot	 ){
  int tmpNused=0; /* counts mapped & used ADC channels */
  short rdo;
  float low_noise_et;

  for(int i=0;i< hitSize;i++,hit++) {
    rdo=hit->rdo;
    low_noise_et=hit->low_noise_et;
    eve_patchEne[patchBin[rdo]]+=low_noise_et;
    eve_phiEne[phiBin[rdo]]+=low_noise_et;
    tmpNused++;

    if(low_noise_et > par_hotTwEtThr) hHot->fill(rdo);//do we want rdo or i?

  }
  return tmpNused;
}


//========================================
//========================================
float L2jetAlgo2012::true2Dscan(){
  //implament a true 2-D scan to test the performance of the current algo
  int iphiBinEdge = 0;
  int ietaBinEdge = 0;
  float maxPatchEt = 0.0;
  float sumTot = 0.0;

  float *totalCaloEt=eve_phiEne;
  float  eneA[cl2jetMaxEtaBins];
  float  secondPatchArray[cl2jetMaxPhiBins][cl2jetMaxEtaBins-cl2jet_par_mxEtaBin+1];
  memset(secondPatchArray,0,sizeof(secondPatchArray));

  int i;//loops over all phi bin. phi edge
  int j;//collapses phi bins
  int j1;//phi wrapped phi bin
  int j2;//loops over eta bins and sums energy
  int k;//loops over eta bins. eta edge

  for(i=0;i<cl2jetMaxPhiBins;i++)
    {
      sumTot+=totalCaloEt[i];
      memset(eneA,0,sizeof(eneA));
      for(j=0;j<cl2jet_par_mxPhiBin;j++)
	{
	  j1 = (i+j)%cl2jetMaxPhiBins;//must wrap up phi
	  float *patchEneA=eve_patchEne+(j1*cl2jetMaxEtaBins);
	  for(j2=0;j2<cl2jetMaxEtaBins;j2++,patchEneA++)
	    {
	      eneA[j2]+=*patchEneA;
	    }
	}
      float *eneAp=eneA;
      for(k=0;k<cl2jetMaxEtaBins-cl2jet_par_mxEtaBin+1;k++,eneAp++) 
	{
	  float sum=eneAp[0]+eneAp[1]+eneAp[2]+eneAp[3]+eneAp[4];
	  secondPatchArray[i][k]=sum;
	  if(maxPatchEt>sum) continue;
	  //float *eneSave=eneA;
	  maxPatchEt=sum;
	  iphiBinEdge=i;
	  ietaBinEdge=k;
	}
    }

  eve_Jet[0]->iphiBin=iphiBinEdge;
  eve_Jet[0]->ietaBin=ietaBinEdge;
  eve_Jet[0]->iene=maxPatchEt;

  //now find second highest patch
  int iphiBinEdge2=-1;
  int ietaBinEdge2=-1;
  float maxPatchEt2=0.0;
  char doWrap=0;
  int a1=iphiBinEdge-par_minPhiBinDiff;
  int a2=iphiBinEdge+par_minPhiBinDiff;
  if (a1<0) { a1+=cl2jetMaxPhiBins; doWrap+=1; }
  if (a2>=cl2jetMaxPhiBins) { a2-=cl2jetMaxPhiBins; doWrap+=2; }
    
  int b;
  int b1;
  if (!doWrap)//masking range does not wrap up
    {
      for(b=0;b<cl2jetMaxPhiBins;b++)
	{
	  if(b>=a1 && b<=a2) continue;
	  for(b1=0;b1<cl2jetMaxEtaBins-cl2jet_par_mxEtaBin+1;b1++)
	    {
	      if(maxPatchEt2>secondPatchArray[b][b1]) continue;
	      maxPatchEt2=secondPatchArray[b][b1];
	      iphiBinEdge2=b;
	      ietaBinEdge2=b1;
	    }
	}
    } 
  else
    {//masking range wraps up
      for(b=a2;b<a1;b++)
	{
	  for(b1=0;b1<cl2jetMaxEtaBins-cl2jet_par_mxEtaBin+1;b1++)
	    {
	      if(maxPatchEt2>secondPatchArray[b][b1]) continue;
	      maxPatchEt2=secondPatchArray[b][b1];
	      iphiBinEdge2=b;
	      ietaBinEdge2=b1;
	    }
	}
    }

  eve_Jet[1]->iphiBin=iphiBinEdge2;
  eve_Jet[1]->ietaBin=ietaBinEdge2;
  eve_Jet[1]->iene=maxPatchEt2;

  return sumTot;
}

//========================================
//========================================
void L2jetAlgo2012::weightedEtaPhi(int iK){
  L2Jet *J=eve_Jet[iK];

  // empty calo protection
  if(J->iene<=0.1) 
    {
      J->fphiBin=J->iphiBin+.333;
      J->fetaBin=J->ietaBin+.333;
      J->rmsPhiBin=0.0;
      J->rmsEtaBin=0.0;
      return;
    }

  int iphi0=J->iphiBin;
  int ieta0=J->ietaBin;
  float iene0=J->iene;

  //variables for weighted phi
  float sumY=0.0;
  float sum1;//wrk variable

  //these variables for phi RMS:
  double sumYY = 0.0; 
  int nEnePhi=0; //  counts bins with energy for RMS computation

  //variable for weighted eta
  float sumX=0.0;

  //these variables for eta RMS:
  double sumXX = 0.0;
  int nEneEta = 0;

  float  etaEneA[cl2jet_par_mxEtaBin];
  memset(etaEneA,0,sizeof(etaEneA));

  int ix,iy;
  // pick 5x5 eta-phi patch and calculate weighted phi==Y
  for(iy=iphi0;iy<iphi0+cl2jet_par_mxPhiBin;iy++)
    {
      int jy=iy % cl2jetMaxPhiBins;
      float  *patchEneA=eve_patchEne+(jy*cl2jetMaxEtaBins);// phi bins are consecutive
    
      sum1=0.0;
      for(ix=ieta0;ix<ieta0+cl2jet_par_mxEtaBin;ix++)
	{
	  sum1+=patchEneA[ix];
	  etaEneA[ix-ieta0]+=patchEneA[ix];
	}
      if(sum1>0) nEnePhi++;
      sumY+=sum1*iy;
      sumYY+=sum1*iy*iy;
    }

  // pick 5x5 eta-phi patch and calculate weighted eta==X
  int jx;
  for(jx=ieta0;jx<ieta0+cl2jet_par_mxEtaBin;jx++)
    {
      if(etaEneA[jx-ieta0]>0) nEneEta++;
      sumX+=jx*etaEneA[jx-ieta0];
      sumXX+=(jx*jx*etaEneA[jx-ieta0]);
    }

  //calculate phi mean and rms
  float phiMean= 1.*sumY/iene0;//was phiSum
  float fphiBinMax=0.5 +phiMean; 
  float rmsPhi=0.0; // for single bin RMS is 0
  if(nEnePhi>1)
    {   //here I calculate RMS
      rmsPhi=sqrt(nEnePhi/(nEnePhi-1.) *(sumYY/iene0 - phiMean*phiMean));
    }
  if(fphiBinMax>cl2jetMaxPhiBins) fphiBinMax-=cl2jetMaxPhiBins;

  //calculate eta mean and rms
  float etaMean = 1.*sumX/iene0;
  float fetaBin=0.5 + etaMean; 
  float rmsEta=0.0; // for single bin RMS is 0
  if(nEneEta>1)
    {   //here I calculate RMS
      rmsEta=sqrt(nEneEta/(nEneEta-1.) *(sumXX/iene0 - etaMean*etaMean));
    }

  J->fphiBin=fphiBinMax;
  J->fetaBin=fetaBin;
  J->rmsPhiBin=rmsPhi;
  J->rmsEtaBin=rmsEta;

}

//========================================
//========================================
void 
L2jetAlgo2012:: dumpPatchEneA(){
  // dump L2 array with energy 
  int ix,iy;
  for(iy=0;iy<cl2jetMaxPhiBins;iy++) {
    float  *patchEneA=eve_patchEne+(iy*cl2jetMaxEtaBins);// phi bins are consecutive
    
    for(ix=0;ix<cl2jetMaxEtaBins;ix++,patchEneA++){
      printf(" %6f",*patchEneA);
    }
    printf(" iPhi=%d\n",iy);
  }
  
}


//=======================================
//=======================================
void 
L2jetAlgo2012::computeE(int token){
}
//=======================================
//=======================================
void 
L2jetAlgo2012::finishRunHisto(){
  // auxialiary operations on histograms at the end of the run

  const int *data20=hA[20]->getData();
  const int *data30=hA[30]->getData();

  int bHotSum=1,bHotId=-1;
  int eHotSum=1;

  const L2EmcDb2012::EmcCDbItem *xE=mDb->getByIndex(402), *xB=mDb->getByIndex(402);
  int i;
  for(i=0; i<EmcDbIndexMax; i++) {
    const L2EmcDb2012::EmcCDbItem *x=mDb->getByIndex(i);
    if(mDb->isEmpty(x)) continue; 
    if (mDb->isBTOW(x) ) {
      int softId=atoi(x->tube+2);
      int ieta= (x->eta-1);
      int iphi= (x->sec-1)*10 + x->sub-'a' ;
      hA[21]->fillW(softId,data20[x->rdo]);
      hA[22]->fillW(ieta, iphi,data20[x->rdo]);
      if(bHotSum<data20[x->rdo]) {
	bHotSum=data20[x->rdo];
	bHotId=softId;
	xB=x;
      }
   }// end of BTOW
    else  if (mDb->isETOW(x) ) {
      int ihard=x->chan+(x->crate-1)*128;
      int ieta= 12-x->eta;
      int iphi= (x->sec-1)*5 + x->sub-'A' ;
      hA[31]->fillW(ihard,data30[x->rdo]);
      hA[32]->fillW(ieta, iphi,data30[x->rdo]);
      if(eHotSum<data30[x->rdo]) {
	eHotSum=data30[x->rdo];
	xE=x;
      }

    }// end of BTOW
  }
  if (mLogFile){
    fprintf(mLogFile,"L2jet::finishRun()\n");
    fprintf(mLogFile,"#BTOW_hot tower _candidate_ (bHotSum=%d) :, softID %d , crate %d , chan %d , name %s\n",bHotSum,bHotId,xB->crate,xB->chan,xB->name); 
   fprintf(mLogFile,"#ETOW_hot tower _candidate_ (eHotSum=%d) :, name %s , crate %d , chan %d\n",eHotSum,xE->name,xE->crate,xE->chan); 
  }

}


/**********************************************************************
  $Log: L2jetAlgo2012.cxx,v $
  Revision 1.4  2012/03/21 18:18:03  jml
  got rid of printfs from 2012 files

  Revision 1.3  2011/10/19 16:12:11  jml
  more 2012 stuff

  Revision 1.2  2011/10/19 15:39:44  jml
  2012

  Revision 1.1  2011/10/18 15:11:43  jml
  adding 2012 algorithms

  Revision 1.1  2010/04/17 16:42:14  pibero
  *** empty log message ***

  Revision 1.1  2007/11/19 22:18:27  balewski
  most L2algos provide triggerID's

  Revision 1.8  2007/11/14 03:58:14  balewski
  cleanup of common timing measurement

  Revision 1.7  2007/11/13 23:06:07  balewski
  toward more unified L2-algos

  Revision 1.6  2007/11/13 00:12:36  balewski
  added offline triggerID, take1

  Revision 1.5  2007/11/08 04:02:31  balewski
  run on l2ana as well

  Revision 1.4  2007/11/02 17:43:08  balewski
  cleanup & it started to work w/ L2upsilon

  Revision 1.3  2007/11/02 03:03:47  balewski
  modified L2VirtualAlgo

  Revision 1.2  2007/10/25 02:07:02  balewski
  added L2upsilon & binary event dump

  Revision 1.1  2007/10/11 00:33:19  balewski
  L2algo added

  Revision 1.5  2006/03/28 19:46:49  balewski
  ver16b, in l2new

  Revision 1.4  2006/03/11 17:08:33  balewski
  now CVS comments should work

*/


