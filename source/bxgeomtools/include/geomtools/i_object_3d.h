// -*- mode: c++; -*-
/// \file geomtools/i_object_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *  Interface for object than can be described in a 3D reference frame
 *  and possibly drawn with some graphics renderer.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_OBJECT_3D_H
#define GEOMTOOLS_I_OBJECT_3D_H 1

/// Standard library:
#include <string>
#include <iostream>
#include <sstream>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
#include <datatools/ocd_macros.h>
#include <datatools/handle.h>

// This project:
#include <geomtools/utils.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward declaration:
  class box;

  /// \brief Mother abstract class for all 3D object classes
  class i_object_3d : public datatools::i_tree_dumpable
  {
  public:

    /// \brief Known dimensions
    enum dimensional_type {
      DIMENSIONAL_0 = 0, // No dimension object (example: a vertex)
      DIMENSIONAL_1 = 1, // One dimension object (example: a segment)
      DIMENSIONAL_2 = 2, // Two dimension object (example: a rectangle)
      DIMENSIONAL_3 = 3  // Three dimension object (example: a box)
    };

    /// \brief Surface bits
    enum surface_bits {
      NO_SURFACES  = 0,            // No surface/side/facet
      ALL_SURFACES = FACE_ALL_BITS // All surfaces/sides/facets
    };

    /// A handle to a 3D object
    typedef datatools::handle<i_object_3d> handle_type;

    /// \brief An entry that stores a 3D object
    struct object_entry
    {
      /// Default constructor
      object_entry();

      /// Destructor
      ~object_entry();

      /// Reset
      void reset();

      uint32_t get_status() const;

      bool has_object() const;

      const i_object_3d & get_object() const;

      uint32_t              status;  //!< Status of the entry
      datatools::properties config;  //!< Configuration of the entry
      handle_type           hobject; //!< Handle to the 3D-object (is created)

    };

    /// Dictionary of handle of 3D object entries
    typedef std::map<std::string, object_entry> handle_dict_type;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Check is a user draw function is available
    bool has_user_draw() const;

    /// Set a user draw function
    void set_user_draw(void * user_draw_);

    /// Get the address of a draw function
    void * get_user_draw() const;

    /// Return the dimension of the object
    virtual int get_dimensional() const = 0;

    /// Return the distance tolerance
    double get_tolerance() const;

    /// Set the distance tolerance
    void set_tolerance(double tolerance_);

    /// Return the angular tolerance
    double get_angular_tolerance() const;

    /// Set the angular tolerance
    void set_angular_tolerance(double tolerance_);

    /// @deprecated Use grab_properties()
    datatools::properties & grab_properties();

    /// @deprecated Use get_properties()
    const datatools::properties & get_properties() const;

    /// Return mutable auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Return const auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return the name of the shape
    virtual std::string get_shape_name() const = 0;

    /// Check if the object is composite
    virtual bool is_composite() const;

    /// Default constructor
    i_object_3d();

    /// Constructor on length tolerance
    i_object_3d(double tolerance_);

    /// Destructor
    virtual ~i_object_3d();

    /// Initialize from properties and a dictionary of 3D-objects
    virtual void initialize(const datatools::properties &, const handle_dict_type * = 0);

    /// Reset
    virtual void reset();

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default values for attributes
    void set_defaults();

    datatools::logger::priority _logging;      //!< Logging priprity threshold

  private:

    double                _tolerance_;         //!< Length tolerance to check surface/curve belonging
    double                _angular_tolerance_; //!< Angular tolerance to check surface/curve belonging
    datatools::properties _auxiliaries_;       //!< List of internal auxiliary properties
    void *                _user_draw_;         //!< An address that may point to some drawing function (may be used by the gnuplot renderer)

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_object_3d);

  public:

    /// \brief Object 3D abstract getter class
    class i_getter
    {
    public:

      virtual const i_object_3d * get(const std::string & name_,
                                      const datatools::properties & params_) = 0;

      const i_object_3d * get(const std::string & name_);

    };

  };

} // end of namespace geomtools

#define GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ModelClassName)      \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::i_object_3d,ModelClassName); \
  /**/

#define GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ModelClassName,ModelClassId) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::geomtools::i_object_3d,ModelClassName,ModelClassId); \
  /**/

#endif // GEOMTOOLS_I_OBJECT_3D_H
