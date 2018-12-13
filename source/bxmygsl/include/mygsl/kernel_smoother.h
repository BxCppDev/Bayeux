// \file mygsl/kernel_smoother.h

#ifndef MYGSL_KERNEL_SMOOTHER_H
#define MYGSL_KERNEL_SMOOTHER_H

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/tabulated_function.h>

namespace mygsl {

  /// \brief Kernel smoother interface
  struct i_kernel_smoother
  {
    virtual ~i_kernel_smoother();
    
    virtual double eval(double xs_, double x_) const = 0;
  };

  /// \brief Gauss kernel smoother
  class gauss_kernel_smoother
    : public i_kernel_smoother
  {
  public:

    /// Constructor
    gauss_kernel_smoother(const double b_);

    virtual ~gauss_kernel_smoother();

    double eval(double xs_, double x_) const override;

  private:
    
    double _b_;
    
  };

  /// \bried Nadaraya-Watson kernel-weigthed average functor
  class nw_sampled_function_smoother
    : public i_unary_function
  {
  public:

    /// Constructor
    nw_sampled_function_smoother(const mygsl::tabulated_function & tf_,
                                 i_kernel_smoother & ks_);

    /// Constructor with default Gauss kernel smoother 
    nw_sampled_function_smoother(const mygsl::tabulated_function & tf_,
                                 const double b_);

    /// Destructor
    virtual ~nw_sampled_function_smoother();
    
  protected:

    /// Evaluation of the smoothed function
    double _eval(double x_) const;
 
  private:

    const mygsl::tabulated_function & _tf_;
    bool _ks_free_needed_ = false;
    i_kernel_smoother * _ks_ = nullptr;
    
  };

} // namespace mygsl

#endif // MYGSL_KERNEL_SMOOTHER_H

// Local Variables:
// mode: c++
// coding: utf-8
// End:
