// $Id: listmap.tcc,v 1.11 2018-01-25 14:19:14-08 - - $

#include "listmap.h"
#include "xpair.h"
#include "debug.h"
using str_str_map = listmap<string,string>;
using str_str_pain = str_str_map::value_type;
//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}
/*
//
// iterator listmap::begin();
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
   return iterator(this, head);
}

//
// iterator listmap::end();
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end() {
   return iterator(this, tail);
}
*/

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   Less less;
   node* temp = new node(NULL, NULL, pair);
   temp->next = NULL;
   temp->prev = NULL;
   auto it = begin();
   if (head == NULL) {
      head = temp;
      tail = temp;
      return it;
   } else {
      node* track = head;
      while (track != NULL) {
	 if (less(temp->value.first, track->value.first)) {
	    temp->next = track;
	    if (track == head) {
	       temp->prev = NULL;
	       track->prev = temp;
	       head = temp;
	       --it;
	       return it;
	    } else {
	       temp->prev = track->prev;
               track->prev->next = temp;
	       track->prev = temp;
	       --it;
	       return it;
	    }
	 } else if (temp->value.first == track->value.first) {
	    track->value.second = temp->value.second;
	    return it;
	 } else {
	    track = track->next;
	    ++it;
	 }
      }
      temp->next = NULL;
      temp->prev = tail;
      tail->next = temp;
      tail = temp;
   }
   return it;
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   DEBUGF ('l', that);
   auto it = begin();
   while (it != end()) {
      if (it->first == that) {
	 break;
      } else {
	 ++it;
      }
   }
   return it;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   DEBUGF ('l', &*position);
cout << position->first << " " << position->second << endl;
   node* temp = position.where_();
   if (temp == head) {
cout << "in if statement" << endl;
      head = temp->next;
      head->prev = NULL;
      ++position;
   } else if (temp == tail) {
cout << "in if-else statement" << endl;
      tail = temp->prev;
      tail->next = NULL;
      --position;
   } else {
cout << "in else statement" << endl;
      temp->prev->next = temp->next;
      temp->next->prev = temp->prev;
   }
   delete temp;
   return position;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   DEBUGF ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   DEBUGF ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}

/*//
// listmap::iterator& listmap::iterator::erase()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::void
listmap<Key,Value,Less>::iterator::erase() {
   DEBUGF ('l', where);
   node* temp = where;
   if (where == head) {
      temp = temp->next;
      temp->prev = NULL;
      head = temp;
   } else if (where == tail) {
      temp = temp->prev;
      temp->next = NULL;
      tail = temp;
   } else {
      temp->prev->next = temp->next;
      temp->next->prev = temp->prev;
   }
   delete temp;
}*/


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}
/*
int main() {
   str_str_map test;
cout << "listmap created" << endl;
   const string key("a");
   string val("b");
cout << "before xpair" << endl;
   const xpair<const string, string> pair(key, val);
   cout << "before insert" << endl;
   test.insert(pair);
   cout << "after insert" << endl;
   return 0;
}
*/



