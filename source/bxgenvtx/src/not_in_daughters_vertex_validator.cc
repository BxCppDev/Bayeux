// not_in_daughters_vertex_validator.cc

// Ourselves:
#include <genvtx/not_in_daughters_vertex_validator.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <geomtools/logical_volume.h>

// This project:
#include <genvtx/vertex_validation.h>

namespace genvtx {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(not_in_daughters_vertex_validator,
                             "genvtx::not_in_daughters_vertex_validator")

  not_in_daughters_vertex_validator::not_in_daughters_vertex_validator(datatools::logger::priority logging_priority_)
    : cuts::i_cut(logging_priority_)
  {
    _reversed_ = false;
    datatools::invalidate(_tolerance_);
    datatools::invalidate(_daughter_tolerance_);
    return;
  }

  not_in_daughters_vertex_validator::~not_in_daughters_vertex_validator()
  {
    if (is_initialized()) {
      this->not_in_daughters_vertex_validator::reset();
    }
    return;
  }

  bool not_in_daughters_vertex_validator::is_reversed() const
  {
    return _reversed_;
  }

  void not_in_daughters_vertex_validator::set_reversed(bool val_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    _reversed_ = val_;
    return;
  }

  void not_in_daughters_vertex_validator::set_tolerance(double val_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    _tolerance_ = val_;
    return;
  }

  void not_in_daughters_vertex_validator::set_daughter_tolerance(double val_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    _daughter_tolerance_ = val_;
    return;
  }

  void not_in_daughters_vertex_validator::initialize(const datatools::properties & config_,
                                                     datatools::service_manager &,
                                                     cuts::cut_handle_dict_type &)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Vertex validator '" << get_name () << "' is already initialized !");
    cuts::i_cut::_common_initialize(config_);

    double default_length_unit = CLHEP::mm;

    if (config_.has_key("reversed")) {
      set_reversed(config_.fetch_boolean("reversed"));
    }

    if (! datatools::is_valid(_tolerance_)) {
      if (config_.has_key("tolerance")) {
        double t = config_.fetch_real("tolerance");
        if (!config_.has_explicit_unit("tolerance")) {
          t *= default_length_unit;
        }
        set_tolerance(t);
      }
    }

    if (! datatools::is_valid(_daughter_tolerance_)) {
      if (config_.has_key("daughter_tolerance")) {
        double t = config_.fetch_real("daughter_tolerance");
        if (!config_.has_explicit_unit("daughter_tolerance")) {
          t *= default_length_unit;
        }
        set_daughter_tolerance(t);
      }
    }

    _set_initialized (true);
    return;
  }

  void not_in_daughters_vertex_validator::reset()
  {
    _set_initialized (false);
    datatools::invalidate(_tolerance_);
    datatools::invalidate(_daughter_tolerance_);
    _reversed_ = false;
    this->i_cut::_reset ();
    return;
  }

  int not_in_daughters_vertex_validator::_accept()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    int status = cuts::SELECTION_ACCEPTED;
    if (! this->is_user_data<vertex_validation::geometry_context>()) {
      status = cuts::SELECTION_INAPPLICABLE;
      DT_THROW_IF(true, std::logic_error, "Cannot process user data with invalid type!");
    } else {
      const vertex_validation::geometry_context & gc
        = this->get_user_data<vertex_validation::geometry_context>();
      const geomtools::logical_volume & logvol = gc.get_logical_volume();
      const geomtools::vector_3d & vtx = gc.get_local_candidate_vertex();
      geomtools::logical_volume::locate_result lr;
      uint32_t locate_bits = logvol.locate(vtx, false, _tolerance_, _daughter_tolerance_, &lr);
      if (locate_bits & geomtools::SHAPE_DOMAIN_INSIDE) {
        DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_INSIDE");
        if (locate_bits & geomtools::SHAPE_DOMAIN_ON_DAUGHTER_SURFACE) {
          DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_ON_DAUGHTER_SURFACE");
          status = cuts::SELECTION_REJECTED;
        }
        if (locate_bits & geomtools::SHAPE_DOMAIN_INSIDE_DAUGHTER) {
          DT_LOG_TRACE(get_logging_priority(), "SHAPE_DOMAIN_INSIDE_DAUGHTER");
          status = cuts::SELECTION_REJECTED;
        }
      } else {
        status = cuts::SELECTION_REJECTED;
      }
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

  void not_in_daughters_vertex_validator::tree_dump(std::ostream & out_,
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
         << "Validator Id       : '" << not_in_daughters_vertex_validator::system_factory_auto_registration_id() << "' " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Reversed           : " << _reversed_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Tolerance          : " << _tolerance_ / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Daughter tolerance : " << _daughter_tolerance_ / CLHEP::mm << " mm" << std::endl;

    return;
  }

} // end of namespace genvtx
