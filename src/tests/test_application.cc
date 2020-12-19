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

#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE application_test

// Third party includes
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/tools/application.h"

using namespace ::votca;

BOOST_AUTO_TEST_SUITE(application_test)

BOOST_AUTO_TEST_CASE(write_man_page_test) {

  class App : public tools::Application {

   public:
    std::string ProgramName() override { return "app"; }
    void HelpText(std::ostream& out) override { out << "Do something awesome"; }
    void Run() override { ; }
    void Initialize() override { ; }
    bool EvaluateOptions() override {
      std::ofstream out;
      ShowRstPage(out);
      return true;
    }
  };

  App app;
  const char* name = "man";
  char* s1 = strdup(name);
  char* argv[1] = {s1};
  app.Exec(1, argv);
  free(s1);
}

BOOST_AUTO_TEST_SUITE_END()
