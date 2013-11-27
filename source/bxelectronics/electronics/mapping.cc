// Ourselves
#include <electronics/mapping.h>

// This project
#include <electronics/component_manager.h>
#include <electronics/mapping_utils.h>

namespace electronics {

  // static
  const unsigned int mapping::depth_no_limit()
  {
    static const unsigned int _value = 0;
    return _value;
  }

  // static
  const std::string & mapping::default_setup_category()
  {
    static const std::string _cat("setup");
    return _cat;
  }

  mapping::mapping()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    _initialized_ = false;
    _manager_ = 0;
    _max_depth_ = depth_no_limit();

    _eid_manager_ = 0;
    _top_level_component_ = 0;
    _depth_ = 0;
    return;
  }

  mapping::~mapping()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  datatools::logger::priority mapping::get_logging_priority() const
  {
    return _logging_;
  }

  void mapping::set_logging_priority(datatools::logger::priority l_)
  {
    _logging_ = l_;
    return;
  }

  bool mapping::has_manager() const
  {
    return _manager_ != 0;
  }


  void mapping::set_manager(const component_manager & mgr_)
  {
   DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");
    _manager_ = &mgr_;
    return;
  }

  const component_manager & mapping::get_manager() const
  {
    return *_manager_;
  }

  bool mapping::has_eid_manager() const
  {
    return _eid_manager_ != 0;
  }

  void mapping::set_eid_manager(const geomtools::id_mgr & eid_mgr_)
  {
   DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");
    _eid_manager_ = &eid_mgr_;
    return;
  }

  const geomtools::id_mgr & mapping::get_eid_manager() const
  {
    return *_eid_manager_;
  }

  void mapping::set_max_depth(unsigned int max_depth_)
  {
   DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");
    _max_depth_ = max_depth_;
    return;
  }

  unsigned int mapping::get_max_depth() const
  {
    return _max_depth_;
  }

  bool mapping::is_mode_only () const
  {
    return _only_categories_.size();
  }

  bool mapping::is_mode_excluded () const
  {
    return _excluded_categories_.size();
  }

  void mapping::add_only(const std::string & category_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Mapping is already initialized !");
    DT_THROW_IF (is_mode_excluded(), std::logic_error, "Mapping uses 'excluded' mode !");
    _only_categories_.push_back(category_);
    return;
  }

  void mapping::add_excluded(const std::string & category_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");
    DT_THROW_IF(is_mode_only(), std::logic_error, "Mapping uses 'only' mode !");
    _excluded_categories_.push_back(category_);
    return;
  }

  void mapping::set_top_level_name(const std::string & name_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");
    _top_level_name_ = name_;
    return;
  }

  const std::string & mapping::get_top_level_name() const
  {
    return _top_level_name_;
  }

  bool mapping::is_initialized() const
  {
    return _initialized_;
  }

  void mapping::configure(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");

    // Parse logging parameteres:
    datatools::logger::priority p = datatools::logger::extract_logging_configuration(config_);
    set_logging_priority (p);

    if (config_.has_key("max_depth")) {
      int md = config_.fetch_integer("max_depth");
      DT_THROW_IF(md < 0, std::logic_error, "Invalid maximum mapping depth !");
      set_max_depth((unsigned int) md);
    }

    if (config_.has_key("top_level_mapping")) {
      _top_level_mapping_ = config_.fetch_boolean("top_level_mapping");
    }

    if (_excluded_categories_.size() == 0) {
      if (config_.has_key("only_categories")) {
        std::vector<std::string> onlys;
        config_.fetch("only_categories", onlys);
        for (int i = 0; i < onlys.size(); i++) {
          add_only(onlys[i]);
        }
      }
    }

    if (_only_categories_.size() == 0) {
      if (config_.has_key("excluded_categories")) {
        std::vector<std::string> excluded;
        config_.fetch("excluded_categories", excluded);
        for (int i = 0; i < excluded.size(); i++) {
          add_excluded(excluded[i]);
        }
      }
    }

    return;
  }

  void mapping::initialize()
  {
    std::cerr << "DEVEL: mapping::initialize: Entering..." << std::endl;
    DT_THROW_IF(is_initialized(), std::logic_error, "Mapping is already initialized !");

    DT_THROW_IF(! has_manager(), std::logic_error, "Missing electronics component manager !");

    DT_THROW_IF(! _manager_->is_initialized(), std::logic_error,
                "Electronics component manager is not initialized !");

    if (! has_eid_manager()) {
      set_eid_manager(_manager_->get_eid_manager());
    }

    build();

    _initialized_ = true;
    std::cerr << "DEVEL: mapping::initialize: Exiting." << std::endl;
    return;
  }

  void mapping::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Mapping is not initialized !");
    _initialized_ = false;

    _component_infos_.clear();
    _manager_ = 0;
    _only_categories_.clear();
    _excluded_categories_.clear();
    _max_depth_ = depth_no_limit();

    _eid_manager_ = 0;
    _top_level_component_ = 0;
    _depth_ = 0;

    return;
  }

  void mapping::build(const std::string & setup_name_)
  {
    std::cerr << "DEVEL: mapping::build: Entering..." << std::endl;
    std::string setup_name = setup_name_;
    if (setup_name.empty()) {
      setup_name = component_manager::default_top_level_name();
    }

    if (_top_level_component_ == 0) {
      std::string top_level_name = "setup";
      DT_THROW_IF(!_manager_->has(top_level_name),
                  std::logic_error,
                  "Cannot find the top-level component model with name '" << top_level_name
                  << "' in the virtual electronic component models manager !");
      _top_level_component_ = &(_manager_->get<component_model_base>(top_level_name));
      std::cerr << "DEVEL: mapping::build: Top level component @ " << _top_level_component_ << std::endl;
    }

    _build();

    std::cerr << "DEVEL: mapping::build: Exiting." << std::endl;
    return;
  }

  void mapping::tree_dump(std::ostream& out,
                          const std::string& title,
                          const std::string& a_indent,
                          bool inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Logging priority : '"
        << datatools::logger::get_priority_label(get_logging_priority())
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Top-level mapping : " << _top_level_mapping_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Max depth : " << _max_depth_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Manager : " << _manager_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "EID manager : " << _eid_manager_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Top-level component : " << _top_level_component_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Component information dictionary : " << _component_infos_.size() << std::endl;

    for (component_info_dict_type::const_iterator i = _component_infos_.begin();
         i != _component_infos_.end();
         i++) {
      component_info_dict_type::const_iterator j = i;
      j++;
      out << indent << i_tree_dumpable::skip_tag;
      if (j == _component_infos_.end()) {
        out << i_tree_dumpable::last_tag;
      } else {
        out << i_tree_dumpable::tag;
      }
      out << "EID=" << i->first << " (as component model named '" << i->second.get_model().get_name() << "')";
      out << std::endl;
    }

    out << indent << i_tree_dumpable::inherit_tag(inherit)
        << "Initialized    : "
        << this->is_initialized()
        << std::endl;

    return;
  }

  void mapping::_build()
  {
    std::cerr << "DEVEL: mapping::_build: Entering..." << std::endl;

    std::string setup_cat_name = mapping::default_setup_category();

    DT_THROW_IF(! _eid_manager_->has_category_info(setup_cat_name),
                std::logic_error,
                "Unknown top-level setup category '" << setup_cat_name << "' !");
    const geomtools::id_mgr::category_info & setup_cat_info
      = _eid_manager_->get_category_info(setup_cat_name);
    geomtools::geom_id setup_eid;
    setup_cat_info.create (setup_eid);
    setup_eid.set_address (0);
    DT_LOG_TRACE (_logging_, "Setup ID = " << setup_eid << ' '
                  << (setup_eid.is_valid () ? "[Valid]": "[Invalid]"));

    // Add setup mapping info :
    if (_top_level_mapping_)  {
      component_info setup_ci;
      setup_ci.set_eid(setup_eid);
      setup_ci.set_model(*_top_level_component_);
      _component_infos_[setup_eid] = setup_ci;
    }

    bool build_mode_strict_mothership = true;
    if (build_mode_strict_mothership) {
      _build_embedded_components(*_top_level_component_, setup_eid);
    }

    std::cerr << "DEVEL: mapping::_build: Exiting." << std::endl;
    return;
  }

  void mapping::_build_embedded_components(const component_model_base & mother_,
                                            const geomtools::geom_id & mother_eid_)
  {
    std::cerr << "DEVEL: mapping::_build_embedded_components: Entering..." << std::endl;
    const component_model_base & mother_model = mother_;
    mother_model.get_auxiliaries().tree_dump(std::cerr, "Mother model auxiliaries: ", "***** ");

    for (component_dict_type::const_iterator i
           = mother_model.get_embedded_components().begin();
         i !=  mother_model.get_embedded_components().end();
         i++) {
      const std::string & embedded_label = i->first;
      const component_model_base & comp_model = mother_model.get_embedded(embedded_label).get();
      std::cerr << "DEVEL: mapping::_build_embedded_components: Embedded = '"
                << embedded_label << "'" << std::endl;

      const std::string embedded_model_name = comp_model.get_name();
      std::string embedded_category_info;
      mother_model.get_auxiliaries().tree_dump(std::cerr, "Mother model auxiliaries: ", "DEVEL: ");
      if (mapping_utils::has_embedded_id(mother_model.get_auxiliaries(),
                                         embedded_label)) {
        mapping_utils::fetch_embedded_id(mother_model.get_auxiliaries(),
                                         embedded_label,
                                         embedded_category_info);
        DT_LOG_TRACE (get_logging_priority(),
                      "Found EID mapping info for embedded component '" << embedded_label << "'");
        std::cerr << "DEVEL: mapping::_build_embedded_components: "
                  << "Found EID mapping info for embedded component '" << embedded_label << "'."
                  << std::endl;
      } else {
        DT_LOG_TRACE (get_logging_priority(),
                      "No EID mapping info for embedded component '" << embedded_label << "'");
        std::cerr << "DEVEL: mapping::_build_embedded_components: "
                  << "No EID mapping info for embedded component '" << embedded_label << "'."
                  << std::endl;
      }

      geomtools::geom_id propagated_parent_eid = mother_eid_;

      if (! embedded_category_info.empty()) {

        geomtools::geom_id embedded_eid;
        std::vector<uint32_t> embedded_index;
        _eid_manager_->compute_id_from_info(embedded_eid,
                                            mother_eid_,
                                            embedded_category_info,
                                            embedded_index);
        if (_eid_manager_->validate_id(embedded_eid)) {
          component_info embedded_ci;
          embedded_ci.set_eid(embedded_eid);
          embedded_ci.set_model(comp_model);
          bool add_it = true;
          if (is_mode_only() || is_mode_excluded()) {
            // get the category associated to the embedded EID:
            const std::string & category
              = _eid_manager_->get_category_info(embedded_eid.get_type()).get_category();
            DT_LOG_TRACE (get_logging_priority (),
                          "Category = '" << category << "' (from EID=" << embedded_eid << ")");
            if (is_mode_only ()) {
              // the list contains only the categories to be registered:
              std::vector<std::string>::const_iterator found =
                std::find(_only_categories_.begin (),
                          _only_categories_.end (),
                          category);
              if (found == _only_categories_.end ()) {
                add_it = false;
              }
            } else if (is_mode_excluded ()) {
              // the list contains only the categories to be excluded:
              std::vector<std::string>::const_iterator found
                = std::find (_excluded_categories_.begin (),
                             _excluded_categories_.end (),
                             category);
              if (found != _excluded_categories_.end ())  {
                add_it = false;
              }
            }
          }
          if (add_it) {
            _component_infos_[embedded_eid] = embedded_ci;
          }
          propagated_parent_eid = embedded_eid;
        }
      } // eid valid
      bool build_it = true;
      if (_max_depth_ > 0) {
        if (_depth_ > _max_depth_) {
          build_it = false;
        }
      }
      if (build_it) {
        _build_embedded_components(comp_model, propagated_parent_eid);
      } else {
        DT_LOG_TRACE (get_logging_priority (), "-> DO NOT TRAVERSE THE GEOMETRY TREE FURTHER.");
      }

    } // for embedded components

    std::cerr << "DEVEL: mapping::_build_embedded_components: Exiting." << std::endl;
    return;
  }

} // end of namespace electronics
