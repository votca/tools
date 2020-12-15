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

// Local VOTCA includes
#include "votca/tools/globals.h"
#include <stdexcept>

namespace votca {

Log::Level Log::current_level = Log::info;
namespace tools {

std::string GetVotcaShare() {
  char *votca_share = getenv("VOTCASHARE");
  if (votca_share == nullptr) {
    throw std::runtime_error("VOTCASHARE not set, cannot open help files.");
  }
  return std::string(votca_share);
}

bool VotcaShareSet() { return (getenv("VOTCASHARE") != nullptr); }

std::string globals::url = "http://www.votca.org";
std::string globals::email = "devs@votca.org";

std::string globals::man::option("\n**%1%** %2%\n");

std::string globals::man::header("%1% %2%\n#################\n\n");

std::string globals::man::name(
    "\nNAME\n****\n"
    "%1% - Part of the VOTCA package\n"
    "For more info please visit %2%\n\n");

std::string globals::man::authors(
    "\n\nAUTHORS\n*******\n"
    "Written and maintained by the VOTCA Development Team <%1%>\n");

std::string globals::man::copyright(
    "\nCOPYRIGHT\n*********\n"
    "Copyright 2009\\-2020 The VOTCA Development Team (%1%).\n"
    "Licensed under the Apache License, Version 2.0 (the \"License\") "
    "you may not use this file except in compliance with the License. "
    "You may obtain a copy of the License at"

    "http://www.apache.org/licenses/LICENSE\\-2.0\n"

    "Unless required by applicable law or agreed to in writing, software "
    "distributed under the License is distributed on an \"AS IS\" BASIS, "
    "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. "
    "See the License for the specific language governing permissions and "
    "limitations under the License.");

std::string globals::man::synopsis(
    "\nSYNOPSIS\n********\n"
    "**%1%** [*OPTION*] [*ARGUMENT*]\n");

std::string globals::man::description(
    "\nDESCRIPTION\n***********\n"
    "%1%\n");

std::string globals::man::options("\nOPTIONS\n*******\n");

std::string globals::tex::section("\n\\subsection{%1%}");
std::string globals::tex::label("\n\\label{prog:%1%}");
std::string globals::tex::description("\n%1%");
std::string globals::tex::option("\n\\item[ ] \\texttt{%1%} %2%");
std::string globals::tex::options(
    "\n\\begin{compactitem}%1%\n\\end{compactitem}\n");

}  // namespace tools
}  // namespace votca
