// -*- mode: c++ ; -*- 
/* logical_volume.h
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

#ifndef __geomtools__logical_volume_h
#define __geomtools__logical_volume_h 1

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;

  class physical_volume;

  class logical_volume
    : public datatools::utils::i_tree_dumpable
  {
  public:
    typedef map<string, const physical_volume *> physicals_col_t;

  private:
    string     __name;
    bool       __locked;
    properties __parameters;
    bool       __own_shape;
    const i_shape_3d * __shape;
    physicals_col_t  __physicals;

  private:
    
    void __clear_shape ();

  public:

    bool is_locked () const;

    void lock ();

    void unlock ();

    const string & get_name () const;

    void set_name (const string &);

    const properties & parameters () const;

    properties & parameters ();

    bool has_shape () const;

    void set_shape (const i_shape_3d &);

    void set_shape (const i_shape_3d *);

    const i_shape_3d & get_shape () const;

    logical_volume ();

    logical_volume (const string &);

    logical_volume (const string &, const i_shape_3d &);

    logical_volume (const string &, const i_shape_3d *);

    virtual ~logical_volume ();

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;

    bool has_physical (const string & name_) const;

    const physical_volume & get_physical (const string & name_) const;
      
    void add_physical (const physical_volume & phys_, const string & name_ = "");

  };

} // end of namespace geomtools

#endif // __geomtools__logical_volume_h

// end of logical_volume.h
