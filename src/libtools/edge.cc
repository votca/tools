/*
 *            Copyright 2009-2018 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <list>
#include <vector>

#include <votca/tools/edge.h>

namespace votca {
namespace tools {

using namespace std;

Edge::Edge(int ID1, int ID2) {
  vertices.first  = ID1;
  vertices.second = ID2;
  if (ID2 < ID1) {
    vertices.first  = ID2;
    vertices.second = ID1;
  }
}

int Edge::getOtherV(int ver) const {
  if (ver == vertices.first) {
    return vertices.second;
  } else {
    return vertices.first;
  }
}

bool Edge::contains(int ID) const {
  return (vertices.first == ID || vertices.second == ID);
}

bool Edge::operator==(const Edge ed) const {
  if (this->vertices.first == ed.vertices.first &&
      this->vertices.second == ed.vertices.second)
    return true;
  if (this->vertices.second == ed.vertices.first &&
      this->vertices.first == ed.vertices.second)
    return true;
  return false;
}

bool Edge::operator!=(const Edge ed) const { return !(*this == ed); }

bool Edge::operator<(const Edge ed) const {
  if (this->vertices.first < ed.vertices.first) return true;
  if (this->vertices.first > ed.vertices.first) return false;
  if (this->vertices.second < ed.vertices.second) return true;
  return false;
}

bool Edge::operator<=(const Edge ed) const {
  return (*this < ed || *this == ed);
}

bool Edge::operator>(const Edge ed) const { return !(*this <= ed); }

bool Edge::operator>=(const Edge ed) const { return !(*this < ed); }

ostream& operator<<(ostream& os, const Edge ed) {
  os << "Vertices" << endl;
  os << ed.vertices.first << " " << ed.vertices.second << endl;
  return os;
}
}
}
