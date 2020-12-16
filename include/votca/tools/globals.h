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

#ifndef VOTCA_TOOLS_GLOBALS_H
#define VOTCA_TOOLS_GLOBALS_H

// Standard includes
#include <string>

namespace votca {

struct Log {
  /// be loud and noisy
  enum Level { error = 0, warning = 1, info = 2, debug = 3 };

  static Level current_level;

  static bool verbose() { return current_level > Level::error; }
};
namespace tools {

/**
    \brief Retrieves VOTCASHARE environment variable. Throws std::runtime_error
   if not set.
*/
std::string GetVotcaShare();
bool VotcaShareSet();
/**
    \brief class to store global variables

    This class is used to access global variables
*/
struct globals {

  /// web of the package
  static std::string url;
  /// email address of the developers
  static std::string email;

  /// If Eigen is overloaded with MKL
  static bool VOTCA_MKL;

  /// man pages format strings
  struct man {
    static std::string option;
    static std::string header;
    static std::string name;
    static std::string authors;
    static std::string copyright;
    static std::string synopsis;
    static std::string description;
    static std::string options;
  };

  /// man pages in rst format strings
  struct manrst {
    static std::string option;
    static std::string header;
    static std::string name;
    static std::string authors;
    static std::string copyright;
    static std::string synopsis;
    static std::string description;
    static std::string options;
  };

  /// TEX pages format strings
  struct tex {
    static std::string section;
    static std::string label;
    static std::string description;
    static std::string options;
    static std::string option;
  };
};

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_GLOBALS_H
