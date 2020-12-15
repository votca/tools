/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
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

// Standard includes
#include <iostream>

// Third party includes
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// Local VOTCA includes
#include "votca/tools/application.h"
#include "votca/tools/globals.h"
#include "votca/tools/propertyiomanipulator.h"
#include "votca/tools/version.h"

// Local private VOTCA includes
#include "votca_tools_config.h"

namespace votca {
namespace tools {
using namespace std;
Application::Application()
    : _op_desc("Allowed options"), _continue_execution(true) {}

Application::~Application() = default;

void Application::ShowHelpText(std::ostream &out) {
  out << "==================================================\n";
  out << "========   VOTCA (http://www.votca.org)   ========\n";
  out << "==================================================\n\n";

  out << "please submit bugs to " TOOLS_BUGREPORT "\n\n";
  out << ProgramName();
  if (VersionString() != "") {
    out << ", version " << VersionString();
  }
  out << endl << "votca_tools, version " << ToolsVersionStr() << "\n\n";

  HelpText(out);

  // remove Hidden group from the option list and print
  out << "\n\n" << VisibleOptions() << endl;
}

std::string Application::ShowManPage() {
  std::string version = (VersionString() != "") ? VersionString() : "1.0";
  std::stringstream stream;
  stream << boost::format(globals::man::header) % ProgramName() % version;
  stream << boost::format(globals::man::name) % ProgramName() % globals::url;
  stream << boost::format(globals::man::synopsis) % ProgramName();
  std::stringstream ss;
  HelpText(ss);
  stream << boost::format(globals::man::description) % ss.str();
  stream << boost::format(globals::man::options);

  for (const auto &option : _op_desc.options()) {
    string format_name = option->format_name() + option->format_parameter();
    stream << boost::format(globals::man::option) % format_name %
                  option->description();
  }

  stream << boost::format(globals::man::authors) % globals::email;
  stream << boost::format(globals::man::copyright) % globals::url;
  return stream.str();
}

void Application::WriteManFile() {
  ofstream man_file;
  man_file.open(ProgramName() + "_man.rst");
  man_file << Application::ShowManPage();
  man_file.close();
}

void Application::ShowTEXPage(std::ostream &out) {
  string program_name = ProgramName();
  boost::replace_all(program_name, "_", "\\_");
  out << boost::format(globals::tex::section) % program_name;
  out << boost::format(globals::tex::label) % ProgramName();
  std::stringstream ss, os;
  HelpText(ss);
  out << boost::format(globals::tex::description) % ss.str();

  for (const auto &option : _op_desc.options()) {
    string format_name =
        option->format_name() + " " + option->format_parameter();
    boost::replace_all(format_name, "-", "{-}");
    os << boost::format(globals::tex::option) % format_name %
              option->description();
  }
  out << boost::format(globals::tex::options) % os.str();
}

int Application::Exec(int argc, char **argv) {
  try {
    //_continue_execution = true;
    AddProgramOptions()("help,h", "  display this help and exit");
    AddProgramOptions()("verbose", "  be loud and noisy");
    AddProgramOptions()("verbose1", "  be very loud and noisy");
    AddProgramOptions()("verbose2,v", "  be extremly loud and noisy");
    AddProgramOptions("Hidden")("man", "  output man-formatted manual pages");
    AddProgramOptions("Hidden")(
        "manfile", " write to a rst formatted file the manual pages");
    AddProgramOptions("Hidden")("tex", "  output tex-formatted manual pages");

    Initialize();  // initialize program-specific parameters

    ParseCommandLine(argc,
                     argv);  // initialize general parameters & read input file

    Log::current_level = Log::error;
    if (_op_vm.count("verbose")) {
      Log::current_level = Log::warning;
    }
    if (_op_vm.count("verbose1")) {
      Log::current_level = Log::info;
    }

    if (_op_vm.count("verbose2")) {
      Log::current_level = Log::debug;
    }

    if (_op_vm.count("man")) {
      cout << ShowManPage();
      return 0;
    }

    if (_op_vm.count("manfile")) {
      WriteManFile();
      return 0;
    }

    if (_op_vm.count("tex")) {
      ShowTEXPage(cout);
      return 0;
    }
    if (_op_vm.count("help")) {
      ShowHelpText(cout);
      return 0;
    }

    if (!EvaluateOptions()) {
      ShowHelpText(cout);
      return -1;
    }

    if (_continue_execution) {
      Run();
    } else {
      cout << "nothing to be done - stopping here\n";
    }
  } catch (std::exception &error) {
    cerr << "an error occurred:\n" << error.what() << endl;
    return -1;
  }
  return 0;
}

boost::program_options::options_description_easy_init
    Application::AddProgramOptions(const string &group) {
  // if no group is given, add it to standard options
  if (group == "") {
    return _op_desc.add_options();
  }

  // does group already exist, if yes, add it there
  std::map<string, boost::program_options::options_description>::iterator iter;
  iter = _op_groups.find(group);
  if (iter != _op_groups.end()) {
    return iter->second.add_options();
  }

  // no group with given name was found -> create group
  _op_groups.insert(
      make_pair(group, boost::program_options::options_description(group)));

  return _op_groups[group].add_options();
}

void Application::ParseCommandLine(int argc, char **argv) {
  namespace po = boost::program_options;

  // default options should be added to visible (the rest is handled via a map))
  _visible_options.add(_op_desc);

  // add all categories to list of available options
  for (const auto &pair : _op_groups) {
    _op_desc.add(pair.second);
    if (pair.first != "Hidden") {
      _visible_options.add(pair.second);
    }
  }

  // parse the command line
  try {
    po::store(po::parse_command_line(argc, argv, _op_desc), _op_vm);
    po::notify(_op_vm);
  } catch (boost::program_options::error &err) {
    throw runtime_error(string("error parsing command line: ") + err.what());
  }
}

void Application::CheckRequired(const string &option_name,
                                const string &error_msg) {
  if (!_op_vm.count(option_name)) {
    ShowHelpText(cout);
    throw std::runtime_error("missing argument " + option_name + "\n" +
                             error_msg);
  }
}

void Application::PrintDescription(std::ostream &out,
                                   const string &calculator_name,
                                   const string help_path, HelpType help_type) {
  boost::format _format("%|3t|%1% %|20t|%2% \n");
  string help_string;
  boost::filesystem::path arg_path;
  Property options;
  // loading the documentation xml file from VOTCASHARE
  string xmlFile = (arg_path / tools::GetVotcaShare() / help_path /
                    (boost::format("%1%.%2%") % calculator_name % "xml").str())
                       .string()
                       .c_str();

  try {

    options.LoadFromXML(xmlFile);
    Property &calculator_options = options.get("options." + calculator_name);
    Property::AttributeIterator atr_it =
        calculator_options.findAttribute("help");

    if (atr_it != calculator_options.lastAttribute()) {
      help_string = (*atr_it).second;
    } else {
      if (Log::current_level > 0) {
        out << _format % calculator_name % "Undocumented";
      }
      return;
    }

    switch (help_type) {
      default:
        break;
      case HelpShort:  // short description of the calculator
        out << _format % calculator_name % help_string;
        break;
      case HelpLong:
        votca::tools::PropertyIOManipulator iom(
            votca::tools::PropertyIOManipulator::HLP, 2, "");
        out << iom << options;
        break;
    }

  } catch (std::exception &) {
    if (Log::current_level > 0) {
      out << _format % calculator_name % "Undocumented";
    }
  }
}

}  // namespace tools
}  // namespace votca
