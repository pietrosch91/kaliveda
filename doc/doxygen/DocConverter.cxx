#include "KVString.h"
#include "KVNameValueList.h"
#include "TSystem.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <KVSystemDirectory.h>
#include <KVSystemFile.h>
using namespace std;

KVNameValueList modules;

void ClassDescriptionConverter(vector<KVString>& classname, KVString& briefdesc, vector<KVString>& desc)
{
   bool just_got_classname = false;
   for (auto& l : desc) {
      if (l.BeginsWith("//") || l.BeginsWith("/*") || l.BeginsWith("*/")) l.Remove(0, 2);
      else if (l.BeginsWith("<!--")) l.Remove(0, 4);
      l.ReplaceAll("BEGIN_HTML", "");
      l.ReplaceAll("END_HTML", "");
      l.ReplaceAll("<!--", "");
      l.ReplaceAll("-->", "");
      l.ReplaceAll("*/", "");
      //l.RemoveAllExtraWhiteSpace();
      if (l.Contains("<h2>")) {
         l.ReplaceAll("<h2>", "");
         l.ReplaceAll("</h2>", "");
         l.Begin(",");
         while(!l.End()){
            if (std::find(classname.begin(),classname.end(),l.Next(kTRUE))!=classname.end()) {
               l = ""; //no need for class name title
               just_got_classname = true;
               break;
            }
         }
         if(!just_got_classname)
         {
            // transform to markdown title
            l.Prepend("## ");
            l.Append(" ##");
            just_got_classname = false;
         }
      } else if (l.BeginsWith("<h4>")) {
         l.ReplaceAll("<h4>", "");
         l.ReplaceAll("</h4>", "");
         if (just_got_classname) {
            // this is brief description
            briefdesc = l;
            just_got_classname = false;
            l = "";
         } else {
            // transform to markdown titie
            l.Prepend("#### ");
            l.Append(" ####");
         }
      } else if (l.BeginsWith("<h3>")) {
         l.ReplaceAll("<h3>", "");
         l.ReplaceAll("</h3>", "");
         // transform to markdown titie
         l.Prepend("### ");
         l.Append(" ###");
      } else if (l == "<code>") l = "~~~~~~~~~~~~~~~{.cpp}";
      else if (l == "</code>") l = "~~~~~~~~~~~~~~~";
      else {
         just_got_classname = false;
      }
   }
}


void write_class_description(KVString& group, int& last_non_code_line, ofstream& output_file, vector<KVString>& class_description, vector<KVString>& classname, queue<KVString>& output)
{
   // spit out everything read so far up to and including the last non-code line
   for (int i = 1; i <= last_non_code_line; ++i) {
      output_file << output.front() << endl;
      output.pop();
   }
   KVString briefdesc;
   if (class_description.size()) {
      ClassDescriptionConverter(classname, briefdesc, class_description);
      if(briefdesc=="") {
         auto errmess = [](const KVString& a){ cout << "CLASS: " << a << " : no brief\n"; };
         std::for_each(classname.begin(),classname.end(),errmess);
      }
   }
   else
   {
      auto errmess = [](const KVString& a){ cout << "CLASS: " << a << " : ***NO DESC***\n"; };
      std::for_each(classname.begin(),classname.end(),errmess);
   }

   if(classname.size()){
      output_file << "/** \\class " << classname[0] << endl;
      output_file << "\\ingroup " << group << endl;
      if (briefdesc != "") output_file << "\\brief " << briefdesc << endl;
      if (class_description.size()) {
         for (auto& l : class_description) {
            output_file << l << endl;
         }
      }
      output_file << "*/" << endl;
   }
   if(classname.size()>1)
   {
      auto it = classname.begin(); ++it;
      for(;it!=classname.end();++it)
      {
         output_file << "/** \\class " << *it << endl;
         output_file << "\\ingroup " << group << endl;
         output_file << "\\copydoc " << classname[0] << endl;
         output_file << "*/" << endl;
      }
   }
   class_description.clear();
}

