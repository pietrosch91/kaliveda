/*
$Id: ScanClasses.cpp,v 1.6 2009/03/27 16:43:40 franklan Exp $
$Revision: 1.6 $
$Date: 2009/03/27 16:43:40 $
*/

//Created by KVClassFactory on Mon Jun 25 18:25:35 2007
//Author: franklan

#include "ScanClasses.h"
#include <TClassTable.h>
#include <TClass.h>
#include <THtml.h>
#include <TROOT.h>
#include <TObjString.h>
#include "KVBase.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include <TError.h>

using namespace std;

//ClassImp(ScanClasses)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>ScanClasses</h2>
<h4>Scan KaliVeda classes and produce a HTML page listing them with links to doc</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

ScanClasses::ScanClasses()
{
   //Default constructor
   fMaxDottyCols = 40;
   fClassTitles = 0;
   fClassList = 0;
   cnames = 0;
   nclasses=0;
}

ScanClasses::~ScanClasses()
{
   //Destructor
   if(fClassTitles) {
	  delete fClassTitles;
   }
   if(fClassList) {
	  delete fClassList;
   }
   if(cnames){
      delete cnames;
   }
}

void ScanClasses::Process()
{
   //Call this method after SetFileName and SetPageTitle in order to
   //analyse the classes and write the HTML page.
   MakeClassCategoriesList();
   FillListOfClasses();
   AnalyseClasses();
   WritePage();
}

const Char_t* ScanClasses::HTMLFileName(const Char_t* classname)
{
    //Takes classname given by TClass::GetName() and generates name of HTML file containing class description.
    //In most cases this is "ClassName.html", but for templated classes the class name is e.g. "TemplateClass<int>",
    //while the HTML file name is "TemplateClass_int_.html"
    //So we look for "<" and ">" in the class name, and replace them with "_"
    //Also replace any whitespace with "_"
    
    fTempString = classname;
    fTempString.Substitute('<', '_'); fTempString.Substitute('>', '_'); fTempString.Substitute(' ', '_');
    fTempString += ".html";
    return fTempString.Data();
}

const Char_t* ScanClasses::HTMLSafeLink(const Char_t* classname)
{
    //Takes classname given by TClass::GetName() and generates name of link to HTML file containing class description.
    //In most cases this is simply "ClassName", but for templated classes the class name is e.g. "TemplateClass<int>",
    //which is not valid HTML.
    //So we look for "<" and ">" in the class name, and replace them with "&ltt;" and "&gt;", respectively.
    fTempString = classname;
    fTempString.ReplaceAll( "<", "&lt;"); fTempString.ReplaceAll( ">", "&gt;" );
    return fTempString.Data();
}

void ScanClasses::MakeClassCategoriesList()
{
   //make a list of KVBase objects which are used to translate subdirectory
   //names from class implementation filenames ("particles", "daq_cec", etc.)
   //into human-readable category names ("Particles & Nuclei" ,
   //"Data acquisition & Detector Command and Control", etc.)
   
   if(fClassTitles) {
	  delete fClassTitles;
   }
	fClassTitles = new TList;
   fClassTitles->SetOwner();
	fClassTitles->Add( new KVBase("base", "Base Classes"));
	fClassTitles->Add( new KVBase("particles", "Particles & Nuclei"));
	fClassTitles->Add( new KVBase("events", "Multiparticle Events"));
	fClassTitles->Add( new KVBase("detectors", "Absorbers, Targets & Detectors"));
	fClassTitles->Add( new KVBase("stopping", "Stopping Power & Range Tables for Heavy Ions"));
	fClassTitles->Add( new KVBase("geometry", "Multidetector Geometry"));
	fClassTitles->Add( new KVBase("indra", "INDRA Multidetector Array"));
	fClassTitles->Add( new KVBase("data_management", "Data Management"));
	fClassTitles->Add( new KVBase("analysis", "Data Analysis"));
	fClassTitles->Add( new KVBase("db", "Database Classes"));
	fClassTitles->Add( new KVBase("idtelescopes", "Charged Particle Identification Telescopes"));
	fClassTitles->Add( new KVBase("idmaps", "Identification Maps & Grids"));
	fClassTitles->Add( new KVBase("identification", "Tools for Particle Identification"));
	fClassTitles->Add( new KVBase("calibration", "Detector Calibration"));
	fClassTitles->Add( new KVBase("daq_cec", "Data acquisition & Detector Command and Control"));
	fClassTitles->Add( new KVBase("vg_base", "Global Variables: Base Classes"));
	fClassTitles->Add( new KVBase("vg_energy", "Global Variables: Energies"));
	fClassTitles->Add( new KVBase("vg_momentum", "Global Variables: Momenta"));
	fClassTitles->Add( new KVBase("vg_multiplicity", "Global Variables: Multiplicities"));
	fClassTitles->Add( new KVBase("vg_shape", "Global Variables: Event Shape"));
	fClassTitles->Add( new KVBase("vg_charge", "Global Variables: Partitions"));
	fClassTitles->Add( new KVBase("trieur", "Event Sorting"));
	fClassTitles->Add( new KVBase("gui", "Graphical User Interfaces"));
}

