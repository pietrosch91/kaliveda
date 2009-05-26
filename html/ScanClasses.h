/*
$Id: ScanClasses.h,v 1.1 2007/06/27 14:55:37 franklan Exp $
$Revision: 1.1 $
$Date: 2007/06/27 14:55:37 $
*/

//Created by KVClassFactory on Mon Jun 25 18:25:35 2007
//Author: franklan

#ifndef __SCANCLASSES_H
#define __SCANCLASSES_H

#include "TList.h"
#include "KVString.h"
#include "Riostream.h"
#include "TObjArray.h"

class ScanClasses
{
   protected:
         
   TList *fClassTitles;//list of class category names
   TList* fClassList;//list of releveant classes sorted into sublists by category (list of lists)
   TObjArray* cnames;//list containing names of all classes to be scanned
   KVString fTempString;//temporary string
   ofstream list_file;//output HTML file
   Int_t fMaxDottyCols;//used for alignment. default = 40
   Int_t nclasses;//total number of classes in all categories
   KVString fFilename;//name of HTML file to write
   KVString fPageTitle;//title of HTML page
   
   public:

   ScanClasses();
   virtual ~ScanClasses();
   
   const Char_t* HTMLFileName(const Char_t* classname);
   const Char_t* HTMLSafeLink(const Char_t* classname);
   virtual void MakeClassCategoriesList();
   virtual void FillListOfClasses();
   virtual void AnalyseClasses();
   virtual void WritePageHead(){;};
   virtual void WritePage();
   
   void SetFileName(const Char_t* output_filename){ fFilename = output_filename; };
   void SetPageTitle(const Char_t* title){ fPageTitle = title; };
   
   virtual void Process();
   
//   ClassDef(ScanClasses,0)//Scan KaliVeda classes and produce a HTML page listing them with links to doc
};

class ScanExamples : public ScanClasses
{
   public:

   ScanExamples(){};
   virtual ~ScanExamples(){};
   
   virtual void FillListOfClasses();
   virtual void AnalyseClasses();
   virtual void WritePageHead();
   
//   ClassDef(ScanExamples,0)//Scan example classes and produce a HTML page listing them with links to doc
};

#endif
