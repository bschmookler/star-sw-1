#include <unistd.h>
#include "JevpPlotSet.h"
#include <TCanvas.h>
#include <rtsLog.h>
#include <TClass.h>
#include <TMessage.h>
#include "EvpMessage.h"
#include "Jevp/StJevpServer/EvpConstants.h"
#include <PDFUtil/PdfIndex.hh>
#include <RTS/include/SUNRT/clock.h>
#include <signal.h>
#include <RTS/include/rtsLog.h>
#include <TSystem.h>

#define NUM_INTERNALPLOTS 3

ClassImp(JevpPlotSet);

static int call_line_number = 0;
static int line_number = 0;
#define CPC call_line_number = __LINE__
#define CP line_number = __LINE__

char myname[256];

static void sigHandler(int arg, siginfo_t *sig, void *v)
{
  static char str[255];
  
  sprintf(str,"Signal %d: shutting down %s! (callline=%d line=%d)", arg, myname, call_line_number, line_number);
  LOG(ERR, "%s", str);

  exit(-1);
}

static void catchSignals(void)
{
  int i ;
  struct sigaction act ;
  
  LOG(DBG, "catching signals");

  // hook signals to my default...
  act.sa_sigaction = sigHandler ;
  act.sa_flags = SA_SIGINFO ;
  
  for(i=0;i<37;i++) {	// hook'em all!
    sigaction(i,&act,NULL) ;
  }
  
  return ;
}



JevpPlotSet::JevpPlotSet()
{
  CP;
  buildxml = NULL;
  die_at_endofrun = 0;
  hello_cmds = (char *)"client";
  diska = NULL;
  daqfile = NULL;
  server = (char *)"evp.starp.bnl.gov";
  serverport = JEVP_PORT;
  pdf = NULL;
  loglevel = NULL;
  socket = NULL;
  current_run = 0;
  update_time = 5;
  confdatadir = (char *)"/RTScache/conf/jevp";
  clientdatadir = (char *)DEFAULT_CLIENTDATADIR;
  plotsetname = (char *)"def_plotset";
  status.setStatus("stopped");
  pause = 0;
  CP;
}

int JevpPlotSet::addPlot(JevpPlot *hist)
{
  CP;
  hist->setParent(plotsetname);
  CP;
  if(plots.FindObject(hist)) {
    CP;
    LOG(CRIT,"Can't add existing histogram: %s",hist->GetPlotName());
    exit(0);
  }
  CP;
  plots.Add(hist);
  CP;
  return 0;
}

JevpPlot *JevpPlotSet::getPlot(char *name)
{
  CP;
  JevpPlot *curr = (JevpPlot *)plots.First();
  CP;

  while(curr) {
    CP;
    if(strcmp(curr->GetPlotName(), name) == 0) {
      return curr;
    }
    CP;
    curr = (JevpPlot *)plots.After(curr);
    CP;
  }
  
  CP;
  return NULL;
}

void JevpPlotSet::removePlot(char *name)
{
  CP;
  JevpPlot *curr = (JevpPlot *)plots.First();
  CP;
  while(curr) {
    CP;
    if(strcmp(curr->GetPlotName(), name) == 0) {
      CP;
      plots.Remove(curr);
      CP;
      return;
    }
    CP;
    curr = (JevpPlot *)plots.After(curr);
    CP;
  }

  CP;
  return;
}

int JevpPlotSet::getNumberOfPlots()
{
  int i=0;
  CP;
  JevpPlot *curr = (JevpPlot *)plots.First();

  while(curr) {
    i++;
    curr = (JevpPlot *)plots.After(curr);
  }

  CP;
  return i;
}

void JevpPlotSet::resetAllPlots()
{
  JevpPlot *curr = (JevpPlot *)plots.First();
  while(curr) {
    curr->reset();
    curr = (JevpPlot *)plots.After(curr);
  }
}

JevpPlot *JevpPlotSet::getPlotByIndex(int i)
{
  CP;
  i += NUM_INTERNALPLOTS;
  int idx=0;

  JevpPlot *curr = (JevpPlot *)plots.First();

  CP;

  while(curr) {
    if(i == idx) return curr;
    idx++;
    curr = (JevpPlot *)plots.After(curr);
  }

  CP;
  return NULL;
}

void JevpPlotSet::dump()
{
  JevpPlot *curr = (JevpPlot *)plots.First();

  int i=0;
  while(curr) {
    LOG("JEFF","hist[%d] = %s\n",i,curr->GetPlotName());
    i++;
    curr = (JevpPlot *)plots.After(curr);
  }
}


