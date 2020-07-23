/// \file mctools/step_hit_processor_factory.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-04
 * Last modified: 2014-04-30
 *
 * License:
 *
 * Description:
 *
 *   Step hit processor factory
 *
 */

#ifndef MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H
#define MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H 1

// Standard library:
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <set>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <mctools/base_step_hit_processor.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace mygsl {
  class rng;
}

namespace mctools {

  /// \brief The factory responsible of the step hit processors
  class step_hit_processor_factory
    : public datatools::i_tree_dumpable
  {
  public:

    typedef datatools::handle<base_step_hit_processor> processor_handle_type;

    /// \brief A processor entry in the factory's embedded dictionary
    struct processor_entry_type {
      std::string name;
      std::string type;
      datatools::properties config;
      processor_handle_type handle;
      virtual ~processor_entry_type();
      void reset();
    };
    typedef std::map<std::string, processor_entry_type>      processor_entry_dict_type;
    typedef std::map<std::string, base_step_hit_processor *> processor_dict_type;

    /// Default name for the output collection of detailed hits (a.k.a. "visu tracks")
    static const std::string & default_detailed_hit_collection();

    /// Property key for the set of output profiles a processor is associated to
    static const std::string & matching_output_profiles_key();

    /// Check the initialization flag
    bool is_initialized () const;

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

    /// Set the flag for instantiation of processor at load stage
    void set_instantiate_at_loading(bool);

    /// Check for a PRNG
    bool has_external_prng () const;

    /// Set the PRNG
    void set_external_prng (mygsl::rng & prng_);

    /// Get a mutable reference to the PRNG
    mygsl::rng & grab_external_prng();

    /// Set output profiles
    void set_output_profiles(const std::set<std::string> & output_profiles_);

    /// Reset output profiles
    void reset_output_profiles();

    /// Initialization method
    void initialize();

    /// Initialization method from a list of parameters
    void initialize(const datatools::properties & config_);

    /// Reset method
    void reset ();

    /// Constructor
    step_hit_processor_factory (datatools::logger::priority logging_ = datatools::logger::PRIO_WARNING);

    /// Destructor
    virtual ~step_hit_processor_factory ();

    // /// Returns the mutable dictionary of instantiated processors
    // processor_dict_type & grab_processors ();

    /// Returns the non mutable dictionary of instantiated processors.
    /// The factory must be initialized.
    const processor_dict_type & get_processors () const;

    /// Build the list of names of the processors managed by the factory
    void fetch_processor_names(std::vector<std::string> & vprocs_, bool only_instantiated_ = false) const;

    /// Create a processor object
    bool create(const std::string & name_,
                const std::string & type_,
                const datatools::properties & config_);

    /// Check if a processor exists given its name
    bool has_processor (const std::string & name_) const;

    /// Check if a processor is instantiated
    bool is_processor_instantiated(const std::string & name_) const;

    /// Check if a processor can be instantiated with respect to the setup (output profiles...)
    bool is_processor_instantiable(const std::string & name_) const;

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
    void load(const datatools::multi_properties & mprop_);

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

  protected:

    /// Instantiate a processor object
    base_step_hit_processor * _create(const std::string & name_);

    /// Initialization method
    void _initialize();

  private:

    base_step_hit_processor::factory_register_type _factory_register_; ///< The factory register for 'Step Hit Processor' classes

    bool                         _initialized_ = false;     ///< Initialized flag
    datatools::logger::priority  _logging_priority_;///< Logging priority threshold
    std::string                  _description_;     ///< Description
    datatools::service_manager * _service_manager_ = nullptr; ///< Service manager
    const geomtools::manager   * _geom_manager_ = nullptr;    ///< Geometry manager
    mygsl::rng                 * _external_prng_ = nullptr;   ///< External PRNG
    std::set<std::string>        _output_profiles_; ///< Set of activated output profile ids
    bool                         _instantiate_at_loading_ = false; ///< Flag to instantiate processors at load stage
    processor_entry_dict_type    _entries_;         ///< The main dictionnary of processor entries
    processor_dict_type          _processors_;      ///< Dictionnary of pointers to instantiated processors

  };


} // end of namespace mctools


/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::step_hit_processor_factory)

#endif // MCTOOLS_STEP_HIT_PROCESSOR_FACTORY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
