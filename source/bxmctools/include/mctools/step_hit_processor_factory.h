// -*- mode: c++ ; -*-
/* step_hit_processor_factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-04
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   Step hit processor factory
 *
 * History:
 *
 */

#ifndef MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H_
#define MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H_ 1

#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>

#include <mctools/base_step_hit_processor.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace mygsl {
  class rng;
}

namespace mctools {

  class step_hit_processor_factory
    : public datatools::i_tree_dumpable
  {
  public:

    typedef datatools::handle<base_step_hit_processor>       processor_handle_type;
    typedef std::map<std::string, processor_handle_type>     processor_handle_dict_type;
    typedef std::map<std::string, base_step_hit_processor *> processor_dict_type;

  public:

    /// Check the initialization flag
    bool is_initialized () const;

    /// Check the debug flag
    bool is_debug () const;

    /// Set the debug flag
    void set_debug (bool);

    /// Returns the description
    const std::string & get_description () const;

    /// Set the description
    void set_description (const std::string & description_);

    /// Check for an existing geometry manager
    bool has_geometry_manager () const;

    /// Set the geometry manager
    void set_geometry_manager (const geomtools::manager & gmgr_);

    /// Return a non mutable reference to the geometry manager
    const geomtools::manager & get_geometry_manager () const;

    /// Check for an existing service manager
    bool has_service_manager () const;

    /// Set the service manager
    void set_service_manager (datatools::service_manager & smgr_);

    /// Return a mutable reference to the service manager
    datatools::service_manager & grab_service_manager ();

    /// Return a non mutable reference to the service manager
    const datatools::service_manager & get_service_manager () const;

    /// Check for a PRNG
    bool has_external_prng () const;

    /// Set the PRNG
    void set_external_prng (mygsl::rng & prng_);

    /// Get a mutable reference to the PRNG
    mygsl::rng & grab_external_prng();

    /// Initialization method
    void initialize(const datatools::properties & config_);

    /// Reset method
    void reset ();

    /// Constructor
    step_hit_processor_factory (bool debug_ = false);

    /// Destructor
    virtual ~step_hit_processor_factory ();

    /// Returns the mutable dictionary of instantiated processors
    processor_dict_type & grab_processors ();

    /// Returns the non mutable dictionary of instantiated processors
    const processor_dict_type & get_processors () const;

    /// Create a processor object
    base_step_hit_processor & create (const std::string & name_,
                                      const std::string & type_,
                                      const datatools::properties & config_);

    /// Check if a processor exists given its name
    bool has_processor (const std::string & name_);

    /// Returns a mutable reference to a given processor addressed by its by name
    base_step_hit_processor & grab_processor (const std::string & name_);

    /// Returns a non mutable reference to a given processor addressed by its by name
    const base_step_hit_processor & get_processor (const std::string & name_) const;

    /// Default print
    void dump (std::ostream & out_) const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    /// Load a configuration object
    void load (const datatools::multi_properties & mprop_);

    datatools::logger::priority  get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

  private:

    base_step_hit_processor::factory_register_type _factory_register_; /// The factory register for 'Step Hit Processor' classes

    bool                         _initialized_;     /// Initialized flag
    datatools::logger::priority  _logging_priority_;/// Logging priority threshold
    std::string                  _description_;     /// Description
    datatools::service_manager * _service_manager_; /// Service manager
    const geomtools::manager   * _geom_manager_;    /// Geometry manager
    mygsl::rng                 * _external_prng_;   /// External PRNG
    processor_handle_dict_type   _handles_;         /// Dictionnary of handles
    processor_dict_type          _processors_;      /// Dictionnary of pointers

  };


} // end of namespace mctools


/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::step_hit_processor_factory)

#endif // MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H_

// end of step_hit_processor_factory.h
