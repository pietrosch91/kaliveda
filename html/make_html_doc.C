#include "TString.h"
#include "TSystem.h"
#include "THtml.h"
#include "TROOT.h"
#include "ScanClasses.h"
#include "TApplication.h"
#include "KVBase.h"
#include "Riostream.h"
#include "MyPathDefinition.h"

int main(int argc, char *argv[])
{
   //without this, the plugins don't work for user classes
   TApplication *myapp=new TApplication("myapp",&argc,argv);
   
   TString source_dir(gSystem->DirName(gSystem->pwd()) );
   //make 'Class Reference' page
   ScanClasses categories;
   categories.SetFileName(Form("ClassCategories_%s.html", KVBase::GetKVVersion()));
   categories.SetPageTitle(Form("KaliVeda v%s Reference Guide", KVBase::GetKVVersion()));
   categories.Process();
   //make 'Examples' page
   ScanExamples examples;
   examples.SetFileName("Examples.html");
   examples.SetPageTitle("Example Classes");
   examples.Process();
   //make all individual class pages
   THtml a;
   MyPathDefinition mypath;
   a.SetPathDefinition(mypath);
   
#ifndef __WITHOUT_THTML_SETPRODUCTNAME
   a.SetProductName("KaliVeda");
#endif
	a.SetInputDir("./examples:../KVMultiDet:../KVIndra:../VAMOS:../KVSpectrometer");
	a.SetOutputDir(Form("$(KVROOT)/KaliVedaDoc/%s", KVBase::GetKVVersion()));
   a.SetHomepage("http://indra.in2p3.fr/KaliVedaDoc");
   a.MakeAll();
   a.Convert( Form("%s/etc/KaliVeda.rootrc", source_dir.Data()) , "KaliVeda configuration script $KVROOT/KVFiles/.kvrootrc");
   
   delete myapp;

   return 0;
 }