void JevpPlotSet::_initialize(int argc, char *argv[])
{
  char tmp[100];
  TH1 *h;
  PlotHisto *ph;

  CP;
  plotEvtsByTrigger = new JevpPlot();
  sprintf(tmp, "%s_EvtsByTrigger", getPlotSetName());
  h = new TH1I(tmp,tmp,64,0,63);
  ph = new PlotHisto();
  ph->histo = h;
  plotEvtsByTrigger->addHisto(ph);
  plotEvtsByTrigger->logy=1;
  addPlot(plotEvtsByTrigger);

  CP;
  plotTimeByTrigger = new JevpPlot();
  sprintf(tmp, "%s_TimeByTrigger", getPlotSetName());
  h = new TH1F(tmp,tmp,64,0,63);
  ph = new PlotHisto();
  ph->histo = h;
  plotTimeByTrigger->addHisto(ph);
  //plotTimeByTrigger->logy=1;
  addPlot(plotTimeByTrigger);
  
  CP;
  plotTime = new JevpPlot();
  sprintf(tmp, "%s_Time", getPlotSetName());
  h = new TH1F(tmp,tmp,100,0,.000000001);
  h->SetBit(TH1::kCanRebin);
  ph = new PlotHisto();
  ph->histo = h;
  plotTime->addHisto(ph);
  addPlot(plotTime);
  plotTime->logy=1;
  // plotTime->logx=1;

  CPC;
  initialize(argc, argv);
  CPC;

  if(buildxml) {
    buildTheXml();
  }
  CP;
}

void JevpPlotSet::initialize(int argc, char *argv[])
{
}

void JevpPlotSet::_startrun(daqReader *rdr)
{
  CP;
  strcpy(myname, plotsetname);

  CP;
  memset((char *)n_pertrg, 0, sizeof(n_pertrg));
  memset((char *)avg_time_pertrg, 0, sizeof(avg_time_pertrg));

  CP;
  run = rdr->run;
  status.setStatus("running");

  CP;
  if(base_client) {    
    status.run = rdr->run;
    status.lastEventTime = time(NULL);
    send((TObject *)&status);
  }

  CP;
  LOG(NOTE, "Here");
  plotEvtsByTrigger->getHisto(0)->histo->Reset();
  CP;
  LOG(NOTE, "Here");
  plotTimeByTrigger->getHisto(0)->histo->Reset();
  CP;
  LOG(NOTE, "Here");
  plotTime->getHisto(0)->histo->Reset();
  CP;
  LOG(NOTE, "Here");
  CPC;
  startrun(rdr);
  CPC;
  CP;
}

void JevpPlotSet::startrun(daqReader *rdr)
{
}

// This calls stoprun...
void JevpPlotSet::_stoprun(daqReader *rdr)  
{
  CP;
  CPC;
  stoprun(rdr);   // perform user actions first...
  CPC;
  CP;
  updatePlots();  // make sure plots are ready to go...
  CP;
//   if(server) {
//     printf("Sending stoprun\n");
//     EvpMessage msg;
//     msg.setSource(plotsetname);
//     msg.setCmd("stoprun");
//     char args[14];
//     sprintf(args, "%d", rdr->run);
//     msg.setArgs(args);
//     send(&msg);
//   }

  CP;
  status.setStatus("stopped");
  if(base_client) {
    LOG("JEFF", "Setting end of run #%d\n",run);
    send((TObject *)&status);
  }
  CP;
}

void JevpPlotSet::stoprun(daqReader *rdr)
{
}

void JevpPlotSet::_event(daqReader *rdr)
{
  CPC;
  event(rdr);
  CPC;

  if(pause) {
    printf("Enter enter to continue: ");
    char nothing[20];
    scanf("%s", nothing);
    pause=0;
  }
}

void JevpPlotSet::event(daqReader *rdr)
{
}

int JevpPlotSet::selectEvent(daqReader *rdr)
{
  return 1;
}

int JevpPlotSet::selectRun(daqReader *rdr)
{
  return 1;
}

char *JevpPlotSet::getPlotSetName() 
{
  return plotsetname;
}
  

