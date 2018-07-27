// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <memory>
#include <string>
#include <vector>
#include <math.h>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"       , &interpreter::make_text       },
   {"ellipse"    , &interpreter::make_ellipse    },
   {"circle"     , &interpreter::make_circle     },
   {"polygon"    , &interpreter::make_polygon    },
   {"rectangle"  , &interpreter::make_rectangle  },
   {"square"     , &interpreter::make_square     },
   {"diamond"    , &interpreter::make_diamond    },
   {"triangle"   , &interpreter::make_triangle   },
   {"equilateral", &interpreter::make_equilateral},
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      cout << name << ": no such shape" << endl;
   } else {
      rgbcolor color {begin[0]};
      vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
      object obj (itor->second, where, color);
      window::push_back(obj);
   }
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 2) throw runtime_error ("syntax error");
   rgbcolor color {begin[0]};
   GLfloat thickness {from_string<GLfloat>(begin[1])};
   window::setcolor(color);
   window::setthick(thickness);
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 1) throw runtime_error ("syntax error");
   int pixels = stoi(begin[0]);
   window::setpixels(pixels);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
cout << "in make_text" << endl;
   auto itor = font.find(begin[0]);
   void* font = itor->second;
   auto itor2 = begin;
   itor2++;
   string words = "";
   while (itor2 != end) {
      words += *itor2;
      words += " ";
      itor2++;
   }
   return make_shared<text> (font, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat>(begin[0]);
   GLfloat height = from_string<GLfloat>(begin[1]);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat diameter = from_string<GLfloat>(begin[0]);
   return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list points;
   auto itor = begin;
   float x = 0.0, y = 0.0, total = 0.0;
   while (itor != end) {
      x += from_string<GLfloat> (*itor);
      itor++;
      y += from_string<GLfloat> (*itor);
      if (itor == end) {
         throw runtime_error("incorrect number of vectors");
      }
      total += 1.0;
      itor++;
   }
   x = x/total;
   y = y/total;
   itor = begin;
   while (itor != end) {
      vertex temp;
      temp.xpos = from_string<GLfloat> (*itor);
      itor++;
      temp.ypos = from_string<GLfloat> (*itor);
      points.push_back(temp);
      itor++;
   }
   return make_shared<polygon> (points);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat>(begin[0]);
   GLfloat length = from_string<GLfloat>(begin[1]);
   return make_shared<rectangle> (width, length);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat>(begin[0]);
   return make_shared<square> (width);
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   if (end - begin != 6) throw runtime_error("syntax error");
   vertex_list points;
   auto itor = begin;
   float x = 0.0, y = 0.0, total = 0.0;
   while (itor != end) {
      x += from_string<GLfloat> (*itor);
      itor++;
      y += from_string<GLfloat> (*itor);
      if (itor == end) {
         throw runtime_error("incorrect number of vectors");
      }
      total += 1.0;
      itor++;
   }
   x = x/total;
   y = y/total;
   itor = begin;
   while (itor != end) {
      vertex temp;
      temp.xpos = from_string<GLfloat> (*itor);
      itor++;
      temp.ypos = from_string<GLfloat> (*itor);
      points.push_back(temp);
      itor++;
   }
   return make_shared<triangle> (points);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat length = from_string<GLfloat>(begin[0]);
   GLfloat height = (length/2) * sqrt(3);
   vertex_list points {{0.0, 0.0}, {length, 0.0}, {height, length}};
   return make_shared<equilateral>(points);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat>(begin[0]);
   GLfloat height = from_string<GLfloat>(begin[1]);
   return make_shared<diamond>(width, height);
}




