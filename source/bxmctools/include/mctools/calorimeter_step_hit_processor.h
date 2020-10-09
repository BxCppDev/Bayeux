/// \file mctools/calorimeter_step_hit_processor.h
/* Author(s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2014-04-23
 *
 * License:
 *
 * Description:
 *
 *  A basic processor of simulated step hits in some plastic scintillator
 *  volume.
 *
 *  This algorithm clusterizes step calorimeter hits (from a
 *  simulation program) and build new hits that cumulate the
 *  physics of original MC step hits (particle type, geometry ID,
 *  energy deposit, timing...)
 *
 *  CAUTION: this processor can manage only one geometry mapping category
 *  so care should be taken to attach only one geometry model (with the proper
 *  mapping category) to this processor.
 *
 *  QUESTION:
 *  Should delta rays energy deposit along the track of an alpha particle
 *  be clusterized within the parent alpha clusterized hit (quenching effect) ?
 *
 */

#ifndef MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H
#define MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
// - Bayeux/geomtools:
#include <geomtools/i_shape_3d.h>
#include <geomtools/smart_id_locator.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/placement.h>

// This project:
#include <mctools/base_step_hit_processor.h>

namespace mctools {

  class calorimeter_step_hit_processor
    : public base_step_hit_processor
  {
  public:

    /// Find the Gid of the calorimeter block at a given position
    virtual bool locate_calorimeter_block(const geomtools::vector_3d & position_,
                                          geomtools::geom_id & gid_) const;

    /// Return the geometry mapping category
    const std::string & get_mapping_category() const;

    /// Set the geometry mapping category
    void set_mapping_category(const std::string & sc_);

    const std::vector<int> & get_mapping_category_any_addresses() const;

    /// Constructor
    calorimeter_step_hit_processor();

    /// Destructor
    ~calorimeter_step_hit_processor() override;

    /// Main setup routine
    void initialize(const ::datatools::properties & config_,
                            ::datatools::service_manager & service_mgr_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_handle_collection_type & handle_hits_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_collection_type & plain_hits_) override;

    /** Check if a step hit in a candidate for clusterization within
     * the proposed scintillation hit
     */
    bool match_scintillation_hit(const base_step_hit & scintillation_hit_,
                                 const base_step_hit & step_hit_) const;

    /// Merge 'compatible' scintillator hit clusters.
    void merge_scintillation_hits(simulated_data::hit_handle_collection_type & scintillation_hits_);

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "" ,
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    void _init(const ::datatools::properties & config_,
               ::datatools::service_manager & service_mgr_);

    /// Main non-public algorithm
    void _process(const base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                  simulated_data::hit_handle_collection_type * calo_hits_,
                  simulated_data::hit_collection_type * plain_calo_hits_);

  private:

    /** the time range for step hit
     * clusterization:
     * typical value is 1 ns
     */
    double _scintillation_cluster_time_range_;

    /** the space range for clusterization:
     * typical value is 1 cm
     */
    double _scintillation_cluster_space_range_;

    /** the name of the mapping
     * category of volumes
     * that should be considered
     * as a sensitive volume
     */
    std::string _mapping_category_;

    /** List of subaddress
     * of the geometry category marked
     * as 'any' for matching and saving MC hits;
     * Used in case of partioned sensitive
     * volumes.
     */
    std::vector<int> _mapping_category_any_addresses_;

    // internals:
    const geomtools::mapping * _mapping_; ///< The reference geometry ID mapping

    const geomtools::id_mgr::categories_by_name_col_type * _categories_; ///< Handle to a dictionary of categories keyed by name

    uint32_t _calo_block_type_; ///< the unique ID of the geometry category

    /** a locator to
     * compute the geometry
     * ID of the detector
     * block where some hit
     * lies in.
     */
    geomtools::smart_id_locator _calo_block_locator_;

    bool _alpha_quenching_; ///< Flag to take into account quenching of alpha particle at low energy

    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(calorimeter_step_hit_processor)

  };

} // end of namespace mctools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::calorimeter_step_hit_processor)

#endif // MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
