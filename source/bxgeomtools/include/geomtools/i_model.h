/// \file geomtools/i_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2015-07-07
 *
 * License:
 *
 * Description:
 *
 *   Interface/base class for all geometry models
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_MODEL_H
#define GEOMTOOLS_I_MODEL_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost
#include <boost/scoped_ptr.hpp>

// Bayeux:
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/logger.h>
#include <datatools/factory_macros.h>
#include <datatools/ocd_macros.h>

// This library:
#include <geomtools/logical_volume.h>
#include <geomtools/detail/model_tools.h>
#include <geomtools/model_with_internal_mesh_tools.h>

namespace geomtools {

  // Forward declaration:
  class shape_factory;

  /// \brief The base class for geometry models
  class i_model
    : public datatools::i_tree_dumpable
  {
  public:

    // Constants parameters used by geometry models:
    static const std::string & model_suffix();
    static const std::string & solid_suffix();
    static const std::string & logical_suffix();
    static const std::string & physical_suffix();
    static const std::string & phantom_solid_flag();
    static const std::string & exported_properties_prefixes_key();

    /// \brief The collection of geometry models:
    typedef geomtools::model_bus_type models_col_type;
    
    model_with_internal_mesh_data & grab_meshes();

    const model_with_internal_mesh_data & get_meshes() const;

    /// Check if the construction of the geometry model is completed
    bool is_constructed() const;

    /// Check if the geometry model is "phantom"
    bool is_phantom_solid() const;

    /// Check if the name of the geometry model exists (it should after construction)
    bool has_name() const;

    /// Set the name of the geometry model
    void set_name(const std::string & name_);

    /// Get the name of the geometry model
    const std::string & get_name() const;

    /// Configuration parameters (should be get_)
    const datatools::properties & parameters() const;

    /// Configuration parameters (should be grab_)
    datatools::properties & parameters();

    /// Constructor
    i_model(const std::string & dummy_ = "");

    /// Destructor
    ~i_model() override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

    /// Get a non mutable reference to the embedded logical volume
    const geomtools::logical_volume & get_logical() const;

    /// Get a mutable reference to the embedded logical volume
    geomtools::logical_volume & grab_logical();

    /// Method that constructs the geometry model
    void construct(models_col_type * models_ = nullptr);

    /// Method that constructs the geometry model
    void construct(const std::string & name_,
                   models_col_type * models_ = nullptr);

    /// Method that constructs the geometry model
    void construct(const std::string & name_,
                   const datatools::properties & setup_,
                   models_col_type * models_ = nullptr);

    /// Main method that constructs the geometry model
    void construct(const std::string & name_,
                   const datatools::properties & setup_,
                   const std::vector<std::string> & properties_prefixes_,
                   models_col_type * models_);

    /// Method that destroys the geometry model
    void destroy(models_col_type * models_ = nullptr);

    /// Get the model ID
    virtual std::string get_model_id() const = 0;

    /// Get the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    /// Check for debug logging priority threshold
    bool is_debug() const;

    /// Force the debug logging priority threshold
    void set_debug(bool);

    /// OCD support
    static void init_ocd(datatools::object_configuration_description & ocd_);

    /// Check if an external shape factory is referenced
    bool has_shape_factory() const;

    /// Set a reference to an external shape factory
    void set_shape_factory(shape_factory &);

    /// Return a reference to an external shape factory
    shape_factory & grab_shape_factory();

    /// Return a reference to an external shape factory
    const shape_factory & get_shape_factory() const;

  protected:

    /// Set the phantom solid flag
    void _set_phantom_solid(bool);

    /// Mandatory pre construction
    void _mandatory_pre_construct(datatools::properties & setup_, models_col_type * models_);

    /// Pre-construction hook
    virtual void _pre_construct(datatools::properties & setup_, models_col_type * models_);

    /// The main construction hook (replace the former construction hook method)
    virtual void _at_construct(const datatools::properties & setup_,
                               models_col_type * models_ = nullptr);

    /// @deprecated Old/broken construction hook interface (a legacy design error)
    virtual void _at_construct(const std::string & name_,
                               const datatools::properties & setup_,
                               models_col_type * models_);

    /// Post-construction hook
    virtual void _post_construct(datatools::properties & setup_, models_col_type * models_);

    /// Mandatory post construction
    void _mandatory_post_construct(datatools::properties & setup_, models_col_type * models_);

    /// The main destruction hook
    virtual void _at_destroy(models_col_type * models_ = nullptr);

  protected:

    datatools::logger::priority _logging_priority; //!< Logging priority threshold
    bool                        _phantom_solid;    //!< Special flag (not used yet)
    geomtools::logical_volume   _logical;          //!< Top logical volume attached to the model

  private:

    bool                  _constructed_; //!< Completed construction flag
    datatools::properties _parameters_;  //!< Definition parameters
    std::string           _name_;        //!< Name of the geometry model
    model_with_internal_mesh_data _meshes_; //!< Internal meshes within the mother logical

    // Work:
    shape_factory *       _shape_factory_ = nullptr; //!< Handle to an external shape factory


  public:

    static std::string make_solid_name(const std::string & basename_);

    static std::string make_logical_volume_name(const std::string & basename_);

    static std::string make_physical_volume_name(const std::string & basename_);

    static std::string make_physical_volume_name_per_item(const std::string & basename_,
                                                          int i_);

    static std::string make_physical_volume_name_per_item(const std::string & basename_,
                                                          int i_, int j_);

    static std::string make_physical_volume_name(const std::string & basename_,
                                                 int nitems_);

    static std::string make_physical_volume_name(const std::string & basename_,
                                                 int ncols_,
                                                 int nrows_);

    static std::string extract_label_from_physical_volume_name(const std::string & physical_volume_name_);

    static std::string extract_basename_from_model_name(const std::string & model_name_);

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_model)

  }; // class i_model

} // end of namespace geomtools

#include <geomtools/model_macros.h>

#endif // GEOMTOOLS_I_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
