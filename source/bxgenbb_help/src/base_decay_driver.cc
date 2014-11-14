// base_decay_driver.cc
/*
 * Copyright 2014 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <genbb_help/base_decay_driver.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
// #include <genbb_help/nuclear_level.h>

namespace genbb {

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_decay_driver,"genbb::base_decay_driver/__system__");


  void base_decay_driver::_set_default()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    return;
  }

  base_decay_driver::base_decay_driver()
  {
    _initialized_ = false;
    _set_default();
    return;
  }

  base_decay_driver::~base_decay_driver()
  {
    if (is_initialized()) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Decay driver has not been reset from its own constructor!");
    }
    return;
  }

  bool base_decay_driver::is_initialized() const
  {
    return _initialized_;
  }

  void base_decay_driver::_set_initialized(bool i_)
  {
    _initialized_ = i_;
    return;
  }

  void base_decay_driver::set_levels(const nuclear_level & /* level_initial_ */,
                                     const nuclear_level & /* level_final_ */)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Decay driver transition is already initialized!");
    return;
  }

  void base_decay_driver::initialize_simple()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  void base_decay_driver::_initialize(const datatools::properties & setup_)
  {

    // Parse logging priority:
    datatools::logger::priority prio =
      datatools::logger::extract_logging_configuration(setup_,
                                                       datatools::logger::PRIO_FATAL,
                                                       true);
    if (prio != datatools::logger::PRIO_UNDEFINED) {
      set_logging(prio);
    }

    return;
  }

  datatools::logger::priority base_decay_driver::get_logging() const
  {
    return _logging_;
  }

  void base_decay_driver::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  void base_decay_driver::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Logging    : '"
         << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized    : "
         << is_initialized() << std::endl;

    return;
  }

  // static
  base_decay_driver *
  decay_driver_factory::create(const std::string & driver_id_)
  {
    base_decay_driver * driver = 0;
    typedef ::genbb::base_decay_driver::factory_register_type factory_reg_type;
    const factory_reg_type & fac_reg = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::genbb::base_decay_driver);
    // fac_reg.print(std::cerr, "DEVEL: ");

    if (fac_reg.has(driver_id_)) {
      const factory_reg_type::factory_type & the_factory
        = fac_reg.get(driver_id_);
      driver = the_factory();
    }

    return driver;
  }

} // end of namespace genbb
