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
   h.MakeAll();
}
