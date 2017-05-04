/// model_with_internal_mesh_tools.cc

// Ourselves:
#include <geomtools/model_with_internal_mesh_tools.h>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/logical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {

  // static
  const std::string model_with_internal_mesh_data::INTERNAL_MESH_PREFIX           = "internal_mesh.";
  const std::string model_with_internal_mesh_data::INTERNAL_MESH_LABELS_KEY       = "labels";
  const std::string model_with_internal_mesh_data::INTERNAL_MESH_MODEL_PREFIX     = "model.";
  const std::string model_with_internal_mesh_data::INTERNAL_MESH_RANK_PREFIX      = "rank.";
  const std::string model_with_internal_mesh_data::INTERNAL_MESH_PLACEMENT_PREFIX = "placement.";

  // static
  const int model_with_internal_mesh_data::mesh_type::INVALID_RANK;

  model_with_internal_mesh_data::mesh_type::mesh_type()
  {
    _label_ = "";
    _mother_log_ = 0;
    _model_ = 0;
    _rank_ = INVALID_RANK;
    _mesh_placement_ = 0;
    return;
  }

  model_with_internal_mesh_data::mesh_type::~mesh_type()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void model_with_internal_mesh_data::mesh_type::reset_mesh_placement()
  {
    if (_mesh_placement_ != 0) {
      _mesh_placement_->reset();
      delete _mesh_placement_;
      _mesh_placement_ = 0;
    }
    return;
  }

  bool model_with_internal_mesh_data::mesh_type::has_mother_logical() const
  {
    return _mother_log_ != 0;
  }

  void model_with_internal_mesh_data::mesh_type::set_mother_logical(logical_volume & mlog_)
  {
    _mother_log_ = &mlog_;
    return;
  }

  const logical_volume & model_with_internal_mesh_data::mesh_type::get_mother_logical() const
  {
    DT_THROW_IF(_mother_log_ == 0, std::logic_error, "No moher logical volume is defined!");
    return *_mother_log_;
  }

  logical_volume & model_with_internal_mesh_data::mesh_type::grab_mother_logical()
  {
    DT_THROW_IF(_mother_log_ == 0, std::logic_error, "No moher logical volume is defined!");
    return *_mother_log_;
  }

  void model_with_internal_mesh_data::mesh_type::set_label(const std::string & label_)
  {
    _label_ = label_;
    return;
  }

  const std::string & model_with_internal_mesh_data::mesh_type::get_label() const
  {
    return _label_;
  }

  void model_with_internal_mesh_data::mesh_type::set_rank(int rank_)
  {
    _rank_ = rank_;
    return;
  }

  int model_with_internal_mesh_data::mesh_type::get_rank() const
  {
    return _rank_;
  }

  void model_with_internal_mesh_data::mesh_type::set_model(const i_model & model_)
  {
    _model_ = &model_;
    return;
  }

  const i_model & model_with_internal_mesh_data::mesh_type::get_model() const
  {
    DT_THROW_IF(_model_ == 0, std::logic_error, "Missing model !");
    return *_model_;
  }

  void model_with_internal_mesh_data::mesh_type::set_mesh_placement_config(const datatools::properties & mpconf_)
  {
    _mesh_placement_config_ = mpconf_;
    return;
  }

  const datatools::properties &
  model_with_internal_mesh_data::mesh_type::get_mesh_placement_config() const
  {
    return _mesh_placement_config_;
  }

  bool model_with_internal_mesh_data::mesh_type::is_initialized() const
  {
    return _mesh_placement_ != 0;
  }

  void model_with_internal_mesh_data::mesh_type::initialize()
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Mesh entry '" << _label_ << "' is already initialized!");
    DT_THROW_IF(!has_mother_logical(), std::logic_error, "Mesh entry '" << _label_ << "' has no mother logical volume!");

    // Initialize the mesh placement:
    _mesh_placement_ = new regular_3d_mesh_placement;
    _mesh_placement_->set_log_vol(*_mother_log_);
    _mesh_placement_->set_node_log_vol(_model_->get_logical());
    _mesh_placement_->initialize(_mesh_placement_config_);

    // Initialize the daughter physical volume:
    _phys_.set_placement(*_mesh_placement_);
    _phys_.set_name(i_model::make_physical_volume_name(_label_));
    _phys_.set_logical(_model_->get_logical());
    _phys_.set_mother(*_mother_log_);

    return;
  }

  void model_with_internal_mesh_data::mesh_type::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Mesh entry is not initialized!");
    reset_mesh_placement();
    return;
  }

  const regular_3d_mesh_placement & model_with_internal_mesh_data::mesh_type::get_mesh_placement () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Mesh entry is not initialized!");
    return *_mesh_placement_;
  }

  const physical_volume & model_with_internal_mesh_data::mesh_type::get_physical_volume() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Mesh entry is not initialized!");
    return _phys_;
  }

  physical_volume & model_with_internal_mesh_data::mesh_type::grab_physical_volume()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Mesh entry is not initialized!");
   return _phys_;
  }

  model_with_internal_mesh_data::model_with_internal_mesh_data()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    return;
  }

  model_with_internal_mesh_data::~model_with_internal_mesh_data()
  {
    reset();
    return;
  }

  void model_with_internal_mesh_data::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  datatools::logger::priority model_with_internal_mesh_data::get_logging() const
  {
    return _logging_;
  }

  const model_with_internal_mesh_data::mesh_dict_type &
  model_with_internal_mesh_data::get_meshes() const
  {
    return _meshes_;
  }

  model_with_internal_mesh_data::mesh_dict_type &
  model_with_internal_mesh_data::model_with_internal_mesh_data::grab_meshes()
  {
    return _meshes_;
  }

  size_t model_with_internal_mesh_data::get_number_of_meshes () const
  {
    return _meshes_.size ();
  }

  bool model_with_internal_mesh_data::has_mesh(const std::string & label_) const
  {
    DT_THROW_IF(label_.length() == 0, std::logic_error, "Empty mesh label !");
    return _meshes_.find(label_) != _meshes_.end();
  }

  const model_with_internal_mesh_data::mesh_type &
  model_with_internal_mesh_data::get_mesh(const std::string & label_) const
  {
    mesh_dict_type::const_iterator found = _meshes_.find(label_);
    DT_THROW_IF(found == _meshes_.end(), std::logic_error, "No mesh with label '" << label_ << "'");
    return found->second;
  }

  model_with_internal_mesh_data::mesh_type &
  model_with_internal_mesh_data::grab_mesh(const std::string & label_)
  {
    mesh_dict_type::iterator found = _meshes_.find(label_);
    DT_THROW_IF(found == _meshes_.end(), std::logic_error, "No mesh with label '" << label_ << "'");
    return found->second;
  }

  void model_with_internal_mesh_data::add_mesh(const std::string & label_,
                                               logical_volume& mother_log_,
                                               const i_model & model_,
                                               int rank_,
                                               const datatools::properties & mesh_placement_config_)
  {
    DT_LOG_DEBUG(_logging_, "Entering internal mesh '" << label_ << "'...");
    DT_THROW_IF(has_mesh(label_), std::logic_error,
                "There is already a mesh with label '" << label_ << "' !");
    DT_THROW_IF(label_.empty(), std::logic_error, "Missing internal mesh label!");
    DT_THROW_IF(rank_ < 0, std::logic_error, "Invalid internal mesh rank [" << rank_ << "] for mesh '" << label_ << "'!");
    for (mesh_dict_type::const_iterator imesh = _meshes_.begin();
         imesh != _meshes_.end();
         imesh++) {
      const mesh_type & mesh = imesh->second;
      DT_THROW_IF(mesh.get_rank() == rank_, std::logic_error,
                  "Cannot add mesh '" << label_ << "' at rank [" << rank_ << "'] "
                  << "because mesh '" << imesh->first << "' already use this rank!");
    }
    DT_LOG_DEBUG(_logging_, "Internal mesh '" << label_ << "' has rank [" << rank_ << "]");
    // Add a new mesh entry:
    {
      mesh_type dummy;
      _meshes_[label_] = dummy;
    }
    mesh_type & the_mesh = _meshes_.find(label_)->second;
    the_mesh.set_label(label_);
    the_mesh.set_mother_logical(mother_log_);
    the_mesh.set_model(model_);
    the_mesh.set_rank(rank_);
    the_mesh.set_mesh_placement_config(mesh_placement_config_);
    DT_LOG_DEBUG(_logging_, "Internal mesh '" << label_ << "' has been addeed.");
    return;
  }

  void model_with_internal_mesh_data::initialize()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    // Rank the mesh entries:
    std::map<int, mesh_type *> meshes_by_rank;
    for (mesh_dict_type::iterator i = _meshes_.begin();
         i != _meshes_.end();
         i++) {
      meshes_by_rank[i->second.get_rank()] = &i->second;
    }

    // Initialize the mesh entries:
    for (std::map<int, mesh_type *>::iterator i = meshes_by_rank.begin();
         i != meshes_by_rank.end();
         i++) {
      DT_LOG_DEBUG(_logging_, "Initializing internal mesh : '" << i->second->get_label()
                   << "' at rank [" << i->first << "] in mother volume '"
                   << i->second->get_mother_logical().get_name() << "'");
      mesh_type & mesh_entry = *i->second;
      mesh_entry.initialize();
      DT_LOG_DEBUG(_logging_, "Done.");
    }

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void model_with_internal_mesh_data::reset()
  {
    _meshes_.clear();
    return;
  }

  void model_with_internal_mesh_data::plug_internal_meshes(const datatools::properties & config_,
                                                           logical_volume & mother_log_,
                                                           models_col_type * models_)
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    DT_LOG_DEBUG(_logging_, "Mother logical : '" << mother_log_.get_name() << "'");

    bool devel = false;
    devel = true;
    if (devel) std::cerr << "DEVEL: model_with_internal_mesh_data::plug_internal_meshes: "
                         << "Entering for mother logical '" << mother_log_.get_name() << "'"
                         << std::endl;
    if (devel) config_.tree_dump(std::cerr, "Config with internal mesh: ", "DEVEL: ");
    devel = false;

    {
      datatools::logger::priority p =
        datatools::logger::extract_logging_configuration(config_,
                                                         _logging_,
                                                         true);
      set_logging(p);
    }

    std::vector<std::string> labels;
    if (config_.has_key(INTERNAL_MESH_LABELS_KEY)) {
      config_.fetch(INTERNAL_MESH_LABELS_KEY, labels);
    }
    DT_LOG_DEBUG(_logging_, "Number of internal meshes = [" << labels.size() << "]");
    if (devel) std::cerr << "DEVEL: model_with_internal_mesh_data::plug_internal_meshes: "
              << "TEST"
              << std::endl;

    if (labels.size() == 0) {
      DT_LOG_DEBUG(_logging_, "No internal mesh was found.");
      // No internal mesh within the 'logical_volume' mother envelope:
      return;
    }

    for (std::vector<std::string>::const_iterator i = labels.begin();
         i != labels.end();
         i++) {
      const std::string & mesh_label = *i;
      DT_LOG_DEBUG(_logging_, "Processing internal mesh '" << mesh_label << "'...");
      if (devel) std::cerr << "DEVEL: model_with_internal_mesh_data::plug_internal_meshes: "
                << "  Internal mesh '" << mesh_label << "'"
                << std::endl;

      // Extract model:
      const i_model * mesh_model = 0;
      {
        std::ostringstream mesh_model_key_oss;
        mesh_model_key_oss << INTERNAL_MESH_MODEL_PREFIX << mesh_label;
        DT_THROW_IF (! config_.has_key(mesh_model_key_oss.str()),
                     std::logic_error,
                     "Missing '" << mesh_model_key_oss.str() << "' property !");
        const std::string mesh_model_name = config_.fetch_string(mesh_model_key_oss.str());
        models_col_type::const_iterator found = models_->find(mesh_model_name);
        DT_THROW_IF (found == models_->end (),
                     std::logic_error,
                     "Cannot find model with name '" << mesh_model_name << "' !");
        mesh_model = found->second;
      }

      // Extract rank:
      int mesh_rank = mesh_type::INVALID_RANK;
      {
        std::ostringstream mesh_rank_key_oss;
        mesh_rank_key_oss << INTERNAL_MESH_RANK_PREFIX << mesh_label;
        DT_THROW_IF(! config_.has_key(mesh_rank_key_oss.str()),
                    std::logic_error,
                    "Missing '" << mesh_rank_key_oss.str() << "' property !");
        mesh_rank = config_.fetch_integer(mesh_rank_key_oss.str());
        DT_THROW_IF (mesh_rank < 0,
                     std::logic_error,
                     "Rank [" << mesh_rank << "] is invalid for mesh '" << mesh_label << "' !");
      }

      // Extract mesh placement parameters:
      datatools::properties mesh_placement_config;
      {
        std::ostringstream mesh_placement_key_oss;
        mesh_placement_key_oss << INTERNAL_MESH_PLACEMENT_PREFIX << mesh_label << '.';
        config_.export_and_rename_starting_with(mesh_placement_config,
                                                mesh_placement_key_oss.str(),
                                                "");

      }

      DT_LOG_DEBUG(_logging_, "Adding internal mesh '" << mesh_label << "'...");
      add_mesh(mesh_label, mother_log_, *mesh_model, mesh_rank, mesh_placement_config);
      DT_LOG_DEBUG(_logging_, "Done.");
    }

    initialize();

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void model_with_internal_mesh_data::tree_dump(std::ostream & out_,
                                                const std::string & title_,
                                                const std::string & indent_,
                                                bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Logging : " << datatools::logger::get_priority_label(_logging_)
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Internal meshes: ";
    if (_meshes_.size() == 0) {
      out_ << "<none>";
    } else {
      out_ << '[' << _meshes_.size() << ']';
    }
    out_ << std::endl;
    if (_meshes_.size() > 0) {
    }

    return;
  }

} // end of namespace geomtools
