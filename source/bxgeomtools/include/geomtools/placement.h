// -*- mode: c++; -*- 
/* placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__placement_h
#define __geomtools__placement_h 1

#include <iostream>
#include <iomanip>
#include <string>

#include <datatools/utils/i_tree_dump.h>

#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  class placement
    : public datatools::utils::i_tree_dumpable
    {
    private: 
      vector_3d    __translation;      // absolute position in mother frame
      rotation_3d  __rotation;         // mother->child frame coord. transformation
      rotation_3d  __inverse_rotation; // child->mother frame coord. transformation
    public:
 
      bool is_valid () const;

      void invalidate ();

      const vector_3d & get_translation () const;

      void set_translation (double, double, double);

      void set_translation (const vector_3d &);

      const rotation_3d & get_rotation () const;

      const rotation_3d & get_inverse_rotation () const;

      void set_orientation (const rotation_3d &);

      void set_orientation (double , double , double);

      // ctor/dtor:
    public: 
      placement ();

      placement (const vector_3d & translation_, 
		 double phi_,
		 double theta_,
		 double delta_);

      placement (double x_,
		 double y_,
		 double z_, 
		 double phi_,
		 double theta_,
		 double delta_);

      virtual ~placement ();

      virtual void reset ();
  
      //  transformation methods:
      void mother_to_child (const vector_3d &, vector_3d &) const;

      vector_3d mother_to_child (const vector_3d &) const;

      void child_to_mother (const vector_3d &, vector_3d &) const;

      vector_3d child_to_mother (const vector_3d &) const;

      void mother_to_child_direction (const vector_3d &, vector_3d &) const;

      vector_3d mother_to_child_direction (const vector_3d &) const;

      void child_to_mother_direction (const vector_3d &, vector_3d &) const;

      vector_3d child_to_mother_direction (const vector_3d &) const;

      virtual void tree_dump (ostream & out_  = clog, 
			      const string & title_ = "geomutils::placement", 
			      const string & indent_ = "", 
			      bool inherit_ = false) const;

      void dump (ostream      & out_    = clog, 
		 const string & title_  = "geomutils::placement", 
		 const string & indent_ = "") const;

      /*
	void dump ( ostream & out_ , 
	const string & title_  = "geomutils::geom_data", 
	const string & indent_ = "" ,
	bool inherit_               =  false ) const;
      */
    
    };

} // end of namespace geomtools

#endif // __geomtools__placement_h

// end of placement.h
