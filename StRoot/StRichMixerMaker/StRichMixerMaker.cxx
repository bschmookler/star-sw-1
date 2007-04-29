/***************************************************************************
 *
 * $Id: StRichMixerMaker.cxx,v 1.5 2007/04/29 17:29:55 hippolyt Exp $
 *
 * Author:  bl
 ***************************************************************************
 *
 * Description: RICH offline software:
 *              combine the data streams of the SIMULATION
 *              and DATA stream for use in embedding.
 *              The data is filled into the StEvent Rich Pixel
 *              collection and passed via a data set.
 *              The embedded data is accessed in the RchMaker.
 ***************************************************************************
 *
 * $Log: StRichMixerMaker.cxx,v $
 * Revision 1.5  2007/04/29 17:29:55  hippolyt
 * Star logger recommendations pass 2
 *
 * Revision 1.4  2007/04/28 17:56:49  perev
 * Redundant StChain.h removed
 *
 * Revision 1.3  2003/09/24 02:57:39  hippolyt
 * init pointers and arrays
 *
 * Revision 1.2  2003/09/02 17:58:53  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.1  2001/08/27 16:43:43  lasiuk
 * Initial Revision
 *
 ***************************************************************************/
#include <algorithm>

#include "St_DataSetIter.h"


#include "StRichMixerMaker.h"
#include "StRichUtilities.h"

// SCL
#include "StGlobals.hh"

// StEvent
// #include "StEvent/StEvent.h"
#include "StEvent/StContainers.h"
#include "StEvent/StRichCollection.h"
#include "StEvent/StRichPixel.h"
#include "StEvent/StRichMCPixel.h"

//
// Interfaces
//
// DAQ Libraries
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDaqLib/RICH/RICH_Reader.hh"
#include "StDAQMaker/StDAQReader.h"
// Simulation
#include "StRrsMaker/StRrsReader.h"
#include "StRrsMaker/StRichPadPlane.h"
#include "StRrsMaker/StRichSinglePixel.h"
#include "StRrsMaker/StRichSingleMCPixel.h"

// Database
#include "StRrsMaker/StRichGeometryDb.h"

ClassImp(StRichMixerMaker) // macro
   
//-----------------------------------------------------------------
    
StRichMixerMaker::StRichMixerMaker(const char *name)
    : StMaker(name)
{
    mTheRichDaqData    = 0;
    mTheDaqDataReader  = 0;
    mSimStreamReader   = 0;
    mDaqStreamReader   = 0;

    mDaq               = 0;
    mPads              = 0;
    mRows              = 0;
    mNumberOfPads      = 0;
    mEventNumber       = 0;
    mSaturatedValue    = 0;

    // flags
    mPixelCollectionPresent   = 0;
    mClusterCollectionPresent = 0;
    mHitCollectionPresent     = 0;

    //pedestal
    mPedestalSubtract         = 0;
    Int_t i                   = 0;
    Int_t j                   = 0;
    for(i=0;i<160;i++){
      for(j=0;j<96;j++){
	mPedestal[i][j]       = 0;
	mSigma[i][j]          = 0;
      }
    }
    mPedestalFile             = 0;
    
    mTheRichCollection        = 0;

#ifdef RCH_HISTOGRAM
    mTupleFile = 0;
    mPadPlane = 0;
    for(i=0;i<4;i++) mRawData[i] = 0;
#endif
    
    drawinit=kFALSE;
}

//-----------------------------------------------------------------

StRichMixerMaker::~StRichMixerMaker() {}

//-----------------------------------------------------------------