void JevpPlotSet::Main(int argc, char *argv[])
{
  static unsigned int last_update = 0;

  if(parseArgs(argc, argv) < 0) {
    return;
  }



  rtsLogOutput(RTS_LOG_NET);
  rtsLogAddDest((char *)"172.16.0.1",8004);
  rtsLogLevel((char *)WARN);

  if(loglevel) rtsLogLevel(loglevel);
    
  LOG(WARN, "Starting BUILDER:%s",getPlotSetName());

  gSystem->ResetSignal(kSigChild);
  gSystem->ResetSignal(kSigBus);
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigAlarm);
  gSystem->ResetSignal(kSigUrgent);
  gSystem->ResetSignal(kSigFloatingException);
  gSystem->ResetSignal(kSigWindowChanged);
  

  catchSignals();

  LOG(DBG, "Initializing reader %s", daqfile ? daqfile : "null");

  // initialize reader
  reader = new daqReader(daqfile);

  
  // Do this after the reader...
  rtsLogOutput(RTS_LOG_NET);
  rtsLogAddDest((char *)"172.16.0.1",8004);
  rtsLogLevel((char *)WARN);
  if(loglevel) rtsLogLevel(loglevel);

  LOG(DBG, "Got a reader");

  if(diska) reader->setEvpDisk(diska);

  CP;

  if(server) {
    LOG(DBG, "Pause for a while");

    if(strcmp(hello_cmds, "steal") == 0) {
      sleep(5);
    }
    else {
      sleep(10);
    }

    while(connect(server, serverport) < 0) {
      LOG(ERR, "Error connecting to server.  Try again in 10 seconds");
      sleep(10);
    }
  }
  else {
    LOG(WARN, "No connect");
  }
  

  CP;
  RtsTimer clock;
  clock.record_time();

  // initialize client...
  _initialize(argc, argv);

  CP;
  if(server == NULL) {
    status.setStatus("stopped");
  }

  for(;;) {
    CP;
    // Lets update the options for the get call eh?

    char *ret = reader->get(0,EVP_TYPE_ANY);

    CP;
    clock.record_time();
    
    CP;
    unsigned int tm = time(NULL);
    if(tm > (last_update + update_time)) {
      if(socket && (status.running())) {
	updatePlots();
	last_update = tm;
      }
    }

    CP;
    if(ret == NULL) {  // all kinds of burps...
      switch(reader->status) {

      case EVP_STAT_OK:  
	CP;
	LOG(DBG, "EVP_STAT_OK");
	continue;

      case EVP_STAT_EOR:
	CP;
	LOG(NOTE, "EVP_STAT_EOR stat=%s",status.status);

	if(!status.running()) {
	  LOG(NOTE, "Already end of run, don't stop it again... %d",status.running());
	  // already end of run, don't stop it again...
	  sleep(5);
	  continue;
	}
	

	LOG(NOTE, "EOR");
	_stoprun(reader);
	LOG(DBG, "Stoprun");

	if(reader->IsEvp()) {
	  LOG("JEFF", "End of Run... [previous run=%d, current run=%d]",
	      current_run, reader->run);
	}
       
	CP;
	if(pdf) writePdfFile();  // Finish and exit...
	CP;
	
	if(die_at_endofrun) {
	  LOG("JEFF", "It's end of run and set to die, goodbye");
	  exit(0);
	}

	if(!reader->IsEvp()) {
	  LOG("JEFF", "It's end of run and the data source is a file, so exiting");
	  exit(0);
	}

	continue;    // don't have an event to parse... go back to start

      case EVP_STAT_EVT:
	CP;
	LOG(WARN, "Problem reading event... skipping");
	sleep(1);
	continue;

      case EVP_STAT_CRIT:
	CP;
	LOG(CRIT, "Critical problem reading event... exiting");
	exit(0);
      }

    }
    
    CP;
    if(reader->status) {
      LOG("JEFF", "bad event: status=0x%x",reader->status);
      continue;
    }

    CP;
    LOG(DBG, "We've got some kind of event:  token=%d seq=%d",reader->token,reader->seq);

    if(reader->run != current_run) {
      LOG("JEFF", "Got an event for a run change: prev run=%d new run=%d",
	  current_run, reader->run);
      
      current_run = reader->run;
      _startrun(reader);
    }
   
    CP;
    if(!selectRun(reader) || !selectEvent(reader)) {
      CP;
      LOG(NOTE, "Event doesn't contribute...");

      double t = clock.record_time();

      for(int i=0;i<32;i++) {

	if(reader->daqbits & (1<<i)) {
	  int idx=2*i+1;
	  
	  double x = n_pertrg[idx]*avg_time_pertrg[idx];
	  x += t;
	  x /= (n_pertrg[idx]+1);

	  avg_time_pertrg[idx] = x;
	  n_pertrg[idx]++;

	  // EvtsByTrg
	  (plotEvtsByTrigger->getHisto(0)->histo)->Fill(idx);
	  // AvgTime
	  (plotTimeByTrigger->getHisto(0)->histo)->Fill(idx,avg_time_pertrg[idx]);

	  //printf("idx=%d avg=%lf\n",idx, avg_time_pertrg[idx]);
	}
      }

      continue;
    }
   
    CP;
    LOG(NOTE, "Call user code");
    _event(reader);   // process...
    LOG(NOTE, "Done with user code");

    CP;
    double t = clock.record_time();

    for(int i=0;i<32;i++) {
      CP;
      if(reader->daqbits & (1<<i)) {
	int idx=2*i;
	  
	LOG(NOTE,"idx = %d",idx);
	double x = n_pertrg[idx]*avg_time_pertrg[idx];
	x += t;
	x /= n_pertrg[idx]+1;

	avg_time_pertrg[idx] = x;
	n_pertrg[idx]++;
	
	// EvtsByTrg
	(plotEvtsByTrigger->getHisto(0)->histo)->Fill(idx);
	// AvgTime
	LOG(DBG, "avg time: %d %lf %lf",idx, avg_time_pertrg[idx], n_pertrg[idx]);
	(plotTimeByTrigger->getHisto(0)->histo)->Fill(idx,avg_time_pertrg[idx]);
      }
    }

    CP;
    // Time...    
    ((TH1F *)plotTime->getHisto(0)->histo)->Fill(t);
    CP;
  }
}


