// -*- mode: c++; -*-
/* i_object_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-04-10
 *
 * License:
 *
 * Description:
 *  Interface for object than can be described in a 3D reference frame
 *  and possibly drawn with some graphics renderer.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_OBJECT_3D_H_
#define GEOMTOOLS_I_OBJECT_3D_H_ 1

#include <string>
#include <iostream>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

#include <geomtools/utils.h>

namespace geomtools {

  class box;

  class i_object_3d : public datatools::i_tree_dumpable
  {
  public:
    enum dimensional_type
      {
        DIMENSIONAL_0 = 0, // No dimension object (example: a vertex)
        DIMENSIONAL_1 = 1, // One dimension object (example: a segment)
        DIMENSIONAL_2 = 2, // Two dimension object (example: a rectangle)
        DIMENSIONAL_3 = 3  // Three dimension object (example: a box)
      };

    enum surface_bits {
      NO_SURFACES  = 0,
      ALL_SURFACES = FACE_ALL_BITS
    };

    bool has_user_draw () const;

    void set_user_draw (void * user_draw_);

    void * get_user_draw () const;

    // must be implemented from inherited classes:
    virtual int get_dimensional () const = 0;

    double get_tolerance () const;

    void set_tolerance (double tolerance_);

    datatools::properties & grab_properties ();

    const datatools::properties & get_properties () const;

    virtual std::string get_shape_name () const = 0;

    virtual bool is_composite () const;

    virtual void compute_bounding_box (box & bb_);

    i_object_3d ();

    i_object_3d (double tolerance_);

    virtual ~i_object_3d ();

    virtual void reset ();

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  private:

    double _tolerance_; //!< Tolerance to check surface/curve belonging
    datatools::properties _properties_; //!< List of properties
    void * _user_draw_; //!< An address that may point to some drawing function (may be used by the gnuplot renderer)

  public:

    /*** Object 3D getter ***/
    class i_getter
    {
    public:

      virtual const i_object_3d * get (const std::string & name_,
                                       const datatools::properties & params_) = 0;

      const i_object_3d * get (const std::string & name_);

    };

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_OBJECT_3D_H_

// end of i_object_3d.h
