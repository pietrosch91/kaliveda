#include "KVMFMDataFileReader.h"

int main(int argc, char** argv)
{
   KVMFMDataFileReader* fr = KVMFMDataFileReader::Open("$FAZIA_DATA_DIR/run_0048.dat.19-04-18_15h17m46s");
   cout << fr->GetNextEvent() << endl;
   fr->PrintFrameRead();
   cout << fr->GetNextEvent() << endl;
   fr->PrintFrameReadHeader();
}
