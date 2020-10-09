/* \file geomtools/bounding_data.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-16
 * Last modified: 2014-11-16
 *
 * License:
 *
 * Description:
 *
 *  Bounding data information for 3D shaped datas
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BOUNDING_DATA_H
#define GEOMTOOLS_BOUNDING_DATA_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  class properties;
}

namespace geomtools {

  // Forward declaration:
  class box;
  class placement;

  /// \brief Bounding volume data for i_shape_3d derived solid shapes
  class bounding_data :
    public i_wires_3d_rendering,
    public datatools::i_tree_dumpable
  {
  public:

    /// \brief Bounding data type
    enum bb_type {
      BD_INVALID  = 0, //!< Invalid type
      BD_BOX      = 1, //!< Bounding box
      BD_CYLINDER = 2 //!< Bounding cylinder
    };

    /// Check validity
    bool is_valid(bool verbose_ = false) const;

    /// Default constructor
    bounding_data();

    /// Destructor
    ~bounding_data() override;

    /// Make a bounding box
    void make_box(double xmin_, double xmax_,
                  double ymin_, double ymax_,
                  double zmin_, double zmax_);

    /// Make a bounding cylinder
    void make_cylinder(double rmax_, double zmin_, double zmax_);

    /// Reset
    void reset();

    /// Check box type
    bool is_box() const;

    /// Check cylinder type
    bool is_cylinder() const;

    /// Return the minimum X
    double get_xmin() const;

    /// Return the maximum X
    double get_xmax() const;

    /// Return the minimum Y
    double get_ymin() const;

    /// Return the maximum Y
    double get_ymax() const;

    /// Return the minimum Z
    double get_zmin() const;

    /// Return the maximum Z
    double get_zmax() const;

    /// Return the maximum R
    double get_rmax() const;

    /// Return the minimum dimension
    double get_min_dimension() const;

    /// Compute a bounding box and its placement in the solid reference frame
    void compute_bounding_box(box & bb_, placement & p_, double safe_skin_ = 0.0) const;

    /// Compute the vertexes (corners) of the bounding box in the solid reference frame
    void compute_bounding_box_vertexes(std::vector<vector_3d> &, double safe_skin_ = 0.0) const;

    /// Compute the face centers of the bounding box in the solid reference frame
    void compute_bounding_box_face_centers(std::vector<vector_3d> &, double safe_skin_ = 0.0) const;

    /// Build a bounding data from a collection of points
    void make_box_from_points(const std::vector<vector_3d> & points_);

    /// Build a stackable daat object
    void build_stackable(stackable_data & stackable_info_) const;

    /// Smart print
    void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit               = false) const override;

    /// Parse enforced bounding data from a properties container
    void parse_bounding_data(const datatools::properties & config_);

    /// Generate wires associated to the bounding volume
    void generate_wires_self(wires_type & wires_, uint32_t options_ = 0) const override;

  private:

    bb_type _type_; //!< Type of bounding data
    double  _xmin_; //!< X min (box)
    double  _xmax_; //!< X max (box)
    double  _ymin_; //!< Y min (box)
    double  _ymax_; //!< Y max (box)
    double  _zmin_; //!< Z min (box/cylinder)
    double  _zmax_; //!< Z max (box/cylinder)
    double  _rmax_; //!< R max (cylinder)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_BOUNDING_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
