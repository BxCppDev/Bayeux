// -*- mode: c++ ; -*-
/* detector_construction.h
 * Author (s) :  Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *               François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-04
 * Last modified: 2013-06-15
 *
 * License:
 *
 * Description:
 *
 *   GEANT 4 detector construction interface
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_DETECTOR_CONSTRUCTION_H_
#define MCTOOLS_G4_DETECTOR_CONSTRUCTION_H_ 1

#include <string>
#include <map>
#include <vector>

#include <datatools/properties.h>
#include <datatools/multi_properties.h>

#include <mygsl/rng.h>

#include <mctools/step_hit_processor_factory.h>

#include <mctools/g4/loggable_support.h>

// G4 stuff:
#include <G4VUserDetectorConstruction.hh>

class G4UserLimits;
class G4VisAttributes;

namespace geomtools {
  class manager;
  class gdml_writer;
}

namespace emfield {
  class electromagnetic_field_manager;
  class base_electromagnetic_field;
  class emfield_geom_plugin;
}

namespace mctools {

  namespace g4 {

    class manager;
    class sensitive_detector;
    class magnetic_field;

    /// \brief The detector construction Geant4 interface class
    class detector_construction : public G4VUserDetectorConstruction,
                                  public loggable_support
    {
    public:

      typedef std::map<std::string, sensitive_detector *>      sensitive_detector_dict_type;
      typedef std::map<std::string, std::vector<std::string> > region_infos_dict_type;
      typedef mctools::step_hit_processor_factory              SHPF_type;

      /// Default control distance for particle tracking in magnetic field
      static const double DEFAULT_MISS_DISTANCE;

      /// Constructor
      detector_construction (manager & mgr_);

      /// Destructor
      virtual ~detector_construction ();

      /// Check initialization flag
      bool is_initialized () const;


      /// Return a non-mutable reference to the simulation manager
      const manager & get_manager () const;

      /// Return a mutable reference to the simulation manager
      manager & grab_manager ();

      /// Check for a referenced geometry manager
      bool has_geometry_manager() const;

      /// Set the reference to a geometry manager
      void set_geometry_manager (const geomtools::manager & gm_);

      /// Return a non-mutable reference to the geometry manager
      const geomtools::manager & get_geometry_manager () const;

      /// Check for a referenced magnetic field manager
      bool has_mag_field_manager () const;

      /// Set the reference to a magnetic field manager
      void set_mag_field_manager (const emfield::electromagnetic_field_manager &);

      /// Return a non-mutable reference to the magnetic field manager
      const emfield::electromagnetic_field_manager & get_mag_field_manager () const;

      /// Main initialization method
      void initialize (const datatools::properties & config_);

      /// Return a non-mutable reference to the collection of embeded sensitive detectors
      const sensitive_detector_dict_type & get_sensitive_detectors () const;

      /// Return a mutable reference to the embeded step hit processor factory
      SHPF_type & grab_step_hit_processor_factory ();

      void set_materials_geom_plugin_name (const std::string & mpn_);

      void set_emfield_geom_plugin_name (const std::string & fpn_);

      /// G4 interface
      G4VPhysicalVolume * Construct ();

      /** Generate the GDML file from the geometry manager */
      void write_tmp_gdml_file ();

    protected:

      /** This method automatically creates and setups G4 sensitive detectors
       * with associated collections of simulated step hits.
       *
       * Two sources of informations are used:
       *
       *  - the geometry model (using the model factory from the geometry manager)
       *    provides some associations between logical volumes and
       *    sensitive detector categories
       *    These rules for sensitive detector definition are considered
       *    as 'official' for they are implemented in the main
       *    geometry model.
       *
       *  - the step hit processor factory (aka SHPF) enables to
       *    add some additionnal (non-official) sensitive detectors
       *
       */
      void _construct_sensitive_detectors ();

      /** Construct regions */
      void _construct_regions ();

      /** Setup magnetic field */
      void _construct_magnetic_field ();

      /** This method automatically setup G4 visualization attributes
       * from the main geometry model.
       */
      void _set_visualization_attributes ();

      /** Setup user limits */
      void _set_user_limits ();

      /** Hidden construction method */
      virtual G4VPhysicalVolume * _g4_construct ();

    private:

      bool _initialized_;                        /// Initialization flag
      bool _abort_on_error_;                     /// Flag to force abortion on error while building the geometry setup

      //! Main Geant4 manager:
      manager * _g4_manager_;                    /// Handle to the Geant4 simulation manager

      //! Geometry manager:
      const geomtools::manager * _geom_manager_; /// Reference to the geometry manager

      //! Magnetic field manager:
      bool   _using_mag_field_;                  /// Flag to use the Geant4 magnetic field system
      std::string _emfield_geom_plugin_name_;    /// The name of the EM field manager geometry plugin
      const emfield::electromagnetic_field_manager * _mag_field_manager_; /// Reference to the EM field manager
      double _miss_distance_unit_;               /// Default miss distance length unit
      double _general_miss_distance_;            /// Default general miss distance
      datatools::properties _mag_field_aux_;     /// Auxiliary properties related to magnetic field

      //! User limits:
      bool                      _using_user_limits_;
      datatools::properties     _limits_config_;
      std::list<G4UserLimits *> _user_limits_col_;

      // GDML stuff:
      bool        _generate_gdml_file_;
      std::string _materials_geom_plugin_name_;
      std::string _gdml_file_dir_;
      std::string _gdml_schema_;
      bool        _gdml_validation_;
      std::string _gdml_filename_;

      // Visualization:
      bool   _using_vis_attributes_;
      std::map<std::string, G4VisAttributes *> _vis_attributes_;

      // Sensitive detectors:
      bool _using_sensitive_detectors_;
      sensitive_detector_dict_type _sensitive_detectors_;
      datatools::multi_properties _SD_params_;

      // Regions:
      bool _using_regions_;
      region_infos_dict_type _region_infos_;

      // Step hit processor factory stuff (from mctools::core::step_hit_processor_factory) :
      std::string        _SHPF_config_;
      SHPF_type          _SHPF_;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_DETECTOR_CONSTRUCTION_H_

// end of detector_construction.h