Int_t StRichMixerMaker::Init() {

    //
    // Read in parameters from the dataBase!
    //
    StRichGeometryDb* geometryDb = StRichGeometryDb::getDb();
    
    mPads =  geometryDb->numberOfPadsInARow(); //160
    mRows =  geometryDb->numberOfRowsInAColumn(); //96
    
    mNumberOfPads = geometryDb->numberOfPads();
    PR(mNumberOfPads);
    mEventNumber = 0;

    mSaturatedValue = 1023.;
    mPedestalSubtract = true;
    
#ifdef RCH_HISTOGRAM
    mTupleFile = new TFile("RichMixerData.root","RECREATE","Rich tuples");
    mPadPlane      = new TNtuple("rawNtuple", "raw data", "row:pad:adc:evt");
#endif
    
    //
    // Access to DataBase for Pedestal
    // and GAIN correction goes here
    //
    if (mPedestalSubtract) {
	ifstream pedfile;
	pedfile.open(mPedestalFile);
	if (!pedfile) {
	  { LOG_WARN << "Can not open ped file: " << mPedestalFile << endm; }
	}
	else {
	    for (unsigned int channelnum=0; channelnum<960; ++channelnum) {
		for (unsigned int cramblock=0; cramblock<16; ++cramblock) {
		    pedfile >> mPedestal[channelnum/6][95 - (cramblock*6 + channelnum%6)] 
			    >> mSigma[channelnum/6][95 - (cramblock*6 + channelnum%6)];
		}
	    }
	    pedfile.close();
	    { LOG_INFO << " Read pedestals" << endm; }
	    
  	    for (unsigned int pad=0; pad < 160; ++pad) {
  		for (unsigned int row=0; row < 96; ++row) {
		  { LOG_DEBUG << " pad " << pad << " row " << row
			      << " ped " << mPedestal[pad][row] << " sig " << mSigma[pad][row] << endm; }
  		}
  	    }
	}
    } // pedestall subtract


//     mTheEmbeddedEvent = new StRichCollection();
//     AddConst(new St_ObjectSet("richMixedEvent" mTheEmbeddedEvent));

    return StMaker::Init();
}


// int StRichMixerMaker::adcDecoder(unsigned long code,
// 			   unsigned long* pad, unsigned long* row, unsigned long* adc)
// {
//     *pad = ( code        & 0xff);
//     *row = ((code >> 8)  & 0xff);
//     //--> Used to be *adc = ((code >> 16) & 0x3ff);
//     *adc = ( (code>>26) & 0x1 ) ? 1024 : ( (code>>16) & 0x3ff);

//     return 0;
// }

//-----------------------------------------------------------------

