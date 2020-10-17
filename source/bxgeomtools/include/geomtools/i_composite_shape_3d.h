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

    public:

      void copy(shape_type &);

      void reset();

      bool is_valid() const;

      shape_type();

      ~shape_type() override;

      bool is_delete() const;

      void set_shape_ref(const std::string &);

      const std::string & get_shape_ref() const;

      const i_shape_3d & get_shape() const;

      i_shape_3d & grab_shape();

      const placement & get_placement() const;

      placement & grab_placement();

      // Factory methods:

      static void make_shape(i_shape_3d &,
                             const placement &,
                             shape_type &);

      static void make_shape(i_shape_3d &,
                             const placement &,
                             const std::string & shape_ref_,
                             shape_type &);

      static void make_shape(i_shape_3d *,
                              const placement &,
                              shape_type &);

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_          = false) const override;

      /// Print
      void dump(std::ostream & out_ = std::clog) const;

    private:

      // forbid implementation of:
      shape_type(const shape_type &);

      shape_type & operator=(const shape_type &);

    private:

      bool         _delete_;    //!< Flag to delete the handled shape
      i_shape_3d * _shape_;     //!< Handle to the component shape
      std::string  _shape_ref_; //!< Reference name of the external shape
      placement    _placement_; //!< Placement of the component shape

    };

    /*
    /// \brief Masks used for the faces of the two solids
    enum faces_mask_type {
      FACE_NONE   = face_identifier::FACE_BITS_NONE,
      _FACE_BEGIN = datatools::bit_mask::bit00,
      FACE_FIRST  = datatools::bit_mask::bit00,
      FACE_SECOND = datatools::bit_mask::bit01,
      _FACE_END   = datatools::bit_mask::bit02,
      FACE_ALL    = (FACE_FIRST| FACE_SECOND)
    };
    */

    static const uint32_t FIRST_PART  = 0;
    static const uint32_t SECOND_PART = 1;

    /// \brief Masks used for the two solids
     enum component_shape_type {
      COMPONENT_SHAPE_NONE   = 0,
      COMPONENT_SHAPE_FIRST  = datatools::bit_mask::bit00,
      COMPONENT_SHAPE_SECOND = datatools::bit_mask::bit01,
      COMPONENT_SHAPE_ALL    = COMPONENT_SHAPE_FIRST | COMPONENT_SHAPE_SECOND
    };

    /// Check if the face identification scheme is based on face bits
    bool using_face_id_bits() const override;

    /// Check if the face identification scheme uses part index
    bool using_face_id_part_index() const override;

    /// Build a face identifier any mask
    void make_any_face(face_identifier &) const override;

    /// Check if the shape is valid
    bool is_valid() const override;

    /// Check if the shape is composite
    bool is_composite() const override;

    /// Check if a forced volume can be set
    bool volume_can_be_forced() const override;

    /// Constructor
    i_composite_shape_3d(double skin_ = GEOMTOOLS_DEFAULT_TOLERANCE);

    /// Destructor
    ~i_composite_shape_3d() override;

    /// Set first shape
    void set_shape1(i_shape_3d &, const placement &, const std::string & shref_ = "");

    /// Set second shape
    void set_shape2(i_shape_3d &, const placement &, const std::string & shref_ = "");

    /// Set both shapes
    void set_shapes(i_shape_3d &, i_shape_3d &, const placement &,
                    const std::string & sh1ref_ = "", const std::string & sh2ref_ = "");

    /// Set first shape
    void set_shape1(i_shape_3d *, const placement &);

    /// Set second shape
    void set_shape2(i_shape_3d *, const placement &);

    /// Set both shapes
    void set_shapes(i_shape_3d *, i_shape_3d *, const placement &);

    /// Return first shape
    const shape_type & get_shape1() const;

    /// Return second shape
    const shape_type & get_shape2() const;

    /// Return shape by index
    const shape_type & get_shape(int i_) const;

    /// Dump
    void dump(std::ostream & out_ = std::clog) const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

    /// Initialize from properties
    void initialize(const datatools::properties &,
                            const handle_dict_type *) override;

    /// Reset
    void reset() override;

    /// \brief 3D rendering options
    enum composite_wires_rendering_option_type {
      WR_COMPOSITE_ONLY_BB           = (i_wires_3d_rendering::WR_BASE_LAST << 1),       //!< Only bounding box
      WR_COMPOSITE_DRAW_FIRST_SOLID  = (i_wires_3d_rendering::WR_BASE_LAST << 2),       //!< Render the first solid
      WR_COMPOSITE_DRAW_SECOND_SOLID = (i_wires_3d_rendering::WR_BASE_LAST << 3),       //!< Render the second solid
      WR_COMPOSITE_BOOST_SAMPLING    = (i_wires_3d_rendering::WR_BASE_LAST << 4),       //!< Boost the solids sampling
      WR_COMPOSITE_LAST              = (WR_COMPOSITE_BOOST_SAMPLING), //!< Last defined bit
      WR_COMPOSITE_MASK              = (WR_COMPOSITE_ONLY_BB
                                        | WR_COMPOSITE_DRAW_FIRST_SOLID
                                        | WR_COMPOSITE_DRAW_SECOND_SOLID
                                        | WR_COMPOSITE_BOOST_SAMPLING
                                        )  //!< Rendering options bit mask
    };

  private:

    shape_type _shape1_; //!< First shape
    shape_type _shape2_; //!< Second shape

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
