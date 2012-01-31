// -*- mode: c++; -*- 
/* element.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-25
 * Last modified: 2008-05-25
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__element_h
#define __geomtools__element_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/utils.h>

#include <geomtools/utils.h>

namespace geomtools {

  class element :
    public datatools::utils::i_tree_dumpable
  {

  private:
    bool        __locked;
    std::string __name;
    std::string __symbol;
    double      __A;
    double      __Z;
    datatools::utils::properties __properties;
      
  public:
    typedef std::map<std::string,element> element_dict;

    class table
    {
      element_dict __elements;

    public:
      table ();

      virtual 
      ~table ();

      const element_dict & get_dict () const
      {
	return __elements;
      }

      const element *
      get (const std::string & name_) const;

      element *
      access_element (const std::string & name_);

      void 
      add (const element & element_);

      bool 
      has (const std::string & name_) const;

    };

    static const element * 
    get (const std::string & name_);
      
  private:
    static boost::scoped_ptr<table> g__table;

  public:
      
    static table & 
    get_table ();

  public:
    bool 
    is_locked () const;

    void 
    lock ();

    double
    get_A () const;

    void 
    set_A (double);

    double
    get_Z () const;

    void 
    set_Z (double);

    const std::string &
    get_name () const;

    void 
    set_name (const std::string & name_);

    const std::string &
    get_symbol () const;

    void 
    set_symbol (const std::string & name_);

    const datatools::utils::properties &
    properties () const;

    datatools::utils::properties &
    properties ();

  private:
    void
    __set_default_values ();

  public:

    // ctor/dtor:
    element ();

    ~element ();

    static void 
    create (const std::string & name_,
	   const std::string & symbol_,
	   double a_,
	   double z_);
      
    virtual void 
    tree_dump (std::ostream & out_         = std::cerr , 
	      const std::string & title_  = "" ,
	      const std::string & indent_ = "",
	      bool inherit_               = false) const;

    static void 
    print_table_of_elements (std::ostream & out_);

    static void 
    load_table_of_elements ();

  };

} // end of namespace geomtools

#endif // __geomtools__element_h

// end of element.h
