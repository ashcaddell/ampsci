#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

// XXX Add option to "extend" grid (good for Continuum)
// ===> just give drdu (dr = drdu*du, r_i = r_i-1 + dr)
// Need be careful w/ rmax. Technically dont need this! XXX

enum class GridType { loglinear, logarithmic, linear };

//******************************************************************************
class Grid {

public:
  const double r0;       // Minimum grid value
  const double rmax;     // Maximum grid value
  const std::size_t ngp; // Number of Grid Points
  const double du;       // Uniform grid step size

  std::vector<double> r;      // Grid values r[i]
  std::vector<double> drdu;   // Jacobian (dr/du)[i]
  std::vector<double> drduor; // Convinient: (1/r)*(dr/du)[i]

public:
  Grid(double in_r0, double in_rmax, std::size_t in_ngp, GridType in_gridtype,
       double in_b = 0);

  int getIndex(double x, bool require_nearest = false) const;

  void print() const;

  // Static functions: can be called outside of instantialised object
  static double calc_du_from_ngp(double in_r0, double in_rmax,
                                 std::size_t in_ngp, GridType in_gridtype,
                                 double in_b = 0);
  static std::size_t calc_ngp_from_du(double in_r0, double in_rmax,
                                      double in_du, GridType in_gridtype,
                                      double in_b = 0);

private:
  void form_loglinear_grid();
  void form_logarithmic_grid();
  void form_linear_grid();

private:
  const GridType gridtype;
  const double b;
};

//******************************************************************************
inline double Grid::calc_du_from_ngp(double r0, double rmax, std::size_t ngp,
                                     GridType gridtype, double b) {
  if (ngp == 1)
    return 0;
  switch (gridtype) {
  case GridType::loglinear:
    if (b == 0)
      std::cerr << "\nFAIL57 in Grid: cant have b=0 for log-linear grid!\n";
    return (rmax - r0 + b * log(rmax / r0)) / (double(ngp - 1));
  case GridType::logarithmic:
    return log(rmax / r0) / double(ngp - 1);
  case GridType::linear:
    return (rmax - r0) / double(ngp - 1);
  }
  std::cerr << "\nFAIL 63 in Grid: wrong type?\n";
  return 1.;
}

//******************************************************************************
inline std::size_t Grid::calc_ngp_from_du(double r0, double rmax, double du,
                                          GridType gridtype, double b) {
  switch (gridtype) {
  case GridType::loglinear:
    if (b == 0)
      std::cerr << "\nFAIL57 in Grid: cant have b=0 for log-linear grid!\n";
    return std::size_t((rmax - r0 + b * log(rmax / r0)) / du) + 2;
  case GridType::logarithmic:
    return std::size_t(log(rmax / r0) / du) + 2;
  case GridType::linear:
    return std::size_t((rmax - r0) / du) + 2;
  }
  std::cerr << "\nFAIL 84 in Grid: wrong type?\n";
  return 1;
}

//******************************************************************************
inline Grid::Grid(double in_r0, double in_rmax, std::size_t in_ngp,
                  GridType in_gridtype, double in_b)
    : r0(in_r0), rmax(in_rmax), ngp(in_ngp),
      du(calc_du_from_ngp(in_r0, in_rmax, in_ngp, in_gridtype, in_b)),
      gridtype(in_gridtype), b(in_b)
//
{

  r.reserve(ngp);
  drdu.reserve(ngp);   // Jacobian:
  drduor.reserve(ngp); //(1/r)*du/dr (just for convinience)

  switch (gridtype) {
  case GridType::loglinear:
    form_loglinear_grid();
    break;
  case GridType::logarithmic:
    form_logarithmic_grid();
    break;
  case GridType::linear:
    form_linear_grid();
    break;
  default:
    std::cerr << "\n FAIL 49 in Grid: no grid type?\n";
  }
}

//******************************************************************************
inline int Grid::getIndex(double x, bool require_nearest) const
/*
Returns index correspoding to given value
Note: finds NEXT LARGEST grid point (greater then or equal to.),
unluess require_nearest=true, when will give closest point.
For linear or exponential, faster to use formula.
But for log-linear, can't.
I don't think this works for "backwards" grids, maybe not negative grids either
*/
{

  auto low = std::lower_bound(r.begin(), r.end(), x);
  auto index = (int)(low - r.begin());

  if (!require_nearest || index == 0)
    return index;

  // Must resturn /nearest/ index (we have (in order): r[i-1], x, r[i])
  if (fabs(x - r[index - 1]) < fabs(r[index] - x))
    return index - 1;
  else
    return index;
}

//******************************************************************************
inline void Grid::print() const {

  switch (gridtype) {
  case GridType::linear:
    std::cout << "Linear ";
    break;
  case GridType::logarithmic:
    std::cout << "Logarithmic ";
    break;
  case GridType::loglinear:
    std::cout << "Log-linear (b=" << b << ") ";
  }
  std::cout << "grid: " << r0 << "->" << rmax << ", N=" << ngp << ", du=" << du
            << "\n";
}

//******************************************************************************
inline void Grid::form_loglinear_grid()
/*
Roughly, grid is logarithmically spaced below r=b, and linear above.
Definition:
  dr/du = r/(b+r)
  => u_0 = r0 + b*ln(r0)
  du = (in_rmax-in_r0+b*log(in_rmax/in_r0))/(in_ngp-1)
  du is constant (step-size for uniformly spaced grid)
Typically (and by default), b = 4 (unit units/bohr radius)
*/
{
  if (b == 0)
    std::cerr << "FAIL 164 in Grid: Cant have b=0 for LogLinear Grid!\n";

  // initial points:
  r.push_back(r0);
  drduor.push_back(1. / (b + r0));
  drdu.push_back(drduor[0] * r0);

  // Use iterative method from Dzuba code to calculate r grid
  double u = r0 + b * log(r0);
  for (std::size_t i = 1; i < ngp; i++) {
    u += du;
    double r_tmp = r[i - 1];
    // Integrate dr/dt to find r:
    double delta_r = 1.;
    int ii = 0; // to count number of iterations
    while (delta_r > (r0 * 1.e-15)) {
      double delta_u = u - (r_tmp + b * log(r_tmp));
      double drdu_tmp = r_tmp / (r_tmp + b);
      delta_r = delta_u * drdu_tmp;
      r_tmp += delta_r;
      ii++;
      if (ii > 30)
        break; // usually converges in ~ 2 or 3 steps!
    }
    r.push_back(r_tmp);
    drduor.push_back(1. / (b + r_tmp));
    drdu.push_back(drduor[i] * r_tmp);
  }
}

//******************************************************************************
inline void Grid::form_logarithmic_grid()
/*
Standard exponential (logarithmic) grid.
Uses:
  dr/du = r0 * exp(u)
  =>  r = r0 * exp(u)
      u = i*du for i=0,1,2,...
*/
{
  for (std::size_t i = 0; i < ngp; i++)
    drdu.push_back(r0 * exp(double(i) * du));
  r = drdu;
  drduor.resize(ngp, 1.);
}

//******************************************************************************
inline void Grid::form_linear_grid() {
  for (std::size_t i = 0; i < ngp; i++) {
    double tmp_r = r0 + double(i) * du;
    r.push_back(tmp_r);
    drduor.push_back(1. / tmp_r);
  }
  drdu.resize(ngp, 1.);
}
