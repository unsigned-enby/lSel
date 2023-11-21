#include <cstdio>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <getopt.h>
#include <ftxui/component/screen_interactive.hpp>
#include "ListSelect.hpp"
using namespace ftxui;
using std::vector;
using std::string;
using std::cout;

void Help() {
   cout << "Usage:\n"
             << " -[-n]egate       : negate choices (i.e start with all selected)\n"
             << " -[-i]dx          : print the indicies(zero-origin) of the selected items, not their values.\n"
             << " -[-l]ist [dir]   : use directory given by [dir] (or cwd if [dir] is not given)\n"
             << "                    for the list of possible choices. The path will be prepended\n"
             << "                    to the selected items.\n"
             << " -[-s]ize  <size> : Display no more than <size> lines at a time (default is 10).\n"
             << " -[-d]elim <delim>: Use <delim> as the output delimiter (default is '\\n')\n"
             << " -[-f]ile  <file> : Send output to both <file> and stdout.\n"
             << " -[-a]ppend       : Append to aformentioned <file> (default is to overwrite).\n"
             << " -[-h]elp         : Show this help.\n";
}
char expand(char* str) {
   if(str[0] != '\\')
      return str[0];
   switch(str[1]) {
      case 'n':
         return '\n';
         break;
      case 't':
         return '\t';
         break;
      case 'v':
         return '\v';
         break;
      case 'r':
         return '\r';
         break;
      case '0':
         return '\0';
         break;
      default:
         return str[1];
   }
}
vector<string> stdIn() {
   string str;
   vector<string> retVec;
   std::cerr << "Reading args from stdin...\n";
   while(getline(std::cin, str)) {
      retVec.push_back(str);
   }
   //TODO: account for windows
#if defined(_WIN32)
   freopen("CON", "rw", stdin);
#else
   freopen("/dev/tty", "rw", stdin);
#endif
   return retVec;
}
vector<string> listDir(string path) {
   vector<string> retVec;
   const std::filesystem::path dir(path);
   for(const auto &dir_entry : std::filesystem::directory_iterator(dir)) {
      retVec.push_back(dir_entry.path().filename().string());
   }
   std::sort(retVec.begin(), retVec.end());
   return retVec;
}
int main(int argc, char* argv[]) {
   auto screen = ScreenInteractive::FitComponent();
   int negate = 0;
   int append = 0;
   int idx    = 0;
   int size   = 10;
   char delim = '\n'; //*output* delimiter
   string outFile = "";
   string rootDir = "";
   vector<string> myVec;
   struct option options[] = {
      {"negate", no_argument,       &negate,  1 },
      {"append", no_argument,       &append,  1 },
      {"idx",    no_argument,       &idx,     1 },
      {"size",   required_argument, nullptr, 's'},
      {"file",   required_argument, nullptr, 'f'},
      {"delim",  required_argument, nullptr, 'd'},
      {"list",   optional_argument, nullptr, 'l'},
      {"help",   no_argument,       nullptr, 'h'},
      {0, 0, 0, 0}
   };
   int ret;
   while((ret = getopt_long(argc, argv, "nais:f:d:l::h", options, NULL)) != -1) {
      switch(ret) {
         case 'n':
            negate = 1;
            break;
         case 'a':
            append = 1;
            break;
         case 'i':
            idx = 1; //boolean; if(idx), print the index, 
            break;   //not the selection itself
         case 's':
            size = atoi(optarg);
            break;
         case 'f':
            outFile = optarg;
            break;
         case 'd':
            delim = expand(optarg);
            break;
         case 'l':
            if (optarg) {
               rootDir = optarg;
            } else if (optind == argc || argv[optind][0] == '-') {
               rootDir = "./";
            } else {
               rootDir = argv[optind];
               optind++;
            }
            break;
         case 'h':
            Help();
            break;
         default:
            std::cerr << "Error proccessing options, exiting now!\n";
            exit(-1);
            break;
      }
   }
   if(optind != argc)
      myVec.assign(&argv[optind], &argv[argc]);
   else if(!rootDir.empty())
      myVec = listDir(rootDir);
   else 
      myVec = stdIn();
   
   //save stdout & redirect stdout to stderr
   FILE* stdsve = fopen("/dev/stdout", "a");
   freopen("/dev/stderr", "w", stdout);
   
   //main loop
   bool* states = nullptr;
   auto container = listSelect(myVec, states, negate, size);
   container = CatchEvent(container, [&](Event event) {
         if(event == Event::F1 || event == Event::Character('y')) {
            screen.Exit();
            return true;
         }
         return false;
   });
   screen.Loop(container);
 
   
   ///proccess selections
   
   ///Append '/' to path (if needed) and 
   ///prepend path to each selection if path is not empty
   if(!rootDir.empty() && !idx) {
      if(rootDir.back() != '/') {
         rootDir += '/';
      }
      for(string &str : myVec) {
         str = rootDir + str;
      }
   }
   ///combine each selection with a trailing delim
   string msg;
   for (int i = 0; i < myVec.size(); ++i) {
      if(states[i]) {
         if(idx)
            msg += std::to_string(i) + delim;
         else
            msg += myVec[i] + delim;
      }
   }
   
   /* 
   ///strip final delim if !'\n' (if it's a newline, it
   ///is likely preferable to keep it)
   if(delim != '\n') {
      msg.pop_back();
   } 
   */
   
   ///output to stdsve(aka stdout)
   std::fputs(msg.c_str(), stdsve);
   ///output to outFile (if given)
   if(!outFile.empty()) {
      if(append)
         freopen(outFile.c_str(), "a", stdsve);
      else
         freopen(outFile.c_str(), "w", stdsve);
      std::fputs(msg.c_str(), stdsve);
   }
}
