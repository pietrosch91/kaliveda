#ifndef _LOGFILE_CLASS
#define _LOGFILE_CLASS

#include "Rtypes.h"
#include"Defines.h"
#include "Riostream.h"

class LogFile
{
 public:
  LogFile();
  virtual ~LogFile(void);
  
  
  Char_t LogFileName;
  std::ofstream Log;
  void Open(char *LogFileName); 
  void Close();

  ClassDef(LogFile,0)
};

#endif

