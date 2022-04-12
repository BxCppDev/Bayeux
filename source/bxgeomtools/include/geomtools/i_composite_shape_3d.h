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
 */

#ifndef GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H
#define GEOMTOOLS_I_COMPOSITE_SHAPE_3D_H 1

// Standard library:
#include <string>
#include <memory>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  /// \brief Base class for binary composite shapes
  class i_composite_shape_3d
    : public i_shape_3d
  {

  public:

    typedef std::shared_ptr<const i_shape_3d> shape_handle;
    
    /// \brief Record for a composite shape
    class shape_type
      : public datatools::i_tree_dumpable
    {

    public:

      shape_type() = default;

      virtual ~shape_type() override;

      void reset();

      bool is_valid() const;

      void set_shape_label(const std::string &);

      const std::string & get_shape_label() const;

      const i_shape_3d & get_shape() const;

      const placement & get_placement() const;

      void set_placement(const placement & p_);

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                     const std::string & title_  = "",
                     const std::string & indent_ = "",
                     bool inherit_          = false) const override;

    private:

      shape_handle _shape_; //!< Handle to the component shape
      const i_shape_3d * _shape_ext_ = nullptr;  //!< Handle to the component shape
      std::string  _shape_label_; //!< Reference name of the external shape
      placement    _placement_; //!< Placement of the component shape

      friend class i_composite_shape_3d;
      
    };

    // Factory methods:
    static shape_type make_shape(const i_shape_3d & shape_,
                                 const placement & p_,
                                 const std::string & shape_label_ = "");
    static shape_type make_shape(const shape_handle & shape_,
                                 const placement & p_,
                                 const std::string & shape_label_ = "");
 
    static const uint32_t FIRST_PART  = 0;
    static const uint32_t SECOND_PART = 1;

    /// \brief Masks used for the two solids
    enum component_shape_type
      {
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
    virtual ~i_composite_shape_3d() override;

    /// Set first shape
    void set_shape1(const i_shape_3d &, const placement &, const std::string & shlabel_ = "");

    /// Set second shape
    void set_shape2(const i_shape_3d &, const placement &, const std::string & shlabel_ = "");

    /// Set both shapes
    void set_shapes(const i_shape_3d & shape1_,
                    const i_shape_3d & shape2_,
                    const placement & placement2_,
                    const std::string & sh1label_ = "",
                    const std::string & sh2label_ = "");

    /// Set first shape
    void set_shape1(const shape_handle & shape1_, const placement & placement1_, const std::string & shlabel_ = "");

    /// Set second shape
    void set_shape2(const shape_handle & shape2_, const placement & placement2_, const std::string & shlabel_ = "");

    /// Set both shapes
    void set_shapes(const shape_handle & shape1_,
                    const shape_handle & shape2_,
                    const placement & placement2_,
                    const std::string & sh1label_ = "",
                    const std::string & sh2label_ = "");

    /// Return first shape
    const shape_type & get_shape1() const;

    /// Return second shape
    const shape_type & get_shape2() const;

    /// Return shape by index
    const shape_type & get_shape(int i_) const;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const override;

    /// Initialize from properties
    void initialize(const datatools::properties &,
                    const handle_dict_type * = nullptr) override;

    /// Reset
    void reset() override;

    /// \brief 3D rendering options
    enum composite_wires_rendering_option_type
      {
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
