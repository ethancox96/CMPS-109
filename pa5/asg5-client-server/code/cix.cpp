// $Id: cix.cpp,v 1.4 2016-05-09 16:01:56-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get" , cix_command::GET },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

void cix_help() {
   static const vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      log << "sent LS, server did not return LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

void cix_get(client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::GET;
   for (size_t i = 0; i < filename.size(); i++) {
      header.filename[i] = filename[i];
   }
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::FILE) {
      log << "sent CIX_GET, server did not return CIX_FILE" << endl;
      log << "server return " << header << endl;
   } else {
      char buffer[header.nbytes+1];
      recv_packet(server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      ofstream ofs {header.filename};
      ofs.write(buffer, sizeof(buffer));
      ofs.close();
      cout << header.filename << " has been created" << endl;
   }
}

void cix_put(client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::PUT;
   for (size_t i = 0; i < filename.size(); i++) {
      header.filename[i] = filename[i];
   }
   ifstream ifs {header.filename};
   string put_output;
   while (not ifs.eof()) {
      char buffer[0x1000];
      ifs.read(buffer, sizeof buffer);
      put_output.append(buffer);
   }
   ifs.close();
   header.nbytes = put_output.size();
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, put_output.c_str(), put_output.size());
   log << "sent " << put_output.size() << " bytes" << endl;
   recv_packet(server, &header, sizeof header);
   if (header.command != cix_command::ACK) {
      log << "sent CIX_PUT, server did not return CIX_AK" << endl;
      log << "server returned " << header << endl;
   }
}

void cix_rm(client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::RM;
   for (size_t i = 0; i < filename.size(); i++) {
      header.filename[i] = filename[i];
   }
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      log << "sent CIX_RM, server did not return ACK" << endl;
      log << "server returned " << header << endl;
   } else { 
      log << filename << " has been removed" << endl;
   }
}


void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         string file;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         log << "command " << line << endl;
         if (line.find("get") != string::npos){
            size_t position = line.find(" ");
            position++;
            file = line.substr(position, line.size());
            line.erase(position-1, line.size());
         } else if (line.find("put") != string::npos) {
            size_t position = line.find(" ");
            position++;
            file = line.substr(position, line.size());
            line.erase(position-1, line.size());
         } else if (line.find("rm") != string::npos) {
            size_t position = line.find(" ");
            position++;
            file = line.substr(position, line.size());
            line.erase(position-1, line.size());
         }
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
               cix_get(server, file);
               break;
            case cix_command::PUT:
               cix_put(server, file);
               break;
            case cix_command::RM:
               cix_rm(server, file);
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

