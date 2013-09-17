/* advanced_object.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-12
 * Last modified: 2013-09-12
 *
 * License:
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   A base class for complex objects with several
 *   basic attributes.
 *
 */

#ifndef DATATOOLS_ADVANCED_OBJECT_H_
#define DATATOOLS_ADVANCED_OBJECT_H_

// Standard library
#include <string>

// This project:
#include <datatools/datatools_config.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/properties.h>

#if DATATOOLS_WITH_REFLECTION == 1
#include <datatools/reflection_macros.h>
#endif

namespace datatools {

  class advanced_object  :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_tree_dumpable {

  public:

    static bool validate_name(const std::string & candidate_name_);

    advanced_object();

    advanced_object(const std::string & name_,
		    const std::string & display_name_ = "",
		    const std::string & terse_description_ = "",
		    logger::priority lp_ = logger::PRIO_WARNING);

    virtual ~advanced_object();

    bool has_name() const;

    void set_name(const std::string & name_);

    advanced_object & set_name_c(const std::string & name_);

    const std::string & get_name() const;

    bool has_display_name() const;

    void set_display_name(const std::string & display_name_);

    advanced_object & set_display_name_c(const std::string & display_name_);

    const std::string & get_display_name() const;

    bool has_terse_description() const;

    void set_terse_description(const std::string & terse_description_);

    advanced_object & set_terse_description_c(const std::string & terse_description_);

    const std::string & get_terse_description() const;

    bool has_auxiliaries() const;

    const datatools::properties & get_auxiliaries() const;

    datatools::properties & grab_auxiliaries();

    void reset_auxiliaries();

    bool is_debug() const;

    void set_logging_priority(logger::priority lp_);

    advanced_object & set_logging_priority_c(logger::priority lp_);

    logger::priority get_logging_priority() const;

    void clear_advanced();

    virtual void tree_dump(std::ostream& out_ = std::clog,
			   const std::string& title_  = "",
			   const std::string& indent_ = "",
			   bool inherit_ = false) const;

  private:

    std::string _name_;
    std::string _display_name_;
    std::string _terse_description_;
    logger::priority _logging_priority_;
    datatools::properties _auxiliaries_;

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(advanced_object);

#if DATATOOLS_WITH_REFLECTION == 1
  //! Reflection interface
  DR_CLASS_RTTI();
#endif

  };

} // end of namespace datatools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::advanced_object, "datatools::advanced_object");
#if DATATOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the 'datatools::advanced_object' class:
DR_CLASS_INIT(::datatools::advanced_object);
#endif // DATATOOLS_WITH_REFLECTION

#endif // DATATOOLS_ADVANCED_OBJECT_H_
