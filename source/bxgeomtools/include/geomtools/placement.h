// -*- mode: c++; -*-
/// \file geomtools/placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 *
 * License:
 *
 * Description:
 *  Utilities.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_PLACEMENT_H
#define GEOMTOOLS_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/geomtools_config.h>

namespace geomtools {

  /// \brief The placement for a geometry volume with its translation and rotation
  class placement : public i_placement
  {

  public:

    /// Check validity
    bool is_valid () const;

    /// Invalidate
    void invalidate ();

    /// Return the translation
    const vector_3d & get_translation () const;

    /// Set the translation from a (x,y,z) coordinate triplet
    void set_translation (double, double, double);

    /// Set the translation from a (x,y,z) coordinate triplet
    void set_translation_xyz (double, double, double);

    /// Set the translation
    void set_translation (const vector_3d &);

    /// Set the translation
    void set_translation_v (const vector_3d &);

    /// Check if the orientation is defined as a simple rotation (X, Y or Z axis)
    bool is_simple_rotation () const;

    /// Check if the orientation is defined as a X axis rotation
    bool is_rotation_x () const;

    /// Check if the orientation is defined as a Y axis rotation
    bool is_rotation_y () const;

    /// Check if the orientation is defined as a Z axis rotation
    bool is_rotation_z () const;

    /// Return rotation axis
    int get_rotation_axis () const;

    /// Return rotation angle
    double get_rotation_angle () const;

    /// Check if the orientation is defined through ZYZ Euler angles
    bool is_zyz_rotation () const;

    /// Return phi Euler angle
    double get_phi () const;

    /// Return theta Euler angle
    double get_theta () const;

    /// Return delta Euler angle
    double get_delta () const;

    /// Chec if Euler angles are defined
    bool has_angles () const;

    /// Return the rotation matrix
    const rotation_3d & get_rotation () const;

    /// Return the inverse rotation matrix
    const rotation_3d & get_inverse_rotation () const;

    // geomtools::i_placement interface:

    /// Return the dimension of the(multi-)placement object
    virtual size_t get_dimension () const;

    /// Return the number of placement entries associated to this placement object
    virtual size_t get_number_of_items () const;

    /// Compute the placement at given index
    virtual void get_placement (int item_, placement & p_) const;

    /// Check if placement is a replica
    virtual bool is_replica () const;

    /// Check if the (multi-)placement has only one rotation for all its placement objects
    virtual bool has_only_one_rotation () const;

    /// Compute the list of multi-dimension multiplet for placement at given index
    virtual size_t compute_index_map (std::vector<uint32_t> & map_,
                                      int item_) const;

    /// Check if the placement is identity
    bool is_identity() const;

    /// Set identity
    void set_identity ();

    /// Set orientation by axis and angle
    void set_orientation (int axis_, double angle_);

    /// Set orientation by axis and angle
    void set_orientation_axis_angle (int axis_, double angle_);

    /// Set orientation by ZYZ Euler angles
    void set_orientation (double phi_, double theta_, double delta_);

    /// Set orientation by ZYZ Euler angles
    void set_orientation_zyz (double phi_, double theta_, double delta_);

    /// Set translation by (x,y,z) coordinate triplet and orientation by ZYZ Euler angles
    void set (double x_, double y_, double z_,
              double phi_, double theta_, double delta_);

    /// Set translation by (x,y,z) coordinate triplet and orientation by axis and angle
    void set (double x_, double y_, double z_,
              int axis_, double angle_);

    /// Set translation by position and orientation ZYZ Euler angles
    void set (const vector_3d & t_,
              double phi_, double theta_, double delta_);

    /// Set translation by position and orientation by axis and angle
    void set (const vector_3d & t_,
              int axis_, double angle_);

    /// Set the rotation (not recommended at all)
    void set_orientation (const rotation_3d &);

    /// Set the rotation using XYZ Euler angles (experimental)
    void set_orientation_xyz (double phi_, double theta_, double delta_);

    /// Constructor
    placement ();

    /// Constructor
    placement (const vector_3d & translation_,
               double phi_,
               double theta_,
               double delta_);

    /// Constructor
    placement (const vector_3d & translation_,
               int axis_,
               double angle_);

    /// Constructor
    placement (double x_,
               double y_,
               double z_,
               double phi_,
               double theta_,
               double delta_);

    /// Constructor
    placement (double x_,
               double y_,
               double z_,
               int    axis_,
               double angle_);

    /// Destructor
    virtual ~placement ();

    /// Reset
    virtual void reset ();

    // Transformation methods:

    void mother_to_child (const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child (const vector_3d &) const;

    void child_to_mother (const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother (const vector_3d &) const;

    void mother_to_child_direction (const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child_direction (const vector_3d &) const;

    void child_to_mother_direction (const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother_direction (const vector_3d &) const;

    //void mother_to_child (const placement &, placement &) const;

    void child_to_mother (const placement &, placement &) const;


    /** Given two placements P1 and  P2 expressed in  some mother
     * coordinate system,  recompute the relative placement P2 in
     * the coordinate system of placement P1 and store the result
     * in P2_1.
     *
     * Usage:
     *
     *   P1.relocate (P2, P2_1);
     *
     */
    void relocate (const placement &, placement &) const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_  = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;

    /// Basic print
    void dump (std::ostream      & out_    = std::clog,
               const std::string & title_  = "geomutils::placement",
               const std::string & indent_ = "") const;

    /// Parse from a description string
    static bool from_string (const std::string &, placement &);

    /// Convert to a description string
    static void to_string (std::string &, const placement &);

    /// Test method
    void test() const;

  private:

    /// Compute internal orientation parameters
    void _compute_orientation_();

    /// Not documented
    void _compute_orientation_xyz_(); // just for test

  private:

    vector_3d    _translation_;      //!< Absolute position in mother frame
    int          _rotation_axis_;    //!< Rotation axis see utils.h: ROTATION_AXIS_X/Y/Z
    double       _rotation_angle_;   //!< Rotation angle (using rotation axis)
    double       _phi_, _theta_, _delta_; //!< ZYZ Euler angles
    rotation_3d  _rotation_;         //!< Mother->Child frame coord. transformation
    rotation_3d  _inverse_rotation_; //!< Child->Mother frame coord. transformation

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION();

    //! Reflection interface
    DR_CLASS_RTTI();

  };

  std::ostream & operator<< (std::ostream & out_, const placement &);

} // end of namespace geomtools

// Activate reflection layer :
DR_CLASS_INIT(::geomtools::placement);

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::placement, 0)
*/

#endif // GEOMTOOLS_PLACEMENT_H
