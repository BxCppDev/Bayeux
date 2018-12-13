// test_kernel_smoother.cxx

/*
 *
 * gnuplot> set key out 
 * gnuplot> set grid
 * gnuplot> plot [:][:] \
 *        'a.data' i 0 w lines, \
 *        '' i 1 w p pt 6 ps 0.5, \  
 *        '' i 2 w l, \
 *        '' i 3 w l, \
 *        '' i 4 w l, \
 *        '' i 5 w l
 *
 */

// Ourselves:
#include <mygsl/kernel_smoother.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// This project:
#include <mygsl/tabulated_function.h>

double f(double x_)
{
  static const double a =  0.02;
  static const double b = -0.2;
  static const double l = 0.02;
  static const double w = 0.22;
  return std::exp(-l * x_) * std::cos(w * x_) + a * x_ + b;
}

double noise(double /* y_ */)
{
  double a = 0.05;
  a = 0.125;
  // a = 0.25;
  return a * (2 * drand48() - 1.0);
}

int main(int /* argc_ */ , char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    double xmin = 0.0;
    double xmax = 100.0;
    uint32_t nsamples = 201;
    double dx = (xmax-xmin) / (nsamples - 1);
    std::clog << "dx = " << dx << std::endl;

    mygsl::tabulated_function waveform_exact;
    mygsl::tabulated_function waveform_noisy;
    for (double xi = xmin; xi < xmax + 0.5 * dx; xi += dx) {
      double fi_med = f(xi);
      double fi = fi_med + noise(fi_med);
      waveform_exact.add_point(xi, fi_med);
      waveform_noisy.add_point(xi, fi);
    }
    waveform_exact.lock_table("linear");
    waveform_noisy.lock_table("linear");

    double b1 = 1.0;
    mygsl::nw_sampled_function_smoother fsmooth1(waveform_noisy, b1);
    mygsl::tabulated_function waveform_smooth1;
    for (double xi = xmin; xi < xmax + 0.5 * dx; xi += dx) {
      double fi = fsmooth1(xi);
      waveform_smooth1.add_point(xi, fi);
    }
    waveform_smooth1.lock_table("linear");

    double b2 = 2.0;
    mygsl::nw_sampled_function_smoother fsmooth2(waveform_noisy, b2);
    mygsl::tabulated_function waveform_smooth2;
    for (double xi = xmin; xi < xmax + 0.5 * dx; xi += dx) {
      double fi = fsmooth2(xi);
      waveform_smooth2.add_point(xi, fi);
    }
    waveform_smooth2.lock_table("linear");

    double b3 = 3.0;
    mygsl::nw_sampled_function_smoother fsmooth3(waveform_noisy, b3);
    mygsl::tabulated_function waveform_smooth3;
    for (double xi = xmin; xi < xmax + 0.5 * dx; xi += dx) {
      double fi = fsmooth3(xi);
      waveform_smooth3.add_point(xi, fi);
    }
    waveform_smooth3.lock_table("linear");

    double b4 = 4.0;
    mygsl::nw_sampled_function_smoother fsmooth4(waveform_noisy, b4);
    mygsl::tabulated_function waveform_smooth4;
    for (double xi = xmin; xi < xmax + 0.5 * dx; xi += dx) {
      double fi = fsmooth4(xi);
      waveform_smooth4.add_point(xi, fi);
    }
    waveform_smooth4.lock_table("linear");


    // Graphics:
    std::ostream & out = std::cout;
    {
      const mygsl::tabulated_function::points_map_type & pm = waveform_exact.points();
      for (mygsl::tabulated_function::points_map_type::const_iterator i = pm.begin();
           i != pm.end();
           i++) {
        double xi = i->first;
        double fi = i->second;
        out << xi << ' ' << fi << std::endl;
      }
      out << std::endl;
      out << std::endl;
    }
    {
      const mygsl::tabulated_function::points_map_type & pm = waveform_noisy.points();
      for (mygsl::tabulated_function::points_map_type::const_iterator i = pm.begin();
           i != pm.end();
           i++) {
        double xi = i->first;
        double fi = i->second;
        out << xi << ' ' << fi << std::endl;
      }
      out << std::endl;
      out << std::endl;
    }
    {
      uint16_t ngsamples = 1000;
      waveform_smooth1.write_ascii(out, xmin, xmax, ngsamples);
      out << std::endl;
      out << std::endl;
      
      waveform_smooth2.write_ascii(out, xmin, xmax, ngsamples);
      out << std::endl;
      out << std::endl;
     
      waveform_smooth3.write_ascii(out, xmin, xmax, ngsamples);
      out << std::endl;
      out << std::endl;
     
      waveform_smooth4.write_ascii(out, xmin, xmax, ngsamples);
      out << std::endl;
      out << std::endl;
    }
     
  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
