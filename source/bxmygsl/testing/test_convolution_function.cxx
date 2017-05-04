// Copyright (C) 2013-2016 François Mauger <mauger@lpccaen.in2p3.fr>

// Standard library:
#include <iostream>   // std::cout, std::cerr, std::clog, std::endl
#include <cstdlib>    // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <fstream>    // std::ofstream
#include <stdexcept>  // std::runtime_error

// This project:
#include <mygsl/convolution_function.h>
#include <mygsl/triangle_function.h>

/// \brief RC filter
class rc_filter
{
public:

  /// Constructor
  rc_filter(double r_, double c_);

  /// Return the time constant of the RC filter
  double get_tau() const;

  /// \brief Impulse response function associated to a RC filter:
  class irf : public mygsl::i_unary_function
  {
  public:

    /// Constructor
    explicit irf(double tau_);

    /// Constructor
    irf(const rc_filter & rc_);

    /// The minimum bound of the non-zero domain
    virtual double get_non_zero_domain_min() const;

  protected:

    /// Function interface
    virtual double _eval(double x_) const;

  private:

    double _tau_; //!< Time constant of the RC filter

  };

private:

  double _r_; //!< Resistor value
  double _c_; //!< Capacitor value

};

/// Main program:
int main(void)
{
  int error_code = EXIT_SUCCESS;

  try {
    // Declare a RC filter object :
    rc_filter RC(1.0, 2.0);

    // Impulse response function of the RC filter :
    std::clog << "INFO: " << "Computing impulse response function..." << '\n';
    rc_filter::irf h(RC.get_tau());

    // Print the shape of the IRF in a file :
    std::ofstream f_rc_irf("rc_irf.data");
    for (double t = -1.0; t < 20.0; t += 0.01) {
      f_rc_irf << t << ' ' << h(t) << '\n';
    }

    // Input triangle signal :
    std::clog << "INFO: " << "Computing input signal..." << '\n';
    double A  = 1.0; // Volt
    double T  = 1.0; // second
    double t0 = 0.0; // second
    /*
     *                ^      ^
     *               /:\     : A
     *              / : \    :
     *             /  :  \   :
     * -----------+---+---+--v---------> t
     *           t0       t0+T
     */
    mygsl::triangle_function Ue(T, t0 + 0.5 * T, A);

    // Print the shape of the input triangle signal in a file :
    std::ofstream f_Ue("Ue.data");
    for (double t = -1.0; t < 20.0; t += 0.1) {
      f_Ue << t << ' ' << Ue(t) << '\n';
    }

    /// Output signal :
    std::clog << "INFO: " << "Computing output signal..." << '\n';
    mygsl::convolution_function Us(Ue, h, 1000);
    Us.initialize_simple();

    // Print the shape of the output signal in a file :
    std::ofstream f_Us("Us.data");
    for (double t = -1.0; t < 20.0; t += 0.1) {
      f_Us << t << ' ' << Us(t) << '\n';
    }

    // Check integrals:
    // double int_Ue = 0.0;
    // double int_Us = 0.0;
    // double dt = 0.0001;
    // for (double t = 0.0; t < 12.0; t += dt) {
    //   int_Ue += Ue(t);
    //   int_Us += Us(t);
    // }
    // int_Ue *= dt;
    // int_Us *= dt;
    // std::clog << "Integral Ue = " << int_Ue << std::endl;
    // std::clog << "Integral Us = " << int_Us << std::endl;

  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "Unexpected exception !" << std::endl;
    error_code = EXIT_FAILURE;
  }

  return error_code;
}

rc_filter::rc_filter(double r_, double c_)
{
  _r_ = r_;
  _c_ = c_;
  return;
}

double rc_filter::get_tau() const
{
  return _r_ * _c_;
}

rc_filter::irf::irf(double tau_)
{
  _tau_ = tau_;
  return;
}

rc_filter::irf::irf(const rc_filter & rc_)
{
  _tau_ = rc_.get_tau();
  return;
}

double rc_filter::irf::get_non_zero_domain_min() const
{
  return 0.0;
}

double rc_filter::irf::_eval(double x_) const
{
  double time = x_;
  double res = 0.0;
  if (time >= 0.0) {
    res = std::exp(-time / _tau_) / _tau_;
  }
  return res;
}
