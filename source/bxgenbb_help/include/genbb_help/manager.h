/// \file genbb_help/manager.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-02-24
 * Last modified :
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Copyright (C) 2012 Ben Morgan <Ben.Morgan@warwick.ac.uk>
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
 *
 * Description:
 *
 *   A particle generator manager.
 *
 * History:
 *
 */
#ifndef GENBB_HELP_MANAGER_H
#define GENBB_HELP_MANAGER_H 1

// Standard Library
#include <string>
#include <sstream>
#include <typeinfo>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// Datatools
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>
#include <datatools/logger.h>

// Mygsl
#include <mygsl/rng.h> // Pseudo random number generator

// genbb_help
#include <genbb_help/detail/pg_tools.h>
#include <genbb_help/i_genbb.h>

namespace datatools {
  class properties;
  class multi_properties;
  class service_manager;
}

/// Top-level namespace of the Bayeux/genbb_help module library
namespace genbb {

  /*! \brief GENBB particle manager management class
   *
   */
  class manager
    : public ::datatools::i_tree_dumpable
  {
  public:
    enum flag_type {
      BLANK              = 0,
      NO_PRELOAD         = datatools::bit_mask::bit00,
      FORCE_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01
    };

  public:

    bool has_service_manager() const;

    void set_service_manager(datatools::service_manager & service_mgr_);

    void reset_service();

    /// Constructor
    manager(datatools::logger::priority p_ = datatools::logger::PRIO_WARNING, int flags_ = 0);

    /// Destructor
    ~manager() override;

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool debug_ = true);

    /// Check the use of some external PRNG
    bool has_external_prng() const;

    /// Set the external PRNG
    void set_external_prng(mygsl::rng &);

    /// Reset the external PRNG
    void reset_external_prng();

    /// Return a mutable reference to the external PRNG
    mygsl::rng & grab_external_prng();

    /// Return a non-mutable reference to the external PRNG
    const mygsl::rng & get_external_prng() const;

    /// Return a mutable reference to the embedded PRNG
    mygsl::rng & grab_embedded_prng();

    /// Return a non-mutable reference to the embedded PRNG
    const mygsl::rng & get_embedded_prng() const;

    /// Set the seed of the embedded PRNG
    void set_embedded_prng_seed(int seed_);

    /// Return a mutable reference to the active PRNG
    mygsl::rng & grab_prng();

    /// Return a non-mutable reference to the active PRNG
    const mygsl::rng & get_prng() const;

    /// Check is a default generator is set
    bool has_default_generator() const;

    /// Set the default generator name
    void set_default_generator(const std::string &);

    /// Return the default generator name
    const std::string & get_default_generator() const;

    /// Check the initialization flag
    bool is_initialized() const;

    /// Initialize the manager from a container of properties
    void initialize(const datatools::properties & config_);

    /// Reset the manager
    void reset();

    /// Check if the factory has a given type
    bool has_pg_type(const std::string & id_) const;

    /// Register a given type in the factory
    template <class PgClass>
    void register_pg_type(const std::string & id_);

    /// Unregister a given type from the factory
    void unregister_pg_type(const std::string & id_);

    /**  @param name_ The name of the PG to be checked
     *   @return true if the manager hosts the PG requested by name
     */
    bool has(const std::string & name_) const;

    /**  @param name_ The name of the PG to be checked
     *   @return true if the PG is initialized
     */
    bool is_initialized(const std::string & name_) const;

    /**  Check if a given PG can be removed
     *   @param name_ The name of the PG to be completely removed
     */
    bool can_drop(const std::string & name_) const;

    /**  @param name_ The name of the PG to be completely removed
     */
    void drop(const std::string & name_);

    /**  @param name_ The name of the PG to be reset
     */
    void reset(const std::string & name_);

    /**  @param name_ The name of the PG to be reset
     *   @return the registration ID of the PG
     */
    const std::string & get_id(const std::string & name_) const;

    /**  @param name_ The name of the PG to be returned
     *   @return a non-mutable reference to the PG
     */
    const i_genbb & get(const std::string & name_) const;

    /**  @param name_ The name of the PG to be returned
     *   @return a mutable reference to the PG
     */
    i_genbb & grab(const std::string & name_);

    void load(const std::string & name_,
              const std::string & id_,
              const datatools::properties & config_);

    void load(const datatools::multi_properties & config_);

    void dump_particle_generators(std::ostream &      out_ = std::clog,
                                  const std::string & title_  = "",
                                  const std::string & indent_ = "") const;

    void print_particle_generators(std::ostream &      out_ = std::clog,
                                   const std::string & title_  = "",
                                   const std::string & indent_ = "",
                                   const std::string & mode_ = "") const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    const datatools::factory_register<i_genbb> & get_factory_register() const;

    datatools::factory_register<i_genbb> & grab_factory_register();

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority p_);

  protected:

    // Note: move to private ASAP

    void _load_pg(const std::string & name_,
                  const std::string & id_,
                  const datatools::properties & config_);

    void _preload_global_dict();

    void _create_pg(detail::pg_entry_type & entry_);

    void _initialize_pg(detail::pg_entry_type & entry_);

    void _reset_pg(detail::pg_entry_type & entry_);

  private:

    /// Set the factory preload flag
    void _set_preload_(bool preload_);

  private:

    datatools::service_manager * _service_mgr_ = nullptr; ///< Handle to a service manager

    bool         _initialized_;                     ///< Initialization flag
    datatools::logger::priority _logging_priority_; ///< Logging priority threshold
    bool         _preload_;                         ///< Factory preload flag
    bool         _force_initialization_at_load_;    ///< Flag for triggering PG initialization at load (rather than first use)
    std::string   _default_generator_; ///< Default generator

    mygsl::rng * _external_prng_;      ///< Handle to an external PRNG (supersedes the embedded)
    int          _embedded_prng_seed_; ///< Embedded PRNG seed
    mygsl::rng   _embedded_prng_;      ///< Embedded PRNG

    datatools::factory_register<i_genbb> _factory_register_;    ///< Particle generator factory
    detail::pg_dict_type                 _particle_generators_; ///< Dictionary of particle generators

  };


  template <class PgClass>
  void manager::register_pg_type(const std::string & id_)
  {
    _factory_register_.registration(id_, boost::factory<PgClass*>());
  }

}  // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::manager)

#endif // GENBB_HELP_MANAGER_H

// Local Variables: --
// mode: c++ --
// End: --
