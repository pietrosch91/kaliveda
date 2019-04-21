#include "KVDataSetManager.h"
#include "KVExpDB.h"
#include "KVDataSet.h"

#include <TApplication.h>

int main(int argc, char* argv[])
{
   if (argc < 3) {
      cout << "Usage: get_total_events [first_run] [last_run]" << endl << endl;
      exit(1);
   }
   TApplication* myapp = new TApplication("myapp", &argc, argv);
   TString r1(argv[1]), r2(argv[2]);
   KVBase::InitEnvironment();
   KVDataSetManager dsm;
   dsm.Init();
   dsm.GetDataSet("INDRAFAZIA.E789")->cd();
   //myapp->Run();
   return (int)gExpDB->GetTotalEvents(r1.Atoi(), r2.Atoi());
}
