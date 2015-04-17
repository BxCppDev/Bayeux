//! \file  mygsl/plain_function.h
//! \brief A composite function from two unary functors
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_PLAIN_FUNCTION_WRAPPER_H
#define MYGSL_PLAIN_FUNCTION_WRAPPER_H

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/math.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace mygsl {

  //! \brief Wrapper for C/C++ plain function with signature 'double (*)(double)'
  class plain_function_wrapper : public i_unary_function
  {
  public:

    //! Set the plain function by name
    void set_plain_function_by_name(const std::string &);

    //! Set the address of the plain function
    void set_plain_function(plain_function_type);

    //! Check is the plain function is defined
    bool has_plain_function() const;

    //! Default constructor
    plain_function_wrapper();

    //! Constructor
    plain_function_wrapper(const std::string &);

    //! Constructor
    plain_function_wrapper(const plain_function_type &);

    //! Destructor
    virtual ~plain_function_wrapper();

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Initialization from a container of parameters and a dictionary of functors
    virtual void initialize(const datatools::properties & config_,
                            unary_function_dict_type & functors_);

    //! Reset the functor
    virtual void reset();

  protected:

    //! Evaluation method
    virtual double _eval(double x_) const;

  private:

    plain_function_type _plain_function_; //!< Address of a plain function

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(plain_function_wrapper);

  };

} // end of namespace mygsl

#endif // MYGSL_PLAIN_FUNCTION_WRAPPER_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
