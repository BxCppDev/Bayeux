// -*- mode: c++ ; -*- 
/* utils.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 */

#ifndef GENVTX_UTILS_H_
#define GENVTX_UTILS_H_ 1

#include <string>

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

    double weight;
    double cumulated_weight;
    const geomtools::geom_info * ginfo;

  public:

    weight_entry_type ();

    bool is_valid () const;

    void reset ();

    void invalidate ();

  };

  struct weight_info
  {
    enum 
      {
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

    weight_info ();

  public:

    double value; //<! Surface or volume (in CLHEP units)
    double mass;  //<! Mass (in CLHEP units)
    int    type;  //<! Type of generator: SURFACE, VOLUME (BULK)

  };

} // end of namespace genvtx

#endif // GENVTX_UTILS_H_

// end of utils.h
