/* 
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <sstream>
#include <votca/tools/datacollection.h>

using namespace std;

namespace votca { namespace tools {

ostream& operator<<(ostream& out, DataCollection<double>::selection &sel)
{
    if(sel.empty()) {
        out << "-- empty selection --" << endl;
        return out;
    }
    
    stringstream s;
    int written;
    for(size_t i=0; ; ++i) {
        s.clear();
        s.str("");
        s.setf(ios::scientific);
        written = 0;
        for(size_t j=0; j<sel.size(); j++) {      
            if(i >= sel[j].size()) {
                s << " -";
                continue;
            }
            written++;
            s << " " << (double)sel[j][i];
            
        }
        if(written == 0) return out;
        out << i << s.str() << endl;        
    }
    return out;
}

}}
