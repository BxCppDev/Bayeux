/// \file genbb_help/lorentz_boost_wrapper.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-12-22
 * Last modified: 2014-12-22
 *
 * License:
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
 * Description:
 *
 *   A generator wrapper that use another generator to
 *   shoot primary events but apply a Lorentz boost to
 *   them.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_LORENTZ_BOOST_WRAPPER_H
#define GENBB_HELP_LORENTZ_BOOST_WRAPPER_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/factory_macros.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/i_genbb.h>

namespace datatools {
  // Forward declarations:
  class properties;
  class service_manager;
}

namespace genbb {

  // Forward declaration:
  class primary_event;

  /// \brief Abstract mother class to fetch Lorentz Boost data
  class i_lorentz_boost_generator
  {
  public:
    /// Default constructor
    i_lorentz_boost_generator();

    /// Destructor
    virtual ~i_lorentz_boost_generator();

    /// Check if the generator can provide more Lorentz Boost information
    virtual bool has_next() = 0;

    /// Add metadata
    virtual void add_metadata(primary_event & pe_);

    /// Check initialization status
    virtual bool is_initialized() const = 0;

    /// Main initialization
    virtual void initialize(const datatools::properties &) = 0;

    /// Reset
    virtual void reset() = 0;

    /// Simple initialization
    void initialize_simple();

    /// Generate Lorentz Boost information
    void generate(geomtools::vector_3d & speed_, geomtools::vector_3d & vtx_);

    /// Generate Lorentz Boost information
    void generate(geomtools::vector_3d & speed_, geomtools::vector_3d & vtx_, double & time_);

    /// Factory method
    static i_lorentz_boost_generator * create(const std::string & class_id_,
                                              const datatools::properties & config_);

  protected:

    /// Generate Lorentz Boost information
    virtual void _generate(geomtools::vector_3d & speed_, geomtools::vector_3d & vtx_, double & time_) = 0;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_lorentz_boost_generator)

  };


  /// \brief Wrapper for another generator
  class lorentz_boost_wrapper : public i_genbb
  {
  public:

    /// Set the particle generator
    void set_generator(i_genbb &);

    /// Set the Lorentz boost generator
    void set_lorentz_boost_generator(i_lorentz_boost_generator & lbg_);

    /// Set the Lorentz boost generator
    void set_lorentz_boost_generator(i_lorentz_boost_generator * lbg_);

    /// Default constructor
    lorentz_boost_wrapper();

    /// Destructor
    ~lorentz_boost_wrapper() override;

    /// Main initialization interface method
    void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check if the generator has a next event
    bool has_next() override;

    /// Check initialization status
    bool is_initialized() const override;

  protected:

    /// Load next primary event from the generator algorithm
    void _load_next(primary_event & event_,
                            bool compute_classification_ = true) override;

  private:

    bool        _initialized_; //!< Initialization flag
    i_genbb *   _pg_;          //!< Handle to a primary particle generator
    i_lorentz_boost_generator * _lbg_; //!< Handle to a generator of Lorentz boost
    bool _lbg_owned_; //!< Flag to own the handle to the generator of Lorentz boost
    GENBB_PG_REGISTRATION_INTERFACE(lorentz_boost_wrapper)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::lorentz_boost_wrapper)

#endif // GENBB_HELP_LORENTZ_BOOST_WRAPPER_H

// Local Variables: --
// mode: c++ --
// End: --
