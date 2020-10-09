//! \file mygsl/zero_function.h

#ifndef MYGSL_ZERO_FUNCTION_H
#define MYGSL_ZERO_FUNCTION_H

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief The zero function
  class zero_function
    : public i_unary_function
  {
  public:

    //! Default constructor
    zero_function();

    //! Destructor
    virtual ~zero_function();

  protected:

    //! Evaluation method
    virtual double _eval(double x_) const;

  private:

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(zero_function)
  };

} // namespace mygsl

#endif // MYGSL_ZERO_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
