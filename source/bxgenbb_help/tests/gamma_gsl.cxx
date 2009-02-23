#include <iostream>
#include <cmath>
#include <gsl/gsl_sf.h>
#include <CLHEP/Units/SystemOfUnits.h>

using namespace std;


double fermi_func (double z_, double e_)
{
  double e = e_ / CLHEP::MeV;
  double emass = 0.510999084; 
  if (e_ < 50.e-6) e = 50.e-6;
  double alfaz = z_/137.036;
  double w = e / emass + 1.; 
  double p = sqrt (w * w - 1.);
  double y = alfaz * w / p;
  double g = sqrt (1. - alfaz * alfaz); 
  clog << "DEVEL: g/y==" << g << ' ' << y << endl;

  gsl_sf_result res, arg;
  int err = gsl_sf_lngamma_complex_e (g, y, &res, &arg);
  double lnzr     = res.val;
  double lnzr_err = res.err;
  double zarg     = arg.val;
  double zarg_err = arg.err;
  return exp ((2. * g - 2.) * log(p)) 
    * exp (M_PI * y + 2. * lnzr);
}

int main (void)
{
  double zx = 4.0;
  double zy = 0.0;
  gsl_sf_result res, arg;
  
  int err = gsl_sf_lngamma_complex_e (zx, zy, &res, &arg);

  double lnzr     = res.val;
  double lnzr_err = res.err;
  double zarg     = arg.val;
  double zarg_err = arg.err;
  double zr = exp (lnzr);
  clog << zr << ' ' << zarg << endl;
  double x = zr * cos (zarg);
  double y = zr * sin (zarg);
  clog << x << ' ' << y << endl;

  double e = 0.496139944 * CLHEP::MeV;
  double z = 22.;
  clog << "e=" << e / CLHEP::MeV << ' ' 
       << "z=" << z << ' ' 
       << fermi_func (z, e) << endl;

  return 0;
  double z1 = 1.0;
  double z2 = 40.0;
  for (double e = 0.0; 
       e < 10. * CLHEP::MeV; 
       e += 0.1 * CLHEP::MeV)
    {
      double f1 = fermi_func (z1, e);
      double f2 = fermi_func (z2, e);
      cout << e / CLHEP::MeV << ' ' << f1 << ' ' << f2 << endl;
    }


  return 0;

}
