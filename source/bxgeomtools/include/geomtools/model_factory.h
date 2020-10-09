/// \file geomtools/model_factory.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 *
 * License:
 *
 * Description:
 *   Factory for geometry models
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MODEL_FACTORY_H
#define GEOMTOOLS_MODEL_FACTORY_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/logical_volume.h>
#include <geomtools/i_model.h>

namespace geomtools {

  // Forward declaration:
  class shape_factory;

  /// \brief The factory of geometry models
  class model_factory : public datatools::i_tree_dumpable
  {
  public:

    /// Return the default world label used for the top-level logical volume
    static const std::string & default_world_label();

    /// Validate a name for logical volume with respect to forseen GDML export
    static bool validate_name_for_gdml(const std::string & candidate_);

    /// Check if the factory is locked
    bool is_locked() const;

    /// Get the logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority
    void set_logging_priority(datatools::logger::priority);

    /// Check the logging debug flag
    bool is_debug() const;

    /// Check the logging trace flag
    bool is_trace() const;

    /// Set the logging debug flag
    void set_debug(bool);

    /// Return a non mutable reference to the embedded multi-container of parameters
    const datatools::multi_properties & get_mp() const;

    /// Return a mutable reference to the embedded multi-container of parameters
    datatools::multi_properties & grab_mp();

    /// Get a non-mutable collection of geometry models
    const models_col_type & get_models() const;

    /// Get a non-mutable collection of geometry logicals associated to models
    const logical_volume::dict_type & get_logicals() const;

    /// Constructor
    explicit model_factory(datatools::logger::priority lp_ = datatools::logger::PRIO_FATAL);

    /// Constructor
    model_factory(bool debug_, bool core_factory_verbose_ = false);

    /// Destructor
    ~model_factory() override;

    /// Load a geometry models definition file
    void load(const std::string & mprop_file_);

    /// Load a file that contains a list of geometry models definition filenames :
    void load_geom_list(const std::string & geom_list_file_);

    /// Lock the geometry model factory
    void lock();

    /// Unlock the geometry model factory
    void unlock();

    /// Reset the factory
    void reset();

    /// Add a property prefix to be preserved in logicals
    void add_property_prefix(const std::string & prefix_);

    /// Return the array of exported property prefixes
    const std::vector<std::string> & get_property_prefixes() const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Check if an external shape factory is referenced
    bool has_shape_factory() const;

    /// Set a reference to an external shape factory
    void set_shape_factory(shape_factory &);

    /// Return a reference to an external shape factory
    shape_factory & grab_shape_factory();

    /// Return a reference to an external shape factory
    const shape_factory & get_shape_factory() const;

    /// Print the list of geometry models
    static int print_list_of_models(const geomtools::model_factory & mf_,
                                    const std::string & command_,
                                    const std::string & argv_ = "",
                                    std::ostream & out_ = std::clog);

    /// Print the list of geometry logical volumes
    static int print_list_of_logicals(const geomtools::model_factory & mf_,
                                      const std::string & command_,
                                      const std::string & argv_ = "",
                                      std::ostream & out_ = std::clog);

  private:

    /// Basic construction of the model factory
    void _basic_construct_();

    /// Lock the geometry model factory
    void _lock_();

    /// Unlock the geometry model factory
    void _unlock_();

    /// Construct the virtual geometry hierarchy
    void _construct_();

  private:

    shape_factory *                _shape_factory_;    //!< Handle to an external shape factory
    i_model::factory_register_type _factory_register_; //!< Register of model factories
    bool                        _locked_;              //!< Lock flag
    datatools::logger::priority _logging_priority_;    //!< Logging priority threshold
    datatools::multi_properties _mp_;                  //!< Multi-container of parameters
    models_col_type             _models_;              //!< Dictionary of geometry models
    logical_volume::dict_type   _logicals_;            //!< Dictionary of geometry logical volumes
    std::vector<std::string>    _property_prefixes_;   //!< List of proprety prefixes to be preserved in logical volumes

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::model_factory)

#endif // GEOMTOOLS_MODEL_FACTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
