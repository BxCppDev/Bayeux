/// \file genvtx/i_vertex_generator.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 *             Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2014-07-15
 *
 * License:
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GENVTX_I_VERTEX_GENERATOR_H
#define GENVTX_I_VERTEX_GENERATOR_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Boost
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
// - Bayeux/datatools
#include <datatools/factory_macros.h>
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>
// - Bayeux/geomtools
#include <geomtools/utils.h>
#include <geomtools/detail/model_tools.h>

// This project:
#include <genvtx/detail/vg_tools.h>
#include <genvtx/utils.h>

namespace datatools {
  class properties;
  class service_manager;
}

namespace geomtools {
  class manager;
}

namespace mygsl {
  class rng;
}

namespace genvtx {

  // Forward declaration:
  class vertex_validation;

  /// \brief The base interface class for all vertex generator classes
  class i_vertex_generator : private boost::noncopyable,
                             public datatools::i_tree_dumpable
  {
  public:

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool debug_);

    /// Check if a name is defined
    bool has_name() const;

    /// Return the name
    const std::string & get_name() const;

    /// Set the name
    void set_name(const std::string &);

    /// Check if the generator has a geometry service's label
    bool has_geo_label() const;

    /// Set the geometry service's label
    void set_geo_label(const std::string & geo_label_);

    /// Return the geometry service's label
    const std::string & get_geo_label() const;

    /// Check if the generator has a geometry setup requirement
    bool has_geom_setup_requirement() const;

    /// Set has a geometry setup requirement
    void set_geom_setup_requirement(const std::string & geom_setup_requirement_);

    /// Return has a geometry setup requirement
    std::string get_geom_setup_requirement() const;

    /// Check the geometry setup requirement with respect to a geometry manager
    void check_geom_setup_requirement(const geomtools::manager * gmgr_) const;

    /// Check is an external PRNG is used
    bool has_external_prng() const;

    /// Set vertex validation support flag
    void set_vertex_validation_support(bool);

    /// Check if vertex validation is supported
    virtual bool is_vertex_validation_supported() const;

    /// Check vertex validation
    bool has_vertex_validation() const;

    /// Set the reference to an external PRNG
    void set_external_prng(mygsl::rng & prng_);

    /// Check if a PRNG is available
    bool has_prng() const;

    /// Return a mutable reference to the available PRNG
    mygsl::rng & grab_prng();

    /// Check if a geometry manager is attached to the vertex generator
    bool has_geom_manager() const;

    /// Set a geometry manager
    void set_geom_manager(const geomtools::manager & gmgr_);

    /// Get the geometry manager attached to the vertex generator
    const geomtools::manager & get_geom_manager() const;

    /// Check if some weight info is attached to the vertex generator
    bool has_total_weight() const;

    /// Get the weight info attached to the vertex generator
    const weight_info & get_total_weight() const;

    /// Check if the generator also generates a random time associated to the vertex
    bool is_time_generator() const;

    /// Check if another vertex is available
    virtual bool has_next_vertex() const;

    /// Wrapper method for vertex randomization
    void shoot_vertex(mygsl::rng & random_, geomtools::vector_3d & vertex_);

    /// Wrapper method for vertex randomization
    void shoot_vertex(geomtools::vector_3d & vertex_);

    /// Wrapper method for vertex randomization
    geomtools::vector_3d shoot_vertex(mygsl::rng & random_);

    /// Wrapper method for vertex/time randomization
    void shoot_vertex_and_time(mygsl::rng & random_, geomtools::vector_3d & vertex_, double & time_);

    /// Wrapper method for vertex/time randomization
    void shoot_vertex_and_time(geomtools::vector_3d & vertex_, double & time_);

    /// Simple initialization(no external resource)
    virtual void initialize_simple();

    /// Initialization from a container of properties
    virtual void initialize_standalone(const datatools::properties & setup_);

    /// Initialization from a container of properties and a service manager
    virtual void initialize_with_service_only(const datatools::properties & setup_,
                                              datatools::service_manager & service_manager_);

    /// Initialization from a container of properties, a service manager and a dictionnary of vertex generators
    virtual void initialize_with_dictionary_only(const datatools::properties & setup_,
                                                 vg_dict_type & dictionary_);

    /// Constructor
    i_vertex_generator();

    /// Destructor
    ~i_vertex_generator() override;

    /// Check initialization status
    virtual bool is_initialized() const = 0;

    /// Main initialization interface method
    virtual void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            vg_dict_type & dictionary_) = 0;

    /// Reset method
    virtual void reset() = 0;

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
    static void ocd_support(datatools::object_configuration_description &, const std::string & label_ = "");

  protected:

    /// Set the time generator flag
    void _set_time_generator(bool);

    /// Internal initialization
    void _initialize(const datatools::properties & setup_,
                     datatools::service_manager & service_manager_);

    /// Internal reset
    void _reset();

    /// Return a mutable total weight information
    weight_info & _grab_total_weight();

    /// Set a total weight information
    void _set_total_weight(const weight_info & a_info);

    void _initialize_basics(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_);

    void _initialize_geo_manager(const datatools::properties & setup_,
                                 datatools::service_manager & service_manager_);

    void _initialize_vertex_validation(const datatools::properties & setup_,
                                       datatools::service_manager & service_manager_);

    /// Return a reference to the embedded mutable vertex validation
    vertex_validation & _grab_vertex_validation();

    /// Main vertex randomization interface method
    virtual void _shoot_vertex(mygsl::rng & random_,
                               geomtools::vector_3d & vertex_);

    /// Main vertex randomization interface method (default: throw exception)
    virtual void _shoot_vertex_and_time(mygsl::rng & random_,
                                        geomtools::vector_3d & vertex_,
                                        double & time_);

  private:

    /// Vertex randomization interface method
    void _shoot_vertex_(mygsl::rng & random_, geomtools::vector_3d & vertex_);

    /// Vertex/time randomization interface method
    void _shoot_vertex_and_time_(mygsl::rng & random_, geomtools::vector_3d & vertex_, double & time_);

  protected:

    datatools::logger::priority  _logging_priority; //!< Logging priority threshold

  private:

    std::string                  _name_;         //!< The name of the vertex generator
    bool                         _time_generator_; //!< The generator also generates a (decay) time associated to the vertex
    std::string                  _geo_label_;    //!< The label of the Geometry Service
    std::string                  _geom_setup_requirement_; //!< The requirement ont the requested geometry setup (label and version)
    const ::geomtools::manager * _geom_manager_; //!< Handle to the geometry manager
    weight_info                  _total_weight_; //!< Weighting data
    mygsl::rng                 * _external_prng_; //!< Handle to an external PRNG
    bool                         _vertex_validation_support_; //!< Flag for vertex validation support
    boost::scoped_ptr<vertex_validation> _vertex_validation_; //!< Handle to the vertex validation

    // Factory declaration :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_vertex_generator)

  };

} // end of namespace genvtx

#include <genvtx/vg_macros.h>

#endif // GENVTX_I_VERTEX_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
