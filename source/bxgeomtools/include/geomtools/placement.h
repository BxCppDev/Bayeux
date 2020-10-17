/// \file geomtools/placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2016-07-06
 *
 * License:
 *
 * Description:
 *
 *  Placement.
 *
 */

#ifndef GEOMTOOLS_PLACEMENT_H
#define GEOMTOOLS_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// This project:
#include <datatools/properties.h>
#include <datatools/bit_mask.h>
#include <geomtools/i_placement.h>
#include <geomtools/geomtools_config.h>

namespace geomtools {

  class stackable_data;

  /// \brief The placement for a geometry volume with its translation and rotation
  ///        with respect to some mother reference frame
  class placement : public i_placement
  {

  public:

    /// \brief Construct a placement
    ///
    /// Example of properties:
    /// \code
    /// x             : real as length =  2.0 mm
    /// y             : real as length =  0.0 mm
    /// z             : real as length =  0.0 mm
    /// rotation_axis : string         = "z"
    /// angle         : real as angle  = 45.0 degree
    /// \endcode
    ///
    /// Example of properties:
    /// \code
    /// x     : real as length =  2.0 mm
    /// y     : real as length =  0.0 mm
    /// z     : real as length =  0.0 mm
    /// phi   : real as angle  = 45.0 degree
    /// theta : real as angle  = 25.0 degree
    /// delta : real as angle  = 10.0 degree
    /// \endcode
    ///
    /// Example of properties:
    /// \code
    /// x.gap_mode     : string = "min_to_min"
    /// x.gap_distance : real as length =  2.0 mm
    /// y              : real as length = 10.0 mm
    /// z              : real as length = 10.0 mm
    /// rotation_axis  : string         =  "z"
    /// angle          : real as angle  = 45.0 degree
    /// \endcode
    ///
    /// \code
    /// geommtools::placement::builder b;
    /// b.set_mother_stackable(mother_stacking_info);
    /// b.set_child_stackable(child_stacking_info);
    /// geomtools::placement p;
    /// b.build(config, p);
    /// \endcode
    class builder
    {
    public:

      enum init_flags {
        no_child_to_mother_gap = datatools::bit_mask::bit00, //!< Inhibit the child/mother gap build modes
        no_rotation            = datatools::bit_mask::bit01  //!< Inhibit rotation
      };

      /// \brief Child to mother gap mode:
      enum child_to_mother_gap_mode {
        gap_undefined     = 0, //!< Undefined gap mode
        gap_center_to_min = 1, //!< Gap between the child origin to mother minimum border
        gap_center_to_max = 2, //!< Gap between the child origin to mother maximum border
        gap_min_to_min    = 3, //!< Gap between the child minimum border to mother minimum border
        gap_min_to_max    = 4, //!< Gap between the child minimum border to mother maximum border
        gap_max_to_min    = 5, //!< Gap between the child maximum border to mother minimum border
        gap_max_to_max    = 6  //!< Gap between the child maximum border to mother maximum border
      };

      /// Return child/mother gap mode from a label
      static child_to_mother_gap_mode get_gap_mode(const std::string & label_);

      /// Return the label associated ti a given child/mother gap mode
      static std::string get_gap_mode_label(child_to_mother_gap_mode);

      /// Check if a given child/mother gap mode needs child stackable informations
      static bool needs_stackable_child(child_to_mother_gap_mode);

      /// Constructor from initialization flags
      builder(uint32_t flags_ = 0);

      bool has_mother_stackable() const;

      void set_mother_stackable(const stackable_data &);

      bool has_child_stackable() const;

      void set_child_stackable(const stackable_data &);

      bool use_child_to_mother_gap() const;

      bool allowed_rotation() const;

      /// Configure from flags
      void configure(uint32_t flags_ = 0);

      /// Reset the builder
      void reset();

      /// Build a placement object from a set of parameters
      void build(const datatools::properties & config_,
                 placement & p_) const;

    protected:

      void _build(const datatools::properties & config_,
                  placement & p_) const;

    private:

      bool  _no_child_to_mother_gap_            = false;
      bool  _no_rotation_                       = false;
      const stackable_data * _mother_stackable_ = nullptr;
      const stackable_data * _child_stackable_  = nullptr;

    };

    /// Check validity
    bool is_valid() const;

    /// Invalidate
    void invalidate();

    /// Return the translation
    const vector_3d & get_translation() const;

    /// Return the mutable translation
    vector_3d & grab_translation();

    /// Set the translation from a (x,y,z) coordinate triplet
    void set_translation(double, double, double);

    /// Set the translation from a (x,y,z) coordinate triplet
    void set_translation_xyz(double, double, double);

    /// Set the translation
    void set_translation(const vector_3d &);

    /// Set the translation
    void set_translation_v(const vector_3d &);

    /// Translate the current translation by some shift
    void translate(const vector_3d &);

    /// Translate the current translation by some shift
    void translate(double dx_, double dy_, double dz_);

    /// Check if the orientation is defined as a simple rotation (X, Y or Z axis)
    bool is_simple_rotation() const;

    /// Check if the orientation is defined as a X axis rotation
    bool is_rotation_x() const;

    /// Check if the orientation is defined as a Y axis rotation
    bool is_rotation_y() const;

    /// Check if the orientation is defined as a Z axis rotation
    bool is_rotation_z() const;

    /// Return rotation axis
    int get_rotation_axis() const;

