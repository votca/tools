For more detailed information about the changes see the history of the
`repository <https://github.com/votca/tools/commits/stable>`__.

Version 2022-dev
================

-  create script to convert help to rst (#312, #328)
-  add rst pages to doc (#330, #332)
-  remove tex and man output from app class (#329)
-  fix example rst section (#327, #339)
-  remove author and copyright from rst (#331)
-  fix rst warnings (#334, #346, #348)
-  export PYTHONPATH in VOTCARC (#340)
-  drop csh support in VOTCARC (#342)
-  add missing cmath include (#351)
-  removed eigenvalue algorithm, now done by eigen  (#352)
-  Added N-DimVector (#360)
-  properties can be removed and string conversion much improved (#365)
-  fix finding MKL (#368)

Version 2021.1 (released XX.03.21)
==================================

-  fix build on F34 and openSUSE (#361)
-  add support for oneapi mkl (#363)

Version 2021 (released 13.03.21)
================================

-  fix CI on rawhide (#336)
-  add auto-cancel workflow to GitHub Actions (#343)
-  remove unused ext/alloc_traits.h from tests (#354)

Version 2021-rc.1 (released 15.01.21)
=====================================

-  add support of txt2tqs 3.4 (#208)
-  fix MKL:: target creation (#207, #265, #269, #270)
-  use local includes (#204)
-  add more support for units (#200)
-  add methods to update user input with defaults (#222)
-  fix usage of PROJECT\_SOURCE\_DIR in cmake (#230)
-  fix merging calculator defaults (#226, #227)
-  document cmake options and packages (#231)
-  make fftw3 a public dependency (#235)
-  rename WITH\_RC\_FILES to INSTALL\_RC\_FILES (#236)
-  check calculator input options (#232, #233)
-  allow calculator choices to be a list (#239)
-  inject defaults into calculator values (#241)
-  fixed bug introduced by defaults injection (#241, #242)
-  make votca\_compare always executable in builddir (#244)
-  add rel. compare to votca\_compare (#143)
-  standardize header formatting (#246)
-  move CI to GitHub Ations (#251, #254, #256, #258, #260,
   #274, #275, #296, #321)
-  improve mkl detection (#257, #299)
-  clean up installed cmake find modules (#263)
-  update codacy badge (#264)
-  add mkl builds to CI (#267, #271)
-  convert markdown files to rst files (#272)
-  enable CXX only in CMake (#273)
-  format code with clang-10 (#277)
-  fix clang-9 conversion warning (#276)
-  add class to writeand read matrices (#290)
-  Bump required CMake version to 3.12 (#301)
-  refactored getenv() into separate function (#302)
-  use unique_ptr in factory (#303)

Version 1.6.4 (released 12.01.21)
=================================

-  no changes

Version 1.6.3 (released 09.12.20)
=================================

-  switch to ghcr.io for CI (#297)
-  use master .clang-format in format action (#304, #307)
-  strip windows line endings in readers (#309)

Version 1.6.2 *SuperGitta* (released 22.08.20)
=================================

-  format code with clang-10 (#278)
-  move CI to GitHub Actions (#280, #285, #287)
-  fix build with mkl (#283)

Version 1.6.1 (released 21.06.20)
=================================

-  fix build with mkl (#229)
-  fix build with non-system libfftw (#234)
-  fix CI on Ubuntu-20.04 (#237)
-  fix bug related to calling c abs instead of c++ fabs (#248)
-  updated floating point comparison in akimaspline.h (#248)
-  fix compile error in structure parameters by adding hash function
   (#248)

Version 1.6 *SuperPelagia* (released 17.04.20)
==============================================

-  fix clang-10 warnings (#217)
-  clean up VOTCARC.\* (#220)
-  fix exported target with mkl (#223, #224)

Version 1.6\_rc2 (released 10.02.20)
====================================

-  fix inheritance bug in cmake files (#197)
-  fix bug in table class (#196)
-  fix build on 32-bit archs (#199)
-  remove old doxygen target (#211)

Version 1.6\_rc1 (released 04.12.19)
====================================

-  replaced fftw with Eigen3 fft
-  replaced random with std::random implementation
-  added many unit tests
-  formated code with clang-format
-  major cmake refactor

Version 1.5.1 (released 20.11.19)
=================================

-  switch votca\_compare to python3
-  more clear error message for xmlreader
-  fix build on CentOs7

Version 1.5 *SuperVictor* (released 31.01.19)
=============================================

-  enable gitlab CI

Version 1.5\_rc3 (released 19.01.19)
====================================

-  travis: fixed bug in building tags

Version 1.5\_rc2 (released 16.01.19)
====================================

-  histogramnew: refactor to fix memory lead

Version 1.5\_rc1 (released 28.12.18)
====================================

-  added unit testing
-  Eigen added as a dependency
-  removed GSL dependency

Version 1.2.2
=============

-  cmake: allow static fftw and gsl
-  added dummy c function for cmake
-  fixed conflicting type headers (real was defined)

Version 1.2.1
=============

-  fixed soname of libs
-  improved cmake checks and error messages
-  fixed pkg-config file

Version 1.2 (SuperDoris)
========================

-  changed buildsystem to cmake
-  added database class through sqlite3

Version 1.1.2
=============

-  fixed head of votca-tools manpage

Version 1.1.1
=============

-  just a bugfix release

Version 1.1 (SuperAnn)
======================

-  added thread class
-  added spline class, with akima spline, linear spline
-  added man7 page
-  random.cc: avoid calling of exit()
-  added lexical cast class

Version 1.0.1
=============

-  fixed a bug in VOTCARC.csh for empty LD\_LIBRARY\_PATH
-  completion file has moved back to csg
-  added --disable-rc-files to configure
-  updated bundled libtool to 2.2.10

Version 1.0
===========

-  added libvotca\_expat to allow compiling without expat
-  allow comments in tables
-  added application class to create standardized applications
-  all boost dependecy are now in tools
-  fixes in table format, flags is always last row now
-  allow compling without fftw (needed for csg\_boltzmann only)
-  allow compling without gsl (needed for csg\_resample and csg\_fmatch)

Version 1.0\_rc5
================

-  using libexpat instead of libxml2
-  added libvotca\_boost to allow compiling without boost
-  using pkg-config to detect package flags
-  compiles under AIX with xlC
-  added VOTCARC to initialize all votca parts
-  updated configure, see --help

Version 1.0\_rc4
================

-  using libtool to build shared libs

Version 1.0\_rc3
================

-  corrected bug in tokenizer
-  fixed a bug in calculation of version string
-  some fixes concerning autotools

Version 1.0\_rc2
================

-  fixed typo in calculation of version string
-  added NOTICE and LICENSE to the dist tarball

Version 1.0\_rc1
================

-  initial version
