//Created by KVClassFactory on Fri Sep  9 14:27:28 2011
//Author: eindra

#ifndef __KVINDRAONLINEDATAANALYSER_H
#define __KVINDRAONLINEDATAANALYSER_H

#include "KVINDRARawDataAnalyser.h"

class GSpectra;
class GNetServerRoot;
class TH1;
class TThread;
class TSocket;

class KVINDRAOnlineDataAnalyser : public KVINDRARawDataAnalyser
{
   Int_t events;
   int last_events;
	TDatime fStart;
   Int_t fEventsRefresh;

  GSpectra* fSpectraDB;
  GNetServerRoot* fSpectraServer;
   virtual void ProcessRun();
	Bool_t fGoEventLoop;
	Int_t port;
         
	static void ecouteSockets(void*);
	TThread* fMessageThread;
	void HandleCommands(TString&, TSocket*);
	
   public:
   KVINDRAOnlineDataAnalyser();
   virtual ~KVINDRAOnlineDataAnalyser();

   void preInitAnalysis();
   void preInitRun();
   void postAnalysis();
   void AddHisto(TH1*, const Char_t* /* family */ = 0);
	
	virtual void PrintControlRate();

   ClassDef(KVINDRAOnlineDataAnalyser,1)//For on-line analysis of INDRA data
};

#endif
