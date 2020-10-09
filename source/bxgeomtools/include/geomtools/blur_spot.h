/// \file geomtools/blur_spot.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-22
 * Last modified: 2014-07-08
 *
 * License:
 *   GPL 3.0
 * Description:
 *
 *  Classes that represent various spot patterns :
 *   - point-like spot (no blur)
 *   - spot blured on a segment (blur impact on a wire)
 *   - spot blured as a stain (blur impact on a surface)
 *   - spot blured as a tiny volume (blur 3D-vertex: 'gazeous drop/cloud/bubble')
 *
 */

#ifndef GEOMTOOLS_BLUR_SPOT_H
#define GEOMTOOLS_BLUR_SPOT_H 1

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
#include <boost/random/normal_distribution.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/placement.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/base_hit.h>

namespace geomtools {

  /// \brief A blur spot is a measurement hit that may represents a vertex with 1D, 2D or 3D dimension
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
    : public base_hit
    , public i_wires_3d_rendering
  {
  public:

    /// Dimensionality of the blur object
    enum blur_dimension_type {
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
    enum mode_type {
      MODE_INTERVAL = 0,
      mode_interval = MODE_INTERVAL,
      MODE_GAUSSIAN = 1,
      mode_gaussian = MODE_GAUSSIAN
    };

    /// Masks to automatically tag the attributes to be stored
    enum store_mask_type {
      STORE_PLACEMENT = datatools::bit_mask::bit03
    };

    /// Special default real value
    static const double DEFAULT_VALUE;

    /// Special sigma factor
    static const double DEFAULT_NSIGMA;

    /// Return the intrinsic geometry tolerance of the object
    double get_tolerance() const;

    /// Set the intrinsic geometry tolerance of the object
    void set_tolerance(double tolerance_);

    /// Check dimension zero (point-like spot)
    bool is_dimension_zero() const;

    /// Check dimension one
    bool is_dimension_one() const;

    /// Check dimension two
    bool is_dimension_two() const;

    /// Check dimension three
    bool is_dimension_three() const;

    /// Set the intrinsic dimension of the object
    void set_blur_dimension(int);

    /// Return the intrinsic dimension of the object
    int get_blur_dimension() const;

    /// Default constructor
    blur_spot();

    /// Constructor
    blur_spot(int8_t dimension_,
              double tolerance_ = DEFAULT_VALUE);

    /// Set the position of the object
    void set_position(const vector_3d & position_);

    /// Return the mutable placement of the object
    placement & grab_placement();

    /// Return the no mutable placement of the object
    const placement & get_placement() const;

    /// Set the placement of the object
    void set_placement(const placement & placement_);

    /// Invalidate the placement of the object
    void invalidate_placement();

    /// Return the position of the object
    const vector_3d & get_position() const;

    /// Return the rotation matrix of the object
    const rotation_3d & get_rotation() const;

    /// Return the inverse rotation matrix of the object
    const rotation_3d & get_inverse_rotation() const;

    /// Destructor
    ~blur_spot() override;

    /// Invalidate the object
    void invalidate() override;

    /// Reset internal data
    void reset();

    /// Check the validity of the object
    bool is_valid() const override;

    /// Return the transverse error of the spot along the X axis
    double get_x_error() const;

    /// Set the transverse error of the spot along the X axis
    void set_x_error(double);

    /// Return the transverse error of the spot along the Y axis
    double get_y_error() const;

    /// Set the transverse error of the spot along the Y axis
    void set_y_error(double);

    /// Return the longitudinal error of the spot along the Z axis
    double get_z_error() const;

    /// Set the longitudinal error of the spot along the Z axis
    void set_z_error(double);

    /// Set all errors of the spot
    void set_errors(double,
                    double = std::numeric_limits<double>::quiet_NaN(),
                    double = std::numeric_limits<double>::quiet_NaN());

    /// Randomize point from the spot region
    template <class Randomizer>
    void randomize_boost(Randomizer & ran_,
                         vector_3d & random_point_,
                         int mode_ = MODE_INTERVAL) const;

    /// Randomize point from the spot region
    void randomize_mygsl(mygsl::rng & ran_,
                         vector_3d & random_point_,
                         int mode_ = MODE_INTERVAL) const;

    /// Check if a given point matches the object within some tolerance
    bool match(const vector_3d & position_,
               int mode_ = MODE_INTERVAL,
               double nsigma1_or_tolerance_ = DEFAULT_VALUE,
               double nsigma2_or_tolerance_ = DEFAULT_VALUE,
               double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

    /// Generate rendering wires
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Smart print
    void tree_dump(std::ostream & out_    = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Smart print shortcut
    void print() const;

  protected:

    bool _match_0d(const vector_3d & position_,
                   double tolerance_ = DEFAULT_VALUE) const;

    bool _match_1d(const vector_3d & position_,
                   int mode_ = MODE_INTERVAL,
                   double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                   double nsigma2_or_tolerance_ = DEFAULT_VALUE) const;

    bool _match_2d(const vector_3d & position_,
                   int mode_ = MODE_INTERVAL,
                   double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                   double nsigma2_or_tolerance_ = DEFAULT_VALUE,
                   double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

    bool _match_3d(const vector_3d & position_,
                   int mode_ = MODE_INTERVAL,
                   double nsigma1_or_tolerance_ = DEFAULT_VALUE,
                   double nsigma2_or_tolerance_ = DEFAULT_VALUE,
                   double nsigma3_or_tolerance_ = DEFAULT_VALUE) const;

  private:

    int8_t    _blur_dimension_;          //!< Dimension of the object
    double    _tolerance_;               //!< Intrinsic tolerance
    placement _placement_;               //!< Placement
    double _x_error_;                    //!< Error/spread of the spot along the X-axis
    double _y_error_;                    //!< Error/spread of the spot along the Y-axis
    double _z_error_;                    //!< Error/spread of the spot along the Z-axis

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Reflection interface
    DR_CLASS_RTTI()
    
    GEOMTOOLS_HIT_REGISTRATION_INTERFACE(blur_spot)
                                         
  };

  template <class BoostRandomizer>
  void blur_spot::randomize_boost(BoostRandomizer & ran_,
                                  vector_3d & random_point_,
                                  int mode_) const
  {
    if (is_dimension_zero()) {
      random_point_ = _placement_.get_translation();
      return;
    }
    vector_3d local_pos(0.0, 0.0, 0.0);
    if (is_dimension_one()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setZ(_z_error_ * ( -1. + 2 * ran_()));
      } else {
        boost::random::normal_distribution<double> gauss_random;
        local_pos.setZ(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _z_error_)));
      }
    } else if (is_dimension_two()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setX(_x_error_ * ( -1. + 2 * ran_()));
        local_pos.setY(_y_error_ * ( -1. + 2 * ran_()));
      } else {
        boost::random::normal_distribution<double> gauss_random;
        local_pos.setX(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _x_error_)));
        local_pos.setY(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _y_error_)));
      }
    } else if (is_dimension_three()) {
      if (mode_ == MODE_INTERVAL) {
        local_pos.setX(_x_error_ * ( -1. + 2 * ran_()));
        local_pos.setY(_y_error_ * ( -1. + 2 * ran_()));
        local_pos.setZ(_z_error_ * ( -1. + 2 * ran_()));
      } else {
        boost::random::normal_distribution<double> gauss_random;
        local_pos.setX(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _x_error_)));
        local_pos.setY(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _y_error_)));
        local_pos.setZ(gauss_random(ran_, boost::random::normal_distribution<double>::param_type(0.0, _z_error_)));
      }
    }
    _placement_.child_to_mother(local_pos, random_point_);
    return;
  }

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::blur_spot, "geomtools::blur_spot")

// Activate reflection layer for the blur_spot class :
DR_CLASS_INIT(::geomtools::blur_spot)

// Class version:
// 2014-07-03 FM: Now inherits from geomtools::base_hit :
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::blur_spot, 1)

#endif // GEOMTOOLS_BLUR_SPOT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

