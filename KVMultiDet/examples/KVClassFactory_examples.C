//# Example of class source code generation using KVClassFactory
//
// KVClassFactory can be used to generate skeleton class source files
//
// To execute this function, either do
//
// $ kaliveda
// kaliveda[0] .L KVClassFactory_examples.C+
// kaliveda[1] make_examples()
//
// or
//
// $ root
// root[0] KVBase::InitEnvironment()
// root[0] .L KVClassFactory_examples.C+
// root[1] make_examples()

#include "TSystem.h"
#include "KVClassFactory.h"
#include "KVNameValueList.h"

void make_examples()
{
   KVList examples;

   // Example 1: a simple base class with no member variables
   //       Generates a class with:
   //         - default constructor
   KVClassFactory ex1("BaseClass", "Example 1: a simple base class with no member variables");
   ex1.GenerateCode();
   examples.Add(new TNamed(ex1.GetImpFileName(), ex1.GetClassName()));

   // Example 2: a simple base class with 1 member variable
   //       Generates a class with:
   //         - default constructor
   //         - constructor with member variable initial values
   //         - copy constructor
   //         - assignment operator
   //         - getters & setters for all members
   KVClassFactory ex2("BaseClassWithMember", "Example 2: a simple base class with 1 member variable");
   ex2.AddMember("MemberVar", "Double_t", "a member variable");
   ex2.GenerateCode();
   examples.Add(new TNamed(ex2.GetImpFileName(), ex2.GetClassName()));

   // Example 3: TNamed-derived class with no member variables
   //       Generates a class with:
   //         - default constructor
   //         - the same constructors as the base class
   //   We also show how to make all constructors and the destructor 'inline'
   //   i.e. implemented in the header file of the class
   KVClassFactory ex3("TNamedClass", "Example 3: a TNamed-derived class with no member variables",
                      "TNamed");
   ex3.InlineAllConstructors();
   ex3.GetDestructor()->SetInline();
   ex3.GenerateCode();
   examples.Add(new TNamed(ex3.GetImpFileName(), ex3.GetClassName()));

   // Example 4: a TNamed-derived class with 1 member variable
   //       Generates a class with:
   //         - default constructor
   //         - same non-default constructors as base class with extra argument(s)
   //           to initialise the member(s) of this class
   //         - copy constructor
   //         - assignment operator
   //         - getters & setters for all members
   KVClassFactory ex4("TNamedClassWithMember", "Example 4: a TNamed-derived class with 1 member variable",
                      "TNamed");
   ex4.AddMember("MemberVar", "Double_t", "a member variable");
   ex4.GenerateCode();
   examples.Add(new TNamed(ex4.GetImpFileName(), ex4.GetClassName()));

   // Compile all generated classes
   TIter next_class(&examples);
   TObject* cl;
   while ((cl = next_class())) {

      gSystem->CompileMacro(cl->GetName());

   }

   examples.Clear();

   // Example 5: a BaseClass-derived class with 1 extra member variable
   //       Generates a class with:
   //         - default constructor
   //         - same non-default constructors as base class with extra argument(s)
   //           to initialise the member(s) of this class
   //         - copy constructor
   //         - assignment operator
   //         - getters & setters for all members
   KVClassFactory ex5("DerivedClassWithMember", "Example 5: a -derived class with 1 member variable",
                      "BaseClassWithMember");
   ex5.AddMember("Toto", "BaseClassWithMember*", "pointer");
   ex5.GenerateCode();
   examples.Add(new TNamed(ex5.GetImpFileName(), ex5.GetClassName()));

   // Example 6: a TColor-derived class with extra member variables
   //    TColor has a constructor with default arguments
   //    The generated constructor treats these arguments correctly
   KVClassFactory ex6("TColorDerived", "Example 6: a TColor-derived class with extra member variables",
                      "TColor");
   ex6.AddMember("A", "int", "a number");
   ex6.AddMember("B", "double", "another number");
   ex6.GenerateCode();
   examples.Add(new TNamed(ex6.GetImpFileName(), ex6.GetClassName()));

   // Example 7: a class generated from a KVNameValueList
   //    in this case the class has member variables with the names & types of the
   //    the parameters in the KVNameValueList
   KVNameValueList nvl;
   nvl.SetValue("A", 1); // integer
   nvl.SetValue("B", 3.14); // double
   nvl.SetValue("C", "hello"); // string
   nvl.WriteClass("NVLclass", "Class generated from a KVNameValueList");
   examples.Add(new TNamed("NVLclass.cpp", "NVLclass"));

   // Compile all generated classes
   TIter next_class2(&examples);
   while ((cl = next_class2())) {

      gSystem->CompileMacro(cl->GetName());

   }
}
