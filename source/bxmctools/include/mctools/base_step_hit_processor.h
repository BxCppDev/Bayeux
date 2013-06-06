// -*- mode: c++ ; -*-
/* base_step_hit_processor.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-03-08
 *
 * License:
 *
 * Description:
 *
 *   Abstract MC base step hit processor.
 *
 * History:
 *
 */

#ifndef MCTOOLS_BASE_STEP_HIT_PROCESSOR_H_
#define MCTOOLS_BASE_STEP_HIT_PROCESSOR_H_ 1

#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <vector>

#include <datatools/handle_pool.h>
#include <datatools/factory_macros.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

#include <mygsl/rng.h>

#include <geomtools/manager.h>

#include <mctools/base_step_hit.h>
#include <mctools/simulated_data.h>

namespace datatools {
  class service_manager;
}

namespace mctools {

  /** Base step hit processor. The 'process' pure abstract method must be
   *  implemented.
   */
  class base_step_hit_processor
    : public datatools::i_tree_dumpable
  {
  public:

    typedef std::vector<base_step_hit *>          step_hit_ptr_collection_type;
    typedef datatools::handle_pool<base_step_hit> pool_type;

    static const size_t DEFAULT_POOL_CAPACITY = 100;

    bool is_debug () const;

    void set_debug (bool);

    void set_pool (pool_type &);

    bool has_pool () const;

    pool_type & get_pool () const;

    bool using_private_pool () const;

    void setup_private_pool (size_t a_capacity = DEFAULT_POOL_CAPACITY);

    void add_new_hit (simulated_data::hit_handle_collection_type & the_hits);

    const std::string & get_name () const;

    void set_name (const std::string &);

    base_step_hit_processor ();

    virtual ~base_step_hit_processor ();

    const std::string & get_hit_category () const;

    void set_hit_category (const std::string & a_hc);

    const std::string & get_sensitive_category () const;

    void set_sensitive_category (const std::string & a_sc);

    bool has_geom_manager () const;

    void set_geom_manager (const geomtools::manager & a_gmgr);

    const geomtools::manager & get_geom_manager () const;

    const datatools::properties & get_auxiliaries () const;

    datatools::properties & grab_auxiliaries ();

    virtual bool accept_external_rng () const;

    virtual void set_external_rng (mygsl::rng & a_rng);

    virtual void initialize (const datatools::properties & a_config);

    /// Main setup routine:
    virtual void initialize (const datatools::properties & a_config,
                             datatools::service_manager & a_service_mgr);

    /** High level routine:
     * Given the input 'the_base_step_hits' collection of pointers to MC step hits,
     * this method adds the proper list of 'hits' in the 'a_sim_data'
     * instance. This method uses one of both 'process' virtual methods below depending on the
     * type of output collection.
     */
    void process (const step_hit_ptr_collection_type & the_base_step_hits,
                  simulated_data & a_sim_data);

    /** main processing routine:
     * Given the input 'the_base_step_hits' collection of pointers to simulated step hits,
     * this method builds the output 'the_hits' collection, typically by clusterization
     * of original step hits.
     */
    virtual void process (const step_hit_ptr_collection_type         & the_base_step_hits,
                          simulated_data::hit_handle_collection_type & the_hits) = 0;

    virtual void process (const step_hit_ptr_collection_type  & the_base_step_hits,
                          simulated_data::hit_collection_type & the_plain_hits);

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

  protected:

    /*
    /// Main hidden algorithm
    virtual void _process(const i_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                          simulated_data::hit_handle_collection_type * the_gg_hits,
                          simulated_data::hit_collection_type * the_plain_gg_hits);
    */

  protected:

    datatools::logger::priority      _logging_priority; /// Logging priority threshold
    bool                             _debug; /// Debug flag
    std::string                      _name;  /// Name of the step hit processor
    std::string                      _hit_category; /** The name of the category
                                                     * of hits the hit processor
                                                     * accumulates
                                                     */
    std::string                      _sensitive_category; /** The name of the category
                                                           * of sensitive detector
                                                           * the hit processor is
                                                           * attached with
                                                           */
    const geomtools::manager       * _geom_manager; /// The geometry manager
    datatools::properties            _auxiliaries;  /// Some auxiliary properties
    pool_type                      * _private_pool; /// A private embeded pool of hits
    bool                             _pool_owner;   /// Pool of hits ownership flag
    pool_type                      * _pool;         /// Reference to an external pool of hits

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_step_hit_processor);

  };

} // end of namespace mctools


#include <mctools/detail/step_hit_processor_macros.h>

namespace mctools {

  /** A step hit processor that pushes the simulated base step hits
   *  as is in an output list, just cleaning the 'user data' pointer.
   */
  MCTOOLS_STEP_HIT_PROCESSOR_CLASS_DECLARE(push_all_step_hit_processor)
  {
  public:

    push_all_step_hit_processor ();

    virtual ~push_all_step_hit_processor ();

    //! Main setup routine
    MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_DECLARE();

    void set_visu_highlighted_hits (bool);

    bool are_visu_highlighted_hits () const;

    bool is_record_mc_step_hit_processor () const;

    void set_record_mc_step_hit_processor (bool);

  private:

    bool _visu_highlighted_hits_; /** Flag to mark MC step hits with a special
                                   *  boolean property ("visu.highlighted_hit")
                                   */
    bool _record_mc_step_hit_processor_; /** Flag to mark MC step hits with the name of the processor
                                          */
    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE (push_all_step_hit_processor);

  };


  /** A step hit processor that does nothing with the available step hits
   *  and let the output list empty.
   */
  MCTOOLS_STEP_HIT_PROCESSOR_CLASS_DECLARE(kill_all_step_hit_processor)
  {
  public:

    kill_all_step_hit_processor ();

    virtual ~kill_all_step_hit_processor ();

    MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_DECLARE();

    //! Main processing routine :
    MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_DECLARE();

    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE (kill_all_step_hit_processor);

  };

} // end of namespace mctools

#endif // MCTOOLS_BASE_STEP_HIT_PROCESSOR_H_

// end of base_step_hit_processor.h
