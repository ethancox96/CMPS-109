// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <iomanip>
using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   directory_ptr d = dynamic_pointer_cast<directory>(root->get_contents());
   d->set_root(root);
}

const string& inode_state::prompt() { return prompt_; }

inode_ptr inode_state::get_root() {
   return root;
}

inode_ptr inode_state::get_cwd() {
   return cwd;
}

void inode_state::set_cwd(inode_ptr p) {
   cwd = p;
}

inode_ptr inode_state::resolve_pathname(const string& pathname) {
   inode_ptr p;
   if (pathname.at(0) == '/') {
      return root;
   } else {
      p = cwd;
      directory_ptr d = dynamic_pointer_cast<directory>(p->get_contents());
      p = d->lookup(pathname);
      return p;
   }
}

void inode_state::set_prompt(string& new_prompt) {
   prompt_ = new_prompt;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
	   this->type = type;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
	   this->type = type;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

void inode::set_name(string name) {
   this->name = name;
}

string inode::get_name() {
   return name;
}

int inode::get_next_inode_nr() const {
   return next_inode_nr;
}

base_file_ptr inode::get_contents() {
   return contents;
}

file_type inode::get_type() {
   return type;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   for (auto word = data.begin(); word != data.end(); ++word) {
      size += word->size();
   }
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   if (data.empty()) {
      data = words;
   }
   else {
      data.erase(data.begin(), data.end());
      data = words;
   }
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}
/*
void plain_file::set_root(inode_ptr root) {
   throw file_error ("is a plain file");
}

void plain_file::set_child(inode_ptr parent, inode_ptr child) {
   throw file_error ("is a plain file");
}
*/

size_t directory::size() const {
   size_t size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   auto it = dirents.find(filename);
   if (it != dirents.end()) {
      dirents.erase(it);
   } else {
      throw file_error("does not exist");
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if (dirents.find(dirname) != dirents.end()) {
      throw file_error("directory already exists");
   }
   inode_ptr curnode = make_shared<inode>(file_type::DIRECTORY_TYPE);
   curnode->set_name(dirname);
   dirents.insert(make_pair(dirname, curnode));
   inode_ptr parent = dirents.at(".");
   directory_ptr d = dynamic_pointer_cast<directory>(curnode->get_contents());
   d->set_child(parent, curnode);
   return curnode;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   if (dirents.find(filename) != dirents.end()) {
      throw file_error("file exists");
   } else {
      inode_ptr file = make_shared<inode>(file_type::PLAIN_TYPE);
      dirents.insert(make_pair(filename, file));
      file->set_name(filename);
      return file;
   }
}

map<string, inode_ptr> directory::get_dirents() {
   return dirents;
}

void directory::set_root(inode_ptr root) {
   dirents.insert(make_pair(".", root));
   dirents.insert(make_pair("..", root));
   root->set_name("/");
}

void directory::set_child(inode_ptr parent, inode_ptr child) {
   dirents.insert(make_pair("..", parent));
   dirents.insert(make_pair(".", child));
}

inode_ptr directory::lookup(const string& name) {
   auto it = dirents.find(name);
   if (it == dirents.end()) {
      return nullptr;
   }
   return it->second;
}

void directory::ls(ostream& out) {
   string suffix;
   size_t size;
   auto it = dirents.begin();
   inode_ptr p = it->second;
   directory_ptr d = dynamic_pointer_cast<directory>(it->second->get_contents());
   out << setw(6) << "1"
       << setw(6) << d->size()
       << "\t" << "." << endl;
   out << setw(6) << "1"
       << setw(6) << d->size()
       << "\t" << ".." << endl;
   ++it; ++it;
   for (; it != dirents.end(); ++it) {
      if (it->second->get_type() == file_type::DIRECTORY_TYPE) {
	 suffix = "/";
	 directory_ptr d = dynamic_pointer_cast<directory>(it->second->get_contents());
	 size = d->size();
      } else {
	 suffix = "";
	 base_file_ptr file = dynamic_pointer_cast<base_file>(it->second->get_contents());
	 size = file->size();
      }
      out << setw(6) << it->second->get_inode_nr()
          << setw(6) << size
          << "\t" << it->first + suffix << endl;
   }
}

void inode_state::lsr(string& pathname, ostream& out) {
   inode_ptr p = resolve_pathname(pathname);
   directory_ptr d = dynamic_pointer_cast<directory>(p->get_contents());
   d->ls(out);
   vector<inode_ptr> subdirs = d->get_subdirs();
   for (size_t i = 0; i < subdirs.size(); ++i) {
      string name = subdirs.at(i)->get_name();
      lsr(name, out);
   }
}

vector<inode_ptr> directory::get_subdirs() {
   vector<inode_ptr> subdirs;
   auto it = dirents.begin();
   for (; it != dirents.end(); ++it) {
      if (it->second->get_type() == file_type::DIRECTORY_TYPE) {
	 if (it->first != "." && it->first != "..") {
	    subdirs.push_back(it->second);
	 }
      }
   }
   return subdirs;
}

/*
int main() {
   inode_state is {};
   inode_ptr root = is.get_root();
   inode_ptr cwd = is.get_cwd();
   directory_ptr root_dir = dynamic_pointer_cast<directory>(root->get_contents());
   root_dir->mkdir("dir");
   inode_ptr p = is.resolve_pathname("dir");
   directory_ptr d = dynamic_pointer_cast<directory>(p->get_contents());
cout << d << endl;
   d->mkfile("file");
   
   d->mkfile("file2");
   d->mkfile("file3");
   
   return 0;
}
*/


