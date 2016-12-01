//! \file mygsl/unary_function_utils.h

#ifndef MYGSL_UNARY_FUNCTION_UTILS_H
#define MYGSL_UNARY_FUNCTION_UTILS_H 1

// Standard library:
#include <iostream>
#include <limits>
#include <functional>
#include <string>
#include <vector>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>

namespace mygsl {

  // Forward class declaration:
  class i_unary_function;

  //! Alias for a handle to an unary function object
  typedef datatools::handle<i_unary_function> unary_function_handle_type;

  //! Alias for a handle to a const unary function object
  typedef datatools::handle<const i_unary_function> const_unary_function_handle_type;

  //! Alias for dictionary of handles to unary function objects
  typedef std::map<std::string, unary_function_handle_type> unary_function_dict_type;

  //! \brief Smart handle to an unary function object
  class unary_function_handle
  {
  public:
    unary_function_handle();
    unary_function_handle(const i_unary_function &);
    unary_function_handle(const const_unary_function_handle_type &);
    virtual ~unary_function_handle();
    const i_unary_function & func() const;
    bool is_null() const;
    void reset(const i_unary_function &);
    void reset(const const_unary_function_handle_type &);
    void reset();
  private:
    void _check_();
  private:
    const_unary_function_handle_type _hfunc_;           //!< Smart handle to a functor
    const i_unary_function *         _pfunc_ = nullptr; //!< Plain pointer to a functor
  };

} // namespace mygsl

#endif // MYGSL_UNARY_FUNCTION_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
