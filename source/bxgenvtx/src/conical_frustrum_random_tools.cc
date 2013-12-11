// -*- mode: c++ ; -*-
/* conical_frustrum_random_tools.cc
 *
 * maxima:
 * dVde(e):=%PI*h*(A1+(A2-A1)*e)*(B1+(B2-B1)*e);
 * V(e):=''(integrate(dVde(e),e));
 * V(0);
 * V1=V(1);
 * Vn(e)=V(e)/V(1);
 * eq1: p = Vn(e);
 * solve(eq1,e);
 */

// Ourselves
#include <genvtx/conical_frustrum_random_tools.h>

// Third party
// - GSL
#include <gsl/gsl_math.h>
#include <gsl/gsl_poly.h>
// - datatools
#include <datatools/exception.h>
#include <datatools/utils.h>
// - geomtools
#include <geomtools/utils.h>

// This project:
#include <genvtx/ring_random_tools.h>

namespace genvtx {

  // static
  /*             b1
   *                |` -
   *                |    ` -
   *                | b(z) ^ ` -
   *                |      :     `| b2
   *                |      :      |
   *                |      :      |
   *                |      :     _| a2
   *                |     ___ --  :
   *                |_ --  ^ a(z) :
   *             a1 :      :      :
   *                :      :      :
   *  --------------+-------------+-----------------------> Z
   *                0      z      h
   *
   *  a(z) = a1 + (a2 - a1) * z / h
   *  b(z) = b1 + (b2 - b1) * z / h
   *
   *  with: z in [0; h]
   *
   *  See:
   *    http://www.gnu.org/software/gsl/manual/html_node/Cubic-Equations.html#Cubic-Equations
   */
  geomtools::vector_3d
  right_circular_conical_frustrum::random_volume (mygsl::rng & random_,
                                                  double a1_,
                                                  double b1_,
                                                  double a2_,
                                                  double b2_,
                                                  double length_,
                                                  double /*thickness_*/,
                                                  double theta1_,
                                                  double theta2_)
  {
    bool devel = false;
    if (devel) std::cerr << "*** DEVEL *** random_volume: Entering..." << std::endl;
    // if (thickness_ > 0.0) {
    //   double dt = 0.5 * thickness_;
    //   r += random_.flat(-dt, dt);
    // }
    double a1 = a1_;
    double b1 = b1_;
    double a2 = a2_;
    double b2 = b2_;
    double h  = length_;
    DT_THROW_IF(a1 > b1, std::range_error, "Invalid radii A1/B1 !");
    DT_THROW_IF(a2 > b2, std::range_error, "Invalid radii A2/B2 !");
    DT_THROW_IF(h <= 0.0, std::range_error, "Invalid length !");
    double A1 = b1 + a1;
    double B1 = b1 - a1;
    double A2 = b2 + a2;
    double B2 = b2 - a2;
    double u  = 2 * ( A2 - A1 ) * ( B2 - B1 );
    double v  = 3 * ( A1 * B2 + A2 * B1 - 2 * A1 * B1 );
    double w  = 6 * ( A1 * B1 );
    /* P.D.F. :
     *  V(e) = - (M_PI * h / 6) * ((e*e*e) * u + (e*e) * v + (e) * w)
     *  V(0) = 0
     *  V(1) = - (M_PI * h / 6) * (u + v + w)
     *  V(1) = - (M_PI * h / 6) * T
     * with :
     *    T = u + v + w
     * Cumulative distribution function:
     *  Vn(e) = V(e) / V(1)
     *        = ((e*e*e) * u + (e*e) * v + (e) * w) / T
     * Solve:
     *   ((e*e*e) * u + (e*e) * v     + (e) * w) / T            = p
     *    (e*e*e) * u + (e*e) * v     + (e) * w     - p * T     = 0
     *    (e*e*e)     + (e*e) * (v/u) + (e) * (w/u) - p * T / u = 0;
     */
    double p = random_.uniform();
    double T = u + v + w;
    double e;
    datatools::invalidate(e);
    if (u == 0.0) {
      if (v == 0.0) {
        e = p * T / w;
      } else {
        double e0, e1;
        datatools::invalidate(e0);
        datatools::invalidate(e1);
        int nroots = gsl_poly_solve_quadratic(v, w, -p * T, &e0, &e1);
        if (nroots == 1) {
          if (devel) std::cerr << "*** DEVEL *** e0 = " << e0 << std::endl;
          if (e0 >= 0.0 && e0 <= 1.0) e = e0;
        } else if (nroots == 2) {
          if (devel) std::cerr << "*** DEVEL *** e0 = " << e0 << std::endl;
          if (devel) std::cerr << "*** DEVEL *** e1 = " << e1 << std::endl;
          if (e0 >= 0.0 && e0 <= 1.0) e = e0;
          else if (e1 >= 0.0 && e1 <= 1.0) e = e1;
        } else  {
          if (devel) std::cerr << "*** ERROR *** No root! " << std::endl;
        }
      }
    } else {
      double a = v / u;
      double b = w / u;
      double c = -p * T / u;
      double e0, e1, e2;
      datatools::invalidate(e0);
      datatools::invalidate(e1);
      datatools::invalidate(e2);
      int nroots = gsl_poly_solve_cubic(a, b, c, &e0, &e1, &e2);
      if (nroots == 1) {
        if (devel) std::cerr << "*** DEVEL *** e0 = " << e0 << std::endl;
        if (e0 >= 0.0 && e0 <= 1.0) e = e0;
      } else if (nroots == 3) {
        if (devel) std::cerr << "*** DEVEL *** e0 = " << e0 << std::endl;
        if (devel) std::cerr << "*** DEVEL *** e1 = " << e1 << std::endl;
        if (devel) std::cerr << "*** DEVEL *** e2 = " << e2 << std::endl;
        if (e0 >= 0.0 && e0 <= 1.0) e = e0;
        else if (e1 >= 0.0 && e1 <= 1.0) e = e1;
        else if (e2 >= 0.0 && e2 <= 1.0) e = e2;
      } else {
        if (devel) std::cerr << "*** ERROR *** No root! " << std::endl;
      }
    }
    if (devel) std::cerr << "*** DEVEL *** e = " << e << std::endl;
    if (e != e) {
      DT_THROW_IF(true, std::logic_error, "Cannot compute e !");
    }
    double z = e * h;
    if (devel) std::cerr << "*** DEVEL *** z = " << z << std::endl;

    double az = a1 + (a2 - a1) * z / h;
    double bz = b1 + (b2 - b1) * z / h;
    double az2 = gsl_pow_2(az);
    double bz2 = gsl_pow_2(bz);
    double r2 = random_.flat(az2, bz2);
    double r = std::sqrt(r2);
    double theta = random_.flat(theta1_, theta2_);
    geomtools::vector_3d vtx;
    geomtools::create_polar(vtx, r, theta, z);
    return vtx;
  }

