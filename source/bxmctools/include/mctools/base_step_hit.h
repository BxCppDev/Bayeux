/// \file mctools/base_step_hit.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2018-08-04
 *
 * License: GPL3
 *
 * Description:
 *
 *   Generic base step hit data model built from a Geant4 a sensitive detector
 *
 */

#ifndef MCTOOLS_BASE_STEP_HIT_H
#define MCTOOLS_BASE_STEP_HIT_H

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/base_hit.h>

// This project:
#include <mctools/utils.h>

namespace mctools {

  /// \brief The base class for all Monte-Carlo (MC) hit objects
  /**
   *   start
   *    o
   *     \
   *      \  particle track step
   *       \
   *        \
   *         \
   *          o
   *          stop
   *
   */
  class base_step_hit
    : public geomtools::base_hit
  {

  public:

    /// \brief Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      // Version 0:
      STORE_POSITION_START = datatools::bit_mask::bit03, //!< Serialization mask for the position start attribute
      STORE_POSITION_STOP  = datatools::bit_mask::bit04, //!< Serialization mask for the position stop attribute
      STORE_TIME_START     = datatools::bit_mask::bit05, //!< Serialization mask for the time start attribute
      STORE_TIME_STOP      = datatools::bit_mask::bit06, //!< Serialization mask for the time stop attribute
      STORE_MOMENTUM_START = datatools::bit_mask::bit07, //!< Serialization mask for the momentum start attribute
      STORE_MOMENTUM_STOP  = datatools::bit_mask::bit08, //!< Serialization mask for the momentum stop attribute
      STORE_ENERGY_DEPOSIT = datatools::bit_mask::bit09, //!< Serialization mask for the energy deposit attribute
      STORE_PARTICLE_NAME  = datatools::bit_mask::bit10, //!< Serialization mask for the particle name attribute
      
      // Added in version 1:
      STORE_BIASING_WEIGHT = datatools::bit_mask::bit11, //!< Serialization mask for the biasing weight attribute (BUG 2015-02-26: was datatools::bit_mask::bit20)
      
      // Added in version 2:
      STORE_KINETIC_ENERGY_START      = datatools::bit_mask::bit12, //!< Serialization mask for the start kinetic energy attribute
      STORE_KINETIC_ENERGY_STOP       = datatools::bit_mask::bit13, //!< Serialization mask for the stop kinetic energy attribute
      STORE_STEP_LENGTH               = datatools::bit_mask::bit14, //!< Serialization mask for the step length
      STORE_ENTERING_VOLUME_FLAG      = datatools::bit_mask::bit15, //!< Serialization mask for the entering volume flag
      STORE_LEAVING_VOLUME_FLAG       = datatools::bit_mask::bit16, //!< Serialization mask for the leaving volume flag
      STORE_CREATOR_PROCESS_NAME      = datatools::bit_mask::bit17, //!< Serialization mask for the creator process name
      STORE_PRIMARY_PARTICLE_FLAG     = datatools::bit_mask::bit18, //!< Serialization mask for the primary particle flag
      STORE_MAJOR_TRACK_FLAG          = datatools::bit_mask::bit19, //!< Serialization mask for the major track flag
      STORE_DELTA_RAY_FROM_ALPHA_FLAG = datatools::bit_mask::bit20, //!< Serialization mask for the delta-ray from alpha flag
      STORE_TRACK_ID                  = datatools::bit_mask::bit21, //!< Serialization mask for the track ID
      STORE_PARENT_TRACK_ID           = datatools::bit_mask::bit22, //!< Serialization mask for the parent track ID
      STORE_MATERIAL_NAME             = datatools::bit_mask::bit23, //!< Serialization mask for the material name
      STORE_SENSITIVE_CATEGORY        = datatools::bit_mask::bit24, //!< Serialization mask for the sensitive category
      STORE_G4_VOLUME_NAME            = datatools::bit_mask::bit25, //!< Serialization mask for the Geant4 volume name
      STORE_G4_VOLUME_COPY_NUMBER     = datatools::bit_mask::bit26, //!< Serialization mask for the Geant4 volume copy number
      STORE_HIT_PROCESSOR             = datatools::bit_mask::bit27, //!< Serialization mask for the step hit processor
      STORE_VISU_HIGHLIGHT_FLAG       = datatools::bit_mask::bit28, //!< Serialization mask for the visu highlight flag
      STORE_LAST                      = STORE_VISU_HIGHLIGHT_FLAG
    };

