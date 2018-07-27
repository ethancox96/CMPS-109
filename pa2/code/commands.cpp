// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $
#include <iomanip>
#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"#"     , fn_cmt   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() < 2) {
      throw command_error("usage: cat filename");
   }
   for (size_t i = 1; i < words.size(); ++i) {
      string name = words.at(i);
      inode_ptr p = state.resolve_pathname(name);
      if (p == nullptr) {
	 throw command_error("cat: " + name + ": does not exist");
      } else {
         base_file_ptr file = dynamic_pointer_cast<base_file>(p->get_contents());
         cout << file->readfile() << endl;
      }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string name = words.at(1);
   if (name == "/") {
      state.set_cwd(state.get_root());
   } else {
      inode_ptr p = state.resolve_pathname(name);
      if (p == nullptr) {
	 throw command_error("cd: " + name + ": does not exist");
      } else {
         state.set_cwd(p);
      }
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
//   string name = words.at(1);
   if (words.size() == 1) {
      cout << ".:" << endl;
      inode_ptr cwd = state.get_cwd();
      directory_ptr d = dynamic_pointer_cast<directory>(cwd->get_contents());
      d->ls(cout);
   } else {
      string name = words.at(1);
      inode_ptr p = state.resolve_pathname(name);
      if (p == nullptr) {
	 throw command_error("fn: " + name + ": directory does not exist");
      }
      cout << name << ":" << endl;
      directory_ptr d = dynamic_pointer_cast<directory>(p->get_contents());
      d->ls(cout);
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   fn_ls(state, words);
   /*if (words.size() == 1) {
      inode_ptr p = state.get_cwd();
      string name = p->get_name();
      state.lsr(name, cout);
   } else {
      string name = words.at(1);
      inode_ptr p = state.resolve_pathname(name);
      state.lsr(name, cout);
   }*/
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // check if valid filename
   if (words.at(1).back() == '/') {
      throw command_error(words.at(1) + ": invalid filename");
   }
   //get file name and create file
   string file_name = words.at(1);
   inode_ptr cwd = state.get_cwd();
   inode_ptr p = state.resolve_pathname(file_name);
   directory_ptr d;
   if (p == nullptr) {
      d = dynamic_pointer_cast<directory>(cwd->get_contents());
   } else {
      d = dynamic_pointer_cast<directory>(p->get_contents());
   }
   inode_ptr file = d->mkfile(file_name);
   // write data to file
   if (words.size() > 2) {
      wordvec data {words.cbegin()+2, words.cend()};
      plain_file_ptr fp = dynamic_pointer_cast<plain_file>(file->get_contents());
      fp->writefile(data);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  // cout << word_range(words.cbegin(), words.cend()) << endl;
   if (words.size() != 2) {
      throw command_error("usage: mkdir dir");
   }
   string name;
   name = *(words.cbegin()+1);
   inode_ptr cwd = state.get_cwd();
   inode_ptr p = state.resolve_pathname(name);
   directory_ptr d;
   if (p == nullptr) {
      d = dynamic_pointer_cast<directory>(cwd->get_contents());
   } else {
      d = dynamic_pointer_cast<directory>(p->get_contents());
   }
   d->mkdir(name);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() < 2) {
      throw command_error("usage: prompt prompt");
   }
   auto it = words.begin();
   ++it;
   string new_prompt;
   for (; it != words.end(); ++it) {
      new_prompt += *it;
      new_prompt += " ";
   }
   state.set_prompt(new_prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   wordvec name_stack;
   inode_ptr cur = state.get_cwd();
   inode_ptr root = state.get_root();
   directory_ptr d;
   while (cur != root) {
      name_stack.push_back(cur->get_name());
      d = dynamic_pointer_cast<directory>(cur->get_contents());
      cur  = d->lookup("..");
   }
   if (name_stack.size() == 0) {
      cout << '/' << endl;
      return;
   }
   while (name_stack.size() > 0) {
      cout << '/';
      cout << name_stack.back();
      name_stack.pop_back();
   }
   cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2) {
      throw command_error("usage: rm filename");
   }
   string name = words.at(1);
   inode_ptr cwd = state.get_cwd();
   directory_ptr d = dynamic_pointer_cast<directory>(cwd->get_contents());
   if (d == nullptr) {
      throw command_error("rm: " + name + ": file does not exist");
   } else {
      d->remove(name);
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() != 2) {
      throw command_error("usage: rm filename");
   }
   string name = words.at(1);
   inode_ptr cwd = state.get_cwd();
   directory_ptr d = dynamic_pointer_cast<directory>(cwd->get_contents());
   if (d == nullptr) {
      throw command_error("rm: " + name + ": file does not exist");
   } else {
      d->remove(name);
   }
}

void fn_cmt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}




