// -*- mode: c++ ; -*-
/* calorimeter_step_hit_processor.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-03-30
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
 * History:
 *
 */

#ifndef MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H_
#define MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H_ 1

#include <string>

#include <boost/cstdint.hpp>

#include <datatools/ioutils.h>

#include <geomtools/i_shape_3d.h>
#include <geomtools/smart_id_locator.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/placement.h>

#include <mctools/base_step_hit_processor.h>

namespace mctools {

  MCTOOLS_STEP_HIT_PROCESSOR_CLASS_DECLARE(calorimeter_step_hit_processor)
  {
  public:

    const std::string & get_mapping_category() const;

    void set_mapping_category(const std::string & sc_);

    const std::vector<int> & get_mapping_category_any_addresses() const;

    /// Constructor
    calorimeter_step_hit_processor();

    virtual ~calorimeter_step_hit_processor();

    //! Main setup routine
    MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_DECLARE();

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
                   bool inherit_ = false) const;

  protected:

    /// Main non-public algorithm
    void _process(const base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                  simulated_data::hit_handle_collection_type * calo_hits_,
                  simulated_data::hit_collection_type * plain_calo_hits_);

  private:

    double _scintillation_cluster_time_range_;  /** the time range for step hit
                                                 * clusterization:
                                                 * typical value is 1 ns
                                                 */

    double _scintillation_cluster_space_range_; /** the space range for clusterization:
                                                 * typical value is 1 cm
                                                 */

    std::string _mapping_category_; /** the name of the mapping
                                     * category of volumes
                                     * that should be considered
                                     * as a sensitive volume
                                     */

    std::vector<int> _mapping_category_any_addresses_; /** List of subaddress
                                                        * of the geometry category marked
                                                        * as 'any' for matching and saving MC hits;
                                                        * Used in case of partioned sensitive
                                                        * volumes.
                                                        */
    // internals:
    const geomtools::mapping * _mapping_; /** The reference geometry ID mapping */

    const geomtools::id_mgr::categories_by_name_col_type * _categories_; /// Handle to a dictionary of categories keyed by name

    uint32_t _calo_block_type_; /// the unique ID of the geometry category

    geomtools::smart_id_locator _calo_block_locator_; /** a locator to
                                                       * compute the geometry
                                                       * ID of the detector
                                                       * block some hit
                                                       * lies in.
                                                       */
    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(calorimeter_step_hit_processor);

  };

} // end of namespace mctools

#endif // MCTOOLS_CALORIMETER_STEP_HIT_PROCESSOR_H_

// end of calorimeter_step_hit_processor.h
