/// \file geomtools/angular_range.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-03-09
 * Last modified: 2016-03-09
 *
 * License:
 *
 * Description:
 *
 *   Angular range
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ANGULAR_RANGE_H
#define GEOMTOOLS_ANGULAR_RANGE_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/object_configuration_description.h>
#include <datatools/ocd_macros.h>

namespace geomtools {

  /// \brief Angular range
  class angular_range :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable
  {
  public:

    /// \brief The type of angular range
    enum range_type {
      RANGE_TYPE_INVALID   = -1, ///< Invalid range type
      RANGE_TYPE_POLAR     =  0, ///< Polar angle
      RANGE_TYPE_AZIMUTHAL =  1  ///< Azimuthal range
    };

    /// Return the label associated to a range type
    static std::string type_to_label(range_type rt_);

    /// Return the range type associated to a label
    static range_type label_to_type(const std::string &);

    /// Return the min start angle
    static double min_start_angle(range_type);

    /// Return the max start angle
    static double max_start_angle(range_type);

    /// Default constructor
    explicit angular_range(range_type rt_ = RANGE_TYPE_INVALID);

    /// Default constructor
    explicit angular_range(const std::string & type_label_);

    /// Constructor
    angular_range(range_type rt_, double start_angle_, double delta_angle_);

    /// Check if the range type is set
    bool has_type() const;

    /// Set the range type
    void set_type(range_type);

    /// Return the range type
    range_type get_type() const;

    /// Check if the type of the range is 'polar'
    bool is_polar() const;

    /// Check if the type of the range is 'azimuthal'
    bool is_azimuthal() const;

    /// Return the min start angle
    double get_min_start_angle() const;

    /// Return the max start angle
    double get_max_start_angle() const;

    /// Check the start angle
    bool has_start_angle() const;

    /// Set the start angle
    void set_start_angle(double);

    /// Return the start angle
    double get_start_angle() const;

    /// Check the dedislta angle
    bool has_delta_angle() const;

    /// Set the delta angle
    void set_delta_angle(double);

    /// Return the delta angle
    double get_delta_angle() const;

    /// Check for the partial angle
    bool is_partial() const;

    /// Set angles
    void set_partial_angles(double start_angle_, double delta_angle_);

    /// Invalidate
    void reset_partial_angles();

    /// Return the first angle
    double get_first_angle() const;

    /// Return the last angle
    double get_last_angle() const;

    /// Return the angular spread
    double get_angle_spread() const;

    /// Check if a given angle is contains in the range
    bool contains(double angle_, double tolerance_ = 0.0, bool strict_range_ = false) const;

    /// Check if the range is valid
    bool is_valid() const;

    /// Invalidate
    void invalidate();

    /// Initialize from a set of parameters
    void initialize(const datatools::properties & config_);

    /// Reset
    void reset();

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &,
                               const std::string & prefix_ = "");

    /// \brief Forward iterator associated to an angular range
    class iterator :  public datatools::i_tree_dumpable {
    public:

      /// Default constructor
      /// @arg the angular range to be traversed
      /// @arg the number of divisions
      iterator(const angular_range & ar_, unsigned int nsteps_ = 36);

      /// Return the number of angle samples (number of steps + 1)
      /// @return the number of angle samples
      unsigned int get_nsamples() const;

      /// Return the current step index
      int get_current_step() const;

      /// Return the current angle value
      double get_current_angle() const;

      /// Check if the iterator is positioned at the first sample
      bool is_at_first() const;

      /// Check if the iterator is positioned at the last sample
      bool is_at_last() const;

      /// Check if the iterator is finished (past-the-end)
      bool is_finished() const;

      /// Set the iterator at first position
      void set_first();

      /// Set the iterator at last position
      void set_last();

      /// Set the iterator at finished (past-the-end)
      void set_finished();

      /// Increment the iterator position by one unit
      void step();

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                     const std::string & title_ = "",
                     const std::string & indent_ = "",
                     bool inherit_= false) const override;

      /// Indirection operator
      /// @return the current sampled angle
      double operator*() const;

      /// Increment operator
      iterator operator++();

      /// Check the validity of the iterator
      bool operator!() const;

    private:

      const angular_range & _ref_; ///< Reference to the traverwed angular range
      unsigned int _nsamples_; ///< Number of sampled angle (number of angular steps + 1)
      double _step_angle_; ///< Step angle value
      unsigned int _current_step_; ///< Current sample index
      double _current_angle_; ///< Value of the current sampled angle

    };

  protected:

    /// Set default value foe attibutes
    void _set_defaults();

  private:

    range_type _type_;        //!< Type of angular range ([0:pi] or [0:2*pi])
    double     _start_angle_; //!< The starting angle
    double     _delta_angle_; //!< The angular spread

    // Serialization interface:
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace geomtools

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/serialization/export.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
BOOST_CLASS_EXPORT_KEY2(geomtools::angular_range, "geomtools::angular_range")

/// OCD support declaration
// @param geomtools::angular_range the name the class with registered OCD support
DOCD_CLASS_DECLARATION(geomtools::angular_range)

#endif // GEOMTOOLS_ANGULAR_RANGE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