    // Extract specific properties from the 'auxiliaries' container:

    /* Track ID */

    /// Check if the hit records the track ID
    bool has_track_id() const;

    /// Get the track ID
    int get_track_id() const;

    /// Set the track ID
    void set_track_id(const int id_);

    /// Invalidate the track ID
    void invalidate_track_id();

    /* Parent track ID */

    /// Check if the hit records the parent track ID
    bool has_parent_track_id() const;

    /// Get the parent track ID
    int get_parent_track_id() const;

    /// Set the parent track ID
    void set_parent_track_id(const int id_);

    /// Invalidate the parent track ID
    void invalidate_parent_track_id();

    /* Position start */

    bool has_position_start() const;
    
    /// Get the start position of the hit (the coordinate system and units are arbitrary)
    const geomtools::vector_3d & get_position_start() const;

    /// Set the start position of the hit (the coordinate system and units are arbitrary)
    void set_position_start(const geomtools::vector_3d &);

    /// Reset/invalidate the start position of the hit
    void invalidate_position_start();

    /* Position stop */

    bool has_position_stop() const;
    
    /// Get the stop position of the hit (the coordinate system and units are arbitrary)
    const geomtools::vector_3d & get_position_stop() const;

    /// Set the stop position of the hit (the coordinate system and units are arbitrary)
    void set_position_stop(const geomtools::vector_3d &);

    /// Reset/invalidate the stop position of the hit
    void invalidate_position_stop();

    /* Momentum stop */

    bool has_momentum_stop() const;

    /// Get the stop momentum of the hit (units are arbitrary)
    const geomtools::vector_3d & get_momentum_stop() const;

    /// Set the stop momentum of the hit (units are arbitrary)
    void set_momentum_stop(const geomtools::vector_3d &);

    /// Reset/invalidate the stop momentum of the hit
    void invalidate_momentum_stop();

    /* Momentum start */
    
    bool has_momentum_start() const;

    /// Get the start momentum of the hit (units are arbitrary)
    const geomtools::vector_3d & get_momentum_start() const;

    /// Set the start momentum of the hit (units are arbitrary)
    void set_momentum_start(const geomtools::vector_3d &);

    /// Reset/invalidate the start momentum of the hit
    void invalidate_momentum_start();
 
    /* Time start */
    
    bool has_time_start() const;

    /// Get the start time of the hit (units are arbitrary)
    double get_time_start() const;

    /// Set the start time of the hit (units are arbitrary)
    void set_time_start(const double);

    /// Reset/invalidate the start time of the hit
    void invalidate_time_start();

    /* Time stop */
    
    bool has_time_stop() const;

    /// Get the stop time of the hit (units are arbitrary)
    double get_time_stop() const;

    /// Set the stop time of the hit (units are arbitrary)
    void set_time_stop(const double);

    /// Reset/invalidate the stop time of the hit
    void invalidate_time_stop();

    /* Energy deposit */
    
    bool has_energy_deposit() const;

    /// Get the energy deposit along the hit (units are arbitrary)
    double get_energy_deposit() const;

    /// Set the energy deposit along the hit (units are arbitrary)
    void set_energy_deposit(const double);

    /// Reset/invalidate the energy deposit along the hit
    void invalidate_energy_deposit();

    /* Particle name */
    bool has_particle_name() const;
    
