/// \file mctools/biasing/primary_event_bias.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-21
 * Last modified: 2014-12-17
 *
 * License:
 *
 * Description:
 *
 *   An algorithm that implements some biasing techniques at primary event generation
 *
 */

#ifndef MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H
#define MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H 1

// Standard library:
#include <string>
#include <map>
#include <set>

// Third party:
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools :
#include <geomtools/utils.h>

// This project:
#include <mctools/biasing/point_of_interest.h>

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
      : public datatools::i_tree_dumpable
    {
    public:

      /// Return the label for a killed event (not tracked at all)
      static const std::string & killed_event_label();

      /// Return the label for a truncated event (some particles are not tracked)
      static const std::string & truncated_event_label();

      /// Return the label for a normal event (all particles are tracked)
      static const std::string & normal_event_label();

      /// Return the property key for a normal event
      static const std::string & biased_event_status_key();

      /// Return the property flag to tag the master particle
      static const std::string & master_particle_flag();

      /// Return the property flag to skip the tracking of a particle
      static const std::string & dont_track_this_particle_flag();

      /// \brief Biasing modes
      enum bias_mode_type {
        BIAS_UNDEFINED            = 0, //!< Undefined bias
        BIAS_NONE                 = 1, //!< No bias
        BIAS_ONLY_MASTER_PARTICLE = 2, //!< Bias based on some criteria applied only to the master particle in the event
        BIAS_ALL_PARTICLES        = 3, //!< Bias based on some criteria applied to all candidate particles in the event
        BIAS_DEFAULT              = BIAS_ONLY_MASTER_PARTICLE //!< Default bias mode
      };

      /// \brief Status of a biased primary event
      enum biased_event_status {
        BES_UNDEFINED   = 0, //!< Undefined status
        BES_NORMAL      = 1, //!< Normal status (event is preserved as is and tracked)
        BES_TRUNCATED   = 2, //!< Normal status (event is preserved but some particles are not tracked)
        BES_KILLED      = 3  //!< Event should be killed (not tracked at all)
      };

      /// \brief Biasing information associated to a biased primary event
      struct biasing_info {
      public:
        /// Default constructor
        biasing_info();

        /// Reset
        void reset();

        /// Check normal status
        bool is_normal() const;

        /// Check truncated status
        bool is_truncated() const;

        /// Check killed status
        bool is_killed() const;

        /// Set the status
        void set_status(biased_event_status);

        /// Return the status
        biased_event_status get_status() const;

      public:
        biased_event_status _status_; //!< Biasing status associated to the event
      };

      //! Record counters
      struct stat_record
      {
        stat_record();
        void reset();
      public:
        int total_counts;
        int killed_counts;
        int truncated_counts;
        int normal_counts;
      };

      /// Return the embedded statistics record
      const stat_record & get_stats() const;

      /// Reset the embedded statistics record
      void reset_stats();

      /// Check if a store file is used to record statistics record
      bool has_stats_store() const;

      /// Set the filename of a file used to record statistics record
      void set_stats_store(const std::string &);

      /// \brief Dictionary of points of interest
      struct poi_entry_type
      {
        point_of_interest poi; //!< Point of interest
        // std::list<point_of_interest> associated_pois; //!< Associated points of interest
      };

      static const int DEFAULT_PARTICLE_RANK =  0;
      static const int INVALID_PARTICLE_RANK = -1;
      static const int ANY_PARTICLE_RANK     = 0x7FFFFFFF;

      /// \brief Dictionary of points of interest
      // typedef std::map<std::string, point_of_interest> poi_dict_type;
      typedef std::map<std::string, poi_entry_type> poi_dict_type;

      /// Default constructor
      primary_event_bias();

      /// Destructor
      ~primary_event_bias() override;

      /// Set the geometry manager
      void set_geometry_manager(const geomtools::manager &);

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization
      void initialize_simple();

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

      /// Add the identifier of the master particle
      void add_particle_by_label(const std::string &);

      /// Add the identifier of the master particle
      void add_particle_by_type(int id_);

      // /// Add the identifier of the master particle
      // void add_particle_by_pdg_code(int code_);

      /// Set the minimum kinetic energy of the master particle
      void set_particle_minimum_energy(double kemin_);

      /// Set the maximum kinetic energy of the master particle
      void set_particle_maximum_energy(double kemax_);

      /// Set the rank of the master particle
      void set_master_particle_rank(int rank_);

      /// Set the track only flag of the master particle
      void set_track_only_master_particle(bool);

      /// Set the total minimum kinetic energy
      void set_total_minimum_energy(double);

      /// Set the total maximum kinetic energy
      void set_total_maximum_energy(double);

      /// Set the mapping name
      void set_mapping_name(const std::string &);

      /// Return the mapping name
      const std::string & get_mapping_name() const;

      /// Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Add a point of interest
      void add_point_of_interest(const std::string & poi_name_, const point_of_interest & poi_);

      /// Check point of interest
      bool has_point_of_interest(const std::string & poi_name_) const;

      /// Return a non mutable reference to a point of interest
      const poi_entry_type & get_point_of_interest(const std::string & poi_name_) const;

      /// Return a mutable reference to a point of interest
      poi_entry_type & grab_point_of_interest(const std::string & poi_name_);

      /// Bias the primary event
      void process(const geomtools::vector_3d & vertex_,
                   genbb::primary_event & event_,
                   biasing_info & info_);

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

      /// Draw
      void draw(const geomtools::vector_3d & vertex_,
                genbb::primary_event & event_) const;

      /// Saev the statistics record
      void store_stats();

    protected:

      /// Set default attribute values
      void _set_default();

      /// Build the list of candidate primary particles
      void _make_candidate_particles(genbb::primary_event &,
                                     std::vector<genbb::primary_particle *> &) const;

      /// Validate a particle with respect to its direction and the list of PoIs
      bool _validate_particle_direction(const genbb::primary_particle &,
                                        const geomtools::vector_3d &) const;

    private:

      bool                          _initialized_;      //!< Initialization flag
      datatools::logger::priority   _logging_;          //!< Logging priority threshold
      const geomtools::manager    * _geom_mgr_;         //!< Handle to the external geometry manager
      std::string         _mapping_name_;               //!< Mapping name for POI definition
      bias_mode_type      _bias_mode_;                  //!< Biasing mode
      std::set<int>       _particle_types_;             //!< Types of the candidate particles
      double              _particle_min_energy_;        //!< The minimum energy of the particle from the list of candidate particles
      double              _particle_max_energy_;        //!< The maximum energy of the particle from the list of candidate particles
      double              _total_min_energy_;           //!< The minimum total kinetic energy from the list of candidate particles
      double              _total_max_energy_;           //!< The maximum total kinetic energy from the list of candidate particles
      int                 _master_particle_rank_;       //!< Rank of the master particle from the list of candidate particles
      bool                _track_only_master_particle_; //!< Flag to track only the master particle and do not track other ones
      poi_dict_type       _pois_;                       //!< Dictionary of points of interest
      stat_record         _stats_;                      //!< Statistics record
      std::string         _stats_store_;                //!< Name of the file where to store statistics record

    };

  } // namespace biasing

} // namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::biasing::primary_event_bias)

#endif // MCTOOLS_BIASING_PRIMARY_EVENT_BIAS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
