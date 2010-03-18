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
#include <map>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/logical_volume.h>
#include <geomtools/i_placement.h>

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
    bool                   __own_placement;
    const i_placement *    __placement;
    bool                   __own_logical;
    const logical_volume * __logical;
    const logical_volume * __mother;

  private:
    
    void __clear_logical ();
    
    void __clear_placement ();
    
  public:

    bool is_locked () const;

    void lock ();

    void unlock ();

    const string & get_name () const;

    void set_name (const string &);

    const properties & parameters () const;

    properties & parameters ();

    bool has_placement () const;

    void set_placement (const i_placement &);

    void set_placement (const i_placement *);

    const i_placement & get_placement () const;

    bool has_logical () const;

    void set_logical (const logical_volume &);

    void set_logical (const logical_volume *);

    const logical_volume & get_logical () const;

    bool has_mother () const;

    void set_mother (const logical_volume &);

    const logical_volume & get_mother () const;

    physical_volume ();

    physical_volume (const string & name_);

    physical_volume (const string & name_, 
		     const logical_volume & logical_,
		     const logical_volume & mother_,
		     const i_placement    & placement_);

    physical_volume (const string & name_, 
		     const logical_volume * logical_,
		     const logical_volume & mother_,
		     const i_placement    & placement_);
    
    physical_volume (const string & name_,   
		     const logical_volume & logical_,
		     const logical_volume & mother_,
		     const i_placement    * placement_);
    
    physical_volume (const string & name_,  
		     const logical_volume * logical_,
		     const logical_volume & mother_,
		     const i_placement    * placement_);

    virtual ~physical_volume ();

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
  public:

    typedef map<string, const physical_volume *> dict_t;
      
  };

} // end of namespace geomtools

#endif // __geomtools__physical_volume_h

// end of physical_volume.h