void ScanClasses::FillListOfClasses()
{
   //Fill 'cnames' with names of all classes to be processed
   
   // force loading of all libraries (KLUDGE!!!)
   gSystem->Load("libKVIndra");
   gSystem->Load("libVAMOS");
   gSystem->Load("libKVSpectrometer");
	//Initialise the class table, loading all classes defined in currently-loaded libraries
	gClassTable->Init();
   //first fill TObjArray of TObjStrings containing all class names in class table
   //this is to avoid using gClassTable->Next() and gROOT->GetClass() at the same time
   //(see TROOT::GetClass doc)
   if(cnames) delete cnames;
   int SizeClassTable = gClassTable->Classes();
   cnames=new TObjArray(SizeClassTable);
   cnames->SetOwner();
   for (int i = 0; i < SizeClassTable; i++) {
      //add entry with name of each KaliVeda class in table
      TObjString* tos=new TObjString(gClassTable->Next());
      TString cl_name = tos->String();
      cout << cl_name << endl;
      TClass* cc = TClass::GetClass(cl_name,kFALSE);
      cout << cc << endl;
      const char* libs = TClass::GetClass(cl_name)->GetSharedLibs();
      cout << libs << endl;
      TString shared_libs = "";
      if(libs) shared_libs = libs;
		if( shared_libs.Contains("libKVMultiDet.so") ){
         cnames->Add(tos);
      }
      else
      {
         delete tos;
      }
   }
}

void ScanClasses::AnalyseClasses()
{
   //Analyse list of class names in 'cnames' and fill the fClassList categorized lists
   
   nclasses=0;
   if(fClassList) {
	  delete fClassList;
   }
	fClassList = new TList;
   fClassList->SetOwner();
	for(int i=0; i<cnames->GetEntries(); i++){
		
		TString cl_name = ((TObjString*)(*cnames)[i])->String();
					            
			nclasses++;
			
		    	//get class definition
			TClass* cl = TClass::GetClass( cl_name.Data() );
			TString  dec_f( cl->GetDeclFileName() );
			//extract subdirectory name from class declaration filename
			TObjArray* toks = dec_f.Tokenize('/');
			int arr_size = toks->GetEntries();
			TString rep("."); int n=0;
			while( rep=="."  && n<arr_size ){
				rep = ((TObjString*)toks->At(n++))->GetString();
			}
			delete toks;
			//we create sub-lists in the list of classes, each having the name of the subdirectory
			//all classes are thus sorted into categories depending on the subdirectory in which they are declared
         // rep = name of subdirectory from class implemenation filepath (="base", "analysis", etc.)
			TList* list=(TList*)fClassList->FindObject( rep.Data() );
			if(list){
				list->Add( cl );
			} else {
				TList *l = new TList;
				l->SetName( rep.Data() );
				fClassList->Add( l );
				l->Add( cl );
			}
		
	}
   
   //delete TObjArray of class names
   delete cnames; cnames=0;
}

