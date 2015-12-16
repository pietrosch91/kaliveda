#ifndef __GRID_LOADER_H__

/**
   WARNING: This class has been deprecated and will eventually be removed.

   Deprecated by: Peter Wigg (peter.wigg.314159@gmail.com)
   Date:          Thu  8 Oct 11:58:35 BST 2015
*/

#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__
#ifdef __ENABLE_DEPRECATED_VAMOS__

// This class is only compiled if __ENABLE_DEPRECATED_VAMOS__ is set in
// VAMOS/analysis/Defines.h

#define __GRID_LOADER_H__

// C
#include <cassert>

// KaliVeda (Standard)
#include "KVIDGraph.h"
#include "KVNumberList.h"
#include "KVIDGridManager.h"

// KaliVeda (VAMOS)
#include "Deprecation.h"

class GridLoader {
   KVList* global_grid_list_;
   KVList* run_grid_list_;

   Bool_t kInitialised_;

   // Prohibit Copy and Copy Assign
   GridLoader(const GridLoader&);
   GridLoader& operator=(const GridLoader&);

public:

   GridLoader();
   virtual ~GridLoader();

   Bool_t InitAnalysis();
   Bool_t InitRun(const UInt_t run_number);
   Bool_t IsInitialised()    const;
   void   PrintGlobalGrids() const;
   void   PrintRunGrids()    const;
   void   PrintAssertionStatus() const;


   const KVIDGraph* GetRunGrid(const TString& grid_name) const;

   ClassDef(GridLoader, 1);

};

#endif // __ENABLE_DEPRECATED_VAMOS__ is set
#endif // __GRID_LOADER_H__ is not set

#ifdef __GRID_LOADER_H__
DEPRECATED_CLASS(GridLoader);
#endif

