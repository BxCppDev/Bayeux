/// identity_function.cc

// Ourselves:
#include <mygsl/identity_function.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(identity_function,
                                             "mygsl::identity_function")

  identity_function::identity_function()
  {
    return;
  }

  identity_function::~identity_function()
  {
    return;
  }

  double identity_function::_eval(double x_) const
  {
    return x_;
  }

} // namespace mygsl