void ScanClasses::WritePage()
{
   //Write HTML page presenting categorized lists of classes
   
   list_file.open( fFilename.Data() );
	list_file << "<html><head>"<<endl;
	list_file << "<title>" << fPageTitle.Data() << "</title><link rel=\"stylesheet\" href=\"css/site.css\" type=\"text/css\">" << endl;
	list_file << "<link rel=\"icon\" href=\"images/favicon.ico\" type=\"image/x-icon\" />"<<endl;
	list_file << "<link rel=\"shortcut icon\" href=\"images/favicon.ico\" type=\"image/x-icon\" />"<<endl;
	list_file << "</head>"<<endl;
	list_file << "<body>";
	list_file << "<h1>" << fPageTitle.Data() << " <it>(" << nclasses << " classes)</it></h1>" << endl;
	list_file << "<div class=\"leftSide\">" << endl;
   list_file <<"<div class=\"KaliVeda\">KaliVeda</div>" << endl;
   list_file << "<img style=\"margin-left: 1px; margin-top: 1px; width: 110px; height: 100px;\" alt=\"indra\" src=\"images/indra_huguet_210x210.png\"><br>" << endl;
   list_file << "<script language=\"javascript\" src=\"js/nav.js\"></script>" << endl;
   list_file << "<br>" << endl;
   list_file << "<!-- SiteSearch Google -->" << endl;
   list_file << "<form method=\"get\" action=\"http://www.google.com/search\"><input" <<endl;
   list_file << " name=\"ie\" value=\"UTF-8\" type=\"hidden\"> <input name=\"oe\" value=\"UTF-8\"" << endl;
   list_file << " type=\"hidden\"><input name=\"q\" style=\"margin-top: 5px;\" size=\"13\" maxlength=\"255\" value=\"\"" << endl;
   list_file << " type=\"text\"> <input name=\"btnG\" style=\"margin-top: 5px;\" value=\"Search site\"" << endl;
   list_file << " onclick=\"send_data(this.form)\" type=\"submit\"></form>" << endl;
   list_file << "<!-- SiteSearch Google -->" << endl;
   list_file << "</div><div class=\"page\"><br>" << endl;
   
   //Write user top of page blurb if required, above links to categories
   WritePageHead();

   //write links to class categories
	TIter next_rep(fClassTitles); KVBase* class_title;
	while( (class_title = (KVBase*)next_rep()) ){
      
		TList* list = (TList*)fClassList->FindObject(class_title->GetName());
		
		if(!list){
			continue;
		}
		// instead of linking to list on this page, we link to the THtml-generated Index page for
		// this module. This may have extra documentation.
		TString index_file(class_title->GetName());
		index_file.ToUpper();
		index_file.Append("_Index.html");
//      list_file << "<h4><a href=\"#_" << class_title->GetName() << "\">"<< class_title->GetTitle() << "</a></h4>" << endl;
      list_file << "<h4><a href=\"" <<KVBase::GetKVVersion()<<"/"<< index_file.Data() << "\">"<< class_title->GetTitle() << "</a></h4>" << endl;
   }
   next_rep.Reset();
   list_file <<"<br>" <<endl;
   
   //write full list of classes by category
	while( (class_title = (KVBase*)next_rep()) ){
		
		TList* list = (TList*)fClassList->FindObject(class_title->GetName());
		
		if(!list){
			continue;
		}
		
		list_file << "<h3 class=\"newsTitle\">" ;
		list_file << "<a name=\"_" << class_title->GetName() <<"\">";
		TString index_file(class_title->GetName());
		index_file.ToUpper();
		index_file.Append("_Index.html");
		
		list_file << "<a href=\"" <<KVBase::GetKVVersion()<<"/"<< index_file.Data() << "\">" << class_title->GetTitle() << "</a>";
		
		list_file << " </a><it>(" << list->GetSize() << " class";
		if(list->GetSize()>1) list_file << "es";
		list_file << ")</it>:</h3><ul>" << endl;
	
		TIter next_class(list); TClass* cl;
		while( (cl = (TClass*)next_class()) ){
			
			// class doc files are in subdirectory starting with version name
			// i.e. in $KVROOT/KaliVedaDoc/1.7.4/
			list_file << "<li><tt><a href=\"" << HTMLFileName(Form("%s/%s", KVBase::GetKVVersion(), cl->GetName())) << "\">";
			
			list_file << HTMLSafeLink(cl->GetName()) << "</a>";
			
			//now add dots and class description
			TString link(cl->GetName());
			Int_t ndots = fMaxDottyCols - link.Length();//dots+classlink = MAX_DOTTY_COLS
			for(register int i=0;i<ndots;i++) list_file << ".";
			list_file << cl->GetTitle();
			
			list_file << "</tt><br>" << endl;
			
		}
		list_file << "</ul>" <<endl;
	}
	
	list_file << "</div></body></html>" << endl;
	list_file.close();
}

