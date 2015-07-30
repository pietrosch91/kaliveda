#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>

#include "THtml.h"
#include "KVBase.h"
using namespace std;

int main(int argc, char *argv[])
{
   // html_convert [-c] [-o output] [-t title] input
   //  options:
   //     -c          convert a class, 'input' is class source (.cpp/.cxx/.C file)
   //     -o output   output directory, default is htmldoc
   //     -t title    title for converted code
   
    int opt;
    string input,output,title;
    bool is_class = false;
    
    while ((opt = getopt(argc, argv, "co:t:")) != -1) {
        switch (opt) {
        case 'c':
            is_class=true;
            break;
        case 'o':
            output = optarg;
            break;
        case 't':
            title = optarg;
            break;
        default: /* '?' */
            cerr << "Usage: " << argv[0] << " [-c] [-o output] [-t title]  input" << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        cerr << "Expected argument after options" << endl;
        exit(EXIT_FAILURE);
    }

    input = argv[optind];
        
    THtml html;
    html.LoadAllLibs();
    html.SetProductName(Form("KaliVeda v%s",KVBase::GetKVVersion()));
    TString source_root = KVBase::GetKVSourceDir();
    html.SetInputDir(source_root);
    if(output!="") html.SetOutputDir(output.c_str());
    
    if(is_class){
    }
    else{
       html.Convert(input.c_str(),title.c_str());
    }
    
    exit(EXIT_SUCCESS);
}
