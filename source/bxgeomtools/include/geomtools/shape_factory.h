/// \file geomtools/shape_factory.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-03
 * Last modified: 2022-04-12
 *
 * License: GPL 3.0
 *
 * Description:
 *   Factory for shapes (3D objects)
 *
 */

#ifndef GEOMTOOLS_SHAPE_FACTORY_H
#define GEOMTOOLS_SHAPE_FACTORY_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <unordered_set>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/i_object_3d.h>

namespace geomtools {

  //! \brief The factory of 3D-shapes
  class shape_factory
    : public datatools::i_tree_dumpable
  {
  public:

    // typedef std::shared_ptr<i_object_3d> ptr_type;

    //! Default constructor
    shape_factory() = default;

    //! Destructor
    virtual ~shape_factory() override;

    //! Return a reference to a non mutable dictionary of shapes
    const i_object_3d::handle_dict_type & get_shapes() const;

    //! Create a new 3D object, store it in the local dictionary and return a shared pointer to it
    i_object_3d::handle_type create(const std::string & name_,
                                    const std::string & type_id_,
                                    const datatools::properties & config_);

    //! Create a new 3D object, store it in the local dictionary and return a reference to it
    const i_object_3d & create_reference(const std::string & name_,
                                         const std::string & type_id_,
                                         const datatools::properties & config_);

    //! Create a new 3D object and return a unique pointer to it
    std::unique_ptr<i_object_3d> create_unique_ptr(const std::string & type_id_,
                                                   const datatools::properties & config_);

    //! Check if a shape with given name and optional type exists
    bool has(const std::string & name_, const std::string & type_id_ = "") const;

    //! Get a reference to the const shape given its name
    const i_object_3d & get(const std::string & name_) const;

    //! Get a shared pointer to the const shape given its name
    i_object_3d::const_handle_type get_shared_ptr(const std::string & name_) const;

    //! Get a shared pointer to the const shape given its name
    i_object_3d::handle_type grab_shared_ptr(const std::string & name_);

    //! Build the list of shapes' names
    void keys(std::vector<std::string> & keys_) const;

    //! Build the list of shapes' names
    void keys(std::unordered_set<std::string> & keys_) const;

    //! Return the logging priority
    datatools::logger::priority get_logging_priority() const;

    //! Set the logging priority
    void set_logging_priority(datatools::logger::priority);

    //! Check initialization flag
    bool is_initialized() const;

    //! Simple initialization
    void initialize_simple();

    //! Initialization
    void initialize(const datatools::properties & config_);

    //! Reset
    void reset();

    //! Load a shapes definition file
    void load(const std::string & shapes_def_file_);

    //! Load a file that contains a list of shapes definition filenames :
    void load_list(const std::string & list_file_);

    //! Parse a container of properties and extract a set of shapes from it
    void parse_shapes(const datatools::properties & defs_);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const override;

  protected:

    //! Initialization action
    void _at_init();

    //! Create a new 3D object and return the unique pointer to it
    std::unique_ptr<i_object_3d> _create(const std::string & type_id_,
                                         const datatools::properties & config_);

  private:

    datatools::logger::priority _logging_priority_ = datatools::logger::PRIO_FATAL; //!< Logging priority threshold
    bool _initialized_ = false; //!< Initialization flag
    i_object_3d::factory_register_type _factory_register_; //!< Register of model factories
    i_object_3d::handle_dict_type      _shapes_; //!< Dictionary of shapes

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::shape_factory)

#endif // GEOMTOOLS_SHAPE_FACTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
