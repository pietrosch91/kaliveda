#include "KVList.h"

ClassImp(KVList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVList</h2>
<h4>Modified TList class</h4>
<h3>Object ownership</h3>
The KVList owns it objects by default: any objects placed in a KVList
will be deleted when the KVList is deleted, goes out of scope, or if
methods Clear() or Delete() are called. To avoid this, either use the
argument to the constructor:<br>
<pre>KVList list_not_owner(kFALSE);//list does not own its objects<br></pre>
or use the SetOwner() method to change the ownership.<br>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
