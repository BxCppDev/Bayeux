/// \file datatools/configuration/variant_dependency_model.h
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
 *   A model of variant dependencies between variant parameters
 *   and variants.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_MODEL_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_MODEL_H

// Standard library:
#include <string>
#include <set>
#include <map>
#include <memory>

// This project (Bayeux/datatools):
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/logger.h>
#include <datatools/configuration/variant_dependency.h>

namespace datatools {

  namespace configuration {

    class variant_repository;
    class variant_registry;

    /// \brief Model of variant dependencies associated to a variant repository
    ///
    /// Dictionary of dependee slots: each dependee of interest publishes its activation state to
    /// a specific slot with unique slot ID:
    /// \code
    /// +----------------+ Slot IDs
    /// |  Dependee "E0" o-->[0]
    /// +----------------+
    /// +----------------+
    /// |  Dependee "E1" o-->[1]
    /// +----------------+
    /// +----------------+
    /// |  Dependee "E2" o-->[2]
    /// +----------------+
    /// +----------------+
    /// |  Dependee "E3" o-->[5]
    /// +----------------+
    /// +----------------+
    /// |  Dependee "E4" o-->[12]
    /// +----------------+
    /// \endcode
    ///
    /// Dictionary of dependencies: each dependency computes its state from a logic which is
    /// connected to a set of input slots:
    /// \code
    ///                       Enabling logic
    /// +-----------------+     +-------+       Input slots
    /// |  Dependency "A" |--<--o  AND  |--<--o ([0]; [12])
    /// +-----------------+     +-------+
    /// +-----------------+     +-------+
    /// |  Dependency "B" |--<--o  OR   |--<--o ([0]; [2]; [5])
    /// +-----------------+     +-------+
    /// +-----------------+     +-------+
    /// |  Dependency "C" |--<--o  NOT  |--<--o ([1])
    /// +-----------------+     +-------+
    /// \endcode
    ///
    struct variant_dependency_model
      : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Description of a dependee slot:
      struct dependee_record {
        std::string            dependee_path;
      };
      typedef std::map<unsigned int, dependee_record> dependee_record_dict_type;

      /// \brief Description of a dependency:
      struct dependency_record {
        std::string            depender_path;
        std::set<unsigned int> input_slots;
        std::string            logic_expression;
        datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      };
      typedef std::map<std::string, dependency_record> dependency_record_dict_type;

      typedef std::shared_ptr<variant_dependency>          dependency_ptr_type;
      typedef std::map<std::string, dependency_ptr_type>   dependency_dict_type;
      typedef std::map<std::string, std::set<std::string>> dependers_table_type;

      /// Constructor
      variant_dependency_model(const variant_repository & repository_);

      /// Constructor
      variant_dependency_model(const variant_registry & registry_);

      /// Check repository
      bool has_repository() const;

      /// Return the repository
      const variant_repository & get_repository() const;

      /// Check registry
      bool has_registry() const;

      /// Return the registry
      const variant_registry & get_registry() const;

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(const datatools::logger::priority);

      /// Check if the dependency model is local to a registry
      bool is_local() const;

      /// Check if a variant is used as a dependee
      bool has_dependee_record_by_path(const std::string & dependee_path_) const;

      /// Check if a dependee is set at given slot
      bool has_dependee_record(unsigned int slot_) const;

      /// Add a dependee at given slot
      unsigned int add_dependee_record(unsigned int slot_, const std::string & dependee_path_);

      /// Return the dependee path at given slot
      const std::string & get_dependee_record_path(unsigned int slot_) const;

      /// Return the dictionary of dependee records
      const dependee_record_dict_type & get_dependee_records() const;

      /// Build the list of dependee slots
      void build_list_of_dependee_slots(std::set<unsigned int> & dep_slots_) const;

      /// Check if a dependency record is set
      bool has_dependency_record(const std::string & name_) const;

      /// Check if a dependency is set given its depender path
      bool has_dependency_record_by_depender_path(const std::string & name_) const;

      /// Build the list of dependency records by name
      void build_list_of_dependency_records(std::set<std::string> & dep_names_) const;

      /// Add a dependency
      void add_dependency_record(const std::string & name_,
                                 const std::string & depender_path_,
                                 const std::set<unsigned int> & input_slots_,
                                 const std::string & logic_expression_ = "",
                                 const datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

      /// Return the dictionary of dependency records
      const dependency_record_dict_type & get_dependency_records() const;

      /// Check validity
      bool is_valid() const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization
      void initialize(const datatools::properties & config_);

      /// Reset
      void reset();

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Check if a dependency is associated to a variant object given its path
      bool has_dependency(const std::string & path_) const;

      /// Return the dependency associated to a variant object given its path
      const variant_dependency & get_dependency(const std::string & path_) const;

      /// Return the handle on the dependency associated to a variant object given its path
      const dependency_ptr_type & get_dependency_ptr(const std::string & path_) const;

      /// Return the collection of dependencies
      const dependency_dict_type & get_dependencies() const;

      /// Return the table of dependers' paths addressed by their respective dependees' paths
      const dependers_table_type & get_dependers_per_dependee() const;

      /// \brief Restructured text formatting
      enum rst_flags {
        PRINT_RST_NO_TITLE = datatools::bit_mask::bit00,
        PRINT_RST_NO_SCOPE = datatools::bit_mask::bit01
      };

      /// Print documentation
      void print_rst(std::ostream & out_,
                     const uint32_t flags_ = 0,
                     const std::string & indent_ = "") const;

    protected:

      void _at_init();

      void _at_init_build_table_of_dependers_();

      void _at_reset();

    private:

      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; //!< The logging priority threshold
      bool _initialized_ = false; //!< Initialization flag
      const variant_repository * _repository_ = nullptr; //!< Handle to the parent repository
      const variant_registry   * _registry_ = nullptr; //!< Handle to the parent registry

      // Configuration:
      dependee_record_dict_type   _dependee_records_;   //!< Collection of dependees records (variants)
      dependency_record_dict_type _dependency_records_; //!< Collection of dependency records

      // Working:
      dependency_dict_type _dependencies_; //!< Dictionary of dependencies
      dependers_table_type _dependers_;    //!< Table of dependers

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_MODEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