    /// Get the particle name associated to the hit
    const std::string & get_particle_name() const;

    /// Set the particle name associated to the hit
    void set_particle_name(const std::string &);

    /// Reset/invalidate the particle name associated to the hit
    void invalidate_particle_name();

    /* Biasing weight */

    /// Check if a biasing weight is associated to the hit
    bool has_biasing_weight() const;

    /// Set the current biasing  weight of the particle track
    void set_biasing_weight(double);

    /// Return the current biasing  weight of the particle track
    double get_biasing_weight() const;

    /// Reset/invalidate the current biasing weight
    void invalidate_biasing_weight();

    /* kinetic energy start */

    bool         has_kinetic_energy_start() const;
    double       get_kinetic_energy_start() const;
    void         set_kinetic_energy_start(const double ke_);
    void         invalidate_kinetic_energy_start();
    
    /* kinetic energy stop */
    
    bool         has_kinetic_energy_stop() const;
    double       get_kinetic_energy_stop() const;
    void         set_kinetic_energy_stop(const double ke_);
    void         invalidate_kinetic_energy_stop();

    /* Step length */
    bool         has_step_length() const;
    double       get_step_length() const;
    void         set_step_length(const double);
    void         invalidate_step_length();

    /* Entering volume */
    bool         has_entering_volume() const;
    bool         is_entering_volume() const;
    void         set_entering_volume(const bool flag_);
    void         invalidate_entering_volume();
 
    /* Leaving volume */
    bool         has_leaving_volume() const;
    bool         is_leaving_volume() const;
    void         set_leaving_volume(const bool flag_);
    void         invalidate_leaving_volume();

    /* Creator process name */
    bool         has_creator_process_name() const;
    const std::string & get_creator_process_name() const;
    void         set_creator_process_name(const std::string &);
    void         invalidate_creator_process_name();

    /// Primary particle flag : check if the MC hit has been generated by a primary particle
    bool         has_primary_particle() const;
    bool         is_primary_particle() const;
    void         set_primary_particle(const bool flag_);
    void         invalidate_primary_particle();
  
    /* Major track */
    bool         has_major_track() const;
    bool         is_major_track() const;
    void         set_major_track(const bool flag_);
    void         invalidate_major_track();

    /* Delta-ray from alpha particle */
    bool         has_delta_ray_from_alpha() const;
    bool         is_delta_ray_from_alpha() const;
    void         set_delta_ray_from_alpha(const bool flag_);
    void         invalidate_delta_ray_from_alpha();

    /* Material */
    bool         has_material_name() const;
    const std::string & get_material_name() const;
    void         set_material_name(const std::string &);
    void         invalidate_material_name();
   
    /* Sensitive category */
    bool         has_sensitive_category() const;
    const std::string & get_sensitive_category() const;
    void         set_sensitive_category(const std::string &);
    void         invalidate_sensitive_category();
 
    /* Hit processor */
    bool         has_hit_processor() const;
    const std::string & get_hit_processor() const;
    void         set_hit_processor(const std::string &);
    void         invalidate_hit_processor();

    /* Geant4 volume name (debug) */
    bool         has_g4_volume_name() const;
    const std::string & get_g4_volume_name() const;
    void         set_g4_volume_name(const std::string &);
    void         invalidate_g4_volume_name();

    /* Geant4 volume copy number (debug) */
    bool         has_g4_volume_copy_number() const;
    int          get_g4_volume_copy_number() const;
    void         set_g4_volume_copy_number(const int);
    void         invalidate_g4_volume_copy_number();

    /* Visu highlight */
    bool         has_visu_highlight() const;
    bool         is_visu_highlight() const;
    void         set_visu_highlight(const bool flag_);
    void         invalidate_visu_highlight();

    /// Check if the hit has a valid internal structure
    /** We consider a base step hit valid if:
     * - a valid hit ID is set
     * - a valid geometry ID is set
     * - start time is set
     * - start position is set
     */
    bool is_valid() const override;

