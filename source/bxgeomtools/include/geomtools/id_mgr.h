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
#include <vector>

#include <boost/algorithm/string.hpp>

#include <datatools/utils/utils.h>
#include <datatools/utils/multi_properties.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  using namespace std;

  class id_mgr : public datatools::utils::i_tree_dumpable
  {

  public:

    static const string CATEGORY_KEY_LABEL;
    static const string TYPE_META_LABEL;

    /***************************************/

    class category_info : public datatools::utils::i_tree_dumpable
    {
    public:
      string category;  // human readable label
      int    type;      // unique integral ID
      string inherits;  // the category from which the category is inherited
      string extends;   // the category from which the category is extented
      vector<string> ancestors; // the list of ancestor categories
      vector<string> extends_by; // the addresses added by the extension
      vector<string> addresses;  // the full list of addresses

    public:

      bool is_valid () const
      {
	return type != geom_id::INVALID_TYPE;
      }

      const string & get_category () const
      {
	return category;
      }

      bool has_ancestor (const string & cat_) const
      {
	return (find (ancestors.begin (), ancestors.end(), cat_) 
		!= ancestors.end ());
      }
      
      void add_ancestor (const string & cat_)
      {
	if (! has_ancestor (cat_))
	  {
	    ancestors.push_back (cat_);
	  }
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
	return extends;
      }

      // ctor:
      category_info ();

      //! returns the size of the list of addresses
      size_t get_depth () const;

      //! returns the size of the list of extenting addresses
      size_t get_by_depth () const;

      //! create a geom ID with the proper type and depth
      // 
      void create (geom_id & id_) const;

      void dump (ostream & = clog) const;

      virtual void tree_dump (ostream & out_        = clog, 
			      const string & title_  = "",
			      const string & indent_ = "",
			      bool inherit_          = false) const;

    };

    /***************************************/
    
    typedef map<string, category_info>      categories_by_name_col_t;
    typedef map<int, const category_info *> categories_by_type_col_t;

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

    int get (const geom_id &, const string & what_) const;

    void set (geom_id &, const string & what_, uint32_t value_) const;

    bool check_inheritance (const geom_id & mother_id_, 
			    geom_id & id_) const;
 
    bool validate_id (const geom_id & id_) const;

    void id_to_human_readable_format_string (const geom_id & id_, string &) const;

    string id_to_human_readable_format_string (const geom_id & id_) const;

    void make_id (geom_id & id_, 
		  uint32_t address_) const;


    /*   
    void make_extended (const geom_id & mother_id_, 
			geom_id & id_, 
			uint32_t address_) const;
    */

    /** Given a string representing the geometry ID "[module:module=0]"
     *  and theID of the mother volume, one computes the full geometry ID
     *  of the daughter volume, eventually using the index of the item. 
     *
     *  Example:
     *  Assume `[100:3]' is the mother ID from the `module' category 
     *  (index=100) with module number = 3 (mother ID info is then
     *  "[module:module=3]"). Assume the child ID info is 
     *  "[submodule:submodule=4]" from the `submodule' category
     *  The resulting child ID is: `[110:3.4]' where 110 is the type
     *  of the `submodule' category.
     */
    int compute_id_from_info (geom_id & id_, 
			      const geom_id & mother_id_,
			      const string & id_info_, 
			      const vector<uint32_t> & items_index_) const;

    int compute_id_from_info (geom_id & id_, 
			      const geom_id & mother_id_,
			      const string & id_info_, 
			      uint32_t nitem0_ = geom_id::INVALID_ADDRESS, 
			      uint32_t nitem1_ = geom_id::INVALID_ADDRESS, 
			      uint32_t nitem2_ = geom_id::INVALID_ADDRESS) const;
  };

} // end of namespace geomtools

#endif // __geomtools__id_mgr_h

// end of id_mgr.h