// Private functions...

int JevpPlotSet::parseArgs(int argc, char *argv[])
{
  CP;
  for(int i=1;i<argc;i++) {
    if(memcmp(argv[i], "-diska", 6) == 0) {
      i++;
      diska = argv[i];
    }
    else if (memcmp(argv[i], "-file", 5) == 0) {
      i++;
      daqfile = argv[i];
      server = NULL;
    }
    else if (memcmp(argv[i], "-noserver", 9) == 0) {
      server = NULL;
    }
    else if (memcmp(argv[i], "-server", 7) == 0) {
      i++;
      server = argv[i];
    }
    else if (memcmp(argv[i], "-die", 4) == 0) {
      die_at_endofrun = 1;
    }
    else if (memcmp(argv[i], "-steal", 6) == 0) {
      hello_cmds = (char *)"steal";
    }
    else if (memcmp(argv[i], "-port", 5) == 0) {
      i++;
      serverport = atoi(argv[i]);
    }
    else if (memcmp(argv[i], "-pdf", 4) == 0) {
      i++;
      static char pdf_buff[256];
      pdf_buff[0] = '\0';

      if(argv[i][0] != '/') {	
	getcwd(pdf_buff, 256);
	strcat(pdf_buff, "/");
      }
      strcat(pdf_buff, argv[i]);
      pdf = pdf_buff;
    }
    else if (memcmp(argv[i], "-loglevel", 9) == 0) {
      i++;
      loglevel = argv[i];
    }
    else if (memcmp(argv[i], "-confdatadir", 12) == 0) {
      i++;
      confdatadir = argv[i];
    }
    else if (memcmp(argv[i], "-clientdatadir", 15) == 0) {
      i++;
      clientdatadir = argv[i];
    }
    else if (strcmp(argv[i], "-pause") == 0) {
      pause = 1;
    }
    else if (strcmp(argv[i], "-buildxml") == 0) {
      i++;
      buildxml = argv[i];
    }
    else {
      printf("No arg #%d = %s\n",i,argv[i]);
      printf("%s arguments\n\t-diska diskapath\n\t-file filename\n\t-noserver\n\t-server servername\n\t-port port\n\t-pdf pdffilename\n\t-loglevel level\n\t-datadir datadir (/RTScache/conf/jevp)\n\t-clientdatadir datadir (/a/jevp/client)",argv[0]);
      printf("\n\t-steal   (be base plot builder)\n\t-die   (die at end of run)\n");
      printf("\t-pause\n\t-buildxml <file>\n");
      CP;
      return -1;
    }    
  }
  CP;
  return 0;
}

