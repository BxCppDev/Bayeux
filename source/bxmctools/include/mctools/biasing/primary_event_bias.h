/// \file mctools/biasing/primary_event_bias.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-21
 * Last modified: 2014-11-24
 *
 * License:
 *
 * Description:
 *
 *   An algorithm that implements some biasing techniques at primary event generation
 *
 * History:
 *
 */

#ifndef MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H
#define MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
#include <datatools/properties.h>

// This project:
// #include <mctools/biasing/point_of_interest.h>

namespace genbb {
  // Forward declarations:
  class primary_event;
  class primary_particle;
}

namespace geomtools {
  // Forward declaration:
  class manager;
}

namespace mctools {

  namespace biasing {

    // Forward declaration:
    class primary_generator;

    /// \brief Biasing algorithm used at primary event generation
    class primary_event_bias
    {
    public:

      /// Return the property flag to kill the event
      static const std::string & kill_this_event_flag();

      /// Return the property flag to tag the master particle
      static const std::string & master_particle_flag();

      /// Return the property flag to skip the tracking of a particle
      static const std::string & dont_track_this_particle_flag();

      /// \brief Biasing modes
      enum bias_mode_type {
        BIAS_UNDEFINED            = 0, //!< Undefined bias
        BIAS_NONE                 = 1, //!< No bias
        BIAS_ONLY_MASTER_PARTICLE = 2, //!< Bias based  on some criteria applied only on the master particle in the event
        BIAS_ALL_PARTICLES        = 3  //!< Bias based on some criteria applied to all particles in the event
      };

      /// \brief Status of a biased primary event
      enum biased_event_status {
        BES_UNDEFINED = 0, //!< Normal status (event is preserved as is)
        BES_NORMAL    = 1, //!< Normal status (event is preserved as is)
        BES_KILLED    = 2, //!< Event should be killed (not tracked)
        BES_WEIGHTED  = 3, //!< Event should be tracked but is given a biasing weight
        BES_TRANSFORMED = 4, //!< Event should be partially tracked but has been transformed
      };

      /// \brief Biasing information associated to a biased primary event
      struct biasing_info {
      public:
        /// Default constructor
        biasing_info();
        /// Reset
        void reset();
        /// Check unbiased status
        bool is_unbiased() const;
        /// Check biased status
        bool is_biased() const;
        /// Check killed status
        bool is_killed() const;
        /// Check weigthed status
        bool is_weighted() const;
        /// Set the status
        void set_status(biased_event_status);
        /// Return the status
        biased_event_status get_status() const;
        /// Set the weight
        void set_weight(double);
        /// Return the weight
        double get_weight() const;
      public:
        biased_event_status _status_; //!< Biasing status associated to the event
        double _weight_; //!< Weight associated to the event
      };

      // /// \brief Dictionary of points of interest
      // std::map<std::string, point_of_interest> poi_dict_type;

      /// Default constructor
      primary_event_bias();

      /// Destructor
      virtual ~primary_event_bias();

      /// Set the geometry manager
      void set_geometry_manager(const geomtools::manager &);

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization
      void initialize(const datatools::properties & config_);

      /// Reset
      void reset();

      /// Set the bias mode
      void set_bias_mode(bias_mode_type);

      /// Return the bias mode
      bias_mode_type get_bias_mode() const;

      /// Check no bias mode
      bool is_using_no_bias() const;

      /// Check mode for using a master particle
      bool is_using_master_particle() const;

      /// Check mode for using all particles
      bool is_using_all_particles() const;

      /// Set the identifier of the master particle
      void set_master_particle_by_label(const std::string &);

      /// Set the identifier of the master particle
      void set_master_particle_by_type(int id_);

      /// Set the identifier of the master particle
      void set_master_particle_by_pdg_code(int code_);

      /// Set the minimum kinetic energy of the master particle
      void set_master_particle_minimum_energy(double kemin_);

      /// Set the rank of the master particle
      void set_master_particle_rank(int rank_);

      /// Set the track only flag of the master particle
      void set_master_particle_track_only(bool);

      /// Set the total minimum kinetic energy
      void set_total_minimum_energy(double);

      /// Set the total maximum kinetic energy
      void set_total_maximum_energy(double);

      /// Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      // /// Add a target
      // void add_target(const std::string & target_name_,
      //                 const geomtools::vector_3 & p_,
      //                 double radius_,
      //                 bool avoid_ = false);

      /// Bias the primary event
      void process(genbb::primary_event &, biasing_info &);

    protected:

      /// Set default attribute values
      void _set_default();

      /// Select the primary particle
      genbb::primary_particle * _select_master_particle(genbb::primary_event &) const;


    private:

      bool                          _initialized_; //!< Initialization flag
      datatools::logger::priority   _logging_;     //!< Logging priority threshold
      const geomtools::manager    * _geom_mgr_;    //!< Handle to the external geometry manager
      bias_mode_type      _bias_mode_;                  //!< Biasing mode
      int                 _master_particle_type_;       //!< Identifier of the master particle
      double              _master_particle_min_energy_; //!< The minimum energy of the master particle from the list of candidate particles
      int                 _master_particle_rank_;       //!< Rank of the master particle from the list of candidate particles
      bool                _master_particle_track_only_; //!< Flag to track only the master particle and discard other ones
      double              _total_min_energy_;           //!< The minimum total kinetic energy
      double              _total_max_energy_;           //!< The maximum total kinetic energy
      // poi_dict_type    _pois_;                 //!< Dictionary of points of interest

    };

  } // namespace biasing

} // namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::biasing::primary_event_bias)

#endif // MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
