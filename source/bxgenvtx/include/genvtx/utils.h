/// \file genvtx/utils.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef GENVTX_UTILS_H
#define GENVTX_UTILS_H 1

// Standard library:
#include <string>
#include <iostream>

namespace geomtools {
  class geom_info;
}

namespace genvtx {

  struct utils
  {
  public :

    static const int MODE_INVALID = -1;
    static const int MODE_BULK    =  0;
    static const int MODE_SURFACE =  1;
    static const int MODE_DEFAULT = MODE_BULK;

    static void origin_invalidate (std::string &);
    static bool origin_is_invalid (const std::string &);

  };

  struct weight_entry_type
  {
  public:

    weight_entry_type ();

    bool is_valid () const;

    void reset ();

    void invalidate ();
  public:

    double weight;
    double cumulated_weight;
    const geomtools::geom_info * ginfo;

  };

  struct weight_info
  {
    enum weighting_type {
      WEIGHTING_NONE    = 0,
      WEIGHTING_SURFACE = 1,
      WEIGHTING_VOLUME  = 2,
      WEIGHTING_BULK    = WEIGHTING_VOLUME,
      WEIGHTING_MASS    = 3
    };

    void invalidate ();

    bool has_type () const;

    bool has_surface () const;

    bool has_volume () const;

    bool has_mass () const;

    bool is_valid () const;

    double get_surface () const;

    double get_volume () const;

    double get_mass () const;

    void set_mass(double);

    void set_value(double);

    void set_type(int);

    int get_type() const;

    double get_value() const;

    weight_info ();

    void dump(std::ostream & = std::clog, const std::string & title_ = "") const;

  private:
    double value; //<! Surface or volume (in CLHEP units)
    double mass;  //<! Mass (in CLHEP units)
    int    type;  //<! Type of generator: SURFACE, VOLUME (BULK)

  };

} // end of namespace genvtx

#endif // GENVTX_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
