/// \file mctools/signal/signal_data.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-28
 * Last modified : 2016-09-28
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A container hosting collections of signal hits.
 *
 */

#ifndef MCTOOLS_SIGNAL_SIGNAL_DATA_H
#define MCTOOLS_SIGNAL_SIGNAL_DATA_H

// Standard Library:
#include <string>
#include <map>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/handle.h>
#include <datatools/properties.h>

// This project:
#include <mctools/signal/base_signal.h>

namespace mctools {
  namespace signal {

    /// The container of collections of modelled analog signals
    class signal_data :
      public datatools::i_serializable,
      public datatools::i_tree_dumpable
    {
    public:

      /// Alias for the MC base step hit
      typedef datatools::handle<base_signal>  signal_handle_type;
      typedef std::vector<signal_handle_type> signal_handle_collection_type;
      typedef std::map<std::string, signal_handle_collection_type> signals_dict_type;

      /// Default constructor
      signal_data();

      /// Destructor
      ~signal_data() override;

      /// Get a reference to the non mutable collection of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Get a reference to the mutable collection of auxiliary properties
      datatools::properties & grab_auxiliaries();

      ///  Set the collection of auxiliary properties
      void set_auxiliaries(const datatools::properties &);

      /// Check is some signals exist in some given category
      bool has_signals(const std::string & a_category) const;

      /// Add a new collection of signal hits with some given category and a default capacity for memory allocation
      void add_signals(const std::string & a_category, size_t a_capacity = 0);

      /// Get the number of signals hits within a given category
      size_t get_number_of_signals(const std::string & a_category) const;

      /// Get a reference to the non mutable signal hit within a given category and index
      const base_signal & get_signal(const std::string & a_category, int a_hit_index) const;

      /// Get a reference to the mutable signal hit within a given category and index
      base_signal & grab_signal(const std::string & a_category, int a_hit_index);

      /// Add/append a new signal hit in a collection of signal hits with some given category
      base_signal & add_signal(const std::string & a_category);

      /// remove a signal hit in a collection of signal hits with some given category and index
      void remove_signal(const std::string & a_category, int a_hit_index);

      /// Return a reference to the mutable collection of signals within a given category
      signal_handle_collection_type &
      grab_signals(const std::string & a_category);

      /// Return a reference to the non mutable collection of signals within a given category
      const signal_handle_collection_type &
      get_signals(const std::string & a_category) const;

      /// Remoive the collection of signals within a given category
      void remove_signals(const std::string & a_category);

      /// Build list of signal categories
      void build_list_of_categories(std::vector<std::string> & cats_) const;

      /// Reset the internal data
      void clear();

      /// Reset data
      void reset();

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

    private:

      datatools::properties _auxiliaries_;  //!< List of auxiliary properties
      signals_dict_type     _signals_dict_; //!< Dictionary of collections of handle of signals

      // datatools/Boost/brio serialization:
      DATATOOLS_SERIALIZATION_DECLARATION()

#if MCTOOLS_WITH_REFLECTION == 1
      //! Reflection interface
      DR_CLASS_RTTI()
#endif

    };

  } // end of namespace signal
} // end of namespace mctools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(mctools::signal::signal_data, "mctools::signal::signal_data")

#if MCTOOLS_WITH_REFLECTION == 1
// Activate reflection support for the mctools::signal::signal_data class :
DR_CLASS_INIT(::mctools::signal::signal_data)
#endif // MCTOOLS_WITH_REFLECTION

#endif // MCTOOLS_SIGNAL_SIGNAL_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