    /// Return rotation angle
    double get_rotation_angle() const;

    /// Check if the orientation is defined through ZYZ Euler angles
    bool is_zyz_rotation() const;

    /// Return phi Euler angle
    double get_phi() const;

    /// Return theta Euler angle
    double get_theta() const;

    /// Return delta Euler angle
    double get_delta() const;

    /// Chec if Euler angles are defined
    bool has_angles() const;

    /// Return the rotation matrix
    const rotation_3d & get_rotation() const;

    /// Return the inverse rotation matrix
    const rotation_3d & get_inverse_rotation() const;

    // geomtools::i_placement interface:

    /// Return the dimension of the(multi-)placement object
    size_t get_dimension() const override;

    /// Return the number of placement entries associated to this placement object
    size_t get_number_of_items() const override;

    /// Compute the placement at given index
    void get_placement(int item_, placement & p_) const override;

    /// Check if placement is a replica
    bool is_replica() const override;

    /// Check if the (multi-)placement has only one rotation for all its placement objects
    bool has_only_one_rotation() const override;

    /// Compute the list of multi-dimension multiplet for placement at given index
    size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const override;

    /// Check if the placement is identity
    bool is_identity() const;

    /// Set identity
    void set_identity();

    /// Set orientation by axis and angle
    void set_orientation(int axis_, double angle_);

    /// Set orientation by axis and angle
    void set_orientation_axis_angle(int axis_, double angle_);

    /// Set orientation by ZYZ Euler angles
    void set_orientation(double phi_, double theta_, double delta_);

    /// Set orientation by ZYZ Euler angles
    void set_orientation_zyz(double phi_, double theta_, double delta_);

    /// Set orientation by arbitrary Euler angles
    void set_orientation(double angle0_, double angle1_, double angle2_, euler_angles_type euler_angle_);

    /// Set translation by (x,y,z) coordinate triplet and orientation by ZYZ Euler angles
    void set(double x_, double y_, double z_,
             double phi_, double theta_, double delta_);

    /// Set translation by (x,y,z) coordinate triplet and orientation by Euler angles
    void set(double x_, double y_, double z_,
             double angle0_, double angle1_, double angle2_, euler_angles_type euler_angle_);

    /// Set translation by (x,y,z) coordinate triplet and orientation by axis and angle
    void set(double x_, double y_, double z_,
             int axis_, double angle_);

    /// Set translation by position and orientation ZYZ Euler angles
    void set(const vector_3d & t_,
             double phi_, double theta_, double delta_);

    /// Set translation by position and orientation by Euler angles
    void set(const vector_3d & t_,
             double angle0_, double angle1_, double angle2_, euler_angles_type euler_angle_);


    /// Set translation by position and orientation by axis and angle
    void set(const vector_3d & t_,
             int axis_, double angle_);

    /// Set the rotation (not recommended at all)
    void set_orientation(const rotation_3d &);

    /// Set the rotation using XYZ Euler angles (experimental)
    void set_orientation_xyz(double phi_, double theta_, double delta_);

    /// Constructor
    placement();

    /// Constructor
    placement(const vector_3d & translation_);

    /// Constructor
    placement(double x_,
              double y_,
              double z_);

    /// Constructor
    placement(const vector_3d & translation_,
              double phi_,
              double theta_,
              double delta_);

    /// Constructor
    placement(const vector_3d & translation_,
              int axis_,
              double angle_);

    /// Constructor
    placement(double x_,
              double y_,
              double z_,
              double phi_,
              double theta_,
              double delta_);

    /// Constructor
    placement(double x_,
              double y_,
              double z_,
              double angle0_,
              double angle1_,
              double angle2_,
              euler_angles_type euler_angles_);

    /// Constructor
    placement(const vector_3d & translation_,
              double angle0_,
              double angle1_,
              double angle2_,
              euler_angles_type euler_angles_);

    /// Constructor
    placement(double x_,
              double y_,
              double z_,
              int    axis_,
              double angle_);

    /// Constructor
    placement(const vector_3d & translation_,
              const rotation_3d & rotation_);

    /// Destructor
    ~placement() override;

    /// Reset
    virtual void reset();

    // Transformation methods:

    void mother_to_child(const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child(const vector_3d &) const;

    void child_to_mother(const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother(const vector_3d &) const;

    void mother_to_child_direction(const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child_direction(const vector_3d &) const;

    void child_to_mother_direction(const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother_direction(const vector_3d &) const;

    //void mother_to_child(const placement &, placement &) const;

    void child_to_mother(const placement &, placement &) const;


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
    void relocate(const placement &, placement &) const;

    void initialize(const datatools::properties & config_, uint32_t flags_ = 0);

    /// Smart print
    void tree_dump(std::ostream & out_  = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Basic print
    void dump(std::ostream      & out_    = std::clog,
               const std::string & title_  = "geomutils::placement",
               const std::string & indent_ = "") const;

    /// Parse from a description string
    static bool from_string(const std::string &, placement &);

    /// Convert to a description string
    static void to_string(std::string &, const placement &);

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
    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Reflection interface
    DR_CLASS_RTTI()

  };

  std::ostream & operator<< (std::ostream & out_, const placement &);

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::placement, "geomtools::placement")

// Activate reflection layer :
DR_CLASS_INIT(::geomtools::placement)

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::placement, 0)
*/

#endif // GEOMTOOLS_PLACEMENT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
