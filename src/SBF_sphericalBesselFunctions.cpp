#include "SBF_sphericalBesselFunctions.h"
#include <gsl/gsl_sf_bessel.h>
#include <cmath>

namespace SBF{

//******************************************************************************
double JL(int L, double x)
{

  //Very low x expansion
  if(fabs(x)<1.e-8){
    if(L==0) return 1.;
    else return 0;
  }

  //Low x expansion for first few Ls. Accurate to ~ 10^9
  if(fabs(x)<0.1){
    if(L==0) return 1.-0.166666667*(x*x)+0.00833333333*pow(x,4);
    if(L==1) return 0.333333333*x-0.0333333333*(x*x*x)+0.00119047619*pow(x,5);
    if(L==2) return 0.0666666667*(x*x)-0.00476190476*pow(x,4)
      +0.000132275132*pow(x,6);
    if(L==3) return 0.00952380952*(x*x*x)-0.000529100529*pow(x,5)
      +0.000012025012*pow(x,7);
    if(L==4) return 0.00105820106*pow(x,4)-0.0000481000481*pow(x,6)
      +9.25000925e-7*pow(x,8);
  }

  //Explicit formalas for first few L's
  if(L==0) return sin(x)/x;
  if(L==1) return sin(x)/(x*x)-cos(x)/x;
  if(L==2) return (3./(x*x)-1.)*sin(x)/x - 3.*cos(x)/(x*x);
  if(L==3) return (15./(x*x*x)-6./x)*sin(x)/x - (15./(x*x)-1.)*cos(x)/x;
  if(L==4) return 5.*(-21.+2.*x*x)*cos(x)/(x*x*x*x)
    +(105.-45.*x*x+x*x*x*x)*sin(x)/(x*x*x*x*x);

  //If none of above apply, use GSL to calc. accurately
  return gsl_sf_bessel_jl(L, x);
}

//******************************************************************************
double exactGSL_JL(int L, double x)
/*
"Exact" form from GSL libraries.
Mainly for testing.
*/
{
  return gsl_sf_bessel_jl(L, x);
}


}//End SBF