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

#include "../../include/votca/tools/application.h"
#include "../../include/votca/tools/globals.h"
#include "../../include/votca/tools/property.h"
#include "../../include/votca/tools/propertyiomanipulator.h"
#include "../../include/votca/tools/version.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>

using namespace std;
using namespace votca::tools;
namespace po = boost::program_options;

class VotcaProperty : public Application {

 public:
  VotcaProperty();
  ~VotcaProperty() override;

  string ProgramName() override { return "votca_property"; }

  void HelpText(ostream& out) override {
    out << "Helper for parsing XML files";
  }

  void Initialize() override {

    format = "XML";
    level = 1;

    AddProgramOptions()("file", po::value<string>(), "xml file to parse")(
        "format", po::value<string>(), "output format [XML TXT TEX]")(
        "level", po::value<votca::Index>(), "output from this level ");
  };

  bool EvaluateOptions() override {
    CheckRequired("file", "Missing XML file");
    return true;
  };

  void Run() override {

    file = _op_vm["file"].as<string>();

    if (_op_vm.count("format")) {
      format = _op_vm["format"].as<string>();
    }
    if (_op_vm.count("level")) {
      level = _op_vm["level"].as<votca::Index>();
    }

    try {

      Property p;
      map<string, PropertyIOManipulator*> _mformat;
      _mformat["XML"] = &XML;
      _mformat["TXT"] = &TXT;
      _mformat["TEX"] = &TEX;
      _mformat["HLP"] = &HLP;
      p.LoadFromXML(file);

      if (_mformat.find(format) != _mformat.end()) {
        PropertyIOManipulator* piom = _mformat.find(format)->second;
        piom->setLevel(level);
        piom->setIndentation("");
        piom->setColorScheme<csRGB>();
        cout << *piom << p;
      } else {
        cout << "format " << format << " not supported \n";
      }

    } catch (std::exception& error) {
      cerr << "an error occurred:\n" << error.what() << endl;
    }
  };

 private:
  string file;
  string format;
  votca::Index level;
};

VotcaProperty::VotcaProperty(void) = default;

VotcaProperty::~VotcaProperty(void) = default;

int main(int argc, char** argv) {
  VotcaProperty vp;
  return vp.Exec(argc, argv);
}
