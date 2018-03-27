#include <iostream>
#include <algorithm>
#include "KVEvent.h"
#include "TRandom.h"

using std::cout;
using std::endl;

bool compareZ(KVNucleus& a, KVNucleus& b)
{
   return a.GetZ() < b.GetZ();
}

void iterator_examples()
{
   KVEvent Event;
   for (int i = 0; i < 10; ++i) {
      KVNucleus* n = Event.AddParticle();
      n->SetZ(i);
      n->SetIsOK(i % 2);
      if (i > 4) n->AddGroup("GROUP");
      if (gRandom->Uniform() > .5) n->AddGroup("RANDOM");
   }

   cout << "Loop over all particles (0-9):" << endl;
#ifdef WITH_CPP11
   for (KVEvent::Iterator it = std::begin(Event); it != std::end(Event); ++it)
#else
   for (KVEvent::Iterator it = Event.begin(); it != Event.end(); ++it)
#endif
   {
      (*it).Print();
   }

   cout << "\nNested loops over N*(N-1)/2 pairs of particles:" << endl;
   for (KVEvent::Iterator it = Event.begin(); it != Event.end(); ++it) {
      KVEvent::Iterator it2(it);
      for (++it2; it2 != Event.end(); ++it2) {
         cout << (*it).GetZ() << "-" << (*it2).GetZ() << " ";
      }
      cout << endl;
   }

#ifdef WITH_CPP11
   cout << "\nLoop over all particles (0-9) [range-based for loop]:" << endl;
   for (auto& nuc : Event) {
      nuc.Print();
   }
#endif

   cout << "\nLoop over OK particles (1,3,5,7,9):" << endl;
#ifdef WITH_CPP11
   for (KVEvent::Iterator it(Event, KVEvent::Iterator::Type::OK); it != KVEvent::Iterator::End(); ++it)
#else
   for (KVEvent::Iterator it(Event, KVEvent::Iterator::OK); it != KVEvent::Iterator::End(); ++it)
#endif
   {
      (*it).Print();
   }

   cout << "\nLoop over GROUP particles (5,6,7,8,9):" << endl;
#ifdef WITH_CPP11
   for (KVEvent::Iterator it = KVEvent::Iterator(Event, KVEvent::Iterator::Type::Group, "GROUP"); it != KVEvent::Iterator::End(); ++it)
#else
   for (KVEvent::Iterator it = KVEvent::Iterator(Event, KVEvent::Iterator::Group, "GROUP"); it != KVEvent::Iterator::End(); ++it)
#endif
   {
      (*it).Print();
   }

   cout << "\nPerform two different iterations with the same iterator" << endl;
   cout << "\n1.) Loop over OK particles (1,3,5,7,9):" << endl;
#ifdef WITH_CPP11
   KVEvent::Iterator iter(Event, KVEvent::Iterator::Type::OK);
#else
   KVEvent::Iterator iter(Event, KVEvent::Iterator::OK);
#endif
   for (; iter != KVEvent::Iterator::End(); ++iter) {
      (*iter).Print();
   }

   cout << "\n2.) Loop over GROUP particles (5,6,7,8,9):" << endl;
#ifdef WITH_CPP11
   iter.Reset(KVEvent::Iterator::Type::Group, "GROUP");
#else
   iter.Reset(KVEvent::Iterator::Group, "GROUP");
#endif
   for (; iter != KVEvent::Iterator::End(); ++iter) {
      (*iter).Print();
   }

#ifdef WITH_CPP11
   cout << "\nLoop over RANDOM particles [range-based for loop]:" << endl;
   for (KVNucleus& nuc : EventIterator(Event, KVEvent::Iterator::Type::Group, "RANDOM")) {
      nuc.Print();
   }
#endif

   cout << "\nSearch using algorithm std::find:" << endl;
   KVNucleus boron;
   boron.SetZ(5);
   KVEvent::Iterator found = std::find(Event.begin(), Event.end(), boron);
   (*found).Print();

   cout << "\nFind largest Z in RANDOM group using std::max_element:" << endl;
#ifdef WITH_CPP11
   EventIterator it(Event, KVEvent::Iterator::Type::Group, "RANDOM");
#else
   EventIterator it(Event, KVEvent::Iterator::Group, "RANDOM");
#endif
   KVEvent::Iterator maxZ = std::max_element(it.begin(), it.end(), compareZ);
   (*maxZ).Print();

   cout << "\nLoop over all particles (0-9) [GetNextParticle]:" << endl;
   KVNucleus* n;
   while ((n = Event.GetNextParticle())) {
      n->Print();
   }

   cout << "\nInterrupted iteration restarted with different criteria [GetNextParticle]:" << endl;
   cout << "\n1.) Loop over OK particles with Z<=5 (1,3,5):" << endl;
   while ((n = Event.GetNextParticle("ok"))) {
      if (n->GetZ() > 5) break;
      n->Print();
   }
   Event.ResetGetNextParticle();
   cout << "\n2.) Loop over GROUP particles (5,6,7,8,9):" << endl;
   while ((n = Event.GetNextParticle("GROUP"))) {
      n->Print();
   }

}

