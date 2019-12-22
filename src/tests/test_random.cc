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

#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE random2_test
#include "../../include/votca/tools/random.h"
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace votca::tools;

BOOST_AUTO_TEST_SUITE(random2_test)

BOOST_AUTO_TEST_CASE(random_int_test) {
  Random random;
  votca::Index seed = 1;
  random.init(seed);
  random.setMaxInt(50);
  std::vector<votca::Index> results;
  votca::Index number = 1e5;
  results.reserve(number);
  for (votca::Index i = 0; i < number; i++) {
    results.push_back(random.rand_uniform_int());
  }

  // average should be close to 25
  double average = std::accumulate(results.begin(), results.end(), 0);
  average /= double(number);

  BOOST_CHECK_CLOSE(average, 25, 1.0);
}

BOOST_AUTO_TEST_CASE(random_double_test) {
  Random random;
  votca::Index seed = 1;
  random.init(seed);
  std::vector<double> results;
  votca::Index number = 1e5;
  results.reserve(number);
  for (votca::Index i = 0; i < number; i++) {
    results.push_back(random.rand_uniform());
  }

  // average should be close to 0.5
  double average = std::accumulate(results.begin(), results.end(), 0.0);
  average /= double(number);

  BOOST_CHECK_CLOSE(average, 0.5, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
