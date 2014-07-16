/// \file genvtx/utils.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef GENVTX_UTILS_H
#define GENVTX_UTILS_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Boost:
#include <boost/type_traits/integral_constant.hpp>

namespace geomtools {
  class geom_info;
}

namespace genvtx {

  /// \brief Utilities for vertex generators
  struct utils
  {
  public :

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

    static void origin_invalidate(std::string &);
    static bool origin_is_invalid(const std::string &);

  };

  /// \brief Weight data for combined vertex generators
  struct weight_entry_type
  {
  public:

    /// Default constructor
    weight_entry_type();

    /// Check the validity of weight parameters
    bool is_valid() const;

    /// Reset the weight parameters
    void reset();

    /// Invalidate the weight parameters
    void invalidate();

    /// Return the weight
    double get_weight() const;

    /// Return the cumulated weight
    double get_cumulated_weight() const;

    /// Return a reference to the non mutable geometry info
    const geomtools::geom_info & get_ginfo() const;

    /// Check if geometry info is available
    bool has_ginfo() const;

  public:

    double weight; //!< Weight
    double cumulated_weight; //!< Cumulated weight
    const geomtools::geom_info * ginfo; //!< Handle to associated geometry data

  };

  /// \brief Information about the weighting of combined vertex generators
  struct weight_info
  {

    /// \brief Weighting type for combined vertex generators
    enum weighting_type {
      WEIGHTING_NONE    = 0, //!< No weighting
      WEIGHTING_SURFACE = 1, //!< weighting by surface
      WEIGHTING_VOLUME  = 2, //!< weighting by volume
      WEIGHTING_BULK    = WEIGHTING_VOLUME, //!< weighting by volume
      WEIGHTING_MASS    = 3 //!< weighting by mass
    };

    /// Invalidate the weighting informations
    void invalidate();

    /// Check if a weighting type is present
    bool has_type() const;

    /// Check for surface weigthing
    bool has_surface() const;

    /// Check for volume weigthing
    bool has_volume() const;

    /// Check for mass weigthing
    bool has_mass() const;

    /// Check the validity of weighting informations
    bool is_valid() const;

    /// Return the associated surface
    double get_surface() const;

    /// Return the associated volume
    double get_volume() const;

    /// Return the associated mass
    double get_mass() const;

    /// Set the associated mass
    void set_mass(double);

    /// Set the reference weighting value
    void set_value(double);

    /// Set the weighting type
    void set_type(int);

    /// Return the weighting type
    int get_type() const;

    /// Return the reference weighting value
    double get_value() const;

    /// Default constructor
    weight_info();

    /// Print
    void dump(std::ostream & = std::clog, const std::string & title_ = "") const;

  private:
    double value; //<! Surface or volume (in CLHEP units)
    double mass;  //<! Mass (in CLHEP units)
    int    type;  //<! Type of generator: SURFACE, VOLUME (BULK)

  };

  // /// \brief Default traits for support of vertex generator validator
  // template <typename Generator>
  // struct vertex_generator_validator_support_traits : public boost::false_type{};

  // template <typename Generator>
  // bool check_vertex_generator_validator_support(typename boost::disable_if< vertex_generator_validator_support_traits<Generator> >::type* dummy = 0) {
  //   if(!dummy) dummy=0;
  //   if (stag_ == T::serial_tag()) return true;
  //   if (! alt_tag_.empty())
  //     {
  //       if (stag_ == alt_tag_) return true;
  //     }
  //   return false;
  // }


} // end of namespace genvtx

#endif // GENVTX_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
