// Ourselves
#include <electronics/component_info.h>

// Third Party
// - Bayeux/datatools
#include <datatools/exception.h>

// This project

namespace electronics {

  component_info::component_info()
  {
    _model_ = 0;
    return;
  }

  component_info::~component_info()
  {
    return;
  }

  bool component_info::has_parent_eid() const
  {
    return _parent_eid_.is_valid();
  }

  void component_info::set_parent_eid(const geomtools::geom_id & peid_)
  {
    _parent_eid_ = peid_;
    return;
  }

  const geomtools::geom_id & component_info::get_parent_eid() const
  {
    return _parent_eid_;
  }


  bool component_info::has_eid() const
  {
    return _eid_.is_valid();
  }

  void component_info::set_eid(const geomtools::geom_id & eid_)
  {
    _eid_ = eid_;
    return;
  }

  const geomtools::geom_id & component_info::get_eid() const
  {
    return _eid_;
  }

  bool component_info::has_model() const
  {
    return _model_ != 0;
  }

  void component_info::set_model(const component_model_base & m_)
  {
    _model_ = &m_;
  }

  const component_model_base & component_info::get_model() const
  {
    DT_THROW_IF(!has_model(), std::logic_error,
                "Missing component model !");
    return *_model_;
  }

  bool component_info::is_valid() const
  {
    return _eid_.is_valid() && has_model() && _model_->is_initialized();
  }

  void component_info::reset()
  {
    _eid_.reset();
    _parent_eid_.reset();
    _model_ = 0;
    return;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const component_info & ci_)
  {
    out_ << ci_._eid_ << "="
         << "{"
         << "model=";
    if (ci_.has_model()) {
      out_ << ci_.get_model().get_name();
    } else {
      out_ << "<none>";
    }
    out_ << ";";
    out_ << "parent=";
    if (ci_.has_parent_eid()) {
      out_ << ci_.get_parent_eid();
    } else {
      out_ << "<none>";
    }
    out_ << "}";
    return out_;
  }

} // end of namespace electronics
