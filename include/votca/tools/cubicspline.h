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

#ifndef VOTCA_TOOLS_CUBICSPLINE_H
#define VOTCA_TOOLS_CUBICSPLINE_H

#include "eigen.h"
#include "spline.h"
#include <iostream>

namespace votca {
namespace tools {

/**
    \brief A cubic spline class

    This class does cubic piecewise spline interpolation and spline fitting.
    As representation of a single spline, the general form
    \f[
        S_i(x) = A(x,h_i) f_i + B(x,h_i) f_{i+1} + C(x,h_i) f''_i + d(x,h_i)
   f''_{i+1} \f] with \f[
        x_i \le x < x_{i+1}\,,\\
        h_i = x_{i+1} - x_{i}
    \f]
    The \f$f_i\,,\,,f''_i\f$ are the function values and second derivates
    at point \f$x_i\f$.

    The parameters \f$f''_i\f$ are no free parameters, they are determined by
   the smoothing condition that the first derivatives are continuous. So the
   only free paramers are the grid points x_i as well as the functon values f_i
   at these points. A spline can be generated in several ways:
    - Interpolation spline
    - Fitting spline (fit to noisy data)
    - calculate the parameters elsewere and fill the spline class
*/

class CubicSpline : public Spline {
 public:
  // default constructor
  CubicSpline() = default;

  // destructor
  ~CubicSpline() override = default;

  // construct an interpolation spline
  // x, y are the the points to construct interpolation, both vectors must be of
  // same size
  void Interpolate(const Eigen::VectorXd &x, const Eigen::VectorXd &y) override;

  // fit spline through noisy data
  // x,y are arrays with noisy data, both vectors must be of same size
  void Fit(const Eigen::VectorXd &x, const Eigen::VectorXd &y) override;

  // Calculate the function value
  double Calculate(double x) override;

  // Calculate the function derivative
  double CalculateDerivative(double x) override;

  using Spline::Calculate;
  using Spline::CalculateDerivative;

  // set spline parameters to values that were externally computed
  void setSplineData(const Eigen::VectorXd &f, const Eigen::VectorXd &f2) {
    _f = f;
    _f2 = f2;
  }

  /**
   * \brief Add a point (one entry) to fitting matrix
   * \param pointer to matrix
   * \param value x
   * \param offsets relative to getInterval(x)
   * \param scale parameters for terms "A,B,C,D"
   * When creating a matrix to fit data with a spline, this function creates
   * one entry in that fitting matrix.
   */
  template <typename matrix_type>
  void AddToFitMatrix(matrix_type &A, double x, Index offset1,
                      Index offset2 = 0, double scale = 1);

  /**
   * \brief Add a point (one entry) to fitting matrix
   * \param pointer to matrix [in] [out]
   * \param value x [in]
   * \param offsets relative to getInterval(x) [in]
   * \param scale1 parameters for terms "A,B,C,D" [in]
   * \param scale2 parameters for terms "AA,BB,CC,DD" [in]
   * When creating a matrix to fit data with a spline, this function creates
   * one entry in that fitting matrix.
   */
  template <typename matrix_type>
  void AddToFitMatrix(matrix_type &A, double x, Index offset1, Index offset2,
                      double scale1, double scale2);

  /**
   * \brief Add a vector of points to fitting matrix
   * \param pointer to matrix
   * \param vector of x values
   * \param offsets relative to getInterval(x)
   * Same as previous function, but vector-valued and with scale=1.0
   */
  template <typename matrix_type, typename vector_type>
  void AddToFitMatrix(matrix_type &M, vector_type &x, Index offset1,
                      Index offset2 = 0);

  /**
   * \brief Add boundary condition of sum_i f_i =0 to fitting matrix
   * \param pointer to matrix
   * \param offsets
   */
  template <typename matrix_type>
  void AddBCSumZeroToFitMatrix(matrix_type &A, Index offset1,
                               Index offset2 = 0);

  /**
   * \brief Add boundary conditions to fitting matrix
   * \param pointer to matrix
   * \param offsets
   */
  template <typename matrix_type>
  void AddBCToFitMatrix(matrix_type &A, Index offset1, Index offset2 = 0);

 private:
  // y values of grid points
  Eigen::VectorXd _f;
  // second derivatives of grid points
  Eigen::VectorXd _f2;
  // A spline can be written in the form
  // S_i(x) =   A(x,x_i,x_i+1)*f_i     + B(x,x_i,x_i+1)*f''_i
  //          + C(x,x_i,x_i+1)*f_{i+1} + D(x,x_i,x_i+1)*f''_{i+1}
  double A(double r);
  double B(double r);
  double C(double r);
  double D(double r);

  double Aprime(double r);
  double Bprime(double r);
  double Cprime(double r);
  double Dprime(double r);

