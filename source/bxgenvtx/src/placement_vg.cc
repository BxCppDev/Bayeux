// genvtx/placement_vg.cc

// Ourselves:
#include <genvtx/placement_vg.h>

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

  GENVTX_VG_REGISTRATION_IMPLEMENT(placement_vg,"genvtx::placement_vg")

  const geomtools::placement & placement_vg::get_placement () const
  {
    return _placement_;
  }

  geomtools::placement & placement_vg::get_placement ()
  {
    return _placement_;
  }

  void placement_vg::set_placement (const geomtools::placement & new_value_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _placement_ = new_value_;
    return;
  }

  bool placement_vg::has_vg () const
  {
    return _vg_ != 0 || _hvg_;
  }

  bool placement_vg::is_owned () const
  {
    return _vg_ != 0 && _owned_;
  }

  i_vertex_generator & placement_vg::grab_vg ()
  {
    if (_vg_ != 0) {
      return *_vg_;
    } else if (! _hvg_) {
      DT_THROW_IF (true, logic_error,"Missing vertex generator !");
    }
    return _hvg_.grab ();
  }

  const i_vertex_generator & placement_vg::get_vg () const
  {
    if (_vg_ != 0) {
      return *_vg_;
    } else if (! _hvg_) {
      DT_THROW_IF (true, logic_error,"Missing vertex generator !");
    }
    return _hvg_.get ();
  }

  void placement_vg::_clear_vg_ ()
  {
    if (_vg_ != 0) {
      if (_owned_) {
        delete _vg_;
      }
      _vg_ = 0;
    }
    _owned_ = false;
    return;
  }

  void placement_vg::set_vg (i_vertex_generator & new_value_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_ ();
    _owned_ = false;
    _vg_ = &new_value_;
    return;
  }

  void  placement_vg::set_vg (i_vertex_generator * new_value_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_ ();
    _owned_ = true;
    _vg_ = new_value_;
    return;
  }

  void  placement_vg::set_vg (vg_handle_type & hvg_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _clear_vg_ ();
    _owned_ = false;
    _hvg_ = hvg_;
    return;
  }

  bool placement_vg::has_next_vertex() const
  {
    if (! has_vg ()) {
      return false;
    }
    return get_vg ().has_next_vertex ();
  }

  bool placement_vg::is_initialized () const
  {
    return _placement_.is_valid () && has_vg ();
  }

  void placement_vg::initialize (const ::datatools::properties & configuration_,
                                 ::datatools::service_manager & /*service_manager_*/,
                                 ::genvtx::vg_dict_type & vgens_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    double x, y, z;
    double phi, theta, delta;
    x = y = z = 0.0;
    phi = theta = delta = 0.0;
    double lunit = 1. * CLHEP::millimeter;
    double aunit = 1. * CLHEP::degree;

    if (! has_vg ()) {
      // parameters of the vertex generator :
      std::string vg_name;

      if (configuration_.has_key ("vertex_generator.name")) {
        vg_name = configuration_.fetch_string ("vertex_generator.name");
        vg_dict_type::iterator vg_found = vgens_.find (vg_name);
        DT_THROW_IF (vg_found == vgens_.end (),std::logic_error,
                     "Cannot find a vertex generator with name '"
                     <<  vg_name << "' !");
        set_vg (vg_found->second.grab_initialized_vg_handle ());
      }
    }

    if (configuration_.has_key ("length_unit")) {
      std::string lunit_str = configuration_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    if (configuration_.has_key ("angle_unit")) {
      std::string aunit_str = configuration_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    if (! get_placement ().is_valid ()) {
      if (configuration_.has_key ("placement.x")) {
        x = configuration_.fetch_real ("placement.x");
        if (! configuration_.has_explicit_unit ("placement.x")) x *= lunit;
      }

      if (configuration_.has_key ("placement.y")) {
        y = configuration_.fetch_real ("placement.y");
        if (! configuration_.has_explicit_unit ("placement.y")) y *= lunit;
      }

      if (configuration_.has_key ("placement.z")) {
        z = configuration_.fetch_real ("placement.z");
        if (! configuration_.has_explicit_unit ("placement.z")) z *= lunit;
      }

      if (configuration_.has_key ("placement.phi")) {
        phi = configuration_.fetch_real ("placement.phi");
        if (! configuration_.has_explicit_unit ("placement.phi")) phi *= aunit;
      }

      if (configuration_.has_key ("placement.theta")) {
        theta = configuration_.fetch_real ("placement.theta");
        if (! configuration_.has_explicit_unit ("placement.theta")) theta *= aunit;
      }

      if (configuration_.has_key ("placement.delta")) {
        delta = configuration_.fetch_real ("placement.delta");
        if (! configuration_.has_explicit_unit ("placement.delta")) delta *= aunit;
      }

      geomtools::placement pl (x, y, z, phi, theta, delta);
      set_placement (pl);
    }
    return;
  }

  void placement_vg::reset()
  {
    _clear_vg_ ();
    _hvg_.reset ();
    _placement_.invalidate ();
    return;
  }

  placement_vg::placement_vg()
    : genvtx::i_vertex_generator()
  {
    _placement_.invalidate();
    return;
  }

  placement_vg::placement_vg(i_vertex_generator & vg_,
			     const geomtools::placement & p_)
    : genvtx::i_vertex_generator()
  {
    set_vg(vg_);
    _placement_ = p_;
    return;
  }

  placement_vg::~placement_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void placement_vg::_shoot_vertex(::mygsl::rng & random_,
                                   ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
    geomtools::invalidate (vertex_);
    geomtools::vector_3d vertex;
    geomtools::invalidate (vertex);
    grab_vg ().shoot_vertex (random_, vertex);
    if (geomtools::is_valid (vertex)) {
      _placement_.child_to_mother (vertex, vertex_);
    }
    return;
  }

} // end of namespace genvtx
