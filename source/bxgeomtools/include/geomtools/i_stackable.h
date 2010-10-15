// -*- mode: c++ ; -*- 
/* i_stackable.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry model with multiple stacked boxes.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_stackable_h
#define __geomtools__i_stackable_h 1

#include <iostream>
#include <string>
#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/units.h>

namespace geomtools {

  using namespace std;

  class i_stackable
  {

  public:
    
    virtual double get_xmin () const = 0;
    virtual double get_xmax () const = 0;
    virtual double get_ymin () const = 0;
    virtual double get_ymax () const = 0;
    virtual double get_zmin () const = 0;
    virtual double get_zmax () const = 0;
 
  };

  class stackable
  {
  public:
    static const string STACKABLE_PREFIX;
    static const string STACKABLE_LENGTH_UNIT_PROPERTY;
    static const string STACKABLE_XMIN_PROPERTY;
    static const string STACKABLE_XMAX_PROPERTY;
    static const string STACKABLE_YMIN_PROPERTY;
    static const string STACKABLE_YMAX_PROPERTY;
    static const string STACKABLE_ZMIN_PROPERTY;
    static const string STACKABLE_ZMAX_PROPERTY;
    static const string STACKABLE_PLAY_PROPERTY;
    static const string STACKABLE_LIMITS_PROPERTY;

    static string make_key (const string & flag_);

    static void extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_);

  };

  class stackable_data : public i_stackable
  {
  public:

    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

  public:

    bool is_valid () const;

    void invalidate ();

    stackable_data ();
  
    virtual double get_xmin () const {return xmin;}
    virtual double get_xmax () const {return xmax;}
    virtual double get_ymin () const {return ymin;}
    virtual double get_ymax () const {return ymax;}
    virtual double get_zmin () const {return zmin;}
    virtual double get_zmax () const {return zmax;}
   
    void dump (ostream & out_ = clog, const string & title_ = "") const;

    bool initialize (const datatools::utils::properties & config_);
		       
  };

} // end of namespace geomtools

#endif // __geomtools__i_stackable_h

// end of i_stackable.h
