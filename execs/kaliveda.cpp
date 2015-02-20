#include "TRint.h"
#include "KVBase.h"

int main(int argc, char **argv)
{
   TRint *myapp=new TRint("kaliveda",&argc,argv,NULL,0);
   KVBase::InitEnvironment();
   myapp->Run();
   return 0;
}
