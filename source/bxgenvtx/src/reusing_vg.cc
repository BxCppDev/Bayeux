// genvtx/reusing_vg.cc

// Ourselves:
#include <genvtx/reusing_vg.h>

// Standard library:
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>

namespace genvtx {

  using namespace std;

  GENVTX_VG_REGISTRATION_IMPLEMENT(reusing_vg, "genvtx::reusing_vg")

  std::size_t reusing_vg::get_reusing_count() const
  {
    return _reusing_count_;
  }

  void reusing_vg::set_reusing_count(const std::size_t reusing_count_)
  {
    DT_THROW_IF(reusing_count_ < 1, std::domain_error, "Invalid reusing count!");
    _reusing_count_ = reusing_count_;
    reset_remaining_counter();
    geomtools::invalidate(_cached_);
    return;
  }

  void reusing_vg::reset_remaining_counter()
  {
    _remaining_counter_ = 0;
    return;
  }

  bool reusing_vg::has_vg() const
  {
    return _vg_ != nullptr || _hvg_;
  }

  bool reusing_vg::is_owned() const
  {
    return _vg_ != nullptr && _owned_;
  }

  i_vertex_generator & reusing_vg::grab_vg()
  {
    if (_vg_ != nullptr) {
      return *_vg_;
    } else if (! _hvg_) {
      DT_THROW_IF(true, logic_error,"Missing vertex generator !");
    }
    return _hvg_.grab();
  }

  const i_vertex_generator & reusing_vg::get_vg() const
  {
    if (_vg_ != nullptr) {
      return *_vg_;
    } else if (! _hvg_) {
      DT_THROW_IF(true, logic_error,"Missing vertex generator !");
    }
    return _hvg_.get();
  }

  void reusing_vg::_clear_vg_()
  {
    if (_vg_ != nullptr) {
      if (_owned_) {
        delete _vg_;
      }
      _vg_ = nullptr;
    }
    _owned_ = false;
    return;
  }

  void reusing_vg::set_vg(i_vertex_generator & new_value_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_();
    _owned_ = false;
    _vg_ = &new_value_;
    return;
  }

  void  reusing_vg::set_vg(i_vertex_generator * new_value_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_();
    _owned_ = true;
    _vg_ = new_value_;
    return;
  }

  void  reusing_vg::set_vg(vg_handle_type & hvg_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_();
    _owned_ = false;
    _hvg_ = hvg_;
    return;
  }

  bool reusing_vg::has_next_vertex() const
  {
    if (! has_vg()) {
      return false;
    }
    if (_remaining_counter_ == 0) {
      return get_vg().has_next_vertex();
    }
    return geomtools::is_valid(_cached_);
  }

  bool reusing_vg::is_initialized() const
  {
    return _reusing_count_ > 0 && has_vg();
  }

  void reusing_vg::initialize(const ::datatools::properties & configuration_,
                                 ::datatools::service_manager & /*service_manager_*/,
                                 ::genvtx::vg_dict_type & vgens_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

    if (! has_vg()) {
      // parameters of the vertex generator :
      std::string vg_name;
      if (configuration_.has_key("vertex_generator.name")) {
        vg_name = configuration_.fetch_string("vertex_generator.name");
        vg_dict_type::iterator vg_found = vgens_.find(vg_name);
        DT_THROW_IF(vg_found == vgens_.end(), std::logic_error,
                     "Cannot find a vertex generator with name '"
                     <<  vg_name << "' !");
        set_vg(vg_found->second.grab_initialized_vg_handle());
      }
    }
    DT_THROW_IF(! has_vg(), std::logic_error,
		"Vertex generator is not set!");

    if (_reusing_count_ == 0) {
      std::size_t reusing_count = 0;
      if (configuration_.has_key("reusing_count")) {
	reusing_count = configuration_.fetch_positive_integer("reusing_count");
	set_reusing_count(reusing_count);
      }
    }
    DT_THROW_IF(_reusing_count_ == 0, std::logic_error,
		"Reusing count is not set!");

    return;
  }

  void reusing_vg::reset()
  {
    _remaining_counter_ = 0;
    geomtools::invalidate(_cached_);
    _clear_vg_();
    _hvg_.reset();
    _reusing_count_ = 0;
    return;
  }

  reusing_vg::reusing_vg()
    : genvtx::i_vertex_generator()
  {
    return;
  }

  reusing_vg::reusing_vg(i_vertex_generator & vg_,
			 const std::size_t reusing_count_)
    : genvtx::i_vertex_generator()
  {
    set_vg(vg_);
    set_reusing_count(reusing_count_);
    return;
  }

  reusing_vg::~reusing_vg()
  {
    if (is_initialized()) reset();
    return;
  }

  void reusing_vg::_shoot_vertex(::mygsl::rng & random_,
				 ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
    geomtools::invalidate(vertex_);
    if (_remaining_counter_ == 0) {
      grab_vg().shoot_vertex(random_, _cached_);
      _remaining_counter_ = _reusing_count_;
    }
    _remaining_counter_--;
    vertex_ = _cached_;
    return;
  }

} // end of namespace genvtx
