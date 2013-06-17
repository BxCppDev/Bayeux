// -*- mode: c++ ; -*-
/* i_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2012-04-09
 *
 * License:
 *
 * Description:
 *
 *   Interface of a geometry model
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_MODEL_H_
#define GEOMTOOLS_I_MODEL_H_ 1

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/logger.h>

#include <geomtools/logical_volume.h>
#include <geomtools/detail/model_tools.h>
#include <datatools/factory_macros.h>

namespace geomtools {

  class i_model : public datatools::i_tree_dumpable
  {
  public:

    /// \brief Constants parameters used by geometry models
    struct constants {
      constants ();
      static const constants & instance ();
      std::string SOLID_SUFFIX; //!< Special suffix for solid naming
      std::string LOGICAL_SUFFIX; //!< Special suffix for logical volume naming
      std::string PHYSICAL_SUFFIX; //!< Special suffix for physical volume naming
      std::string PHANTOM_SOLID_FLAG; //!< Special flag for phantom envelope solid
      std::string EXPORTED_PROPERTIES_PREFIXES_KEY; //!< Special key for exporting prefixed properties in logical volumes
    };

    typedef geomtools::models_col_type models_col_type;

    /// Check if the construction of the geometry model is completed
    bool is_constructed () const;

    /// Check if the geometry model is "phantom"
    bool is_phantom_solid () const;

    /// Check if the name of the geometry model exists
    bool has_name() const;

    /// Set the name of the geometry model
    void set_name (const std::string & name_);

    /// Get the name of the geometry model
    const std::string & get_name () const;

    /// Configuration parameters (should be get_)
    const datatools::properties & parameters () const;

    /// Configuration parameters (should be grab_)
    datatools::properties & parameters ();

    /// Constructor
    i_model (const std::string & dummy_ = "");

    /// Destructor
    virtual ~i_model ();

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    /// Get a non mutable reference to the embedded logical volume
    const geomtools::logical_volume & get_logical () const;

    /// Get a mutable reference to the embedded logical volume
    geomtools::logical_volume & grab_logical ();

    /// Method that constructs the geometry model
    virtual void construct (const std::string & name_,
                            const datatools::properties & setup_,
                            models_col_type * models_ = 0);

    /// Main virtual method that constructs the geometry model
    virtual void construct (const std::string & name_,
                            const datatools::properties & setup_,
                            const std::vector<std::string> & properties_prefixes_,
                            models_col_type * models_);

    /// Get the model ID
    virtual std::string get_model_id () const = 0;

    /// Get the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    /// Check for debug logging priority threshold
    bool is_debug () const;

    /// Force the debug logging priority threshold
    void set_debug (bool);

  protected:

    /// Set the phantom solid flag
    void _set_phantom_solid (bool);

    /// Common construction method
    void _common_construct (datatools::properties & setup_);

    /// Pre-construction method
    virtual void _pre_construct (datatools::properties & setup_, models_col_type * models_);

    /// Post-construction method
    virtual void _post_construct (datatools::properties & setup_, models_col_type * models_);

    /// The main construction method (abstract)
    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & setup_,
                                models_col_type * models_ = 0) = 0;

  protected:

    bool                        _phantom_solid;     //!< Special flag (not used yet)
    geomtools::logical_volume   _logical;           //!< Top logical volume attached to the model
    datatools::logger::priority _logging_priority;  //!< Logging priority threshold

  private:

    bool                  _constructed_; //!< Completed construction flag
    datatools::properties _parameters_;  //!< Definition parameters
    std::string           _name_;        //!< Name of the geometry model

  public:

    static std::string make_solid_name (const std::string & basename_);

    static std::string make_logical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_,
                                                           int i_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_,
                                                           int i_, int j_);

    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int nitems_);

    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int ncols_,
                                                  int nrows_);

    static std::string extract_label_from_physical_volume_name (const std::string & physical_volume_name_);

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_model);

  }; // class i_model

} // end of namespace geomtools

#include <geomtools/model_macros.h>

#endif // GEOMTOOLS_I_MODEL_H_

// end of i_model.h
