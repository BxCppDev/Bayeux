/// \file mctools/base_step_hit_processor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-03-08
 *
 * License:
 *
 * Description:
 *
 *   Abstract MC base step hit processor.
 *
 */

#ifndef MCTOOLS_BASE_STEP_HIT_PROCESSOR_H
#define MCTOOLS_BASE_STEP_HIT_PROCESSOR_H 1

// Standard library:
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle_pool.h>
#include <datatools/factory_macros.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>

// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project:
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

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool);

    /// Set the pool of hits
    void set_pool(pool_type &);

    /// Check for a pool of hits
    bool has_pool() const;

    /// Return a reference to a mutable pool of hits
    pool_type & get_pool() const;

    bool using_private_pool() const;

    void setup_private_pool(size_t capacity_ = DEFAULT_POOL_CAPACITY);

    void add_new_hit(simulated_data::hit_handle_collection_type & hits_);

    const std::string & get_name() const;

    void set_name(const std::string &);

    /// Default constructor
    base_step_hit_processor();

    /// Destructor
    ~base_step_hit_processor() override;

    const std::string & get_hit_category() const;

    void set_hit_category(const std::string & hc_);

    const std::string & get_sensitive_category() const;

    void set_sensitive_category(const std::string & sc_);

    bool has_geom_manager() const;

    void set_geom_manager(const geomtools::manager & gmgr_);

    const geomtools::manager & get_geom_manager() const;

    const datatools::properties & get_auxiliaries() const;

    datatools::properties & grab_auxiliaries();

    virtual bool accept_external_rng() const;

    virtual void set_external_rng(mygsl::rng & rng_);

    /// Initialization from a set of configuration properties
    void initialize(const datatools::properties & config_);

    /// Main setup routine:
    virtual void initialize(const datatools::properties & config_,
                            datatools::service_manager & service_mgr_);

    /** High level routine:
     * Given the input 'the_base_step_hits' collection of pointers to MC step hits,
     * this method adds the proper list of 'hits' in the 'a_sim_data'
     * instance. This method uses one of both 'process' virtual methods below depending on the
     * type of output collection.
     */
    void process(const step_hit_ptr_collection_type & base_step_hits_,
                 simulated_data & sim_data_);

    /** Main processing routine:
     *  Given the input 'the_base_step_hits' collection of pointers to simulated step hits,
     *  this method builds the output 'the_hits' collection, typically by clusterization
     *  of original step hits.
     */
    virtual void process(const step_hit_ptr_collection_type         & base_step_hits_,
                         simulated_data::hit_handle_collection_type & hits_) = 0;

    virtual void process(const step_hit_ptr_collection_type  & base_step_hit_s,
                         simulated_data::hit_collection_type & plain_hits_);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    void _initialize(const datatools::properties & config_,
                     datatools::service_manager & service_mgr_);

    /*
   /// Main hidden algorithm
   virtual void _process(const i_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
   simulated_data::hit_handle_collection_type * the_gg_hits,
   simulated_data::hit_collection_type * the_plain_gg_hits);
    */

  protected:

    datatools::logger::priority      _logging_priority; ///< Logging priority threshold
    std::string                      _name;             ///< Name of the step hit processor
    std::string                      _hit_category; /**< The name of the category
                                                     * of hits the hit processor
                                                     * accumulates
                                                     */
    std::string                      _sensitive_category; /**< The name of the category
                                                           * of sensitive detector
                                                           * the hit processor is
                                                           * attached with
                                                           */
    const geomtools::manager       * _geom_manager = nullptr; ///< The geometry manager
    datatools::properties            _auxiliaries;            ///< Some auxiliary properties
    pool_type                      * _private_pool = nullptr; ///< A private embedded pool of hits
    bool                             _pool_owner;             ///< Pool of hits ownership flag
    pool_type                      * _pool = nullptr;         ///< Reference to an external pool of hits

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_step_hit_processor)

  };

} // end of namespace mctools


#include <mctools/detail/step_hit_processor_macros.h>

namespace mctools {

  /** A step hit processor that pushes the simulated base step hits
   *  as is in an output list, just cleaning the 'user data' pointer.
   */
  class push_all_step_hit_processor
    : public base_step_hit_processor
  {
  public:

    /// Constructor
    push_all_step_hit_processor();

    /// Destructor
    ~push_all_step_hit_processor() override;

    /// Main setup routine
    void initialize(const ::datatools::properties & config_,
                            ::datatools::service_manager & service_mgr_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_handle_collection_type & handle_hits_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_collection_type & plain_hits_) override;

    void set_visu_highlighted_hits(bool);

    bool are_visu_highlighted_hits() const;

    bool is_record_mc_step_hit_processor() const;

    void set_record_mc_step_hit_processor(bool);

  private:

    bool _visu_highlighted_hits_; /** Flag to mark MC step hits with a special
                                   *  boolean property ("visu.highlighted_hit")
                                   */
    bool _record_mc_step_hit_processor_; /** Flag to mark MC step hits with the name of the processor
                                          */
    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(push_all_step_hit_processor)

  };


  /** A step hit processor that does nothing with the available step hits
   *  and let the output list empty.
   */
  class kill_all_step_hit_processor
    : public base_step_hit_processor
  {
  public:

    /// Constructor
    kill_all_step_hit_processor();

    /// Destructor
    ~kill_all_step_hit_processor() override;

    /// Main setup routine
    void initialize(const ::datatools::properties & config_,
                            ::datatools::service_manager & service_mgr_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_handle_collection_type & handle_hits_) override;

    /// Main processing routine :
    void process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & base_step_hits_,
                         ::mctools::simulated_data::hit_collection_type & plain_hits_) override;

    // Registration macro :
    MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(kill_all_step_hit_processor)

  };

} // end of namespace mctools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::push_all_step_hit_processor)
DOCD_CLASS_DECLARATION(mctools::kill_all_step_hit_processor)

#endif // MCTOOLS_BASE_STEP_HIT_PROCESSOR_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
