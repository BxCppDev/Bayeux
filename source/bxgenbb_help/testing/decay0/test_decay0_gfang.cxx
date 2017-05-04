#include <iostream>
#include <cmath>
#include <genbb_help/decay0/gfang.h>
#include <geomtools/utils.h>

int main(void)
{
  double p[3];
  double ct, st, cp, sp;
  bool rotate;

  p[0] =  0.848847;
  p[1] = -0.104306;
  p[2] = -1.10841;
  genbb::decay0::decay0_gfang(p, ct, st, cp, sp, rotate);
  std::clog << "Cos(theta) = " << ct << '\n';
  std::clog << "Sin(theta) = " << st << '\n';
  std::clog << "Cos(phi)   = " << cp << '\n';
  std::clog << "Sin(phi)   = " << sp << '\n';
  std::clog << "rotate     = " << rotate << '\n';

  geomtools::vector_3d pp(p[0],p[1],p[2]);
  std::clog << "Cos(theta) = " << std::cos(pp.theta()) 
            << " (" << pp.cosTheta() << ')' << '\n';
  std::clog << "Sin(theta) = " << std::sin(pp.theta()) << '\n';
  std::clog << "Cos(phi)   = " << std::cos(pp.phi()) << '\n';
  std::clog << "Sin(phi)   = " << std::sin(pp.phi()) << '\n';
  std::clog << '\n';
  

  p[0] =  0;
  p[1] =  0;
  p[2] =  3.3;
  genbb::decay0::decay0_gfang(p, ct, st, cp, sp, rotate);
  std::clog << "Cos(theta) = " << ct << '\n';
  std::clog << "Sin(theta) = " << st << '\n';
  std::clog << "Cos(phi)   = " << cp << '\n';
  std::clog << "Sin(phi)   = " << sp << '\n';
  std::clog << "rotate     = " << rotate << '\n';

  return 0;
}
