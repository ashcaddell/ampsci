#include "StandardHaloModel.h"
#include "Grid.h"
#include <cmath>
#include <vector>

// make this work w/ Grid?
// Give it a grid [v], it returns an array [f(v)]

using namespace SHMCONSTS;

StandardHaloModel::StandardHaloModel(double in_cosphi, double dves, double dv0)
    : cosphi(in_cosphi), v0(V0 + dv0 * DEL_V0), v_sun(VSUN + dv0 * DEL_V0),
      vesc(VESC + dves * DEL_VESC), veorb(VEORB) {

  // NormCost should be 1, but *= more general
  // (means) it will work for _any_ existing m_normConst
  m_normConst *= normfv();
}

//******************************************************************************
double StandardHaloModel::fv(double v) const
// Standard halo model for velocity distribution, in laboratory frame.
//  f ~ v^2 exp(-v^2)
// Note: distribution for DM particles that cross paths with Earth.
//
// Note: normalised for all extras = 0.
// Otherwise, NOT normalised!
//
//  sinphi should be [-1,1]
//  dv0 should be [0,1]..
//  dves = [-1,1]
{
  // local velocity (lab)
  double vl = v_sun + veorb * COSBETA * cosphi;

  // Norm const * v^2:
  double A = m_normConst * pow(v, 2);

  double arg1 = -pow((v - vl) / v0, 2);

  if (v <= 0) {
    return 0;
  } else if (v < vesc - vl) { // here
    double arg2 = -pow((v + vl) / v0, 2);
    return A * (exp(arg1) - exp(arg2));
  } else if (v < vesc + vl) { // here
    double arg2 = -pow(vesc / v0, 2);
    return A * (exp(arg1) - exp(arg2));
  } else {
    return 0;
  }
}

//******************************************************************************
double StandardHaloModel::normfv() const {
  int num_vsteps = 2000;
  double dv = MAXV / num_vsteps;

  double v = dv;
  double A = 0;
  for (int i = 0; i < num_vsteps; i++) {
    A += fv(v);
    v += dv;
  }
  return 1. / (A * dv);
}

//******************************************************************************
std::vector<double> StandardHaloModel::makeFvArray(const Grid &vgrid) const {
  // Fills an (external) vector array with f(v) values.
  // Note: units will be km/s. AND VGRID must be in same units
  // Possible to re-scale; must rescale v and fv!
  std::vector<double> fv_array;
  fv_array.reserve(vgrid.ngp);
  for (auto v : vgrid.r) {
    fv_array.push_back(fv(v));
  }
  return fv_array;
}
