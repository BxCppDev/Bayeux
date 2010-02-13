// -*- mode: c++ ; -*- 
/* physical_volume.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   Geometry ID
 * 
 * History: 
 * 
 */

#ifndef __geomtools__physical_volume_h
#define __geomtools__physical_volume_h 1

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/logical_volume.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;
 
  class physical_volume
    : public datatools::utils::i_tree_dumpable
  {
  private:
    string     __name;
    bool       __locked;
    properties __parameters;
    placement  __placement;
    bool       __own_logical;
    const logical_volume * __logical;
    const logical_volume * __mother;

  private:
    
    void __clear_logical ();
    
  public:

    bool is_locked () const;

    void lock ();

    void unlock ();

    const string & get_name () const;

    void set_name (const string &);

    const properties & parameters () const;

    properties & parameters ();

    void set_placement (const placement &);

    const placement & get_placement () const;

    placement & get_placement ();

    bool has_logical () const;

    void set_logical (const logical_volume &);

    void set_logical (const logical_volume *);

    const logical_volume & get_logical () const;

    bool has_mother () const;

    void set_mother (const logical_volume &);

    const logical_volume & get_mother () const;

    physical_volume ();

    physical_volume (const string &);

    physical_volume (const string &, 
		     const logical_volume & logical_,
		     const logical_volume & mother_,
		     const placement & placement_);

    physical_volume (const string &, 
		     const logical_volume * logical_,
		     const logical_volume & mother_,
		     const placement & placement_);

    virtual ~physical_volume ();

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
      
  };

} // end of namespace geomtools

#endif // __geomtools__physical_volume_h

// end of physical_volume.h
