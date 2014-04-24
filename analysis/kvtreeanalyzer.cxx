#include <KVTreeAnalyzer.h>
#include <TApplication.h>

int main(int argc, char **argv)
{
   //without this, the plugins don't work for user classes
   TApplication *myapp=new TApplication("myapp",&argc,argv);
   new KVTreeAnalyzer(kFALSE);
   delete myapp;
   return 0;
}

