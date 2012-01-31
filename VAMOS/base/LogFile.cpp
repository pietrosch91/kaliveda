
#include "LogFile.h"
#include <cstdlib>

//Author: Maurycy Rejmund

ClassImp(LogFile)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Logfile</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

LogFile::LogFile(void)
{
#ifdef DEBUG
  cout << "LogFile::Constructor" << endl;
#endif
}
LogFile::~LogFile(void)
{
#ifdef DEBUG
  cout << "LogFile::Destructor" << endl;
#endif
  Close();
}

void LogFile::Open(char *LogFileName)
{
#ifdef DEBUG
  cout << "LogFile::Open" << endl;
#endif
  
  Log.open(LogFileName);
  if(!Log)
  {
    cout << "Could not open the LogFile: " << LogFileName << " !" << endl;
     exit(EXIT_FAILURE);
  }
  else
  {
    cout << "LogFile: " << LogFileName << " opened." << endl;
  }

  Log.setf(ios::showpoint);

}
void LogFile::Close(void)
{
#ifdef DEBUG
  cout << "LogFile::Close" << endl;
#endif
  
  Log.close();
  
}

void LogFile::Message(const char *location, const char *msg){ // Added by Peter (may be useful)

    if(Log.good() == 1){
        Log << "[" << this->GetTime() << "][" << location << "] " << msg << endl;
    }else{
        cerr << "Stream not good for I/O operations" << endl;
    }
}

string LogFile::GetTime(){

    string dateString;

    struct tm *result = new struct tm;
    time_t *t = new time_t;
    time(t);

    localtime_r((const time_t*)t, result);

    stringstream dateStringStream;
    dateStringStream
            << setfill('0') << setw(2) << result->tm_hour << ":"
            << setfill('0') << setw(2) << result->tm_min << ":"
            << setfill('0') << setw(2) << result->tm_sec;

    dateString = dateStringStream.str();

    delete t;
    delete result;

    return dateString;
}
