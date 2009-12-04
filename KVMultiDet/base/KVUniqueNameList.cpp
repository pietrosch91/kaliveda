//Created by KVClassFactory on Thu Dec  3 15:49:33 2009
//Author: John Frankland,,,

#include "KVUniqueNameList.h"

ClassImp(KVUniqueNameList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVUniqueNameList</h2>
<h4>Optimised list in which objects can only be placed once</h4>
Use this list when you need to check as each object is added that it is not already in
the list (assuming that objects are the same if they have the same name), in order to
avoid the same object being added several times.<br>
Using a standard TList/KVList, implementing such a check can become VERY time-consuming
as the list becomes longer, because for each new object we add we have to perform a
sequential scan of all objects in the list and check the name of each one. However, using
a hash list based on object names, the number of objects to check is kept small, even
as the list becomes very long, thanks to automatic rehashing of the list.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVUniqueNameList::KVUniqueNameList()
{
   // Default constructor
}

KVUniqueNameList::~KVUniqueNameList()
{
   // Destructor
}

