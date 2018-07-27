// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
   //DEBUGF ('~', this << " -> " << ubig_value);
   string s = to_string(that);
   unsigned char c;
   auto it = s.end();
   for (; it != s.begin(); --it) {
      c = *it;
      this->ubig_value.push_back(c);
   }
   c = *it;
   this->ubig_value.push_back(c);
}

ubigint::ubigint (const string& that): ubig_value(0) {
//   DEBUGF ('~', "that = \"" << that << "\"");
   unsigned char c;
   auto it = that.cend();
   for (; it != that.cbegin(); --it) {
      c = *it;
      this->ubig_value.push_back(c);
   }
   c = *it;
   this->ubig_value.push_back(c);
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigvalue_t::const_iterator it1 = this->ubig_value.cbegin()+1;
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin()+1;
   ubigint result;
   int carry = 0, sum, num1, num2;
   unsigned char c;
   int size1 = this->ubig_value.size()-1, size2 = that.ubig_value.size()-1;
   int maxSize;
   if (size1 < size2)
      maxSize = size2;
   else
      maxSize = size1;
   for (int i = 0; i < maxSize; ++i) {
      if (i < size1) {
	 c = *it1;
	 num1 = c - '0';
	 ++it1;
      } else { num1 = 0; }
      if (i < size2) {
	 c = *it2;
	 num2 = c - '0';
	 ++it2;
      } else { num2 = 0; }
      sum = carry + num1 + num2;
      if(sum>9){
         carry = 1;
         sum -= 10;
	 c = sum + '0';
      } else {
         carry = 0;
	 c = sum + '0';
      }
      result.ubig_value.push_back(c);
   }
   if (carry == 1) {
      c = carry + '0';
      result.ubig_value.push_back(c);
   }
   while(result.ubig_value.size() > 0 && result.ubig_value.back() == '0') {
      result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   //if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigvalue_t::const_iterator it1 = this->ubig_value.begin()+1;
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin()+1;
   ubigint result;
   int left, right, temp, borrow = 0;
   unsigned char c;
   int size1 = this->ubig_value.size()-1, size2 = that.ubig_value.size()-1;
   int maxSize;
   if (size1 < size2)
      maxSize = size2;
   else
      maxSize = size1;
   for (int i = 0; i < maxSize; i++) {
      if (i < size1) {
	 c = *it1;
	 left = c - '0';
	 left += borrow;
	 ++it1;
      } else { left = 0; }
      if (i < size2) {
	 c = *it2;
	 right = c - '0';
	 //right += borrow;
	 ++it2;
      } else { right = 0; }
      if (left < right) {
	 left += 10;
	 borrow = -1;
      }
      temp = left - right;
      c = temp + '0';
      result.ubig_value.push_back(c);
   }
   while(result.ubig_value.size() > 0 && result.ubig_value.back() == '0') {
      result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigvalue_t::const_iterator it1 = this->ubig_value.begin();
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin();
   int size1 = this->ubig_value.size()-1, size2 = that.ubig_value.size()-1;
   ubigint result(0);
   for (int i = size1+size2; i >= 0; i--) {
      result.ubig_value[i] = '0';
   }
   udigit_t c, d;
   int a, b, e, f, sum;
   for (int i = 0; i < size1; i++) {
      c = '0';
      for (int j = 0; j < size2; j++) {
	 a = result.ubig_value[i+j] - '0';
	 b = this->ubig_value[i+1] - '0';
	 e = that.ubig_value[j+1] - '0';
	 f = c -'0';
	 sum = a + (b*e) + f;
	 d = (sum % 10) + '0';
	 result.ubig_value[i+j] = d;
	 f = sum / 10;
	 c = f + '0';
      }
      result.ubig_value[i+size2] = c;
   }
   while(result.ubig_value.size() > 0 && result.ubig_value.back() == '0') {
      result.ubig_value.pop_back();
   }
   return result;
}

void ubigint::multiply_by_2() {
   auto it = this->ubig_value.begin()+1;
   unsigned char c;
   int num, carry = 0;
   while (it != this->ubig_value.end()) {
      c = *it;
      num = c - '0';
      num *= 2;
      num += carry;
      carry = num / 10;
      num %= 10;
      c = num + '0';
      *it = c;
      ++it;
   }
   if (carry) {
      c = '1';
      this->ubig_value.push_back(c);
   }
}

void ubigint::divide_by_2() {
   auto it = this->ubig_value.begin()+1;
   int size = this->ubig_value.size();
   int a, b, carry = 0;
   udigit_t c;
   for (int i = 1; i < size; i++) {
      a = (this->ubig_value[i] -'0');
      a += carry;
      b = this->ubig_value[i+1] - '0';
      if (b < 0) {
	 b = 0;
      }
      if (a == 0) {
	 if (b != 0) {
	    a += 10;
	    carry = -1;
	 }
      } else if ((b%2) == 1) {
	 a += 10;
	 carry = -1;
      }
      a /= 2;
      c = a + '0';
      this->ubig_value[i] = c;
   }
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {"0"};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {"1"};
   ubigint quotient {"0"};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
cout << "power of 2 = " << power_of_2;
cout << "divisor = " << divisor;
cout << "remainder = " << remainder;
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if (this->ubig_value.size() != that.ubig_value.size()) {
      return false;
   } else {
      auto it1 = this->ubig_value.end();
      auto it2 = that.ubig_value.end();
      unsigned char c, d;
      int left, right;
      while (it1 != that.ubig_value.begin()) {
	 right = *it1;
	 right = *it2;
	 left = c - '0';
	 right = d - '0';
	 if (left != right) {
	    return false;
	 }
	 --it1; --it2;
      }
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   if (this->ubig_value.size() < that.ubig_value.size()) {
      return true;
   } else {
      unsigned char c, d;
      int left, right;
      int size = this->ubig_value.size()-1;
      for (int i = size; i > 0; --i) {
	 c = this->ubig_value[i];
	 d = that.ubig_value[i];
	 left = c -'0';
	 right = d -'0';
	 if (left < right) {
	    return true;
	 }
      }
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   unsigned char currc;
   auto itor = that.ubig_value.end();
   //currc = *itor;
   //out << currc;
   for ( ; itor >= that.ubig_value.begin(); --itor) {
      currc = *itor;
      out << currc;
   }
   return out;
}
/*
int main() {
   unsigned long a = 4;
   ubigint left(a);
   cout<< left << endl;
//   unsigned long s = -4;
  // string a = to_string(s);
  // cout << a << endl;
   return 0;
}*/
