/// \file datatools/configuration/variant_repository.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2014-09-22
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A repository for variant registries.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REPOSITORY_H
#define DATATOOLS_CONFIGURATION_VARIANT_REPOSITORY_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/shared_ptr.hpp>

// This project (Bayeux/datatools):
#include <datatools/configuration/utils.h>
#include <datatools/enriched_base.h>
#include <datatools/command_utils.h>

namespace datatools {

  namespace configuration {

    class variant_registry;
    class variant_registry_manager;

    /// \brief Variant repository
    class variant_repository : public datatools::enriched_base
    {
    public:

      /// \brief Entry that hosts a variant registry
      struct registry_entry
      {
      public:
        registry_entry();
        const variant_registry & get_registry() const;
        variant_registry & grab_registry();
        void reset();
        bool is_external() const;
        bool is_embedded() const;
        bool is_valid() const;
        void set_external_registry(variant_registry &);
        void set_rank(int);
        int get_rank() const;
        bool has_rank() const;
      private:
        int _rank_;
        boost::shared_ptr<variant_registry_manager> _embedded_manager_; //!< Embedded registry factory/manager
        boost::shared_ptr<variant_registry> _embedded_registry_; //!< Embedded registry
        variant_registry                  * _external_registry_; //!< Handle to an external registry
        friend class variant_repository;
      };

      /// Type of dictionary of configuration variant registries
      typedef std::map<std::string, registry_entry> registry_dict_type;
      //typedef std::map<std::string, int>            registry_rank_type;

      /// Default constructor
      variant_repository();

      /// Destructor
      ~variant_repository();

      /// Check the name of the organization
      bool has_organization() const;

      /// Return the name of the organization
      const std::string & get_organization() const;

      /// Set the name of the organization
      void set_organization(const std::string &);

      /// Check the name of the application
      bool has_application() const;

      /// Return the name of the application
      const std::string & get_application() const;

      /// Set the name of the application
      void set_application(const std::string &);

      /// Initialize from a properties container
      void initialize(const datatools::properties & config_);

      /// Load embedded registries from a properties container
      void load_registries(const datatools::properties & config_);

      /// Basic initialize
      void initialize_simple();

      /// Reset
      void reset();

      /// Check the initialization flag
      bool is_initialized() const;

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      /// Check if a registry exists
      bool has_registry(const std::string & registry_name_) const;

      /// Return a reference to a non mutable registry
      const variant_registry & get_registry(const std::string & registry_name_) const;

      /// Return a reference to a mutable registry
      variant_registry & grab_registry(const std::string & registry_name_);

      /// Return a reference to the non-mutable dictionary of registries
      const registry_dict_type & get_registries() const;

      /// Return a reference to the mutable dictionary of registries
      registry_dict_type & grab_registries();

      /// Registration of an embedded registry/manager
      void registration_embedded(const std::string & registry_manager_config_,
                                 const std::string & top_variant_name_ = "",
                                 const std::string & registry_name_ = "",
                                 const std::string & registry_display_name_ = "",
                                 const std::string & registry_terse_description_ = "");

      /// Registration of an external registry
      void registration_external(variant_registry & external_registry_,
                                 const std::string & registry_name_ = "");

      /// Unregistration of some specific registry
      void unregistration(const std::string & registry_name_);

      /// Clear registries
      void clear_registries();

      /// Unregistration of external registries
      void external_registries_unregistration();

      /// Check if all registries are accomplished
      bool is_accomplished() const;

      /// Check if the repository is locked
      bool is_locked() const;

      /// Lock the repository
      void lock();

      /// Unlock the repository
      void unlock();

      /// Set the value of a variant parameter
      command::returned_info
      cmd_set_parameter(const std::string & registry_key_,
                        const std::string & param_path_,
                        const std::string & param_value_token_);

      /// Get the value of a variant parameter
      command::returned_info
      cmd_get_parameter(const std::string & registry_key_,
                        const std::string & param_path_,
                        std::string & param_value_token_) const;

      /// Check if a variant is active
      command::returned_info
      cmd_is_active_variant(const std::string & registry_key_,
                            const std::string & variant_path_,
                             bool & active_) const;

      /// Export to the system variant repository
      void system_export(uint32_t flags_ = 0);

      /// Discard from the system variant repository
      void system_discard() const;

    protected:

      registry_entry & _add_entry(const std::string & registry_name_);

    private:

      bool               _initialized_;  //!< Initialization flag
      std::string        _organization_; //!< The name of the organization
      std::string        _application_;  //!< The name of the application
      bool               _locked_;       //!< Lock flag
      registry_dict_type _registries_;   //!< Dictionary of configuration variant registries

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
