// -*- mode: c++; -*- 
/* logical_volume.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-25
 * Last modified: 2008-05-29
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__logical_volume_h
#define __geomtools__logical_volume_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/utils.h>

#include <geomtools/utils.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/material.h>

namespace geomtools {

  class logical_volume :
    public datatools::utils::i_tree_dumpable
  {
      
    /***************************************/
  public:

    typedef std::map<std::string, boost::shared_ptr<logical_volume> > logical_volume_dict;

    class table
    {
      logical_volume_dict __logical_dict;

    public:
      table ();

      virtual 
      ~table ();
	  
      const logical_volume_dict & 
      get_dict () const
      {
	return __logical_dict;
      }
	  
      logical_volume_dict & 
      get_dict ()
      {
	return __logical_dict;
      }

      bool
      has (const std::string & name_) const;

      const logical_volume & 
      get (const std::string & name_) const;

      logical_volume & 
      get (const std::string & name_);

      void 
      add (logical_volume * logical_volume_);

      void 
      remove (const std::string & name_);

    };
      
    static const logical_volume & 
    get (const std::string & name_);
      
  private:
    static boost::scoped_ptr<table> g__table;
      
  public:
      
    static table & 
    get_table ();

    /**************************************************/

  private:
    bool              __locked;
    std::string       __name;
    const material *  __material;
    boost::shared_ptr<i_shape_3d> __own_shape; /* shape owned by the
						* logical
						*/

    const i_shape_3d *            __ext_shape; /* shape owned by
						* external 
						*/
    datatools::utils::properties  __properties; 

  public:
    bool 
    is_locked () const;

    void 
    lock ();

    void 
    unlock ();

    const std::string &
    get_name () const;

    void 
    set_name (const std::string & name_);

    const datatools::utils::properties &
    properties () const;

    datatools::utils::properties &
    properties ();

    const material &
    get_material () const;
     
    void 
    set_material (const std::string & material_name_);
      
    void
    set_shape (const i_shape_3d & shape_);
      
    void
    set_shape (i_shape_3d * shape_);
      
    bool
    has_shape () const;

    const i_shape_3d &
    get_shape () const;
      
    i_shape_3d &
    get_shape ();
	
  private:
    void
    __set_default_values ();

    //public:

    // ctor/dtor:
    logical_volume ();

    logical_volume (const logical_volume &);
 
    logical_volume & operator= (const logical_volume &);
  
  public:
    ~logical_volume ();
      
  public:
    virtual void 
    tree_dump (std::ostream & out_         = std::cerr , 
	       const std::string & title_  = "" ,
	       const std::string & indent_ = "",
	       bool inherit_               = false) const;

    /****************/

    static void
    create (const std::string & name_,
	    const std::string & material_name_,
	    const i_shape_3d & shape_,
	    const datatools::utils::properties & properties_);

    static void 
    print_table_of_logical_volumes (std::ostream & out_);

    static void 
    load_table_of_logical_volumes ();

  };

} // end of namespace geomtools

#endif // __geomtools__logical_volume_h

// end of logical_volume.h
