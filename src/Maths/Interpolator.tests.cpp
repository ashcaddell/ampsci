#include "Interpolator.hpp"
#include "catch2/catch.hpp"
#include <cmath>
#include <iostream>
#include <vector>

TEST_CASE("Maths::Interpolator", "[interpolator][unit]") {
  std::cout << "\n----------------------------------------\n";
  std::cout << "Interpolator\n";

  const auto func = [](double x) {
    return std::exp(-x * x / 2) * std::sin(3.0 * x) / std::sqrt(2.0 * M_PI);
  };

  const std::vector x{-5.0, -4.5, -4.0, -3.5, -3.0, -2.5, -2.0,
                      -1.5, -1.0, -0.5, 0.0,  0.5,  1.0,  1.5,
                      2.0,  2.5,  3.0,  3.5,  4.0,  4.5,  5.0};
  const std::vector y{-9.66796e-7, -0.0000128475, 0.0000718097,  0.000767695,
                      -0.00182645, -0.0164415,    0.0150859,     0.126607,
                      -0.0341469,  -0.351183,     0.0,           0.351183,
                      0.0341469,   -0.126607,     -0.0150859,    0.0164415,
                      0.00182645,  -0.000767695,  -0.0000718097, 0.0000128475,
                      9.66796e-7};

  using namespace Interpolator;

  std::vector<double> xo;
  for (double t = -5.0; t <= 5.0; t += 0.1) {
    xo.push_back(t);
  }

  auto y_linear = Interpolator::interpolate(x, y, xo, Method::linear);
  auto y_poly = Interpolator::interpolate(x, y, xo, Method::polynomial);
  auto y_cspl = Interpolator::interpolate(x, y, xo, Method::cspline);

  // For linear, polynomial, and cubic spline, test against mathematica
  const std::vector data_linear{
      -9.66796e-7,   -3.34293e-6,   -5.71907e-6,   -8.09521e-6,   -0.0000104713,
      -0.0000128475, 4.08395e-6,    0.0000210154,  0.0000379468,  0.0000548782,
      0.0000718097,  0.000210987,   0.000350164,   0.000489341,   0.000628518,
      0.000767695,   0.000248867,   -0.000269962,  -0.00078879,   -0.00130762,
      -0.00182645,   -0.00474947,   -0.00767249,   -0.0105955,    -0.0135185,
      -0.0164415,    -0.0101361,    -0.00383056,   0.00247493,    0.00878042,
      0.0150859,     0.0373902,     0.0596945,     0.0819988,     0.104303,
      0.126607,      0.0944565,     0.0623056,     0.0301548,     -0.00199606,
      -0.0341469,    -0.0975542,    -0.160962,     -0.224369,     -0.287776,
      -0.351183,     -0.280947,     -0.21071,      -0.140473,     -0.0702367,
      0.0,           0.0702367,     0.140473,      0.21071,       0.280947,
      0.351183,      0.287776,      0.224369,      0.160962,      0.0975542,
      0.0341469,     0.00199606,    -0.0301548,    -0.0623056,    -0.0944565,
      -0.126607,     -0.104303,     -0.0819988,    -0.0596945,    -0.0373902,
      -0.0150859,    -0.00878042,   -0.00247493,   0.00383056,    0.0101361,
      0.0164415,     0.0135185,     0.0105955,     0.00767249,    0.00474947,
      0.00182645,    0.00130762,    0.00078879,    0.000269962,   -0.000248867,
      -0.000767695,  -0.000628518,  -0.000489341,  -0.000350164,  -0.000210987,
      -0.0000718097, -0.0000548782, -0.0000379468, -0.0000210154, -4.08395e-6,
      0.0000128475,  0.0000104713,  8.09521e-6,    5.71907e-6,    3.34293e-6,
      9.66796e-7};

  const std::vector data_poly{
      -9.66796e-7,   -44.1792,    -39.3121,    -22.5886,    -8.32491,
      -0.0000128475, 3.24874,     3.41759,     2.27451,     0.955911,
      0.0000718097,  -0.468474,   -0.54507,    -0.398321,   -0.182436,
      0.000767695,   0.106381,    0.133137,    0.104361,    0.0506856,
      -0.00182645,   -0.0374426,  -0.0514036,  -0.0472741,  -0.0328899,
      -0.0164415,    -0.00367623, 0.00337646,  0.00622514,  0.00867748,
      0.0150859,     0.0286242,   0.0500301,   0.0770759,   0.10483,
      0.126607,      0.135361,    0.125219,    0.0928555,   0.038416,
      -0.0341469,    -0.11741,    -0.20142,    -0.275094,   -0.327844,
      -0.351183,     -0.340051,   -0.293665,   -0.215768,   -0.114218,
      0.0,           0.114218,    0.215768,    0.293665,    0.340051,
      0.351183,      0.327844,    0.275094,    0.20142,     0.11741,
      0.0341469,     -0.038416,   -0.0928555,  -0.125219,   -0.135361,
      -0.126607,     -0.10483,    -0.0770759,  -0.0500301,  -0.0286242,
      -0.0150859,    -0.00867748, -0.00622514, -0.00337646, 0.00367623,
      0.0164415,     0.0328899,   0.0472741,   0.0514036,   0.0374426,
      0.00182645,    -0.0506856,  -0.104361,   -0.133137,   -0.106381,
      -0.000767695,  0.182436,    0.398321,    0.54507,     0.468474,
      -0.0000718097, -0.955911,   -2.27451,    -3.41759,    -3.24874,
      0.0000128475,  8.32491,     22.5886,     39.3121,     44.1792,
      9.66796e-7};

  const std::vector data_cspl{
      -9.66796e-7,   0.0000147955,  0.0000171784,  0.000010492,   -9.53382e-7,
      -0.0000128475, -0.0000208801, -0.0000207409, -8.11971e-6,   0.0000212937,
      0.0000718097,  0.000147546,   0.000251848,   0.000387871,   0.000558769,
      0.000767695,   0.000983247,   0.00103579,    0.000721153,   -0.000164866,
      -0.00182645,   -0.00436799,   -0.00749478,   -0.0108123,    -0.0139261,
      -0.0164415,    -0.0177937,    -0.016735,     -0.0118475,    -0.0017132,
      0.0150859,     0.0389912,     0.0665377,     0.0932837,     0.114787,
      0.126607,      0.124949,      0.108604,      0.0770134,     0.0296165,
      -0.0341469,    -0.112702,     -0.195934,     -0.271591,     -0.327425,
      -0.351183,     -0.334137,     -0.28163,      -0.202528,     -0.105697,
      0.0,           0.105697,      0.202528,      0.28163,       0.334137,
      0.351183,      0.327425,      0.271591,      0.195934,      0.112702,
      0.0341469,     -0.0296165,    -0.0770134,    -0.108604,     -0.124949,
      -0.126607,     -0.114787,     -0.0932837,    -0.0665377,    -0.0389912,
      -0.0150859,    0.0017132,     0.0118475,     0.016735,      0.0177937,
      0.0164415,     0.0139261,     0.0108123,     0.00749478,    0.00436799,
      0.00182645,    0.000164866,   -0.000721153,  -0.00103579,   -0.000983247,
      -0.000767695,  -0.000558769,  -0.000387871,  -0.000251848,  -0.000147546,
      -0.0000718097, -0.0000212937, 8.11971e-6,    0.0000207409,  0.0000208801,
      0.0000128475,  9.53382e-7,    -0.000010492,  -0.0000171784, -0.0000147955,
      9.66796e-7};

  REQUIRE_THAT(y_linear, Catch::Approx(data_linear).margin(1.0e-4));
  REQUIRE_THAT(y_poly, Catch::Approx(data_poly).margin(1.0e-4));
  REQUIRE_THAT(y_cspl, Catch::Approx(data_cspl).margin(1.0e-4));

  // For the rest, require that they approximate the function func.
  // Note: not looking for high accuracy - approximation itself not what we are testing.

  Interp icp(x, y, Method::cspline_periodic);
  const Interp ia(x, y, Method::akima);
  Interp iap(x, y, Method::akima_periodic);

  for (double t = -5.0; t <= 5.0; t += 0.1) {
    REQUIRE(icp(t) == Approx(func(t)).margin(1.0e-1));
    REQUIRE(ia(t) == Approx(func(t)).margin(1.0e-1));
    REQUIRE(iap(t) == Approx(func(t)).margin(1.0e-1));
  }

  if constexpr (Interpolator::has_steffen_method()) {
    const Interp is(x, y, Method::steffen);
    for (double t = -5.0; t <= 5.0; t += 0.1) {
      REQUIRE(is(t) == Approx(func(t)).margin(1.0e-1));
    }
  }
}