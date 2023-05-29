/// \file genbb_help/poi_generator.h
/* Author(s) :  Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-05-29
 * Last modified: 2023-05-29
 *
 * License:
 * Copyright 2023 F. Mauger
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
 *   A generator of GENBB-like event that uses another generator and
 *   select only generated events woth a specific particle/energy topology.
 *
 */

#ifndef GENBB_HELP_POI_GENERATOR_H
#define GENBB_HELP_POI_GENERATOR_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>

// This project:
#include <genbb_help/i_genbb.h>
#include <genbb_help/primary_event.h>

namespace genbb {

  /// \brief A event generator wrapper which select a specific event topology from another generator
  ///
  class poi_generator
    : public i_genbb
  {
  public:

  public:

    struct poi_info_type 
    {
      int type = (int) primary_particle::PARTICLE_UNDEFINED;
      double min_energy = 0.0;
      double max_energy = std::numeric_limits<double>::infinity();
    };

    typedef std::vector<poi_info_type> poi_cut_type;
 
    bool has_generator() const;

    void set_generator(i_genbb &);

    i_genbb & grab_generator();

    const i_genbb & get_generator() const;

    std::size_t get_poi_size() const;

    poi_info_type get_poi_info(std::size_t poi_index_) const;

    void set_record_original_event_id(bool flag_);

    bool is_record_original_event_id() const;

    std::size_t get_original_event_counter() const;

    std::size_t get_event_counter() const;

    /// Check if the generator accepts an external PRNG
    bool can_external_random() const override;

    /// Constructor
    poi_generator();

    /// Destructor
    ~poi_generator() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Main initialization interface method
    ///
    /// Example:
    ///
    /// Request at least one electron with 1 MeV <= E <= 10 MeV and one gamme with E > 500 keV
    ///
    /// poi.number_of_particules : integer = 2
    /// poi.particle_0.type       : string = "electron"
    /// poi.particle_0.min_energy : real as energy = 1.0 MeV
    /// poi.particle_0.max_energy : real as energy = 10.0 MeV
    /// poi.particle_1.type       : string = "gamma"
    /// poi.particle_1.min_energy : real as energy = 0.5 MeV
    ///
    void initialize(const datatools::properties & setup_,
		    datatools::service_manager & service_manager_,
		    detail::pg_dict_type & dictionary_) override;

    /// Reset
    void reset() override;

    /// Check if a next primary event is available
    bool has_next() override;

  protected:

    /// Main generation function
    void _load_next(primary_event & event_,
		    bool compute_classification_ = true) override;

  private:

    void _search_for_next_event_();

    bool _select_(primary_event & event_) const;

    void _at_init_();

    void _at_reset_();
   
  private:

    // Management:
    bool _initialized_ = false; //!< Initialization flag

    // Configuration:
    i_genbb * _pg_ = nullptr; //!< Reference to a wrapped particle generator
    poi_cut_type _poi_cut_;
    bool _record_original_event_id_ = true; //!< Flag to record the original event ID in the auxiliaries
 
    // Working data:
    primary_event _buffer_event_;
    std::size_t   _original_event_counter_ = 0;
    std::size_t   _event_counter_ = 0;

    GENBB_PG_REGISTRATION_INTERFACE(poi_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::poi_generator)

#endif // GENBB_HELP_POI_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --
