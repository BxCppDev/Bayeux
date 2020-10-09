/// \file geomtools/id_mgr.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2016-07-04
 *
 * License:
 *
 * Description:
 *
 *   Geometry ID manager
 *
 */

#ifndef GEOMTOOLS_ID_MGR_H
#define GEOMTOOLS_ID_MGR_H 1

// Standard library:
#include <string>
#include <vector>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/logger.h>
#include <datatools/multi_properties.h>

// This project:
#include <geomtools/geom_id.h>

namespace geomtools {

  /** A manager class for geometry identifiers (GID, class : 'geom_id')
   *  and their relationship described through objects of 'category_info'
   * class
   */
  class id_mgr
    : public datatools::i_tree_dumpable
  {

  public:

    static const std::string & category_key_label();
    static const std::string & type_meta_label();
    static const std::string & default_world_category();
    static const std::string & default_plain_category();
    static const std::string & plain_item_address();
    static const uint32_t WORD_TYPE  = 0;
    static const uint32_t PLAIN_TYPE = 1;

    /** A class that documents the hierarchical relationship
     *  of a geometry category :
     *  - category (unique string identifier)
     *  - type (unique numerical identifier)
     *  - inheritance (parent category), ancestor (+grand-parents)
     *  - addressing fields with labels
     */
    class category_info
      : public datatools::i_tree_dumpable
    {

    public:

      /// Check if a subaddress with a given label exists at given occurence
      bool has_subaddress(const std::string & a_label, int a_count = 1) const;

      /// Fetch the subaddress associated to a given label at given occurence
      int get_subaddress_index(const std::string & a_label, int a_count = 1) const;

      /// Check if the category data are valid
      bool is_valid() const;

      // Check if the category is final (cannot be inherited)
      bool is_final_category() const;

      // Set the final category flag
      void set_final_category(bool);

      // Set the category name
      void set_category(const std::string & cat_);

      /// Get the name of the category
      const std::string & get_category() const;

      /// Check if ancestor with a given category name exists
      bool has_ancestor(const std::string & cat_) const;

      /// Add an ancestor with a given category name dexists
      void add_ancestor(const std::string & cat_);

      /// Add an address
      void add_address(const std::string & address_);

      /// Add an extends by
      void add_extends_by(const std::string & by_);

      /// Check if a group is defined
      bool has_group() const;

      /// Set the group
      void set_group(const std::string &);

      /// Return the group
      const std::string & get_group() const;

      /// Set the type
      void set_type(uint32_t);

      /// Get the type
      uint32_t get_type() const;

      /// Get the name of the parent category (if any)
      const std::string & get_inherits() const;

      /// Set the inherited parent category
      void set_inherits(const std::string &);

      /// Check if a parent category exists
      bool is_inherited() const;

      /// Check if the category extends another one
      bool is_extension() const;

      /// Set the extended parent category
      void set_extends(const std::string &);

      /// Get the name of the extended category (if any)
      const std::string & get_extends() const;

      void set_ancestors(const std::vector<std::string> &);

      const std::vector<std::string> & get_ancestors() const;

      void set_extends_by(const std::vector<std::string> &);

      const std::vector<std::string> & get_extends_by() const;

      void set_addresses(const std::vector<std::string> &);

      const std::vector<std::string> & get_addresses() const;

      /// Constructor
      category_info();

      /// Destructor
      ~category_info() override;

      /// Returns the size of the list of addresses
      size_t get_depth() const;

      /// Returns the size of the list of extending addresses
      size_t get_by_depth() const;

      /// Create a geom ID with the proper type and depth following the description of the category
      void create(geom_id & id_) const;

      /// Default print
      void dump(std::ostream & = std::clog) const;

      /// Smart print
      //!
      //! Supported options:
      //! \code
      //! {
      //!   "title"    : "My title: ",
      //!   "indent"   : "[debug] ",
      //!   "inherit"  : false,
      //!   "list_categories" : true,
      //!   "full_categories" : true
      //! }
      //! \endcode
      void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;

      /// Check lock status
      bool is_locked() const;

      /// Lock the category
      void lock();

      /// Reset the category
      void reset();

    private:
      bool        _locked_;                  ///< Lock flag
      std::string _category_;                ///< human readable category label
      uint32_t    _type_;                    ///< unique integral ID
      std::string _group_;                   ///< Group XXX
      std::string _inherits_;                ///< the mother category from which the category is inherited
      std::string _extends_;                 ///< the mother category from which the category is extented
      bool        _final_category_;          ///< Final category flag
      std::vector<std::string> _ancestors_;  ///< the list of ancestor categories
      std::vector<std::string> _extends_by_; ///< the addresses added by the extension
      std::vector<std::string> _addresses_;  ///< the full list of addresses
      std::vector<int>         _nbits_;      ///< the number of bits used to encode addresses

    };

    /***************************************/

    typedef std::map<std::string, category_info> categories_by_name_col_type;
    typedef std::map<int, const category_info *> categories_by_type_col_type;

  public:

    bool is_debug() const;

    void set_debug(bool);

    const categories_by_type_col_type & categories_by_type() const;

    const categories_by_name_col_type & categories_by_name() const;

    /// Constructor
    id_mgr();

    /// Destructor
    ~id_mgr() override;

    void set_force_world(bool);

    bool is_force_world() const;

    void set_force_plain(bool);

    bool is_force_plain() const;

    /// Set the world category name
    void set_world_category(const std::string & n_);

    /// Return the world category name
    const std::string & get_world_category() const;

    /// Set the plain category name
    void set_plain_category(const std::string & n_);

    /// Return the plain category name
    const std::string & get_plain_category() const;

    /// Initialize the manager from a properties multi container
    void init_from(const datatools::multi_properties & mp_);

    /// Load manager configuration from a file
    void load(const std::string & filename_);

    // /// Smart print
    // virtual void tree_dump(std::ostream & out_         = std::clog,
    //                        const std::string & title_  = "",
    //                        const std::string & indent_ = "",
    //                        bool inherit_               = false) const;

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                            const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;

    /// Check if some category's description exists for a given type
    bool has_category_info(int type_) const;

    /// Check if some category's description exists for a given category name
    bool has_category_info(const std::string &) const;

    /// Get the category descriptor assocated to a category associated to a given type
    const category_info & get_category_info(int type_) const;

    /// Get the category descriptor assocated to a category associated to a given name
    const category_info & get_category_info(const std::string &) const;

    /// Check the category of an ID
    bool is_category(const geom_id & id_, const std::string & category_) const;

    /// Check if an ID inherits a category
    bool inherits(const geom_id & id_, const std::string & category_) const;

    /// Check is an ID has an subaddress labelled 'what':
    bool has(const geom_id &, const std::string & what_) const;

    /// Returns the category associated to an ID
    const std::string & get_category(const geom_id &) const;

    /// Return the type associated to a category
    uint32_t get_category_type(const std::string & a_category) const;

    /// Returns the value of the address labelled 'what' from an ID
    int get(const geom_id &, const std::string & what_) const;

    /// Set the value of the address labelled 'what' from an ID
    void set(geom_id &, const std::string & what_, uint32_t value_) const;

    /// Check if an ID inherits or extends a candidate mother ID
    bool check_inheritance(const geom_id & mother_id_,
                           const geom_id & id_) const;

    /** Extract the part of the address of some child ID that
     *  is inherited from a mother ID.
     */
    void extract(const geom_id & child_id_, geom_id & mother_id_) const;

    /// Check if a given ID is valid and managed
    bool validate_id(const geom_id & id_) const;

    /// String formatting
    void id_to_human_readable_format_string(const geom_id & id_, std::string &) const;

    /// String formatting
    std::string id_to_human_readable_format_string(const geom_id & id_) const;

    /// Factory method that ... (I don't know what for !!!)
    void make_id(geom_id & id_,
                 uint32_t address_) const;

    /// Factory method that initialize a GID of a given category by name
    void make_id(const std::string & category_, geom_id & id_) const;

    /// Print the list of categories
    int print_list_of_categories(const std::string & command_,
                                 const std::vector<std::string> & argv_,
                                 std::ostream & out_ = std::clog) const;

    /*
      void make_extended(const geom_id & mother_id_,
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
    int compute_id_from_info(geom_id                     & id_,
                             const geom_id               & mother_id_,
                             const std::string           & id_info_,
                             const std::vector<uint32_t> & items_index_) const;

    int compute_id_from_info(geom_id           & id_,
                             const geom_id     & mother_id_,
                             const std::string & id_info_,
                             uint32_t nitem0_ = geom_id::INVALID_ADDRESS,
                             uint32_t nitem1_ = geom_id::INVALID_ADDRESS,
                             uint32_t nitem2_ = geom_id::INVALID_ADDRESS) const;

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

    /// Check lock status
    bool is_initialized() const;

    /// Initialize the manager from a properties container
    void initialize(const datatools::properties & config_);

    /// Reset/invalidate the manager
    void reset();

  protected:

    virtual void _at_initialize();

  private:

    // Management;
    datatools::logger::priority _logging_priority_;    ///< Logging priority threshold
    bool                        _initialized_;         ///< Initialization flag

    // Configuration:
    bool                        _force_world_;         ///< Force world category
    bool                        _force_plain_;         ///< Force plain category
    std::string                 _world_category_;      ///< World category name
    std::string                 _plain_category_;      ///< Plain category name
    std::vector<std::string>    _categories_lists_;    ///< List of files wich contains definitions of categories

    // Working data:
    categories_by_name_col_type _categories_by_name_;  ///< Dictionary of categories keyed by name
    categories_by_type_col_type _categories_by_type_;  ///< Dictionary of categories keyed by type

  };

} // end of namespace geomtools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::id_mgr)

#endif // GEOMTOOLS_ID_MGR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
