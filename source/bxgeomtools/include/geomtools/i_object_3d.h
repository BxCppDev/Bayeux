// -*- mode: c++; -*- 
/* i_object_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface for object than can be described in a 3D reference frame.
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_object_3d_h
#define __geomtools__i_object_3d_h 1

#include <string>
#include <iostream>
#include <sstream>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  class box;

  class i_object_3d :
    public datatools::utils::i_tree_dumpable
    {
    public:
      enum dimensional_t
      {
	DIMENSIONAL_0 = 0,
	DIMENSIONAL_1 = 1,
	DIMENSIONAL_2 = 2,
	DIMENSIONAL_3 = 3
      };

    public:
      static const uint32_t ALL_SURFACES;
      static const uint32_t NO_SURFACES;
      static const double DEFAULT_TOLERANCE;
      static const double USING_PROPER_TOLERANCE;

    private:
      double __tolerance; // proper tolerance to check surface/curve belonging
      datatools::utils::properties __properties;

    public:

      // must be implemented from inherited classes:
      virtual int get_dimensional () const = 0;

      double get_tolerance () const;

      void set_tolerance (double tolerance_);

      datatools::utils::properties & properties ();

      const datatools::utils::properties & properties () const;

      virtual std::string get_shape_name () const = 0;

      virtual bool is_composite () const;

      virtual void compute_bounding_box (box & bb_);

    public:

      i_object_3d ();

      i_object_3d (double tolerance_);

      virtual ~i_object_3d ();

      virtual void reset ();

      virtual void tree_dump (ostream & out_         = clog, 
			      const string & title_  = "", 
			      const string & indent_ = "", 
			      bool inherit_          = false) const;
      
    };
    
} // end of namespace geomtools

#endif // __geomtools__i_object_3d_h

// end of i_object_3d.h
