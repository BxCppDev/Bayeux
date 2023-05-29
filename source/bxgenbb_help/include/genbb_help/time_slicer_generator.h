/// \file genbb_help/time_slicer_generator.h
/* Author(s) :  Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2018-04-09
 * Last modified: 2018-04-09
 *
 * License:
 * Copyright 2018 F. Mauger
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
 *   clusterize primary particles in special time slices.
 *
 */

#ifndef GENBB_HELP_TIME_SLICER_GENERATOR_H
#define GENBB_HELP_TIME_SLICER_GENERATOR_H 1

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

  /// \brief Time slicer generator
  ///
  /// This generator uses the output primary event from another particle generator and classifies
  /// its primary particles in two sets of particles falling in subsequent time slices.
  ///
  /// \code
  ///       Event #0                                                           Event #1
  ///         primary event with many particles along time
  ///   ----[o--o-oo---o--o---------o-o----o-----------o-o--o--------o]--------[-o-oo------o-/ /---> time
  /// \endcode
  ///
  /// The result consists in several primary event instances:
  /// - the "prompt" event contains particles with time in [0,time_threshold[,
  /// - the "delayed" event contains particles with time in [time_threshold,time_cut[,
  /// - particles with time in [time_cut,+infinity[ are dropped.
  ///
  /// \code
  ///         Event #0.0          Event #0.1                                   Event #1.0
  ///         prompt particles   delayed particles   ignored particles          prompt particles...
  ///   ----[o--o-oo---o--o---][----o-o----o------][---o-o--o--------o]--------[-o-oo------o-/ /---> time
  ///                    time_threshold        time_cut
  /// \endcode
  ///
  class time_slicer_generator
    : public i_genbb
  {
  public:

    //! Event slice mode
    enum slice_mode_type {
      SM_UNDEF              = 0, //!< Undefined event time slice mode
      SM_PROMPT_EVENT_ONLY  = 1, //!< Select only the prompt event (particles in the prompt time interval)
      SM_DELAYED_EVENT_ONLY = 2, //!< Select only the delayed event (particles in the delayed time interval)
      SM_BOTH_EVENTS        = 3  //!< Select both prompt and delayed events (particles in both time intervals)
    };

    bool has_mode() const;

    void set_mode(const slice_mode_type);

    slice_mode_type get_mode() const;

    bool has_generator() const;

    void set_generator(i_genbb &);

    i_genbb & grab_generator();

    const i_genbb & get_generator() const;

    bool has_time_threshold() const;

    double get_time_threshold() const;

    void set_time_threshold(const double t_);

    bool has_time_cut() const;

    double get_time_cut() const;

    void set_time_cut(const double t_);

    void set_record_original_event_id(bool flag_);

    bool is_record_original_event_id() const;

    /// Check if the generator accepts an external PRNG
    bool can_external_random() const override;

    /// Constructor
    time_slicer_generator();

    /// Destructor
    ~time_slicer_generator() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Main initialization interface method
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

    void _at_init_();

    void _at_reset_();

  private:

    // Management:
    bool _initialized_ = false; //!< Initialization flag

    // Configuration:
    i_genbb * _pg_ = nullptr; //!< Reference to a wrapped particle generator
    double _time_threshold_;  //!< Time threshold used to slice the set of primary particles
    double _time_cut_;        //!< Time cut above whom particles are ignored
    slice_mode_type _mode_ = SM_UNDEF; //!< Slicing mode
    bool _record_original_event_id_ = true; //!< Flag to record the original event ID in the auxiliaries
 
    // Working data:
    primary_event _buffer_event_;
    std::size_t   _original_event_counter_ = 0;

    GENBB_PG_REGISTRATION_INTERFACE(time_slicer_generator)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::time_slicer_generator)

#endif // GENBB_HELP_TIME_SLICER_GENERATOR_H

// Local Variables: --
// mode: c++ --
// End: --