    /// Reset/invalidate the internal structure of the hit
    void invalidate() override;


    /// Default constructor
    base_step_hit() = default;

    // Destructor
    ~base_step_hit() override = default;

    // Copy constructor
    base_step_hit(const base_step_hit &) = default;

    // Copy assignment
    base_step_hit & operator=(const base_step_hit &) = default;

    // Move constructor
    base_step_hit(base_step_hit &&);

    // Move assignment
    base_step_hit & operator=(base_step_hit &&) = default;

    /// Reset/invalidate the internal structure of the hit
    void reset();

    /// Reset/invalidate the internal structure of the hit
    void clear() override;

    /// \deprecated Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;
    
    //!
    //! Supported options:
    //! \code
    //! {
    //!   "title"    : "My title: ",
    //!   "indent"   : "[debug] ",
    //!   "inherit"  : false,
    //!   "list_auxiliaries" : true
    //! }
    //! \endcode
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;
  
    /// Smart print (default behaviour/shortcut)
    void dump() const;

  private:
    // Attributes :

    // Original attributes in version 0:
    geomtools::vector_3d _position_start_ = geomtools::invalid_vector_3d(); //!< Start position : beginning of the tiny track segment (step)
    geomtools::vector_3d _position_stop_ = geomtools::invalid_vector_3d();  //!< Stop position  : end of the tiny track segment (step)
    double               _time_start_ = datatools::invalid_real();     //!< Start time at start position
    double               _time_stop_ = datatools::invalid_real();      //!< Stop time at stop position
    geomtools::vector_3d _momentum_start_ = geomtools::invalid_vector_3d(); //!< Momentum at start position
    geomtools::vector_3d _momentum_stop_ = geomtools::invalid_vector_3d();  //!< Momentum at stop position
    double               _energy_deposit_ = datatools::invalid_real(); //!< Energy deposit along the track segment (step)
    std::string          _particle_name_ = "";  //!< Name of the particle associated to the hit

    // Original attributes in version 1:
    double               _biasing_weight_ = datatools::invalid_real(); //!< The biasing weight of the particle track

    // Added attributes in version 2:
    double               _kinetic_energy_start_ = datatools::invalid_real();
    double               _kinetic_energy_stop_ = datatools::invalid_real();
    double               _step_length_ = datatools::invalid_real();
    bool                 _entering_volume_flag_ = false; 
    bool                 _leaving_volume_flag_ = false; 
    std::string          _creator_process_name_ = "";
    bool                 _primary_particle_flag_ = false; 
    bool                 _major_track_flag_ = false; 
    bool                 _delta_ray_from_alpha_flag_ = false; 
    int                  _track_id_ = -1;
    int                  _parent_track_id_ = -1;
    std::string          _material_name_ = "";
    std::string          _sensitive_category_ = "";
    std::string          _g4_volume_name_ = "";
    int                  _g4_volume_copy_number_ = -1;
    std::string          _hit_processor_ = "";
    bool                 _visu_highlight_ = false; 

    DATATOOLS_SERIALIZATION_DECLARATION()
    
    GEOMTOOLS_HIT_REGISTRATION_INTERFACE(base_step_hit)
    
#if MCTOOLS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI()
#endif

  };

} // end of namespace mctools

// #ifdef __clang__
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wunused-local-typedef"
// #endif
// #include <boost/serialization/export.hpp>
// #ifdef __clang__
// #pragma clang diagnostic pop
// #endif
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(mctools::base_step_hit, "mctools::base_step_hit")

#if MCTOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the mctools::base_step_hit class :
DR_CLASS_INIT(::mctools::base_step_hit)
#endif // MCTOOLS_WITH_REFLECTION

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(mctools::base_step_hit, 2)

#endif // MCTOOLS_BASE_STEP_HIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
