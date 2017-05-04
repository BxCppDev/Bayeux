// mygsl::numerical_differentiation.cc

#include <mygsl/numerical_differentiation.h>

#include <stdexcept>
#include <sstream>

#include <functional>
#include <limits>
#include <gsl/gsl_deriv.h>

#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  double derivative_central (const i_unary_function & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = i_unary_function::g_function;
    func.params   = const_cast<i_unary_function *>(&eval_);
    if (gsl_deriv_central (&func, x_, h_, &result, &abserr) != 0) {
      DT_THROW_IF(true, runtime_error, "Cannot compute central derivative !");
    }
    abserr_ = abserr;
    return result;
  }

  double derivative_central (const i_unary_function & eval_, double x_, double h_)
  {
    double abserr;
    return derivative_central (eval_, x_, h_, abserr);
  }


  double derivative_forward (const i_unary_function & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = i_unary_function::g_function;
    func.params   = const_cast<i_unary_function *>(&eval_);
    if (gsl_deriv_forward (&func, x_, h_, &result, &abserr) != 0) {
      DT_THROW_IF(true, runtime_error, "Cannot compute forward derivative !");
    }
    abserr_ = abserr;
    return result;
  }

  double derivative_forward (const i_unary_function & eval_, double x_, double h_)
  {
    double abserr;
    return derivative_forward (eval_, x_, h_, abserr);
  }

  double derivative_backward (const i_unary_function & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = i_unary_function::g_function;
    func.params   = const_cast<i_unary_function *>(&eval_);
    if (gsl_deriv_backward (&func, x_, h_, &result, &abserr) != 0) {
      DT_THROW_IF(true, runtime_error, "Cannot compute backward derivative !");
    }
    abserr_ = abserr;
    return result;
  }

  double derivative_backward (const i_unary_function & eval_, double x_, double h_)
  {
    double abserr;
    return derivative_backward (eval_, x_, h_, abserr);
  }

  double derivative (const i_unary_function & eval_,
                     double x_,
                     const interval & domain_,
                     double & abserr_)
  {
    double h = domain_.get_eps ();
    if (domain_.is_in_safe (x_))
      {
        return derivative_central (eval_, x_, h, abserr_);
      }
    else if (domain_.is_in_min_unsafe (x_))
      {
        return derivative_forward (eval_, x_, h, abserr_);
      }
    else if (domain_.is_in_max_unsafe (x_))
      {
        return derivative_backward (eval_, x_, h, abserr_);
      }
    else
      {
        DT_THROW_IF(true, domain_error,
                    "Cannot compute derivative for domain is unsafe !"
                    << "Domain is " << domain_ << " and x is " << x_
                    << " !");
      }
    return 0.0;
  }

  double derivative (const i_unary_function & eval_,
                     double x_,
                     const interval & domain_)
  {
    double abserr;
    return derivative (eval_, x_, domain_, abserr);
  }


  /*****************************************************
   *                                                   *
   *   Derivative formula for regular sampled function *
   *                                                   *
   *****************************************************/

  // central:

  double derivative_central_regular_sampled_2 (double f_1_,
                                               double f1_,
                                               double h_)
  {
    return (f1_ - f_1_) / (h_ + h_);
  }

  double derivative_central_regular_sampled_4 (double f_2_,
                                               double f_1_,
                                               double f1_,
                                               double f2_,
                                               double h_)
  {
    double dfdx = -f2_ + 8. * (f1_ - f_1_) + f_2_;
    return dfdx / (12. * h_);
  }

  double second_derivative_central_regular_sampled_2 (double f_1_,
                                                      double f0_,
                                                      double f1_,
                                                      double h_)
  {
    return (f1_ - 2 * f0_ + f_1_) / (h_ * h_);
  }

  double second_derivative_central_regular_sampled_4 (double f_2_,
                                                      double f_1_,
                                                      double f0_,
                                                      double f1_,
                                                      double f2_,
                                                      double h_)
  {
    double dfdx = -f2_ + 16. * (f1_ + f_1_) + - 30. * f0_ - f_2_;
    return dfdx / (12. * h_ * h_);
  }


  // forward:

  double derivative_forward_regular_sampled_1 (double f0_,
                                               double f1_,
                                               double h_)
  {
    return (f1_ - f0_) / h_;
  }

  double derivative_forward_regular_sampled_2 (double f0_,
                                               double f1_,
                                               double f2_,
                                               double h_)
  {
    return (4. * f1_ - 3. * f0_ - f2_) / (h_ + h_);
  }

  double derivative_forward_regular_sampled_3 (double f0_,
                                               double f1_,
                                               double f2_,
                                               double f3_,
                                               double h_)
  {
    return (18. * f1_ - 9. * f2_ + 2. * f3_ - 11 * f0_) / (6 * h_);
  }

  double derivative_forward_regular_sampled_4 (double f0_,
                                               double f1_,
                                               double f2_,
                                               double f3_,
                                               double f4_,
                                               double h_)
  {
    return (-18. * f4_ + 96. * f3_ - 216. * f2_ + 288. * f1_ - 150. * f0_) / (72. * h_);
  }

  double second_derivative_forward_regular_sampled_1 (double f0_,
                                                      double f1_,
                                                      double f2_,
                                                      double h_)
  {
    double dfdx = (f0_ - 2. * f1_ + f2_) / (h_ * h_);
    return dfdx;
  }

  double second_derivative_forward_regular_sampled_2 (double f0_,
                                                      double f1_,
                                                      double f2_,
                                                      double f3_,
                                                      double h_)
  {
    double dfdx = (2 * f0_ - 5. * f1_ + 4 * f2_ - f3_) / (h_ * h_);
    return dfdx;
  }

  // backward:
  double derivative_backward_regular_sampled_1 (double f_1_,
                                                double f0_,
                                                double h_)
  {
    return -derivative_forward_regular_sampled_1 (f0_, f_1_, h_);
  }

  double derivative_backward_regular_sampled_2 (double f_2_,
                                                double f_1_,
                                                double f0_,
                                                double h_)
  {
    return -derivative_forward_regular_sampled_2 (f0_, f_1_, f_2_, h_);
  }

  double derivative_backward_regular_sampled_3 (double f_3_,
                                                double f_2_,
                                                double f_1_,
                                                double f0_,
                                                double h_)
  {
    return -derivative_forward_regular_sampled_3 (f0_, f_1_, f_2_, f_3_, h_);
  }

  double derivative_backward_regular_sampled_4 (double f_4_,
                                                double f_3_,
                                                double f_2_,
                                                double f_1_,
                                                double f0_,
                                                double h_)
  {
    return -derivative_forward_regular_sampled_4 (f0_, f_1_, f_2_, f_3_, f_4_, h_);
  }

  double second_derivative_backward_regular_sampled_1 (double f_2_,
                                                       double f_1_,
                                                       double f0_,
                                                       double h_)
  {
    return second_derivative_forward_regular_sampled_1 (f0_, f_1_, f_2_, h_);
  }

  double second_derivative_backward_regular_sampled_2 (double f_3_,
                                                       double f_2_,
                                                       double f_1_,
                                                       double f0_,
                                                       double h_)
  {
    return second_derivative_forward_regular_sampled_2 (f0_, f_1_, f_2_, f_3_, h_);
  }

}

// end of mygsl::numerical_differentiation.cc
