// -*- mode: c++; -*-
/** \file geomtools/i_composite_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Basic interface for binary composite 3D shapes (union, intersection, difference)
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H
#define GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  /// \brief Base class for binary composite shapes
  class i_composite_shape_3d : public i_shape_3d
  {

  public:

    /// \brief Record for a composite shape
    class shape_type : public datatools::i_tree_dumpable
    {
      bool         _delete_;    //!< Flag to delete the handled shape
      i_shape_3d * _shape_;     //!< Handle to the component shape
      placement    _placement_; //!< Placement of the component shape

    private:

      // forbid implementation of:
      shape_type (const shape_type &);

      shape_type & operator=(const shape_type &);

    public:

      void copy(shape_type &);

      void reset ();

      bool is_valid () const;

      shape_type ();

      virtual ~shape_type ();

      bool is_delete () const;

      const i_shape_3d & get_shape () const;

      i_shape_3d & grab_shape ();

      const placement & get_placement () const;

      placement & grab_placement ();

      // Factory methods:

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

    enum component_shape_type {
      COMPONENT_SHAPE_NONE   = 0,
      COMPONENT_SHAPE_FIRST  = datatools::bit_mask::bit00,
      COMPONENT_SHAPE_SECOND = datatools::bit_mask::bit01,
      COMPONENT_SHAPE_ALL    = COMPONENT_SHAPE_FIRST | COMPONENT_SHAPE_SECOND
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
                            bool inherit_               = false) const;

    /// Initialize from properties
    virtual void initialize(const datatools::properties &,
                            const handle_dict_type *);

    /// Reset
    virtual void reset();

  private:

    shape_type _shape1_;
    shape_type _shape2_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H
