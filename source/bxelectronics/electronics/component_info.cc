// Ourselves
#include <electronics/component_info.h>

// Third Party
// - Bayeux/datatools
#include <datatools/exception.h>

// This project

namespace electronics {

  component_info::component_info()
  {
    _logical_ = 0;
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

  bool component_info::has_logical() const
  {
    return _logical_ != 0;
  }

  void component_info::set_logical(const logical_component & l_)
  {
    _logical_ = &l_;
  }

  const logical_component & component_info::get_logical() const
  {
    DT_THROW_IF(! has_logical(), std::logic_error,
                "Missing logical component !");
    return *_logical_;
  }

  bool component_info::is_valid() const
  {
    return _eid_.is_valid() && has_logical() && _logical_->is_initialized();
  }

  void component_info::reset()
  {
    _eid_.reset();
    _parent_eid_.reset();
    _logical_ = 0;
    return;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const component_info & ci_)
  {
    out_ << ci_._eid_ << "="
         << "{"
         << "logical=";
    if (ci_.has_logical()) {
      out_ << ci_.get_logical().get_name();
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
