#include <cstdio>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <getopt.h>
#include <ftxui/component/screen_interactive.hpp>
#include "ListToggle.hpp"
using namespace ftxui;
using std::vector;
using std::string;
using std::cout;

void Help() {
   cout << "Usage:\n"
             << " -[-n]egate       : Negate choices (i.e start with all selected)\n"
             << " -[-i]dx          : Print the indices(zero-origin) of the selected items, not their values.\n"
             << " -[-l]ist [dir]   : Use directory given by [dir] (or cwd if [dir] is not given)\n"
             << "                    for the list of possible choices. The path will be prepended\n"
             << "                    to the selected items.\n"
             << " -[-h]idden       : When listing files, list all files. including those starting with '.'\n"
             << " -[-q]uote [char] : Escape (quote) each item with [char], or \" if [char] is not supplied"
             << " -[-m]ax   <max>  : Automatically exit after making <max> selections.\n"
             << " -[-s]ize  <size> : Display no more than <size> lines at a time (default is 10).\n"
             << " -[-d]elim <delim>: Use <delim> as the output delimiter (default is '\\n')\n"
             << " -[-f]ile  <file> : Send output to both <file> and stdout.\n"
             << " -[-a]ppend       : Append to aformentioned <file> (default is to overwrite).\n"
             << " --help           : Show this help.\n";
   exit(0);
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
   while(getline(std::cin, str)) {
      retVec.push_back(str);
   }
#if defined(_WIN32)
   freopen("CON", "rw", stdin);
#else
   freopen("/dev/tty", "rw", stdin); //NOLINT
#endif
   return retVec;
}
//TODO: use an actual list
vector<string> listDir(const string& path, bool showHidden) {
   vector<string> dirs;
   vector<string> files;
   const std::filesystem::path dir(path);
   for(const auto &dir_entry : std::filesystem::directory_iterator(dir)) {
      string str = dir_entry.path().filename().string();
      if(!showHidden && str[0] == '.') {
         continue;
      }
      if(dir_entry.is_directory()) {
         dirs.push_back(str + "/");
      } else {
         files.push_back(str);
      }
   }
   std::sort(dirs.begin(), dirs.end());
   std::sort(files.begin(), files.end());
   dirs.insert(dirs.end(), files.begin(), files.end());
   return dirs;
}
void addRootDir(string* rootDir, vector<string>* Choices) {
   if(rootDir->back() != '/') {
      *rootDir += '/';
   }
   for(string &str : *Choices) {
      str = *rootDir + str;
   }
}
string makeMsg(const vector<string>& Choices, const vector<bool>& States, char delim, char quote, bool useIdx) {
   string msg;
   for (int i = 0; i < Choices.size(); ++i) {
      string str;
      if(States[i]) {
         str = useIdx ? std::to_string(i) : Choices[i];
         msg += quote ? (quote+str+quote) : str;
         msg += delim;
      }
   }
   //strip final delim if !'\n' (if it's a newline, it
   //is likely preferable to keep it)
   if(delim != '\n') { msg.pop_back(); }
   return msg;
};
int main(int argc, char* argv[]) {
   auto screen = ScreenInteractive::FitComponent();
   int Negate = false;
   int append = false;
   int useIdx = false;
   int hidden = false;
   char quote = false;
   int Max    = 0;
   int size   = 10;
   char delim = '\n'; //*output* delimiter
   string outFile = "";
   string rootDir = "";
   vector<string> Choices;
   struct option options[] = {
      {"negate", no_argument,       &Negate,  true },
      {"append", no_argument,       &append,  true },
      {"idx",    no_argument,       &useIdx,     true },
      {"hidden", no_argument,       &hidden,  true },
      {"quote",  optional_argument, nullptr, 'q'},
      {"max",    required_argument, nullptr, 'm'},
      {"size",   required_argument, nullptr, 's'},
      {"file",   required_argument, nullptr, 'f'},
      {"delim",  required_argument, nullptr, 'd'},
      {"list",   optional_argument, nullptr, 'l'},
      {"help",   no_argument,       nullptr, 'H'},
      {0, 0, 0, 0}
   };
   int ret;
   while((ret = getopt_long(argc, argv, "naihq::m:s:f:d:l::", options, NULL)) != -1) {
      switch(ret) {
         case 'n': //negate
            Negate = true;
            break;
         case 'a': //append
            append = 1;
            break;
         case 'i': //idx (print indicies, not items)
            useIdx = 1;
            break;      
         case 'h': //hidden
            hidden = true;
            break;
         case 'q': //quote: semi-boolean
            if(optarg) {
               quote = *optarg;
            } else if (optind < argc && argv[optind][0] != '-') {
               quote = argv[optind++][0];
            } else {
               quote = '"';
            }
            break;
         case 'm': //max(number of selections)
            Max = atoi(optarg);
            break;
         case 's': //size (of displayed list, in lines)
            size = atoi(optarg);
            break;
         case 'f': //file
            outFile = optarg;
            break;
         case 'd': //delim
            delim = expand(optarg);
            break;
         case 'l': //list(directory)
            if (optarg) {
               rootDir = optarg;
            } else if (optind < argc && argv[optind][0] != '-') {
               //the next 'arg' is actually the optarg, getopt just assumes
               //the use of '=' when providing an option argument
               rootDir = argv[optind++];
            } else { 
               rootDir = "./";
            }
            break;
         case 'H': //Help
            Help();
            break;
         case '?':
            std::cerr << "Error processing options, exiting now!\n";
            exit(-1);
            break;
      }
   }
   if(optind != argc) {
      Choices.assign(&argv[optind], &argv[argc]);
   } else if(!rootDir.empty()) {
      Choices = listDir(rootDir, hidden);
   } else {
      Choices = stdIn();
   }

   //save stdout & redirect stdout to stderr
   FILE* stdsve = fopen("/dev/stdout", "a");
   freopen("/dev/stderr", "w", stdout);

   //main loop
   vector<bool> States;
   Component container = ListToggle(&Choices, &States, Negate, Max);
   container = CatchEvent(container, 
      [&](Event event) {
            if(event == Event::F1 || event == Event::Character('y')) {
               screen.Exit();
               return true;
            }
            return false;
      }
   );
   screen.Loop(container | ftxui::size(HEIGHT, LESS_THAN, size) | border);


   ///process selections

   ///Append '/' to path (if needed) and
   ///prepend path to each selection if path is not empty
   if(!rootDir.empty() && !useIdx) {
      addRootDir(&rootDir, &Choices);
   }

   ///combine each item/idx with a trailing delim
   string msg = makeMsg(Choices, States, delim, quote, useIdx);

   ///output to stdsve(aka stdout)
   ///return false (in bash) to indicate no selection
   std::fputs("\E[?12l", stdout); //XXX fix cursor blink
   if(msg.empty()) {
      return 1;
   } else {
      std::fputs(msg.c_str(), stdsve);
   }

   ///output to outFile (if given)
   if(!outFile.empty()) {
      if(append)
         freopen(outFile.c_str(), "a", stdsve);
      else
         freopen(outFile.c_str(), "w", stdsve);
      std::fputs(msg.c_str(), stdsve);
   }
}
