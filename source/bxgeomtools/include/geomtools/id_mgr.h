// -*- mode: c++ ; -*- 
/* id_mgr.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 * 
 * License: 
 * 
 * Description: 
 *   Geometry ID manager
 * 
 * History: 
 * 
 */

#ifndef __geomtools__id_mgr_h
#define __geomtools__id_mgr_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <datatools/utils/utils.h>
#include <datatools/utils/multi_properties.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  using namespace std;

  class id_mgr :
    public datatools::utils::i_tree_dumpable

  {

  public:

    static const string CATEGORY_KEY_LABEL;
    static const string TYPE_META_LABEL;

    /***************************************/

    class category_info :
      public datatools::utils::i_tree_dumpable
    {
    public:
      string category;  // human readable label
      int    type;      // unique integral ID
      string inherits;
      string extends;
      vector<string> addresses;

    public:

      bool is_valid () const
      {
	return type != geom_id::INVALID_TYPE;
      }

      const string & get_category () const
      {
	return category;
      }

      int get_type () const
      {
	return type;
      }

      const string & get_inherits () const
      {
	return inherits;
      }

      bool is_inherited () const
      {
	return ! inherits.empty ();
      }

      bool is_extension () const
      {
	return ! extends.empty ();
      }

      const string & get_extends () const
      {
	return inherits;
      }

      // ctor:
      category_info ();

      size_t get_depth () const;

      void dump (ostream & = clog) const;

      virtual void tree_dump (ostream & out_        = clog, 
			      const string & title_  = "",
			      const string & indent_ = "",
			      bool inherit_          = false) const;

      void make (geom_id & id_) const;

    };

    /***************************************/
    
    typedef map<string, category_info> categories_by_name_col_t;
    typedef map<int, const category_info *>    categories_by_type_col_t;

  private: 
    bool __debug;
    categories_by_name_col_t __categories_by_name;
    categories_by_type_col_t __categories_by_type;

  public: 
    bool is_debug () const;
    void set_debug (bool);
    const categories_by_type_col_t & categories_by_type () const;
    const categories_by_name_col_t & categories_by_name () const;

  public: 

    // ctor:
    id_mgr ();

    // dtor:
    virtual ~id_mgr ();
  
    void init_from (const datatools::utils::multi_properties & mp_);

    void load (const string & filename_);

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "",
			    const string & indent_ = "",
			    bool inherit_          = false) const;

    bool has_category_info (int) const;

    bool has_category_info (const string &) const;
    
    const category_info & get_category_info (int) const;

    const category_info & get_category_info (const string &) const;

  };

} // end of namespace geomtools

#endif // __geomtools__id_mgr_h

// end of id_mgr.h
