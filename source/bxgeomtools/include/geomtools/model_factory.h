// -*- mode: c++ ; -*-
/* model_factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_MODEL_FACTORY_H_
#define GEOMTOOLS_MODEL_FACTORY_H_ 1

#include <iostream>
#include <string>
#include <map>

#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory.h>
#include <datatools/logger.h>

#include <geomtools/logical_volume.h>
#include <geomtools/i_model.h>


namespace geomtools {

  class model_factory : public datatools::i_tree_dumpable
  {
  public:

    static const std::string DEFAULT_WORLD_LABEL;

    /// Check if the factory is locked
    bool is_locked () const;

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

    /// Check the debug flag
    bool is_debug () const;

    bool is_trace () const;

    /// Set the debug flag
    void set_debug (bool);

    /// Get a non-mutable collection of geometry models
    const models_col_type & get_models () const;

    /// Get a non-mutable collection of geometry logicals associated to models
    const logical_volume::dict_type & get_logicals () const;

    /// Constructor
    explicit model_factory (datatools::logger::priority lp_ =
                            datatools::logger::PRIO_WARNING);

    /// Constructor
    model_factory (bool debug_, bool core_factory_verbose_ = false);

    /// Destructor
    virtual ~model_factory ();

    void load (const std::string & mprop_file_);

    // 2012-05-25 FM : add support for loading a file that contains a list of geometry filenames :
    void load_geom_list (const std::string & geom_list_file_);

    /// Lock the geometry model factory
    void lock ();

    /// Unlock the geometry model factory
    void unlock ();

    /// Reset the factory
    void reset ();

    /// Add a property prefix to be preserved in logicals
    void add_property_prefix(const std::string & prefix_, int = 1);

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    /// Print the list of geometry models
    static bool print_list_of_models(const geomtools::model_factory & mf_,
                                     std::ostream & out_ = std::clog,
                                     uint32_t options_ = 0);

  private:

    /// Basic construction of the model factory
    void _basic_construct_ ();

    /// Lock the geometry model factory
    void _lock_ ();

    /// Unlock the geometry model factory
    void _unlock_ ();

    /// Construct the virtual geometry hierarchy
    void _construct_ ();

  private:

    i_model::factory_register_type _factory_register_;
    bool _locked_;
    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    datatools::multi_properties _mp_;
    models_col_type             _models_;
    logical_volume::dict_type   _logicals_;
    std::map<std::string,int>   _property_prefixes_;

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::model_factory)

#endif // GEOMTOOLS_MODEL_FACTORY_H_

// end of model_factory.h
