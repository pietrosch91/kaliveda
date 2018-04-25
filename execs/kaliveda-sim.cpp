#include "TApplication.h"
#include "KVSimDirGUI.h"

int main(int argc, char** argv)
{
   //TRint* myapp = new TRint("kaliveda", &argc, argv, NULL, 0);
   TApplication* myapp = new TApplication("myapp", &argc, argv);
   new KVSimDirGUI;
   myapp->Run();
   return 0;
}
