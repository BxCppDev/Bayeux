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

#include <string>
#include <vector>
#include <map>

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

    static bool g_devel;

    /***************************************/

    class category_info : public datatools::utils::i_tree_dumpable
    {
    public:
      string category;  // human readable category label
      int    type;      // unique integral ID
      string inherits;  // the mother category from which the category is inherited
      string extends;   // the mother category from which the category is extented
      vector<string> ancestors;  // the list of ancestor categories
      vector<string> extends_by; // the addresses added by the extension
      vector<string> addresses;  // the full list of addresses
      vector<int>    nbits;      // the number of bits used to encode addresses

    public:

      bool has_subaddress (const string & a_label, int a_count = 1) const;

      int get_subaddress_index (const string & a_label, int a_count = 1) const;

      bool is_valid () const;

      const string & get_category () const;

      bool has_ancestor (const string & cat_) const;

      void add_ancestor (const string & cat_);

      int get_type () const;

      const string & get_inherits () const;

      bool is_inherited () const;

      bool is_extension () const;

      const string & get_extends () const;
 
      // ctor:
      category_info ();

      //! returns the size of the list of addresses
      size_t get_depth () const;

      //! returns the size of the list of extending addresses
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

    void initialize (const datatools::utils::multi_properties & mp_);

    void load (const string & filename_);

    void reset ();

    virtual void tree_dump (ostream & out_         = clog,
			    const string & title_  = "",
			    const string & indent_ = "",
			    bool inherit_          = false) const;

    bool has_category_info (int) const;

    bool has_category_info (const string &) const;

    const category_info & get_category_info (int) const;

    const category_info & get_category_info (const string &) const;

    //! check the category of an ID:
    bool is_category (const geom_id & id_, const string & category_) const;

    //! check is an ID has an address 'what':
    bool has (const geom_id &, const string & what_) const;

    //! returns the category associated to an ID:
    const string & get_category (const geom_id &) const;

    //! return the type associated to a category:
    uint32_t get_category_type (const string & a_category) const;

    //! returns the value of the address 'what' from an ID:
    int get (const geom_id &, const string & what_) const;

    //! set the value of the address 'what' from an ID:
    void set (geom_id &, const string & what_, uint32_t value_) const;

    //! check if an ID inherits or extends a candidate mother ID:
    bool check_inheritance (const geom_id & mother_id_,
			    const geom_id & id_) const;

    //! extract the part of the address of some child ID that
    //  is inherited from a mother ID.
    //
    void extract (const geom_id & child_id_, geom_id & mother_id_) const;

    bool validate_id (const geom_id & id_) const;

    void id_to_human_readable_format_string (const geom_id & id_, string &) const;

    string id_to_human_readable_format_string (const geom_id & id_) const;

    void make_id (geom_id & id_,
		  uint32_t address_) const;

    void make_id (const string & category_, geom_id & id_) const;

    /*
    void make_extended (const geom_id & mother_id_,
			geom_id & id_,
			uint32_t address_) const;
    */

    /** Given a string representing the geometry ID "[module:module=0]"
     *  and the ID of the mother volume, one computes the full geometry ID
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