  // tabulated derivatives at grid points. Second argument: 0 - left, 1 - right
  double A_prime_l(Index i);
  double A_prime_r(Index i);
  double B_prime_l(Index i);
  double B_prime_r(Index i);
  double C_prime_l(Index i);
  double C_prime_r(Index i);
  double D_prime_l(Index i);
  double D_prime_r(Index i);
};

template <typename matrix_type>
inline void CubicSpline::AddToFitMatrix(matrix_type &M, double x, Index offset1,
                                        Index offset2, double scale) {
  Index spi = getInterval(x);
  M(offset1, offset2 + spi) += A(x) * scale;
  M(offset1, offset2 + spi + 1) += B(x) * scale;
  M(offset1, offset2 + spi + _r.size()) += C(x) * scale;
  M(offset1, offset2 + spi + _r.size() + 1) += D(x) * scale;
}

// for adding f'(x)*scale1 + f(x)*scale2 as needed for threebody interactions
template <typename matrix_type>
inline void CubicSpline::AddToFitMatrix(matrix_type &M, double x, Index offset1,
                                        Index offset2, double scale1,
                                        double scale2) {
  Index spi = getInterval(x);
  M(offset1, offset2 + spi) += Aprime(x) * scale1;
  M(offset1, offset2 + spi + 1) += Bprime(x) * scale1;
  M(offset1, offset2 + spi + _r.size()) += Cprime(x) * scale1;
  M(offset1, offset2 + spi + _r.size() + 1) += Dprime(x) * scale1;

  AddToFitMatrix(M, x, offset1, offset2, scale2);
}

template <typename matrix_type, typename vector_type>
inline void CubicSpline::AddToFitMatrix(matrix_type &M, vector_type &x,
                                        Index offset1, Index offset2) {
  for (Index i = 0; i < x.size(); ++i) {
    Index spi = getInterval(x(i));
    M(offset1 + i, offset2 + spi) = A(x(i));
    M(offset1 + i, offset2 + spi + 1) = B(x(i));
    M(offset1 + i, offset2 + spi + _r.size()) = C(x(i));
    M(offset1 + i, offset2 + spi + _r.size() + 1) = D(x(i));
  }
}

template <typename matrix_type>
inline void CubicSpline::AddBCSumZeroToFitMatrix(matrix_type &M, Index offset1,
                                                 Index offset2) {
  for (Index i = 0; i < _r.size(); ++i) {
    M(offset1, offset2 + i) = 1;
    M(offset1, offset2 + _r.size() + i) = 0;
  }
}

template <typename matrix_type>
inline void CubicSpline::AddBCToFitMatrix(matrix_type &M, Index offset1,
                                          Index offset2) {
  for (Index i = 0; i < _r.size() - 2; ++i) {
    M(offset1 + i + 1, offset2 + i) = A_prime_l(i);
    M(offset1 + i + 1, offset2 + i + 1) = B_prime_l(i) - A_prime_r(i);
    M(offset1 + i + 1, offset2 + i + 2) = -B_prime_r(i);

    M(offset1 + i + 1, offset2 + _r.size() + i) = C_prime_l(i);
    M(offset1 + i + 1, offset2 + _r.size() + i + 1) =
        D_prime_l(i) - C_prime_r(i);
    M(offset1 + i + 1, offset2 + _r.size() + i + 2) = -D_prime_r(i);
  }
  switch (_boundaries) {
    case splineNormal:
      M(offset1, offset2 + _r.size()) = 1;
      M(offset1 + _r.size() - 1, offset2 + 2 * _r.size() - 1) = 1;
      break;
    case splineDerivativeZero:
      // y
      M(offset1 + 0, offset2 + 0) = -1 * A_prime_l(0);
      M(offset1 + 0, offset2 + 1) = -1 * B_prime_l(0);

      M(offset1 + _r.size() - 1, offset2 + _r.size() - 2) =
          A_prime_l(_r.size() - 2);
      M(offset1 + _r.size() - 1, offset2 + _r.size() - 1) =
          B_prime_l(_r.size() - 2);

      // y''
      M(offset1 + 0, offset2 + _r.size() + 0) = D_prime_l(0);
      M(offset1 + 0, offset2 + _r.size() + 1) = C_prime_l(0);

      M(offset1 + _r.size() - 1, offset2 + 2 * _r.size() - 2) =
          C_prime_l(_r.size() - 2);
      M(offset1 + _r.size() - 1, offset2 + 2 * _r.size() - 1) =
          D_prime_l(_r.size() - 2);
      break;
    case splinePeriodic:
      M(offset1, offset2) = 1;
      M(offset1, offset2 + _r.size() - 1) = -1;
      M(offset1 + _r.size() - 1, offset2 + _r.size()) = 1;
      M(offset1 + _r.size() - 1, offset2 + 2 * _r.size() - 1) = -1;
      break;
  }
}

}  // namespace tools
}  // namespace votca

#endif /* VOTCA_TOOLS_CUBICSPLINE_H */