void DocConverter(const KVString& file)
{
   KVString _file = gSystem->ExpandPathName(file.Data());
   KVString group = gSystem->BaseName(gSystem->DirName(_file));
   group.Capitalize();
   vector<KVString> classname;

   KVString backup = _file + ".bck";
   gSystem->CopyFile(_file, backup, kTRUE);
   ifstream source_file(backup.Data());
   ofstream output_file;
   output_file.open(_file.Data());

   // put group definition in first file of group
   if (!modules.HasParameter(group)) {
      modules.SetValue(group, 1);
      output_file << "/**" << endl;
      output_file << "   \\defgroup " << group << " The " << group << " module" << endl;
      output_file << endl << "    Part of the KaliVeda Analysis Toolkit" << endl;
      output_file << "*/" << endl;
   }

   KVString source_line;
   int line_no = 0;
   int brace_count = 0; //number of open '{'
   int ns_count = 0; // number of open '{' associated with namespaces
   bool look_for_comments = kFALSE;
   vector<KVString> comments;
   source_line.ReadToDelim(source_file, '\n');
   queue<KVString> output;
   bool found_first_method = false;
   bool found_first_ns = false;
   int last_non_code_line = 0;
   bool in_a_method = false;
   bool line_is_comment = false;
   bool multiline_comment = false;
   bool in_method_code = false;
   vector<KVString> class_description;
   bool in_class_desc = false;
   bool in_a_ifdef = false;
   bool in_a_ifdefelse = false;

   while (source_file.good()) {
      ++line_no;
      KVString original_line = source_line;
      //cout << line_no << ":" << original_line << endl;

      source_line = source_line.StripAllExtraWhiteSpace();
      if (source_line.BeginsWith("/*")) multiline_comment = true;
      if (source_line.Contains("*/")) multiline_comment = false;
      line_is_comment = false;
      if (source_line.BeginsWith("//") || multiline_comment
          || (!source_line.Contains("/*") && source_line.Contains("*/"))) line_is_comment = true;
      if (!in_a_method && (source_line.IsWhitespace() || source_line.BeginsWith("/")))
         last_non_code_line = line_no;
      if (in_a_method && !source_line.IsWhitespace() && !line_is_comment) in_method_code = true;
      else in_method_code = false;
      if (in_class_desc) {
         if (source_line.BeginsWith("//////////////")) in_class_desc = false;
         else class_description.push_back(original_line);
      } else if (source_line.BeginsWith("//////////////")) in_class_desc = true;
      if (found_first_method && !in_a_method && line_is_comment) {}
      else
         output.push(original_line);

      if (look_for_comments) {
         if (line_is_comment) comments.push_back(source_line);
         else if (in_method_code) look_for_comments = kFALSE;
      }

      if(source_line.BeginsWith("ClassImp")){
         KVString tutu = source_line;
         tutu.ReplaceAll(")","(");
         tutu.Begin("(");
         tutu.Next();
         classname.push_back(tutu.Next());
      }

      if(in_a_ifdef)
      {
         //#if...#else...#endif may contain two different versions of a brace-opening piece of
         //code, i.e. method or for-loop. We arbitrarily ignore any brace-opening or closing
         //in the #else...#endif part
         if(source_line.BeginsWith("#else")) in_a_ifdefelse=true;
         else if(source_line.BeginsWith("#endif")){
            in_a_ifdef=in_a_ifdefelse=false;
         }
      }
      else if(source_line.BeginsWith("#if"))
      {
         in_a_ifdef=true;
      }


      int delta_brace = 0; int delta_ns=0;
      if (!line_is_comment) {
         delta_brace = (in_a_ifdefelse ? 0 : (source_line.CountChar('{') - source_line.CountChar('}')));
         if(delta_brace!=0){
            if(source_line.BeginsWith("namespace"))
            {
               ns_count += delta_brace;
               delta_ns=delta_brace;
               delta_brace=0;
            }
            else if(delta_brace<0 && brace_count==0){
               ns_count += delta_brace;
               delta_ns=delta_brace;
               delta_brace=0;
            }
            else
               brace_count += delta_brace;
         }
      }
      if(ns_count == 1 && delta_ns==1)
      {
         // we just opened a namespace
         // class description has to go before
         found_first_ns=true;
         if(!found_first_method) write_class_description(group, last_non_code_line, output_file, class_description, classname, output);
      }
      if (brace_count == 1 && delta_brace == 1) {
         //cout << "Method body begins line " << line_no << endl;
         //cout << "Last non-code line " << last_non_code_line << endl;
         look_for_comments = kTRUE;
         comments.clear();
         in_a_method = true;
         if (!found_first_method) {
            found_first_method = true;
            if(!found_first_ns) write_class_description(group, last_non_code_line, output_file, class_description, classname, output);
            else
            {
               // write everything we read since namespace definition (including the namespace definition)
               // upto but notincludgin the method declaration
               int n = last_non_code_line-line_no+output.size();
               while (n) {
                  KVString toto = output.front();
                  output_file << toto << endl;
                  output.pop();
                  --n;
               }
            }
         }
      }
      else if (brace_count == 0 && delta_brace == -1) {
         //cout << "Method body ends line " << line_no << endl;
         in_a_method = false;
         // spit out entire method with added doxygen comments at start
         output_file << "///////////////////////////////////////////////////////////////////" << endl;
         KVString prefix = "///";
         for (auto comment : comments) {
            comment.ReplaceAll("BEGIN_LATEX", "\\f[");
            comment.ReplaceAll("begin_latex", "\\f[");
            comment.ReplaceAll("END_LATEX", "\\f]");
            comment.ReplaceAll("end_latex", "\\f]");
            comment.ReplaceAll("/*", "");
            comment.ReplaceAll("*/", "");
            if (comment.BeginsWith("//")) output_file << "/" << comment << endl;
            else if (comment.BeginsWith("\\f[")) {
               comment.Prepend("/**\n");
               output_file << comment << endl;
               prefix = "";
            } else if (comment.BeginsWith("\\f]")) {
               comment.Append("\n*/");
               output_file << comment << endl;
               prefix = "///";
            } else if (!comment.IsWhitespace()) output_file << prefix << " " << comment << endl;
         }
         output_file << endl;
         while (!output.empty()) {
            KVString toto = output.front();
            if (!toto.IsWhitespace()) output_file << toto << endl;
            output.pop();
         }
         output_file << endl;
      }
      source_line.ReadToDelim(source_file, '\n');
   }
   // if no methods defined in file (template class defined in header)
   if (!found_first_method && !found_first_ns)
      write_class_description(group, last_non_code_line, output_file, class_description, classname, output);
   else
   {
      while (!output.empty()) {
         KVString toto = output.front();
         if (!toto.IsWhitespace()) output_file << toto << endl;
         output.pop();
      }
      output_file << endl;
   }

   //cout << " " << line_no << " lines" << endl;
   //gSystem->CopyFile(backup, _file, kTRUE);
   gSystem->Unlink(backup);
}

void ConvertDocIn(const KVString& path)
{
   // Convert all .cpp files in module directory given by path

   cout << "Converting doc in " << path << endl;
   KVString _path = gSystem->ExpandPathName(path.Data());
   KVSystemDirectory d("dir", _path);
   _path.Append("/");
   TIter next(d.GetListOfFiles());
   KVSystemFile* f;
   while ((f = (KVSystemFile*)next())) {
      KVString name = f->GetName();
      if (name.EndsWith(".cpp")||name.EndsWith(".cxx")||name.EndsWith(".C")) DocConverter(_path + name);
   }
}
void ConvertAllModules(const KVString& path)
{
   // Convert all modules in path

   KVString _path = gSystem->ExpandPathName(path.Data());
   _path.Append("/");
   KVString modlist = "analysis,calibration,daq_cec,data_management,examples,globvars,identification,simulation,trieur";
   modlist += ",base,db,events,exp_events,geometry,gui,particles,stopping";

   modlist.Begin(",");
   while (!modlist.End()) {
      KVString mod = modlist.Next();
      if (!gSystem->AccessPathName(_path + mod)) ConvertDocIn(_path + mod);
   }
}