void ScanClasses::WritePageHead()
{
   list_file << "<b>Last updated: ";
   TDatime now;
   list_file << now.AsString();
   list_file << "</b><br>" << endl;
   list_file << "Revision#" << KVBase::bzrRevisionNumber()<<"  ["<< KVBase::bzrRevisionDate() << "]<br>"<< endl;
   list_file << "<I>"<<KVBase::bzrBranchNick()<<"</I><br><br>"<< endl;
}

//ClassImp(ScanExamples)
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>ScanExamples</h2>
<h4>Scan example classes and produce a HTML page listing them with links to doc</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void ScanExamples::FillListOfClasses()
{
   //Fill 'cnames' with names of all classes to be processed
   //We scan the "examples" directory and compile every class in it
   //We identify the classes from their ".h" file
   
	TString pwd = gSystem->WorkingDirectory();//keep work dir
   TSystemDirectory ex_dir("ex_dir", "examples");
   TList *file_list = ex_dir.GetListOfFiles();
   
   TIter next_file(file_list);
   TNamed* filename;
   if(cnames) delete cnames;
   cnames=new TObjArray;
   cnames->SetOwner();
   while( (filename = (TNamed*)next_file()) ){
      
      TString name = filename->GetName();
            
      if(name.EndsWith(".h")){
         
         //strip '.h' = classname
         name.Remove( name.Index('.'), 2 );
         
         //make sure we have implementation & header file for class
         KVString ImpFile, DecFile;
         if( KVBase::FindClassSourceFiles( name.Data(), ImpFile, DecFile ) ){
            
            //OK: compile the class
            if( !gROOT->LoadMacro( Form("%s+", ImpFile.Data()) ) ){
               
               cnames->Add( new TObjString( name.Data() ) );
               
            }
         }
      }
   }
   
   delete file_list;
	gSystem->cd(pwd.Data());//change back to work dir after implicit 'cd' of GetListOfFiles...
}

void ScanExamples::AnalyseClasses()
{
   //Analyse list of class names in 'cnames' and fill the fClassList categorized lists
   
   nclasses=0;
   if(fClassList) {
	  delete fClassList;
   }
	fClassList = new TList;
   fClassList->SetOwner();
	for(int i=0; i<cnames->GetEntries(); i++){
		
		TString cl_name = ((TObjString*)(*cnames)[i])->String();
					            
			nclasses++;
			
		    	//get class definition
			TClass* cl = TClass::GetClass( cl_name.Data() );
         
         //get base class definition
         TList* bases = cl->GetListOfBases();
         TClass *baseclass = TClass::GetClass( bases->At(0)->GetName() );
         
			//extract subdirectory name from base class declaration filename
			TString  dec_f( baseclass->GetDeclFileName() );
			TObjArray* toks = dec_f.Tokenize('/');
			int arr_size = toks->GetEntries();
			TString rep("."); int n=0;
			while( rep=="."  && n<arr_size ){
				rep = ((TObjString*)toks->At(n++))->GetString();
			}
			delete toks;
			//we create sub-lists in the list of classes, each having the name of the subdirectory
			//all classes are thus sorted into categories depending on the subdirectory in which they are declared
         // rep = name of subdirectory from base class implementation filepath (="base", "analysis", etc.)
			TList* list=(TList*)fClassList->FindObject( rep.Data() );
			if(list){
				list->Add( cl );
			} else {
				TList *l = new TList;
				l->SetName( rep.Data() );
				fClassList->Add( l );
				l->Add( cl );
			}
		
	}
   
   //delete TObjArray of class names
   delete cnames; cnames=0;
}

void ScanExamples::WritePageHead()
{
   list_file << "<p>The following list of classes give examples of data analysis classes," << endl;
   list_file << "data selector classes and user global variable classes. Use them for inspiration," << endl;
   list_file << "or if you're lucky, maybe one of them does exactly what you want already!</p>" << endl;
}
