// -*- mode: c++; -*-
/* i_composite_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Basic interface for composite 3D shapes (union, intersection, difference)
 *
 * History:
 *
 */

#ifndef __geomtools__i_composite_shape_3d_h
#define __geomtools__i_composite_shape_3d_h 1

#include <string>

#include <geomtools/i_shape_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  class i_composite_shape_3d : public i_shape_3d
  {

  public:

    class shape_type : public datatools::i_tree_dumpable
    {
      bool         _delete_;
      i_shape_3d * _shape_;
      placement    _placement_;

    private:

      // forbid implementation of:
      shape_type (const shape_type &);

      shape_type & operator=(const shape_type &);

    public:

      void copy (shape_type &);

      void reset ();

      bool is_valid () const;

      shape_type ();

      virtual ~shape_type ();

      bool is_delete () const;

      const i_shape_3d & get_shape () const;

      i_shape_3d & get_shape ();

      const placement & get_placement () const;

      placement & get_placement ();

      // factory methods:
      static void make_shape (i_shape_3d &,
                              const placement &,
                              shape_type &);

      static void make_shape (i_shape_3d *,
                              const placement &,
                              shape_type &);


      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_          = false) const;

      void dump (std::ostream & out_ = std::clog) const;

    };


    bool is_composite () const;

    i_composite_shape_3d (double skin_ = GEOMTOOLS_DEFAULT_TOLERANCE);

    virtual ~i_composite_shape_3d ();

    void set_shape1 (i_shape_3d &, const placement &);

    void set_shape2 (i_shape_3d &, const placement &);

    void set_shapes (i_shape_3d &, i_shape_3d &, const placement &);

    void set_shape1 (i_shape_3d *, const placement &);

    void set_shape2 (i_shape_3d *, const placement &);

    void set_shapes (i_shape_3d *, i_shape_3d *, const placement &);

    const shape_type & get_shape1 () const;

    const shape_type & get_shape2 () const;

    const shape_type & get_shape (int i_) const;

    void dump (std::ostream & out_ = std::clog) const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  private:

    shape_type _shape1_;
    shape_type _shape2_;

  };

} // end of namespace geomtools

#endif // __geomtools__i_composite_shape_3d_h

// end of i_composite_shape_3d.h
