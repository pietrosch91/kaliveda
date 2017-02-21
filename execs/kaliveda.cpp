#include "TRint.h"
#include "KVBase.h"
#include <iostream>

int main(int argc, char** argv)
{
   // kaliveda command-line interpreter
   //
   //  kaliveda --gitinfos: print git branch name and commit and exit
   //  kaliveda --gitbranch: print git branch name and exit
   //  kaliveda --gitcommit: print git commit and exit

   KVBase::InitEnvironment();
#ifdef WITH_GIT_INFOS
   for (int i = 0; i < argc; ++i) {
      if (!strcmp(argv[i], "--gitinfos")) {
         std::cout << KVBase::gitBranch() << "@" << KVBase::gitCommit() << std::endl;
         return 0;
      } else if (!strcmp(argv[i], "--gitbranch")) {
         std::cout << KVBase::gitBranch() << std::endl;
         return 0;
      }
      if (!strcmp(argv[i], "--gitcommit")) {
         std::cout << KVBase::gitCommit() << std::endl;
         return 0;
      }
   }
#endif
   TRint* myapp = new TRint("kaliveda", &argc, argv, NULL, 0, kTRUE);
   KVBase::PrintSplashScreen();
   myapp->SetPrompt("kaliveda [%d] ");
   myapp->Run();
   return 0;
}
