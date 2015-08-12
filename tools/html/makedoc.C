#include "THtml.h"
#include "TSystem.h"
#include "KVBase.h"
#include "KVString.h"
#include "KVPathDef.h"
#include "KVFileDef.h"

void makedoc(const TString& output_dir = "htmldoc")
{
   // load all libraries and generate class reference
   THtml h;
   h.SetProductName(Form("KaliVeda v%s",KVBase::GetKVVersion()));
   TString source_root = KVBase::GetKVSourceDir();
   h.SetInputDir(source_root);
   TString version = KVBase::GetKVVersion();
   TString maj_vers = version;
   maj_vers.Remove(maj_vers.Index("/"));
   version.ReplaceAll("/",".");
   h.SetOutputDir(output_dir);
   h.SetViewCVS(Form("https://github.com/kaliveda-dev/kaliveda/blob/%s/",maj_vers.Data()));
   h.SetPathDefinition(KVPathDef());
   h.SetFileDefinition(KVFileDef());
   
   // "hide" the CMakeCache.txt & install_manifest.txt files
   // or links will be generated for them under "Further reading..."
   TString path = TString(KVBase::GetKVBuildDir()) + "/";
   TString f1 = path + "CMakeCache.txt";
   TString f2 = path + "install_manifest.txt";
   TString f11 = f1 + ".bck";
   TString f22 = f2 + ".bck";
   gSystem->Rename(f1,f11);
   gSystem->Rename(f2,f22);
   
   h.MakeAll();
   
   gSystem->Rename(f11,f1);
   gSystem->Rename(f22,f2);
}
