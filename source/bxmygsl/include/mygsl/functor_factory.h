//! \file  mygsl/functor_factory.h
//! \brief A factory for unary functor
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

#ifndef MYGSL_FUNCTOR_FACTORY_H
#define MYGSL_FUNCTOR_FACTORY_H

// This project:
#include <mygsl/i_unary_function.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools :
#include <datatools/i_tree_dump.h>

namespace mygsl {

  //! \brief Factory of functors
  class functor_factory:
    public datatools::i_tree_dumpable
  {
  public:

    //! \brief construct flags
    enum construct_flags {
      CONSTRUCT_NONE            = 0,   //!< No flags
      CONSTRUCT_NO_PRELOAD      = 0x1, //!< Do not preload factories from system register
      CONSTRUCT_FACTORY_VERBOSE = 0x2  //!< Activate verbosity of the embedded factory
    };

    //! Check lock flag
    bool is_locked() const;

    //! Lock the factory
    void lock();

    //! Unlock the factory
    void unlock();

    //! Default constructor
    functor_factory();

    //! Constructor
    functor_factory(uint32_t flags_);

    //! Destructor
    ~functor_factory() override;

    //! Create a functor without initialization
    void create(unary_function_handle_type & handle_,
                const std::string & functor_id_);

    //! Create a functor
    void create(unary_function_handle_type & handle_,
                const std::string & functor_id_,
                const datatools::properties & functor_config_);

    //! Create a functor
    void create(unary_function_handle_type & handle_,
                const std::string & functor_id_,
                const datatools::properties & functor_config_,
                unary_function_dict_type & functors_);

    //! Smart dump
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    i_unary_function * _allocate(const std::string & functor_id_);

    virtual void _at_lock();

    virtual void _at_unlock();

    void _basic_construct(uint32_t flags_);

    void _basic_destroy();

  private:

    bool _locked_; //!< Lock flag
    boost::scoped_ptr<i_unary_function::factory_register_type> _factory_register_; //!< Register of functor factories

  };

} // end of namespace mygsl

#endif // MYGSL_FUNCTOR_FACTORY_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
