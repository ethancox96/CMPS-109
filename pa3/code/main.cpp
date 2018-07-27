// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <fstream>
#include <typeinfo>
#include <vector>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"
#include "debug.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void parsefile(const string& filename, istream &file) {
   str_str_map test;
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   string line;
   int line_number = 0;
   while (getline(file, line)) {
      cout << filename << ": " << ++line_number << ": " << line << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         continue;
      } else if (regex_search (line, result, key_value_regex)) {
         const string key {result.str(1)};
         string value = to_string(result.str(2));
         bool key_exists {key != ""};
         bool value_exists {value != ""};
         if (key_exists && value_exists) {
            str_str_pair pair(key, value);
            test.insert(pair);
            cout << key << " = " << value << endl;
         } else if (key_exists && !value_exists) {
            auto it = test.find(key);
            test.erase(it);
         } else if (!key_exists && value_exists) {
            auto it = test.begin();
            for (; it != test.end(); ++it) {
               if (it->second == value) {
                  cout << it->first << " = " << it->second << endl;
               }
            }
            if (it->second == value) {
               cout << it->first << " = " << it->second << endl;
            }
         } else {
            auto it = test.begin();
            for (; it != test.end(); ++it) {
               cout << it->first << " = " << it->second << endl;
            }
            cout << it->first << " = " << it->second << endl;
         }
      } else if (regex_search (line, result, trimmed_regex)) {
         if (result.str(1).size() > 10) {
            cout << result[1] << ": key not found" << endl;
         } else {
            auto it = test.find(result[1]);
            cout << it->first << " = ";
            if (it != test.end()) {
               cout << it->second << endl;
            } else {
               if (it->first == result[1]) {
                  cout << it->second << endl;
               } else {
                  cout << "key does not exist" << endl;
               }
            }
         }
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
cout << argv[0] << argv[1] << endl;   
   string progname (basename (argv[0]));
   vector<string> filenames (&argv[1], &argv[argc]);
   if (filenames.size() == 0) filenames.push_back ("-");
   for (const auto& filename: filenames) {
      if (filename == "-") parsefile (filename, cin);
      else {
         ifstream infile (filename);
         if (infile.fail()) {
            cerr << progname << ": " << filename << ": "
                 << strerror (errno) << endl;
         }else {
            parsefile(filename, infile);
            infile.close();
         }
      }
   }

   return EXIT_SUCCESS;
}







