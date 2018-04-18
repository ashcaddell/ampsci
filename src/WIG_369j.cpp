#include "WIG_369j.h"
/*
Wrapper functions to calculate wigner 3,6,9-J symbols.
Uses GSL:
https://www.gnu.org/software/gsl/doc/html/specfunc.html?highlight=3j#coupling-coefficients
NOTE:
Since j always integer or half-integer, inputs must be integer.
Two versions of each:
 * One '_1' version takes integers as-is. Only work for
integer angular momentum (l).
 * Other '_2' version, takes in 2*j (as an integer). Works for l and j
*/

//******************************************************************************
double WIG_3j_1(int j1, int j2, int j3, int m1, int m2, int m3)
/*
Calculates wigner 3j symbol:
  (j1 j2 j3)
  (m1 m2 m3)
Note: this function takes INTEGER values, only works for l (not half-integer j)!
*/
{
  return gsl_sf_coupling_3j(2*j1,2*j2,2*j3,2*m1,2*m2,2*m3);
}

//------------------------------------------------------------------------------
double WIG_3j_2(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2,
  int two_m3)
/*
Calculates wigner 3j symbol:
  (j1 j2 j3)
  (m1 m2 m3)
Note: this function takes INTEGER values, that have already multiplied by 2!
Works for l and j (integer and half-integer)
*/
{
  return gsl_sf_coupling_3j(two_j1,two_j2,two_j3,two_m1,two_m2,two_m3);
}

//******************************************************************************
double WIG_cg_1(int j1, int m1, int j2, int m2, int J, int M)
/*
Calculates Clebsh-Gordon coeficient:
<j1 m1, j2 m2 | J M> = (-1)^(j1-j2+M) * Sqrt(2J+1) * (j1 j2  J)
.                                                    (m1 m2 -M)
(Last term is 3j symbol)
Note: this function takes INTEGER values, only works for l (not half-integer j)!
*/
{
  int sign = -1;
  if((j1-j2+M)%2==0) sign = 1;
  return sign*sqrt(2.*J+1.)*gsl_sf_coupling_3j(2*j1,2*j2,2*J,2*m1,2*m2,-2*M);
}

//------------------------------------------------------------------------------
double WIG_cg_2(int two_j1, int two_m1, int two_j2, int two_m2, int two_J,
  int two_M)
/*
<j1 m1, j2 m2 | J M> = (-1)^(j1-j2+M) * Sqrt(2J+1) * (j1 j2  J)
.                                                    (m1 m2 -M)
(Last term is 3j symbol)
Note: this function takes INTEGER values, that have already multiplied by 2!
Works for l and j (integer and half-integer)
*/
{
  int sign = -1;
  if((two_j1-two_j2+two_M)%4==0) sign = 1; //mod 4 (instead 2), since x2
  return sign*sqrt(two_J+1.)*gsl_sf_coupling_3j(two_j1,two_j2,two_J,two_m1,
    two_m2,-two_M);
}

//******************************************************************************
double WIG_6j_1(int j1, int j2, int j3, int j4, int j5, int j6)
/*
Calculates wigner 6j symbol:
  {j1 j2 j3}
  {j4 j5 j6}
Note: this function takes INTEGER values, only works for l (not half-integer j)!
*/
{
  return gsl_sf_coupling_6j(2*j1,2*j2,2*j3,2*j4,2*j5,2*j6);
}

//------------------------------------------------------------------------------
double WIG_6j_2(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5,
  int two_j6)
/*
Calculates wigner 6j symbol:
  {j1 j2 j3}
  {j4 j5 j6}
Note: this function takes INTEGER values, that have already multiplied by 2!
Works for l and j (integer and half-integer)*/
{
  return gsl_sf_coupling_6j(two_j1,two_j2,two_j3,two_j4,two_j5,two_j6);
}

//******************************************************************************
double WIG_9j_1(int j1, int j2, int j3, int j4, int j5, int j6, int j7, int j8,
  int j9)
/*
Calculates wigner 9j symbol:
  {j1 j2 j3}
  {j4 j5 j6}
  {j7 j8 j9}
Note: this function takes INTEGER values, only works for l (not half-integer j)!
*/
{
  return gsl_sf_coupling_9j(2*j1,2*j2,2*j3,2*j4,2*j5,2*j6,2*j7,2*j8,2*j9);
}

//------------------------------------------------------------------------------
double WIG_9j_2(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5,
  int two_j6, int two_j7, int two_j8, int two_j9)
/*
Calculates wigner 9j symbol:
  {j1 j2 j3}
  {j4 j5 j6}
  {j7 j8 j9}
Note: this function takes INTEGER values, that have already multiplied by 2!
Works for l and j (integer and half-integer)*/
{
  return gsl_sf_coupling_9j(two_j1,two_j2,two_j3,two_j4,two_j5,two_j6,two_j7,
    two_j8,two_j9);
}