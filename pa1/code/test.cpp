#include <deque>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
using namespace std;

#include <unistd.h>

#include "ubigint.h"

class Test {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
      using udigit_t = unsigned char;
      using ubigvalue_t = vector<udigit_t>;
      ubigvalue_t ubig_value;
   public:
      Test(const string&);
      int main();
};

Test::Test(const string& that) {
   char c;
   for (auto it = that.cend(); it != that.cbegin(); --it) {
      c = *it;
      this->ubig_value.push_back(c);
   }
}

stream& operator<< (ostream& out, const Test& that) {
   return out << "ubigint(" << that.uvalue << ")";
}

int main() {
   Test left("4329");
   //ubigint right("1234");
   //cout << left.ubig_value << endl;
   //DEBUGF ('d', "result = " << left);
   cout << left << endl;
   return 0;
}






