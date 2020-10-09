//! \file mygsl/identity_function.h

#ifndef MYGSL_IDENTITY_FUNCTION_H
#define MYGSL_IDENTITY_FUNCTION_H

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief The identity function
  class identity_function
    : public i_unary_function
  {
  public:

    //! Default constructor
    identity_function();

    //! Destructor
    virtual ~identity_function();

  protected:

    //! Evaluation method
    virtual double _eval(double x_) const;

  private:

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(identity_function)

  };

} // namespace mygsl

#endif // MYGSL_IDENTITY_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
