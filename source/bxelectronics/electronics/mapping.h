// -*- mode: c++; -*-
#ifndef ELECTRONICS_MAPPING_H
#define ELECTRONICS_MAPPING_H

// Standard library
#include <string>
#include <map>

// Third party
// - Bayeux/datatools
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>
// - Bayeux/geomtools
#include <geomtools/id_mgr.h>
#include <geomtools/geom_id.h>

// This project
#include <electronics/component_info.h>

namespace electronics {

  // Forward declaration
  class component_manager;
  class component_model_base;

  /// \brief The mapping class for the electronic components in a setup
  class mapping : public datatools::i_tree_dumpable
  {
  public:

    /// Dictionary of component informations
    typedef std::map<geomtools::geom_id, component_info> component_info_dict_type;

    /// Constant representing unlimited mapping depth
    static const unsigned int depth_no_limit();

    /// Constant representing the default electronic setup category
    static const std::string & default_setup_category();

    /// Default constructor
    mapping();

    /// Destructor
    virtual ~mapping();

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority);

    bool has_manager() const;

    void set_manager(const component_manager & mgr_);

    const component_manager & get_manager() const;

    bool has_eid_manager() const;

    void set_eid_manager(const geomtools::id_mgr & mgr_);

    const geomtools::id_mgr & get_eid_manager() const;

    void set_max_depth(unsigned max_depth_);

    unsigned int get_max_depth() const;

    void set_top_level_name(const std::string &);

    const std::string & get_top_level_name() const;

    bool is_mode_only() const;

    bool is_mode_excluded() const;

    void add_only(const std::string & category_);

    void add_excluded(const std::string & category_);

    bool is_initialized() const;

    void configure(const datatools::properties & config_);

    void initialize();

    void reset();

    void build(const std::string & setup_ = "");

    bool validate_eid(const geomtools::geom_id & id_) const;

    const component_info & get_component_info (const geomtools::geom_id &) const;

    bool has_component_info (const geomtools::geom_id &) const;

    /// Smart print
    virtual void tree_dump(std::ostream& out         = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit              = false) const;

  protected:

    void _build();

    void _build_embedded_components(const component_model_base & mother_,
                                    const geomtools::geom_id & mother_eid_);

  private:

    // Configuration attributes:
    datatools::logger::priority  _logging_;     //!< Logging priority threshold
    bool                         _initialized_; //!< Initialization flag
    const component_manager  *   _manager_;     //!< Handle to the component model manager
    bool                         _top_level_mapping_;
    unsigned int                 _max_depth_;   //!< Maximum mapping depth at build
    std::string                  _top_level_name_; //!< Name of the top-level component model
    std::vector<std::string>     _only_categories_; //!< List of electronic component categories to be mapped
    std::vector<std::string>     _excluded_categories_; //!< List of electronic component categories to be excluded from the mapping

    // Dynamic attributes:
    const component_model_base * _top_level_component_; //!< Handle to the top-level component model
    const geomtools::id_mgr *    _eid_manager_; //!< EID manager
    unsigned int                 _depth_;       //!< Running depth at build
    component_info_dict_type     _component_infos_; //!< Dictionnary of information about electronic components in the setup

  };

} // end of namespace electronics

#endif // ELECTRONICS_MAPPING_H