  // static
  /*              r1
   *                |`
   *                |   `
   *                |      `
   *                |         `. r2
   *                |          |
   *  --------------+----------+-----------------------> Z
   *               0            h
   *
   *
   *    r(z) = r1 + (r2-r1) * z /h
   *
   *
   *    Infinitesimal section of height dz:
   *
   *    dS = pi (r+r') * sqrt( (r-r')^2 + dz^2)
   *
   *    r' = r + dr
   *    dr = dz (r2-r1)/h
   *    r' = r + dz (r2-r1)/h
   *
   *    dS = pi (2r + dz (r2-r1)/h ) * sqrt(dz^2 ((r2-r1)/h)^2 + dz^2)
   *    dS = pi (2r + dz (r2-r1)/h ) dz * sqrt(1 + ((r2-r1)/h)^2)
   *    dS ~ 2 pi r(z) dz * sqrt(1 + ((r2-r1)/h)^2)
   *    dS ~ 2 pi r(z) dz
   *
   *   PDF:
   *     dS ~ 2 pi * (r1 + (r2-r1) * z /h) * dz
   *
   *   S(z) = 2 pi r1 z + 2 pi (r2-r1) * z^2 / (2 h)
   *   S(0) = 0
   *   S(1) =  2 pi h (r1+r2)/2 # normalization
   *
   *  Cumulative distribution function:
   *
   *   Sn(z) = (2z) * (r1 + (r2-r1)*z / (2h)) / (h*(r1+r2))
   *   Sn(0) = 0
   *   Sn(1) = 1
   *
   *  Given p in [0,1], find z from:
   *
   *  p = (2z) * (r1 + (r2-r1)*z / (2h)) / (h*(r1+r2))
   *
   *  e = z/h with e in [0,1]
   *
   *  [(r2-r1)/(r1+r2)] e^2 +  [2r1/(r1+r2)]e - p = 0
   *
   *  Solution:
   *
   *   e = {-r1 + sqrt[r1^2 + p(r2^2-r1^2)]} / (r2-r1)
   *
   * with:
   *   p = 0 -> e = 0 -> z = 0
   *   p = 1 -> e = 1 -> z = h
   *
   */
  geomtools::vector_3d
  right_circular_conical_frustrum::random_surface (mygsl::rng & random_,
                                                   double radius1_,
                                                   double radius2_,
                                                   double length_,
                                                   double thickness_,
                                                   double theta1_,
                                                   double theta2_)
  {
    double r1 = radius1_;
    double r2 = radius2_;
    double h = length_;
    double r, theta, z;
    DT_THROW_IF(h <= 0.0,
                std::range_error,
                "Invalid frustrum height ('" << h << "') !");
    DT_THROW_IF(r1 < 0.0,
                std::range_error,
                "Invalid frustrum R1 radius ('" << r1 << "') !");
    DT_THROW_IF(r2 < 0.0,
                std::range_error,
                "Invalid frustrum R2 radius ('" << r1 << "') !");
    double p = random_.uniform();
    double e;
    if (r1 == r2) {
      e = p;
    } else {
      e  = (-r1 + std::sqrt(r1*r1+p * (r2*r2 - r1*r1)))/ (r2-r1);
    }
    z = e * h;
    r = r1 + (r2 - r1) * z / h;
    theta = random_.flat(theta1_, theta2_);
    if (thickness_ > 0.0) {
      double dt = 0.5 * thickness_;
      r += random_.flat(-dt, dt);
    }
    geomtools::vector_3d vtx;
    geomtools::create_polar(vtx, r, theta, z);
    return vtx;
  }

} // end of namespace genvtx

// end of conical_frustrum_random_tools.cc

/**
double GetRandZValue(double w1, double L1, double w2, double L2, double h)
{
        double A = (w2 - w1) * (L2 - L1)/(h * h);
        double B = (L1 * (w2 - w1) + w1 * (L2 - L1))/h;
        double C = w1 * L1;
        double area = h * (C + h * (0.5 * B + h * THIRD * A));
        double r = (double)rand() / RAND_MAX * area;
        double det = B * B - 4 * A * C;
        double part1 = B * (B * B - 6 * A * C) - 12 * A * A * r;
        double part2 = sqrt(part1 * part1 - det * det * det);
        double part3 = pow(part1 + part2, THIRD);

        return (-(B + det / part3 + part3) / (2 * A));
}
*/
