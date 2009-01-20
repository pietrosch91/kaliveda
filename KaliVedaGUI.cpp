//
// Standalone program launching the KVDataAnalysisLauncher 
//

#include "TApplication.h"
#include "TRint.h"
#include "Riostream.h"
#include "KVDataAnalysisLauncher.h"
#include "KVDataRepositoryManager.h"

//#define WITHRINT

int main(int argc, char *argv[])
{
//cout << "Creating myapp" << endl;
#ifdef WITHRINT
TRint *myapp=new TRint("RootSession",&argc,argv,NULL,0);
#else
TApplication *myapp=new TApplication("myapp",&argc,argv);
#endif
gSystem->AddIncludePath("-I$KVROOT/include");
//cout << "Launching GUI" << endl;
new KVDataAnalysisLauncher(gClient->GetRoot());
myapp->SetReturnFromRun(kTRUE);
myapp->Run();
if(gDataRepositoryManager) delete gDataRepositoryManager;
return 0;
}
