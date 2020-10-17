/// \file datatools/configuration/variant_dependency_logic.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-26
 * Last modified : 2016-11-01
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
 *   Variant dependency logic.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_H

// Standard library:
#include <map>
#include <memory>
#include <string>

// Third party:
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

// This project (Bayeux/datatools):
#include <datatools/i_tree_dump.h>
#include <datatools/configuration/variant_dependency_utils.h>

namespace datatools {

  namespace configuration {

    class variant_dependency;
    class variant_object_info;
    class variant_repository;
    class variant_registry;

    /// \brief Base class for variant enabling logic predicates
    class base_dependency_logic
      : public datatools::i_tree_dumpable
    {
    public:

      /// Dictionary of input ports
      typedef std::map<unsigned int, dependency_logic_ptr_type> input_dict_type;

      /// Constructor
      base_dependency_logic(variant_dependency &);

      /// Destructor
      ~base_dependency_logic() override;

      /// Return the global unique class identifier
      virtual const char * guid() const = 0;

      /// Return the minimum number of input ports
      virtual std::size_t min_ports() const;

      /// Return the maximum number of input ports
      virtual std::size_t max_ports() const;

      /// Check validity
      virtual bool is_valid() const;

      /// Check the logic
      virtual bool operator()() const = 0;

      /// Check if a given variant is active
      bool check_active_variant(const variant_object_info &) const;

      /// Connect an input port to a logic
      void connect(const unsigned int input_port_, dependency_logic_ptr_type logic_);

      /// Disconnect an input port from a logic
      void disconnect(const unsigned int input_port_);

      /// Connect input port to the ouput of the created logic
      base_dependency_logic & connect(const unsigned int input_port_, const std::string & guid_);

      /// Connect input port to the dependee variant at given slot
      void connect_dependee(const unsigned int input_port_, const unsigned int dependee_slot_);

      /// Return the repository handle
      const variant_repository & get_repository() const;

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Return the const reference to the owner dependency
      const variant_dependency & get_owner_dependency() const;

    protected:

      /// Return the mutable reference to the owner dependency
      variant_dependency & _grab_owner_dependency();

    private:

      variant_dependency * _owner_dependency_ = nullptr; //!< The owner dependency the logic object belongs to

    protected:

      const variant_repository * _repository = nullptr; //!< The handle to the parent repository
      input_dict_type            _inputs; //!< Connected input ports

    };

    /// \brief Logic factory type
    typedef boost::function<base_dependency_logic*(variant_dependency &)>
    dependency_logic_factory_type;

    /// \brief Dictionary of logic factories
    typedef std::map<std::string, dependency_logic_factory_type>
    dependency_logic_factory_dict_type;

    /// Return the dictionary of dependency logic factories:
    dependency_logic_factory_dict_type & dependency_logic_factories();

    /// \brief Variant slot predicate
    class slot_logic : public base_dependency_logic
    {
    public:

      /// Constructor
      slot_logic(variant_dependency &);

      /// Destructor
      ~slot_logic() override;

      /// Return the global unique class identifier
      const char * guid() const override;

      /// Return the minimum number of input ports
      std::size_t min_ports() const override;

      /// Return the maximum number of input ports
      std::size_t max_ports() const override;

      /// Check validity
      bool is_valid() const override;

      /// Check the logic
      bool operator()() const override;

      /// Set the slot identifier of the dependee
      void set_dependee_slot(const unsigned int dependee_slot_);

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;
    private:

      unsigned int _dependee_slot_
      = std::numeric_limits<unsigned int>::max(); //!< Slot of the input dependee

    };

    /// \brief NOT predicate
    class not_logic : public base_dependency_logic
    {
    public:

      /// Constructor
      not_logic(variant_dependency &);

      /// Destructor
      ~not_logic() override;

      /// Return the global unique class identifier
      const char * guid() const override;

      /// Return the maximum number of input ports
      std::size_t max_ports() const override;

      /// Check the logic
      bool operator()() const override;

    };

    /// \brief AND predicate
    class and_logic : public base_dependency_logic
    {
    public:

      /// Constructor
      and_logic(variant_dependency &);

      /// Destructor
      ~and_logic() override;

      /// Return the global unique class identifier
      const char * guid() const override;

      /// Check the logic
      bool operator()() const override;

    };

    /// \brief OR predicate
    class or_logic : public base_dependency_logic
    {
    public:

      /// Constructor
      or_logic(variant_dependency &);

      /// Destructor
      ~or_logic() override;

      /// Return the global unique class identifier
      const char * guid() const override;

      /// Check the logic
      bool operator()() const override;

    };

    /// \brief XOR predicate
    class xor_logic : public base_dependency_logic
    {
    public:

      /// Constructor
      xor_logic(variant_dependency &);

      /// Destructor
      ~xor_logic() override;

      /// Return the global unique class identifier
      const char * guid() const override;

      /// Check the logic
      bool operator()() const override;

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
