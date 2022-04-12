/// \file geomtools/i_object_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2022-04-12
 *
 * License: GPL 3.0
 *
 * Description:
 *
 *  Interface for object than can be described in a 3D reference frame
 *  and possibly drawn with some graphics renderer.
 *
 */

#ifndef GEOMTOOLS_I_OBJECT_3D_H
#define GEOMTOOLS_I_OBJECT_3D_H 1

/// Standard library:
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <memory>

// Third party:
// - Bayeux/datatools:
#include <datatools/ocd_macros.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward declaration:
  class box;

  /// \brief Mother abstract class for all 3D object classes
  class i_object_3d :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable
  {
  public:

    /// \brief Known dimensions
    enum dimensional_type {
      DIMENSIONAL_0 = 0, //!< No dimension object (example: a vertex)
      DIMENSIONAL_1 = 1, //!< One dimension object (example: a segment)
      DIMENSIONAL_2 = 2, //!< Two dimension object (example: a rectangle)
      DIMENSIONAL_3 = 3  //!< Three dimension object (example: a box)
    };

    /// A handle to a 3D object
    typedef std::shared_ptr<i_object_3d> handle_type;
    typedef std::shared_ptr<const i_object_3d> const_handle_type;
 
    /// \brief An entry that stores a 3D object
    class object_entry
    {
    public:

      /// Default constructor
      object_entry();

      /// Constructor
      object_entry(const std::string & name_,
                   const std::string & type_id_,
                   const datatools::properties & config_,
                   const handle_type & handle_);
        
      /// Destructor
      ~object_entry();

      /// Reset
      void reset();

      /// Set the name
      void set_name(const std::string &);

      /// Return the name of the object
      const std::string & get_name() const;

      /// Set the type identifier of the object
      void set_type_id(const std::string & type_id_);

      /// Return the type identifier of the object
      const std::string & get_type_id() const;

      /// Return the reference to the configuration
      const datatools::properties & get_config() const;

      // /// Return the reference to the configuration
      // datatools::properties & grab_config();

      /// Set the configuration of the object
      void set_config(const datatools::properties & config_);

      /// Check object
      bool has_object() const;

      /// Return the cosnt reference to the object
      const i_object_3d & get_object() const;

      /// Set object handle
      void set_handle(const handle_type & handle_);

      /// Return the non mutable object handle
      const_handle_type get_handle() const;

      /// Return the mutable object handle
      handle_type grab_handle();

    private:

      std::string           _name_;    //!< Object name
      std::string           _type_id_; //!< Object type identifier
      datatools::properties _config_;  //!< Configuration of the entry
      handle_type           _hobject_; //!< Handle to the 3D-object (if created)

    };

    /// Dictionary of handle of 3D object entries
    typedef std::unordered_map<std::string, object_entry> handle_dict_type;

    /// Return the dimension of the object
    virtual int get_dimensional() const = 0;

    /// Return the name of the shape
    virtual std::string get_shape_name() const = 0;

    /// Check the validity of the object
    virtual bool is_valid() const = 0;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    /// Check is a wires drawer is available
    bool has_wires_drawer() const;

    /// Set a wires drawer
    void set_wires_drawer(i_wires_3d_rendering & wires_drawer_);

    /// Reset the wires drawer
    void reset_wires_drawer();

    /// Return a wires drawer
    i_wires_3d_rendering & grab_wires_drawer();

    /// Return a wires drawer
    const i_wires_3d_rendering & get_wires_drawer() const;

    /// Return the distance tolerance
    double compute_tolerance(double tolerance_) const;

    /// Return the angular tolerance
    double compute_angular_tolerance(double angular_tolerance_) const;

    /// Return the distance tolerance
    double get_tolerance() const;

    /// Set the distance tolerance
    void set_tolerance(double tolerance_);

    /// Return the angular tolerance
    double get_angular_tolerance() const;

    /// Set the angular tolerance
    void set_angular_tolerance(double tolerance_);

    /// Check if the object is composite
    virtual bool is_composite() const;

    /// Default constructor
    i_object_3d();

    /// Constructor on length tolerance
    i_object_3d(double tolerance_);

    /// Constructor on length tolerance
    i_object_3d(double tolerance_, double angular_tolerance_);

    /// Copy constructor
    i_object_3d(const i_object_3d &);

    /// Assignment operator
    i_object_3d & operator=(const i_object_3d &);

    /// Destructor
    ~i_object_3d() override;

    /// Initialize the 3D object
    void initialize_simple();

    /// Initialize from properties and a dictionary of 3D-objects
    virtual void initialize(const datatools::properties &, const handle_dict_type * = nullptr);

    /// Reset
    virtual void reset();

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default values for attributes
    void _set_defaults();

    /// Initialize from properties
    void _initialize(const datatools::properties &, const handle_dict_type * = nullptr);

    /// Reset
    void _reset();

  private:

    // Parameters:
    datatools::logger::priority _logging_priority_ = datatools::logger::PRIO_FATAL; //!< Logging priority threshold
    double _tolerance_ = constants::get_default_tolerance();         //!< Length tolerance to check surface/curve belonging
    double _angular_tolerance_ = constants::get_default_angular_tolerance(); //!< Angular tolerance to check surface/curve belonging

    // Work:
    i_wires_3d_rendering * _wires_drawer_ = nullptr; //!< The handle on an external wires drawer object (may be used by some display tool, i.e. the gnuplot renderer)

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_object_3d)

  public:

    /// \brief Object 3D abstract getter class
    class i_getter
    {
    public:

      virtual ~i_getter();

      virtual const i_object_3d * get(const std::string & name_,
                                      const datatools::properties & params_) = 0;

      const i_object_3d * get(const std::string & name_);

    };

  };

} // end of namespace geomtools

#define GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ModelClassName)      \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::i_object_3d,ModelClassName) \
  /**/

#define GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ModelClassName,ModelClassId) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::geomtools::i_object_3d,ModelClassName,ModelClassId) \
  /**/

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_object_3d, 0)
*/

#endif // GEOMTOOLS_I_OBJECT_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
