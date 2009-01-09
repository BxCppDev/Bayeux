// -*- mode: c++; -*- 
/* physical_volume.h
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

#ifndef __geomtools__physical_volume_h
#define __geomtools__physical_volume_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

//#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/utils.h>

#include <geomtools/utils.h>
#include <geomtools/placement.h>
#include <geomtools/logical_volume.h>

namespace geomtools {

  class logical_volume;

  class physical_volume :
    public datatools::utils::i_tree_dumpable
  {
  public:
    static const std::string TOP_LOGICAL_NAME;

    /***************************************/
  public:
    typedef std::map<std::string,boost::shared_ptr<physical_volume> > physical_volume_dict;

    class table
    {
      physical_volume_dict __physical_dict;

    public:
      table ();

      virtual 
      ~table ();

      const physical_volume_dict & 
      get_dict () const
      {
	return __physical_dict;
      }
	  
      physical_volume_dict & 
      get_dict ()
      {
	return __physical_dict;
      }
	  
      bool
      has (const std::string & name_) const;
	  
      const physical_volume & 
      get (const std::string & name_) const;
	  
      physical_volume & 
      get (const std::string & name_);
	  
      void 
      add (physical_volume * physical_volume_);
	  
      void 
      remove (const std::string & name_);
	  
    };
      
    static const physical_volume & 
    get (const std::string & name_);
      
  private:
    static boost::scoped_ptr<table> g__table;

  public:
      
    static table & 
    get_table ();
      
    /*************************************************/
  private: 
    std::string      __name;
    placement        __placement;
    logical_volume * __logical;
    const logical_volume * __mother;
    datatools::utils::properties __properties;
      
  public: 
    const std::string &
    get_name () const;
      
    void 
    set_name (const std::string & name_);
      
    const placement & 
    get_placement () const;
      
    void 
    set_placement (const placement &);
 
    bool
    has_logical () const;
     
    const logical_volume &
    get_logical () const;
      
    logical_volume &
    get_logical ();
      
    void 
    set_logical (logical_volume & logical_);

    bool
    has_mother () const;
      
    const logical_volume &
    get_mother () const;

    void 
    set_mother (const logical_volume & mother_);
      
    const datatools::utils::properties &
    properties () const;
      
    datatools::utils::properties &
    properties ();
      
    //public: 

    // ctor/dtor:
    physical_volume ();
      
    physical_volume (const physical_volume &);
      
    physical_volume & operator= (const physical_volume &);

  public: 
    virtual 
    ~physical_volume ();
      
  public: 
    // virtual void 
    //reset ();
      
    virtual void 
    tree_dump (std::ostream & out_         = std::cerr , 
	       const std::string & title_  = "" ,
	       const std::string & indent_ = "",
	       bool inherit_               = false) const;
      
    static void
    create_physical_volume (const std::string & name_,
			    const placement & placement_,
			    const std::string & logical_name_,
			    const std::string & mother_logical_name_ 
			    = physical_volume::TOP_LOGICAL_NAME);
      
    static void 
    print_table_of_physical_volumes (std::ostream & out_);

  };

} // end of namespace geomtools

#endif // __geomtools__physical_volume_h

// end of physical_volume.h
