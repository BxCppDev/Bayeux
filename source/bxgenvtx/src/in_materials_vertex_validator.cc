// in_materials_vertex_validator.cc

// Ourselves:
#include <genvtx/in_materials_vertex_validator.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/utils.h>

// This project:
#include <genvtx/vertex_validation.h>

namespace genvtx {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(in_materials_vertex_validator,
                             "genvtx::in_materials_vertex_validator");

  in_materials_vertex_validator::in_materials_vertex_validator(datatools::logger::priority logging_priority_)
    : cuts::i_cut(logging_priority_)
  {
    _reversed_ = false;
    _max_depth_ = 10;
    return;
  }

  in_materials_vertex_validator::~in_materials_vertex_validator()
  {
    if (is_initialized()) {
      this->in_materials_vertex_validator::reset();
    }
    return;
  }

  bool in_materials_vertex_validator::is_reversed() const
  {
    return _reversed_;
  }

  void in_materials_vertex_validator::set_reversed(bool val_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    _reversed_ = val_;
    return;
  }

  void in_materials_vertex_validator::add_material(const std::string & material_name_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    _materials_.insert(material_name_);
    return;
  }

  bool in_materials_vertex_validator::has_material(const std::string & material_name_) const
  {
    return _materials_.count(material_name_) > 0;
  }

  void in_materials_vertex_validator::set_max_depth(int md_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    if (md_ > 0) {
      _max_depth_ = md_;
    } else {
      _max_depth_ = 0;
    }
    return;
  }

  bool in_materials_vertex_validator::has_max_depth() const
  {
    return _max_depth_ > 0;
  }

  int in_materials_vertex_validator::get_max_depth() const
  {
    return _max_depth_;
  }

  void in_materials_vertex_validator::initialize(const datatools::properties & config_,
                                                 datatools::service_manager &,
                                                 cuts::cut_handle_dict_type &)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    cuts::i_cut::_common_initialize(config_);

    // double default_length_unit = CLHEP::mm;

    if (config_.has_key("reversed")) {
      set_reversed(config_.fetch_boolean("reversed"));
    }

    if (config_.has_key("materials.ref")) {
      std::vector<std::string> matrefs;
      config_.fetch("materials.ref", matrefs);
      for (int i = 0; i < (int) matrefs.size(); i++) {
        add_material(matrefs[i]);
      }
    }

    if (config_.has_key("max_depth")) {
      set_max_depth(config_.fetch_integer("max_depth"));
    }

    DT_THROW_IF (_materials_.size() == 0, std::logic_error,
                 "Empty set of validating materials!");

    _set_initialized (true);
    return;
  }

  void in_materials_vertex_validator::reset()
  {
    _set_initialized (false);
    _materials_.clear();
    _reversed_ = false;
    this->i_cut::_reset ();
    return;
  }

  int in_materials_vertex_validator::_accept()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    int status = cuts::SELECTION_ACCEPTED;
    if (! this->is_user_data<vertex_validation::geometry_context>()) {
      status = cuts::SELECTION_INAPPLICABLE;
      DT_THROW_IF(true, std::logic_error, "Cannot process user data with invalid type!");
    } else {
      double tolerance = 0.0 * CLHEP::mm;
      double daughter_tolerance = 0.0 * CLHEP::mm;
      const vertex_validation::geometry_context & gc
        = this->get_user_data<vertex_validation::geometry_context>();
      const geomtools::logical_volume & logvol = gc.get_logical_volume();
      const geomtools::vector_3d & vtx = gc.get_local_candidate_vertex();
      DT_LOG_TRACE(get_logging_priority(), "Local candidate vertex = " << vtx);
      int current_depth = 0;
      const geomtools::logical_volume * current_logvol = &logvol;
      geomtools::vector_3d current_vtx = vtx;
      bool ignore_daughters = false;
      do {
        DT_LOG_TRACE(get_logging_priority(), "New try...");
        if (has_max_depth() && (current_depth >= _max_depth_)) {
          ignore_daughters = true;
        }
        geomtools::logical_volume::locate_result lr;
        uint32_t locate_bits = current_logvol->locate(current_vtx,
                                                      ignore_daughters,
                                                      tolerance,
                                                      daughter_tolerance,
                                                      &lr);
        if (locate_bits & geomtools::SHAPE_DOMAIN_INSIDE) {
          DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_INSIDE");
          bool only_parent = true;
          if (locate_bits & geomtools::SHAPE_DOMAIN_ON_DAUGHTER_SURFACE) {
            DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_ON_DAUGHTER_SURFACE");
            only_parent = false;
            // status = cuts::SELECTION_REJECTED;
          }
          if (locate_bits & geomtools::SHAPE_DOMAIN_INSIDE_DAUGHTER) {
            DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_INSIDE_DAUGHTER");
            only_parent = false;
            // status = cuts::SELECTION_REJECTED;
          }
          DT_LOG_TRACE(get_logging_priority(),
                       "Checking material '" << current_logvol->get_material_ref() << "' ...");
          if (only_parent) {
            status = cuts::SELECTION_ACCEPTED;
            if (! has_material(current_logvol->get_material_ref())) {
              status = cuts::SELECTION_REJECTED;
            } else {
              DT_LOG_TRACE(get_logging_priority(),
                           "Found matching material '" << current_logvol->get_material_ref() << "'");
            }
            break;
          } else {
            // Investigate the matching daughter volume:
            status = cuts::SELECTION_REJECTED;
            if (lr.has_daughter()) {
              DT_LOG_TRACE(get_logging_priority(),
                           "Investigating the matching daughter volume '" << lr.get_daughter_name() << "'...");
              const geomtools::physical_volume & phys = lr.get_daughter();
              const geomtools::vector_3d & phys_vtx = lr.get_position_in_daughter();
              const geomtools::logical_volume & log = phys.get_logical();
              current_logvol = &log; // Reference it
              current_vtx = phys_vtx; // Copy it
            } else {
              status = cuts::SELECTION_REJECTED;
              break;
            }
          }
        } else {
          status = cuts::SELECTION_REJECTED;
          break;
        }
        current_depth++;
      } while (current_depth < 1000);
    }
    if (is_reversed()) {
      if (status == cuts::SELECTION_REJECTED) {
        status = cuts::SELECTION_ACCEPTED;
      } else if (status == cuts::SELECTION_ACCEPTED) {
        status = cuts::SELECTION_REJECTED;
      }
    }
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return status;
  }

  void in_materials_vertex_validator::tree_dump(std::ostream & out_,
                                                    const std::string & title_,
                                                    const std::string & indent_,
                                                    bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    this->cuts::i_cut::tree_dump(out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Validator Id : '"
         << in_materials_vertex_validator::system_factory_auto_registration_id() << "' " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Reversed : " << _reversed_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Max depth : " << _max_depth_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Materials : " << _materials_.size() << std::endl;
    for (std::set<std::string>::const_iterator it = _materials_.begin();
         it != _materials_.end();
         it++) {
      out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      std::set<std::string>::const_iterator jt = it;
      jt++;
      if (jt == _materials_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << "Material : '" << *it << "'" << std::endl;
    }

    return;
  }

} // end of namespace genvtx