Int_t StRichMixerMaker::Make() {
#ifdef RCH_DEBUG
    ofstream raw("./richMixerMaker.txt");
#endif

//     cout << "StRichMixerMaker::Make()" << endl;

    //
    // Initialize Flags
    //
    mPixelCollectionPresent   = 0;
    mClusterCollectionPresent = 0;
    mHitCollectionPresent     = 0;

    //
    // ptr initialization for StEvent
    //
    mTheRichCollection = 0;
    
    //
    // increase event counter
    //
    mEventNumber++;

//     cout << "StRichMixerMaker::Make()\n";
//     cout << "\tMake the StRichCollection" << endl;
    mTheRichCollection = new StRichCollection();
    if(!mTheRichCollection) {
      { LOG_ERROR << "Could not make a RichCollection Aborting..." << endm; }
	abort();
    }

    //
    // Get the Simulation and Daq data and access them
    // with the readers.  The sim reader is located
    // in the StRrsMaker while the Daq reader is
    // located in the StDAQReader
    //
    // Simulation Stream data first
    //
    St_ObjectSet *rrsEvent = (St_ObjectSet*)GetDataSet("Rrs/.const/richPixels");
    if(!rrsEvent) {
      { LOG_WARN << "DataSet: rrsEvent not there %n Skip this event" << endm; }
      return kStWarn;
    }

    StRichPadPlane* theRichSimData = (StRichPadPlane*)(rrsEvent->GetObject());
    if(!theRichSimData) {
      { LOG_WARN << "RichSimData: not there %n Skip this event" << endm; }
      return kStWarn;
    }

    mSimStreamReader = new StRrsReader(theRichSimData, -9);
    if(!mSimStreamReader) {
      { LOG_WARN << "Cannot get the Sim Stream Reader" << endm; }
      return kStWarn;
    }
    else {
      { LOG_INFO << " Sim Stream Reader acquired" << endm; }
    }
    
    //
    // DAQ Stream data
    //
    mTheRichDaqData   = GetDataSet("StDAQReader");
    if(!mTheRichDaqData) {
      { LOG_WARN << "DataSet: StDAQReader not there %n Skip this event" << endm; }
      return kStWarn;
    }
    
    mTheDaqDataReader = (StDAQReader*)(mTheRichDaqData->GetObject());
    if(!mTheDaqDataReader) {
      { LOG_WARN << "DataSet: StDAQReader not there %n Skip this event" << endm; }
      return kStWarn;
    }
    if (!(mTheDaqDataReader->RICHPresent())) {
      { LOG_WARN << "RICH not in datastream %n Skip this event" << endm; }
	return kStWarn;
    }
    mDaqStreamReader = mTheDaqDataReader->getRICHReader();

    if(!mDaqStreamReader) {
      { LOG_WARN << "Could not get a Reader %n Skip this event" << endm; }
	return kStWarn;
    }
    else {
      { LOG_WARN << "Daq Stream Reader acquired" << endm; }
    }
    
    //
    // In possession of two RichReader's:
    //  --> use the interface to extract the pixels
    //     and load into the StEvent PixelStore
    // The decoding is done such that it is a
    // saturated pad if the 11th bit is set
    //
    int daqPixels = 0;
    int simPixels = 0;
    for(int iPad=0; iPad<mPads; iPad++) {  //x--> 160
	for(int iRow=0; iRow<mRows; iRow++) { // y -> 96
	    
	    bool daqSaturatedPad = false;
	    bool simSaturatedPad = false;
	    
	    unsigned long theDaqAdcValue =
		mDaqStreamReader->GetADCFromCoord(iPad,iRow);

	    if(theDaqAdcValue>=mSaturatedValue)
		daqSaturatedPad = true;

	    if (mPedestalSubtract && (iPad == 0) &&  (iRow%6==5)) {
		unsigned long theCut =
		    static_cast<unsigned long>(mPedestal[iPad][iRow] + mSigma[iPad][iRow]);
		
		if (theDaqAdcValue > theCut) {
		    theDaqAdcValue -= static_cast<unsigned long>(mPedestal[iPad][iRow]);
		}
		else {
		    theDaqAdcValue = 0;
		}
	    }
	    
	    unsigned long theSimAdcValue =
		mSimStreamReader->GetADCFromCoord(iPad,iRow);
	    
	    if(theSimAdcValue>=mSaturatedValue)
		simSaturatedPad = true;

	    if(!theDaqAdcValue && !theSimAdcValue)
		continue;
	    
 	    if(theDaqAdcValue) {
		daqPixels++;
//   		cout << "Daq pad exists col/row ("
//  		     << iPad << " " << iRow << " " << theDaqAdcValue << ")" << endl;
 	    }

 	    if(theSimAdcValue) {
		simPixels++;
//   		cout << "Sim pad exists col/row ("
//  		     << iPad << " " << iRow << " " << theSimAdcValue << ")" << endl;
 	    }

	    unsigned long codedAdcValue =
		packAdc(iPad, iRow, (theSimAdcValue+theDaqAdcValue));
// 	    PR(codedAdcValue);
	    unsigned long dr, dp, dadc;
	    unPackAdc(codedAdcValue, &dp, &dr, &dadc);
// 	    cout << "DECODE p/r/adc -->" << dp << '/' << dr << '/' << dadc << endl;
		
	    //
	    // Daq only
	    //
// 	    cout << "sim/daq" << theSimAdcValue << '/' << theDaqAdcValue << endl;
	    if(!theSimAdcValue && theDaqAdcValue) {
// 		cout << "DAQ ONLY" << endl;
		mTheRichCollection->addPixel(new StRichPixel(codedAdcValue));
	    }
	    else if(theSimAdcValue) {
// 		cout << "SIM contained" << endl;
		StRichMCPixel *persistentPixel = new StRichMCPixel(codedAdcValue);
		anIDList mcInfo = dynamic_cast<StRrsReader*>(mSimStreamReader)->GetMCDetectorInfo(iPad,iRow);
		for(size_t jj=0; jj<mcInfo.size(); jj++) {
		    persistentPixel->addInfo(new StRichMCInfo(mcInfo[jj].mHitID,
							  mcInfo[jj].mG_ID,
							  mcInfo[jj].mTrackp,
							  mcInfo[jj].mCharge,
							  static_cast<unsigned short>(mcInfo[jj].mSignalType)));
		}
		mTheRichCollection->addPixel(persistentPixel);
	    }
#ifdef RCH_DEBUG
// 	    size_t whichOne =  mTheRichCollection->getRichPixels().size();
// 	    if(whichOne) {
// 		StRichPixel* tmp = mTheRichCollection->getRichPixels()[whichOne-1];
// 		cout << "last " << tmp->pad() << '/' << tmp->row() << '/' << tmp->adc() << endl;
// 	    }
#endif
	} // loop over rows (y)
    } // loop over pads (x)
    
    { LOG_DEBUG << "StRichMixerMaker-->combined : " << daqPixels << '/' << simPixels << " daq/sim pixels (" << (daqPixels+simPixels) << ")" << endm; }

#ifdef RCH_HISTOGRAM
    const StSPtrVecRichPixel& pixelStore = mTheRichCollection->getRichPixels();
    PR(pixelStore.size());

    size_t kk;
    for(kk=0; kk<pixelStore.size(); kk++) {
	mRawData[0] = pixelStore[kk]->row();
	mRawData[1] = pixelStore[kk]->pad();
	mRawData[2] = pixelStore[kk]->adc();
	mRawData[3] = mEventNumber;
	
	mPadPlane->Fill(mRawData);
    }
#endif
    

    //
    // Cleanup:  A new reader is made for each event
    //           if we are using the daq libraries
    //

    if(mSimStreamReader) {
	delete mSimStreamReader;
    }
    mSimStreamReader = 0;

    //
    // Pass the collection via the data set now as well
    // This should be removed at a later date
    //
    if(!mTheRichCollection) {
      { LOG_ERROR << "StRichMixerMaker::mTheRichCollection doesn't exist!!! %n" << "\tthis is fatal" << endm; }
      abort();
    }
    { LOG_INFO << "StRichMixerMaker --> add the pixels to the dataset"  << endm; }
    AddData(new St_ObjectSet("richMixedEvent", mTheRichCollection));
    
#ifdef RCH_DEBUG
    if(!mTheRichCollection->pixelsPresent()) {
      { LOG_WARN << "StEvent RichPixelCollection DOES NOT Exist" << endm; }
	return kStWarn;
    }
    const StSPtrVecRichPixel& thePixels = mTheRichCollection->getRichPixels();

    { LOG_DEBUG << "StRichMixerMaker:  There are " << thePixels.size() << " Rich Pixels" << endm; }
    for(size_t ii=0; ii<thePixels.size(); ii++) {
      { LOG_DEBUG << "StEvent: p/r/adc "
		  << (thePixels[ii]->pad()) << '/'
		  << (thePixels[ii]->row()) << '/'
		  << (thePixels[ii]->adc()) << endm; }
	if(dynamic_cast<StRichMCPixel*>(thePixels[ii])) {
	  { LOG_DEBUG << "MCInfo id/gid/track/q/pro" << endm; }
	    const StSPtrVecRichMCInfo& info = dynamic_cast<StRichMCPixel*>(thePixels[ii])->getMCInfo();
	    PR(info.size());
	    for(size_t i3=0; i3<info.size(); i3++) { 
	      { LOG_DEBUG << info[i3]->id() << '/'
			  << info[i3]->gid() << '/'
			  << info[i3]->trackp() << '/'
			  << info[i3]->charge() << '/'
			  << info[i3]->process() << endm; }
	    }
	}
    }
#endif
     return kStOK;

}

//-----------------------------------------------------------------
void StRichMixerMaker::PrintInfo() 
{
  { LOG_INFO << "*****************************************************************" << endm; }
  { LOG_INFO << "*  StRichMixerMaker.cxx,v 1.4 2007/04/28 17:56:49 perev Exp *" << endm; }
  { LOG_INFO << "*****************************************************************" << endm; }
    if (Debug()) StMaker::PrintInfo();
}

//-----------------------------------------------------------------


Int_t StRichMixerMaker::Finish() {

  { LOG_INFO << "StRichMixerMaker::Finish()" << endm; }

	
#ifdef RCH_HISTOGRAM
    cout << "close the Histogram files!!!!!!" << endl;
    mTupleFile->Write();
    mTupleFile->Close();
#endif
    return StMaker::Finish();
}

void StRichMixerMaker::setPedestalSubtract (int v, const char* file="/home/daq/data/crams_1172044.ped") 
{
    mPedestalSubtract = v;
    if (mPedestalSubtract) {
	mPedestalFile = file;
    }
    
}


