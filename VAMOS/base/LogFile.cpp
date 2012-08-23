
#include "LogFile.h"
#include <cstdlib>

//Author: Maurycy Rejmund

using namespace std;

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

