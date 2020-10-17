/// \file geomtools/i_wires_drawer.h
/* Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-07
 * Last modified: 2015-02-07
 *
 * License:
 *
 * Description:
 *
 *   Base class for all shape drawer in wires mode
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_WIRES_DRAWER_H
#define GEOMTOOLS_I_WIRES_DRAWER_H 1

// Standard library:
#include <iostream>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /*
  //! \brief A wires drawer with cache
  class cached_wires_drawer
  {
  public:

    //! Default constructor
    cached_wires_drawer(i_wires_3d_rendering & renderer_);

    //! Destructor
    virtual ~cached_wires_drawer();

    //! Generate a list of polylines representing the contour of the shape (for display clients)
    virtual void generate_wires_self(std::list<polyline_3d> & wires_,
                                     uint32_t options_ = 0) const;

    //! Remove the precomputed wires collection
    void clear_wires();

    //! Update the precomputed wires collection
    void update_wires();

    //! Check if wires collection is available
    bool has_wires() const;

    //! Return the non mutable reference to the wires collection
    const std::list<polyline_3d> & get_wires() const;

  private:

    i_wires_3d_rendering * _renderer_; //!< handle to an external renderer
    uint32_t _options_; //!< Current rendering options
    boost::scoped_ptr<std::list<polyline_3d> > _cached_wires_; //!< Cached wires collection

  };
  */

  //! \brief Base class for all shape drawer objects in wires mode
  template <class T>
  class i_wires_drawer : public i_wires_3d_rendering
  {
  public:

    // Type aliases:
    typedef T          object_type;
    typedef T *        pointer_type;
    typedef const T *  const_pointer_type;
    typedef T &        reference_type;
    typedef const T &  const_reference_type;

    //! Default constructor
    i_wires_drawer() : _address_(0)
    {
      return;
    }

    //! Constructor
    i_wires_drawer(const_reference_type object_) : _address_(0)
    {
      set_address(object_);
      return;
    }

    //! Destructor
    ~i_wires_drawer() override
    {
      return;
    }

    //! Check the validity of the address
    bool has_address() const
    {
      return _address_ != 0;
    }

    //! Set the address of the external object
    void set_address(const_reference_type object_)
    {
      _address_ = &object_;
      return;
    }

    //! Reset the address of the external object
    void reset_address()
    {
      _address_ = 0;
      return;
    }

    //! Return a non mutable reference to the object
    const_reference_type get() const
    {
      return *_address_;
    }

  protected:

    const_pointer_type _address_; //!< Reference to an external object

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_WIRES_DRAWER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
