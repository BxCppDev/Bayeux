// -*- mode: c++ ; -*-
/* id_mgr.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2010-03-14
 *
 * License:
 *
 * Description:
 *
 *   Geometry ID manager
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ID_MGR_H_
#define GEOMTOOLS_ID_MGR_H_ 1

#include <string>
#include <vector>
#include <map>

#include <datatools/utils.h>
#include <datatools/logger.h>

#include <datatools/multi_properties.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  /** A manager class for geometry identifiers (GID, class : 'geom_id')
   *  and their relationship described through objects of 'category_info'
   * class
   */
  class id_mgr : public datatools::i_tree_dumpable
  {

  public:

    static const std::string CATEGORY_KEY_LABEL;
    static const std::string TYPE_META_LABEL;
    static const std::string DEFAULT_WORLD_CATEGORY;
    static const int         WORD_TYPE;

    /** A class that documents the hierarchical relationship
     *  of a geometry category :
     *  - category (unique string identifier)
     *  - type (unique numerical identifier)
     *  - inheritance (parent category), ancestor (+grand-parents)
     *  - addressing fields with labels
     */
    class category_info : public datatools::i_tree_dumpable
    {
    public:
      std::string category;                /// human readable category label
      int         type;                    /// unique integral ID
      std::string inherits;                /// the mother category from which the category is inherited
      std::string extends;                 /// the mother category from which the category is extented
      std::vector<std::string> ancestors;  /// the list of ancestor categories
      std::vector<std::string> extends_by; /// the addresses added by the extension
      std::vector<std::string> addresses;  /// the full list of addresses
      std::vector<int>         nbits;      /// the number of bits used to encode addresses

    public:

      /// Check if a subaddress with a given label exists at given occurence
      bool has_subaddress (const std::string & a_label, int a_count = 1) const;

      /// Fetch the subaddress associated to a given label at given occurence
      int get_subaddress_index (const std::string & a_label, int a_count = 1) const;

      /// Check if the category data are valid
      bool is_valid () const;

      /// Get the name of the category
      const std::string & get_category () const;

      /// Check if ancestor with a given category name exists
      bool has_ancestor (const std::string & cat_) const;

      /// Add an ancestor with a given category name dexists
      void add_ancestor (const std::string & cat_);

      /// Get the type
      int get_type () const;

      /// Get the name of the parent category (if any)
      const std::string & get_inherits () const;

      /// Check if a parent category exists
      bool is_inherited () const;

      /// Check if the category extends another one
      bool is_extension () const;

      /// Get the name of the extended category (if any)
      const std::string & get_extends () const;

      /// Constructor
      category_info ();

      /// Destructor
      virtual ~category_info ();

      /// Returns the size of the list of addresses
      size_t get_depth () const;

      /// Returns the size of the list of extending addresses
      size_t get_by_depth () const;

      /// Create a geom ID with the proper type and depth followinf the description of the category
      void create (geom_id & id_) const;

      /// Default print
      void dump (std::ostream & = std::clog) const;

      /// Smart print
      virtual void tree_dump (std::ostream & out_        = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_          = false) const;

    };

    /***************************************/

    typedef std::map<std::string, category_info> categories_by_name_col_type;
    typedef std::map<int, const category_info *> categories_by_type_col_type;

  public:

    bool is_debug () const;

    void set_debug (bool);

    const categories_by_type_col_type & categories_by_type () const;

    const categories_by_name_col_type & categories_by_name () const;

    /// Constructor
    id_mgr ();

    /// Destructor
    virtual ~id_mgr ();

    /// Initialize the manager from a properties multi container
    void init_from (const datatools::multi_properties & mp_);

    /// Initialize the manager from a properties multi container
    void initialize (const datatools::multi_properties & mp_);

    /// Load manager configuration from a file
    void load (const std::string & filename_);

    /// Reset/invalidate the manager
    void reset ();

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    /// Check if some category's description exists for a given type
    bool has_category_info (int type_) const;

    /// Check if some category's description exists for a given category name
    bool has_category_info (const std::string &) const;

    /// Get the category descriptor assocated to a category associated to a given type
    const category_info & get_category_info (int type_) const;

    /// Get the category descriptor assocated to a category associated to a given name
    const category_info & get_category_info (const std::string &) const;

    /// Check the category of an ID
    bool is_category (const geom_id & id_, const std::string & category_) const;

    /// Check if an ID inherits a category
    bool inherits (const geom_id & id_, const std::string & category_) const;

    /// Check is an ID has an subaddress labelled 'what':
    bool has (const geom_id &, const std::string & what_) const;

    /// Returns the category associated to an ID
    const std::string & get_category (const geom_id &) const;

    /// Return the type associated to a category
    uint32_t get_category_type (const std::string & a_category) const;

    /// Returns the value of the address labelled 'what' from an ID
    int get (const geom_id &, const std::string & what_) const;

    /// Set the value of the address labelled 'what' from an ID
    void set (geom_id &, const std::string & what_, uint32_t value_) const;

    /// Check if an ID inherits or extends a candidate mother ID
    bool check_inheritance (const geom_id & mother_id_,
                            const geom_id & id_) const;

    /** Extract the part of the address of some child ID that
     *  is inherited from a mother ID.
     */
    void extract (const geom_id & child_id_, geom_id & mother_id_) const;

    /// Check if a given ID is valid and managed
    bool validate_id (const geom_id & id_) const;

    /// String formatting
    void id_to_human_readable_format_string (const geom_id & id_, std::string &) const;

    /// String formatting
    std::string id_to_human_readable_format_string (const geom_id & id_) const;

    /// Factory method that ... (I don't know what for !!!)
    void make_id (geom_id & id_,
                  uint32_t address_) const;

    /// Factory method that initialize a GID of a given category by name
    void make_id (const std::string & category_, geom_id & id_) const;

    /*
      void make_extended (const geom_id & mother_id_,
      geom_id & id_,
      uint32_t address_) const;
    */

    /** Given a std::string representing the geometry ID "[module:module=0]"
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
    int compute_id_from_info (geom_id                     & id_,
                              const geom_id               & mother_id_,
                              const std::string           & id_info_,
                              const std::vector<uint32_t> & items_index_) const;

    int compute_id_from_info (geom_id           & id_,
                              const geom_id     & mother_id_,
                              const std::string & id_info_,
                              uint32_t nitem0_ = geom_id::INVALID_ADDRESS,
                              uint32_t nitem1_ = geom_id::INVALID_ADDRESS,
                              uint32_t nitem2_ = geom_id::INVALID_ADDRESS) const;

    datatools::logger::priority get_logging_priority () const;

    void set_logging_priority (datatools::logger::priority);

  private:

    datatools::logger::priority _logging_priority_;
    categories_by_name_col_type _categories_by_name_; /// Dictionnary of categories keyed by name
    categories_by_type_col_type _categories_by_type_; /// Dictionnary of categories keyed by type

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::id_mgr)

#endif // GEOMTOOLS_ID_MGR_H_

// end of id_mgr.h
