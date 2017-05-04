// functor_factory.cc
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

// Ourselves:
#include <mygsl/functor_factory.h>

// Standard library:
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mygsl {

  bool functor_factory::is_locked() const
  {
    return _locked_;
  }

  void functor_factory::lock()
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Factory is already locked!");
    _at_lock();
    _locked_ = true;
    return;
  }

  void functor_factory::unlock()
  {
    DT_THROW_IF(!is_locked(), std::logic_error, "Factory is not locked!");
    _locked_ = false;
    _at_unlock();
    return;
  }

  // virtual
  void functor_factory::_at_lock()
  {
    return;
  }

  // virtual
  void functor_factory::_at_unlock()
  {
    return;
  }

  functor_factory::functor_factory()
  {
    _locked_ = false;
    _basic_construct(CONSTRUCT_NONE);
    return;
  }

  functor_factory::functor_factory(uint32_t flags_)
  {
    _locked_ = false;
    _basic_construct(flags_);
    return;
  }

  functor_factory::~functor_factory()
  {
    _basic_destroy();
    return;
  }

  void functor_factory::_basic_destroy()
  {
    if (_factory_register_) {
      _factory_register_.reset();
    }
    return;
  }

  void functor_factory::_basic_construct(uint32_t flags_)
  {
    _factory_register_.reset(new i_unary_function::factory_register_type("mygsl::i_unary_function/functor_factory",
                                                                         flags_ & CONSTRUCT_FACTORY_VERBOSE));
    bool preload = ! (flags_ & CONSTRUCT_NO_PRELOAD);
    if (preload) {
      _factory_register_->import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(i_unary_function));
    }

    return;
  }

  i_unary_function * functor_factory::_allocate(const std::string & functor_id_)
  {
    // DT_THROW_IF(!is_locked(), std::logic_error, "Functor factory is not locked!");
    i_unary_function * fctor = 0;
    if (_factory_register_->has(functor_id_)) {
      const i_unary_function::factory_register_type::factory_type & the_factory
        = _factory_register_->get(functor_id_);
      fctor = the_factory();
    }
    return fctor;
  }

  void functor_factory::create(unary_function_handle_type & handle_,
                               const std::string & functor_id_)
  {
    i_unary_function * fctor = _allocate(functor_id_);
    DT_THROW_IF(fctor == 0, std::logic_error, "No functor was created!");
    handle_.reset(fctor);
    return;
  }

  void functor_factory::create(unary_function_handle_type & handle_,
                              const std::string & functor_id_,
                              const datatools::properties & functor_config_)
  {
    unary_function_dict_type dummy_dict;
    create(handle_, functor_id_, functor_config_, dummy_dict);
    return;
  }

  void functor_factory::create(unary_function_handle_type & handle_,
                              const std::string & functor_id_,
                              const datatools::properties & functor_config_,
                              unary_function_dict_type & functors_)
  {
    create(handle_, functor_id_);
    i_unary_function & functor = handle_.grab();
    functor.initialize(functor_config_, functors_);
    return;
  }

  void functor_factory::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << ':' << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Locked : " << _locked_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Factory register : " << std::endl;
    std::ostringstream indent2;
    indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
    _factory_register_->tree_dump(out_, "", indent2.str());

    return;
  }

} // end of namespace mygsl
