// -*- mode: c++; -*-
/* blur_spot.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-22
 * Last modified: 2012-03-22
 *
 * License:
 *   GPL 3.0
 * Description:
 *
 *  Classes that represent various spot patterns :
 *   - point-like spot (no blur)
 *   - spot blured on a segment (blur impact on a wire)
 *   - spot blured as a stain (blur impact on a surface)
 *   - spot blured as a tiny volume (blur 3D-vertex: 'gazeous drop/cloud')
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BLUR_SPOT_H_
#define GEOMTOOLS_BLUR_SPOT_H_ 1

#include <boost/serialization/access.hpp>
#include <boost/random/normal_distribution.hpp>

#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

#include <mygsl/rng.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/placement.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// A blur spot classes
  /*
   *
   *   <---> tolerance
   *     :
   *   ..*..  point like spot +/- tolerance in 3D
   *     :\
   *       placement
   *
   *
   *     :           zerror  :
   *   ..++++++++++*++++++++++..    blur_spot (1D)
   *     :        /          :
   *          placement
   *
   *
   *     :     xerror    :
   * y ..+++++++++++++++++..
   * e ..+++++++++++++++++..
   * r ..++++++++*++++++++..   blur_spot (2D)
   * r ..+++++++++\+++++++..
   * o ..++++++++++\++++++..
   * r   :          \    :
   *             placement
   *
   *
   *      <no representation>  for blur_spot (3D)
   *
   *
   */
  class blur_spot
    : DATATOOLS_SERIALIZABLE_CLASS,
      public i_wires_3d_rendering,
      public datatools::i_tree_dumpable

  {
  public:

    /// Dimensionality of the blur object
    enum blur_dimension_type
      {
        DIMENSION_INVALID = -1,
        DIMENSION_ZERO    =  0,
        dimension_zero    =  DIMENSION_ZERO,
        DIMENSION_ONE     =  1,
        dimension_one     =  DIMENSION_ONE,
        DIMENSION_TWO     =  2,
        dimension_two     =  DIMENSION_TWO,
        DIMENSION_THREE   =  3,
        dimension_three   =  DIMENSION_THREE
      };

    /// Mode
    enum mode_type
      {
        MODE_INTERVAL = 0,
        mode_interval = MODE_INTERVAL,
        MODE_GAUSSIAN = 1,
        mode_gaussian = MODE_GAUSSIAN
      };

    static const double DEFAULT_VALUE;
    static const double DEFAULT_NSIGMA;

    /// Return the intrinsic geometry tolerance of the object
    double get_tolerance () const;

    /// Set the intrinsic geometry tolerance of the object
    void set_tolerance (double tolerance_);

    bool is_dimension_zero () const;
    bool is_dimension_one () const;
    bool is_dimension_two () const;
    bool is_dimension_three () const;

    /// Set the intrinsic dimension of the object
    void set_blur_dimension (int);

    /// Return the intrinsic dimension of the object
    int get_blur_dimension () const;

   /// Default constructor
    blur_spot ();

    /// Constructor
    blur_spot (int8_t dimension_,
               double tolerance_ = DEFAULT_VALUE);

    /// Set the position of the object
    void set_position (const vector_3d & position_);

    /// Return the mutable placement of the object
    placement & grab_placement ();

    /// Return the no mutable placement of the object
    const placement & get_placement () const;

    /// Set the placement of the object
    void set_placement (const placement & placement_);

    /// Return the position of the object
    const vector_3d & get_position () const;

    /// Return the rotation matrix of the object
    const rotation_3d & get_rotation () const;

    /// Return the inverse rotation matrix of the object
    const rotation_3d & get_inverse_rotation () const;

    /// Return a mutable reference to the  container of auxiliary properties
    datatools::properties & grab_auxiliaries ();

    /// Return a non-mutable reference to the  container of auxiliary properties
    const datatools::properties & get_auxiliaries () const;

    /// Set  auxiliary properties
    void set_auxiliaries (const datatools::properties &);

    /// Destructor
    virtual ~blur_spot ();

    /// Invalidate the object
    void invalidate ();

    /// Reset internal data
    void reset ();

    /// Check the validity of the object
    bool is_valid () const;

    /// Return the transverse error of the spot along the X axis
    double get_x_error () const;

    /// Set the transverse error of the spot along the X axis
    void set_x_error (double);

    /// Return the transverse error of the spot along the Y axis
    double get_y_error () const;

    /// Set the transverse error of the spot along the Y axis
    void set_y_error (double);

    /// Return the longitudinal error of the spot along the Z axis
    double get_z_error () const;

    /// Set the longitudinal error of the spot along the Z axis
    void set_z_error (double);

    /// Set all errors of the spot
    void set_errors (double,
                     double = std::numeric_limits<double>::quiet_NaN(),
                     double = std::numeric_limits<double>::quiet_NaN());

    /// Randomize point from the spot region
    template <class Randomizer>
    void randomize_boost (Randomizer & ran_,
                          vector_3d & random_point_,
                          int mode_ = MODE_INTERVAL) const;

    void randomize_mygsl (mygsl::rng & ran_,
                          vector_3d & random_point_,
                          int mode_ = MODE_INTERVAL) const;

    /// Check if a given point matches the object within some tolerance
    bool match (const vector_3d & position_,
                int mode_ = MODE_INTERVAL,
                double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                double nsigma2_or_tolerance_ = DEFAULT_VALUE,
                double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement &,
                                 uint32_t options_ = 0) const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_    = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    /// Smart print shortcut
    void print() const;

  protected:

    bool _match_0d (const vector_3d & position_,
                    double tolerance_ = DEFAULT_VALUE) const;

    bool _match_1d (const vector_3d & position_,
                    int mode_ = MODE_INTERVAL,
                    double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                    double nsigma2_or_tolerance_ = DEFAULT_VALUE) const;

    bool _match_2d (const vector_3d & position_,
                    int mode_ = MODE_INTERVAL,
                    double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                    double nsigma2_or_tolerance_ = DEFAULT_VALUE,
                    double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

    bool _match_3d (const vector_3d & position_,
                    int mode_ = MODE_INTERVAL,
                    double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                    double nsigma2_or_tolerance_ = DEFAULT_VALUE,
                    double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

  private:

    int8_t    _blur_dimension_;          /// Dimension of the object
    double    _tolerance_;               /// Intrinsic tolerance
    placement _placement_;               /// Placement
    double _x_error_;                    /// Error/spread of the spot along the X-axis
    double _y_error_;                    /// Error/spread of the spot along the Y-axis
    double _z_error_;                    /// Error/spread of the spot along the Z-axis
    datatools::properties _auxiliaries_; /// Container of auxiliary properties

    DATATOOLS_SERIALIZATION_DECLARATION();

    //! Reflection interface
    DR_CLASS_RTTI();

  };

  template <class BoostRandomizer>
  void blur_spot::randomize_boost (BoostRandomizer & ran_,
                                   vector_3d & random_point_,
                                   int mode_) const
  {
    if (is_dimension_zero ())
      {
        random_point_ = _placement_.get_translation ();
        return;
      }
    vector_3d local_pos (0.0, 0.0, 0.0);
    if (is_dimension_one ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setZ (_z_error_ * ( -1. + 2 * ran_ ()));
          }
        else
          {
            boost::random::normal_distribution<double> gauss_random;
            local_pos.setZ (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _z_error_)));
          }
      }
    else if (is_dimension_two ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setX (_x_error_ * ( -1. + 2 * ran_ ()));
            local_pos.setY (_y_error_ * ( -1. + 2 * ran_ ()));
          }
        else
          {
            boost::random::normal_distribution<double> gauss_random;
            local_pos.setX (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _x_error_)));
            local_pos.setY (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _y_error_)));
          }
      }
    else if (is_dimension_three ())
      {
        if (mode_ == MODE_INTERVAL)
          {
            local_pos.setX (_x_error_ * ( -1. + 2 * ran_ ()));
            local_pos.setY (_y_error_ * ( -1. + 2 * ran_ ()));
            local_pos.setZ (_z_error_ * ( -1. + 2 * ran_ ()));
          }
        else
          {
            boost::random::normal_distribution<double> gauss_random;
            local_pos.setX (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _x_error_)));
            local_pos.setY (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _y_error_)));
            local_pos.setZ (gauss_random (ran_, boost::random::normal_distribution<double>::param_type (0.0, _z_error_)));
          }
      }
    _placement_.child_to_mother (local_pos, random_point_);
    return;
  }

} // end of namespace geomtools

// Activate reflection layer for the blur_spot class :
DR_CLASS_INIT(::geomtools::blur_spot);

#endif // GEOMTOOLS_BLUR_SPOT_H_

// end of blur_spot.h
