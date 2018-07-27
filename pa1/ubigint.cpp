// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
#include "ubigint.h"

ubigint::ubigint (unsigned long that): ubig_value (that) {
   //DEBUGF ('~', this << " -> " << ubig_value)
}

ubigint::ubigint (const string& that): ubig_value(0) {
  // DEBUGF ('~', "that = \"" << that << "\"");
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
   ubigvalue_t::const_iterator it1 = this->ubig_value.cbegin();
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin();
   ubigint result;
   int carry = 0, sum, num1, num2;
   unsigned char c;
   int size1 = this->ubig_value.size(), size2 = that.ubig_value.size();
   int maxSize;
   if (size1 < size2)
      maxSize = size2;
   else
      maxSize = size1;
   for (int i = 0; i < maxSize; ++i) {
cout << "i = " << i << endl;
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
      sum %= 10;
      carry /= 10;
      c = sum + '0';
cout << "carry = " << carry;
cout << " num1 = " << num1;
cout << " num2 = " << num2 << endl;
cout << "sum = " << sum << endl;
cout << "c = " << c << endl;
      result.ubig_value.push_back(c);
   }
   if (carry == 1) {
      c = (unsigned char) carry;
      result.ubig_value.push_back(c);
   }
   while(result.ubig_value.size() > 0 && result.ubig_value.back() == '0') {
      result.ubig_value.pop_back();
   }
   return result;
}
/*
ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigvalue_t::const_iterator it1 = this->ubig_value.begin();
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin();
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigvalue_t::const_iterator it1 = this->ubig_value.begin();
   ubigvalue_t::const_iterator it2 = that.ubig_value.begin();
}
*/
void ubigint::multiply_by_2() {
   ubigvalue_t::const_iterator it1 = this->ubig_value.cend();
   int temp, carry;
   while (it1 != this->ubig_value.cbegin()) {
      temp = (int) *it1;
      temp *= 2;
      if (temp >= 10) {
	 carry = temp % 10;
      }
   }
}
/*
void ubigint::divide_by_2() {
   ubigvalue_t::const_iterator it1 = this->ubig_value.begin();
   
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
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
   return uvalue == that.uvalue;
}
*/
bool ubigint::operator< (const ubigint& that) const {
   if (this->ubig_value.size() < that.ubig_value.size()) {
      return true;
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
   out << endl;
   return out;
}

int main() {
   ubigint left("123");
   ubigint right("234");
   cout << left << right;
   cout << left + right;
   return 0;
}
