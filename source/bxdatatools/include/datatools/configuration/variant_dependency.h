/// \file datatools/configuration/variant_dependency.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-26
 * Last modified : 2016-10-26
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
 *   Description of long range dependencies between variant parameters
 *   and variants.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_H

// Standard library:
#include <string>
#include <map>
#include <memory>
#include <limits>
#include <set>

// This project (Bayeux/datatools):
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/configuration/variant_object_info.h>
#include <datatools/configuration/variant_dependency_utils.h>

namespace datatools {

  namespace configuration {

    class variant_repository;
    class variant_registry;
    class base_dependency_logic;

    /// \brief A variant dependency associated a depender variant object o a set of dependee variants.
    /// The object may be one of the following types:
    ///  - a variant registry,
    ///  - a variant,
    ///  - a group of parameter values,
    ///  - a specific parameter string value.
    /// An embedded logic object implements the criterion used to resolve the activation of the depender
    /// with respect to the dependees.
    /// Currently the only supported logic is the default AND(...) operator which implies that all dependees are active
    /// to activate the depender.
    ///
    /// General dependency scheme:
    /// \code
    ///                The logic object                     The collection of dependees
    ///                compute the final      dependee       addressed by slot numbers
    ///                 "enable" bit           slots
    ///                                          :
    ///                 unique   some            : +------------+
    ///                 output   input           v |            |
    ///                  port    ports       ,--[0]s Dependee#0 |
    ///                   :        :        /      | (variant)  |
    ///                   v        v       v       +------------+
    /// +------------+    +--------+      /        +------------+
    /// |            |    |        i[0]--'         |            |
    /// | Depender   e-<--o Logic  i[1]----<----[3]s Dependee#1 |
    /// |            |    |        i[2]--,         | (variant)  |
    /// +------------+    +--------+      \        +------------+
    ///              ^                     ^       +------------+
    ///              :                      \      |            |
    ///              :                       `--[9]s Dependee#2 |
    ///          enable bit                        | (variant)  |
    ///      associated to the depender            +------------+
    ///
    /// \endcode
    /// \see For the definition of depender/dependee terms: https://en.wiktionary.org/wiki/dependee
    class variant_dependency
      : public datatools::i_tree_dumpable
    {
    public:

      typedef std::map<unsigned int, variant_object_info> dependee_dict_type;

      /// Constructor
      variant_dependency(const variant_repository & repository_);

      /// Constructor
      variant_dependency(const variant_registry & registry_);

      /// Destructor
      ~variant_dependency() override;

      /// Check repository
      bool has_repository() const;

      /// Return the repository handle
      const variant_repository & get_repository() const;

      /// Check registry
      bool has_registry() const;

      /// Return the repository handle
      const variant_registry & get_registry() const;

      /// Check registry
      bool has_global_scope() const;

      /// Check registry
      bool has_local_scope() const;

      /// Check if the dependency model is local to a registry
      bool is_local() const;

      /// Check validity
      bool is_valid() const;

      /// Set the depender from its identifier
      void set_depender_by_path(const std::string & depender_name_, bool registry_scope_ = false);

      /// Set the depender
      void set_depender(const variant_object_info & depender_);

      /// Return the depender
      const variant_object_info & get_depender() const;

      /// Return the number of dependee slots
      std::size_t get_number_of_dependees() const;

      /// Associate a dependee from its identifier to a slot
      unsigned int add_dependee_by_path(const std::string & dependee_path_,
                                        const unsigned int slot_ = std::numeric_limits<unsigned int>::max());

      /// Associate a dependee to a slot
      unsigned int add_dependee(const variant_object_info & dependee_,
                                const unsigned int slot_ = std::numeric_limits<unsigned int>::max());

      /// Build set of dependee slots
      void build_list_of_dependee_slots(std::set<unsigned int> & slots_) const;

      /// Check if a dependee is set at given slot
      bool has_dependee(const unsigned int slot_) const;

      /// Check if a dependee is set with given path
      bool has_dependee_by_path(const std::string & dependee_path_) const;

      /// Return the dependee associated to a given slot
      const variant_object_info & get_dependee(const unsigned int slot_) const;

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Instantiate the embedded predicate:
      base_dependency_logic & create_logic(const std::string & guid_);

      /// Return the embedded predicate
      const base_dependency_logic & get_logic() const;

      /// Return the embedded predicate
      base_dependency_logic & grab_logic();

      /// Buil the logic from formula
      void build_logic_from_formula(const std::string & logic_expression_);

      /// Check if dependency is locked
      bool is_locked() const;

      /// Lock the dependency
      void lock();

      /// Check if the dependency is fulfilled
      bool operator()() const;

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(const datatools::logger::priority);

    protected:

      void _lock();

      void _check();

      /// Build and install a default logic object is no formula/expresion has been
      /// explicitely given.
      ///
      /// If there are several dependees, we force an AND operator:
      /// all dependees must be fulfilled to enable the dependency:
      /// \code
      ///                       +-------+      +----------+
      ///                    ,--o SLOT  |<==[0]s Dependee |
      /// +-------+         /   | logic |      +----------+
      /// |  AND  i[0]--<--'    +-------+
      /// o       |
      /// | logic i[1]--<--,
      /// +-------+         \   +-------+      +----------+
      ///                    `--o SLOT  |<==[7]s Dependee |
      ///                       | logic |      +----------+
      ///                       +-------+
      /// \endcode
      /// It there is only one dependee, then the embedded logic
      /// is a single SLOT logic connected to the unique dependee slot:
      /// \code
      ///
      ///
      ///
      /// +-------+
      /// | SLOT  |      +----------+
      /// o       |<==[1]s Dependee |
      /// | logic |      +----------+
      /// +-------+
      ///
      ///
      ///
      /// \endcode
      void _install_default_logic();

    private:

      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; //!< The logging priority threshold
      bool _locked_ = false;
      const variant_repository * _repository_ = nullptr; //!< The variant repository
      const variant_registry * _registry_ = nullptr;     //!< The variant registry
      variant_object_info _depender_;                    //!< The object which depends on other variants
      dependee_dict_type _dependees_;                    //!< The variant objects on which the dependee object depends on
      std::shared_ptr<base_dependency_logic> _logic_;    //!< Embedded predicate

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