int JevpPlotSet::updatePlots()
{
  CP;
  if(!server) return 0;

  CP;
  JevpPlot *curr = (JevpPlot *)plots.First();

  while(curr) {
    curr->run = current_run;
    send(curr);
    curr = (JevpPlot *)plots.After(curr);
  }

  CP;
  return 0;
}

void JevpPlotSet::buildTheXml()
{
  JevpPlot *curr = (JevpPlot *)plots.First();

  int i=0;
  while(curr) {
    //printf("curr->GetPlotName():  %s\n",curr->GetPlotName());

    i++;

    printf("<tab>%s<canvas><wide>1</wide><deep>1</deep>\n",curr->GetPlotName());
    printf("        <histogram>%s</histogram>\n",curr->GetPlotName());
    printf("</canvas></tab>\n");


    curr = (JevpPlot *)plots.After(curr);
  }

  exit(0);
}

void JevpPlotSet::writePdfFile()
{
  LOG(DBG,"writing pdf\n");

  PdfIndex index;
  int page = 1;

  // TCanvas canvas("c1","c1",(400*4)/3,400);
  
  JevpPlot *curr = (JevpPlot *)plots.First();

  char firstname[256];
  char lastname[256];

  strcpy(firstname, pdf);
  strcat(firstname, "(");
  strcpy(lastname, pdf);
  strcat(lastname, ")");

  //tonkoLogLevel = 0;

  while(curr) {  

    TCanvas *canvas = new TCanvas("c1","c1", 500, 400);

    LOG("DBG", "page=%d curr->GetPlotName()=%s",page,curr->GetPlotName());

    index.add(NULL,curr->GetPlotName(),page++,0);
    
    LOG(DBG, "Here");
    curr->draw();
    LOG(DBG, "About to print:  before=0x%x after=0x%x",plots.Before(curr),plots.After(curr));

    if(plots.Before(curr) == NULL) {  
      LOG(DBG, "b: print %s",firstname);
      canvas->Print(firstname,"pdf,Portrait");
    }
    else if (plots.After(curr) == NULL) {
      LOG(DBG, "a: print %s",lastname);
      canvas->Print(lastname,"pdf,Portrait");
    }
    else {
      LOG(DBG, "print %s",pdf);
      canvas->Print(pdf,"pdf,Portrait");
    }

    curr = (JevpPlot *)plots.After(curr);
    LOG(DBG, "curr = 0x%x",curr);

    delete canvas;
  }

  LOG(NOTE, "Done with pdf");

  index.CreateIndexedFile(pdf,pdf);

  LOG(NOTE, "Done with index");
  return;
}

int JevpPlotSet::connect(char *host, int port) {

  LOG(WARN, "Connecting...to %s:%d",host,port);

  socket = new TSocket(host,port);
  if(!socket->IsValid()) {
    LOG(ERR, "Error connecting to server: %d",socket->GetErrorCode());

    delete socket;
    socket = NULL;
    return -1;
  }

  LOG(DBG, "Hello Message: (%s) %s",plotsetname,hello_cmds);
  EvpMessage m;
  LOG(DBG, "C.");
  m.setSource(plotsetname);
  LOG(DBG, "A.");
  m.setCmd((char *)"hello");
  LOG(DBG, "B.");
  m.setArgs(hello_cmds);
 

  LOG(DBG, "Send...");
  send(&m);

  LOG(DBG, "Recieving hello message");
  TMessage *mess;
  int ret = socket->Recv(mess);
  if( ret == 0) {
    LOG(ERR, "Couldn't connect to server...");
    return -1;
  }

  LOG(DBG, "Getting hello message out...");
  if(strcmp(mess->GetClass()->GetName(), (char *)"EvpMessage") != 0) {
    LOG(ERR, "Invalid message returned:  %s",mess->GetClass()->GetName());
    return -1;
  }

  EvpMessage *msg = (EvpMessage *)mess->ReadObject(mess->GetClass());
  
  LOG(DBG, "Hello message is: %s",msg->args);

  if(strcmp(msg->args, "base") == 0) {
    LOG("JERR", "%s:  I am the new base client", plotsetname);
    base_client = 1;
  }
  else {
    LOG("JERR", "%s:  I am not the base client", plotsetname);
    base_client = 0;
  }

  delete(mess);
  return 0;
}  

int JevpPlotSet::send(TObject *msg) {
  if(!server) return 0;

  TMessage mess(kMESS_OBJECT);
  
  mess.WriteObject(msg);
  socket->Send(mess);
  return 0;
}
