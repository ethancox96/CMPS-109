// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"
#include "ubigint.h"

bigint::bigint (long that): is_negative (that < 0) {
   //DEBUGF ('~', this << " -> " << uvalue)
   if (that < 0) {
      unsigned long b = abs(that);
      ubigint a {b};
      this->uvalue = a;
   } else {
      unsigned long b = (unsigned long) that;
      ubigint a {b};
      this->uvalue = a;
   }
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   unsigned char c;
   auto it = that.cbegin();
   if (*it == '_') {
      this->is_negative = true;
      ++it;
   }
   const string& s {it, that.cend()};
   ubigint u {s};
   this->uvalue = u;
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   bigint sum;
   if (this->is_negative == that.is_negative) {
      sum.uvalue = this->uvalue + that.uvalue;
      if (this->is_negative) {
	 sum.is_negative = true;
      }
   } else {
       if (that.uvalue < this->uvalue) {
         sum.uvalue = this->uvalue - that.uvalue;
	 sum.is_negative = this->is_negative;
      } else if (this->uvalue < that.uvalue) {
         sum.uvalue = that.uvalue - this->uvalue;
	 sum.is_negative = that.is_negative;
      } else {
	 ubigint temp {0};
	 sum.uvalue = temp;
      }
   }
   return sum;
}

bigint bigint::operator- (const bigint& that) const {
   bigint difference;
   if (this->is_negative != that.is_negative) {
      difference.is_negative = this->is_negative;
      difference.uvalue = this->uvalue + that.uvalue;
   } else {
      if (this->uvalue < that.uvalue) {
	 difference.uvalue = that.uvalue - this->uvalue;
	 if (that.is_negative) {
	    difference.is_negative = false;
	 } else {
	    difference.is_negative = true;
	 }
      } else if (that.uvalue < this->uvalue) {
	 difference.uvalue = this->uvalue - that.uvalue;
         difference.is_negative = this->is_negative;
      } else {
	 ubigint temp {0};
         difference.uvalue = temp;
      }
   }
   return difference;
}

bigint bigint::operator* (const bigint& that) const {
   bigint product;
   product.uvalue = this->uvalue * that.uvalue;
   if (this->is_negative == that.is_negative) {
      product.is_negative = false;
   } else {
      product.is_negative = true;
   }
   return product;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint quotient;
   quotient.uvalue = this->uvalue / that.uvalue;
   if (this->is_negative == that.is_negative) {
      quotient.is_negative = false;
   } else {
      quotient.is_negative = true;
   }
   return quotient;
}

bigint bigint::operator% (const bigint& that) const {
   bigint remainder;
   remainder.uvalue = this->uvalue / that.uvalue;
   if (this->is_negative == that.is_negative) {
      remainder.is_negative = false;
   } else {
      remainder.is_negative = true;
   }
   return remainder;
}

bool bigint::operator== (const bigint& that) const {
   return this->is_negative == that.is_negative && this->uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.uvalue;
}
/*
int main() {
   bigint left("1111111111111");
   bigint right("2222222222222");
   //ubigint a("5");
   //bigint right(a, true);
   cout << left;
   cout << right;
   cout << left*right;
   return 0;
}
*/


