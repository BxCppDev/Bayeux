/* sphere_vg.cc
 */

// Ourselves:
#include <genvtx/sphere_vg.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(sphere_vg,"genvtx::sphere_vg");

  int sphere_vg::get_mode () const
  {
    return _mode_;
  }

  void sphere_vg::set_mode (int mode_)
  {
    DT_THROW_IF ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE),
                 std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void sphere_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void sphere_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void sphere_vg::set_bulk (double /*skin_thickness_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _mode_ = MODE_BULK;
    return;
  }

  bool sphere_vg::has_sphere_ref () const
  {
    return _sphere_ref_ != 0;
  }

  void sphere_vg::set_sphere_ref (const geomtools::sphere & sphere_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    DT_THROW_IF (! sphere_.is_valid (), std::logic_error, "Invalid sphere !");
    _sphere_ref_ = &sphere_;
    return;
  }

  void sphere_vg::set_sphere (const geomtools::sphere & sphere_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _sphere_ = sphere_;
    return;
  }

  const geomtools::sphere & sphere_vg::get_sphere_ref () const
  {
    DT_THROW_IF (_sphere_ref_ == 0, std::logic_error, "No sphere ref !");
    return *_sphere_ref_;
  }

  const geomtools::sphere & sphere_vg::get_sphere () const
  {
    return _sphere_;
  }

  bool sphere_vg::has_sphere_safe () const
  {
    if (_sphere_.is_valid ()) return true;
    if (_sphere_ref_ != 0 && _sphere_ref_->is_valid ()) return true;
    return false;
  }

  const geomtools::sphere & sphere_vg::get_sphere_safe () const
  {
    if (has_sphere_ref ()) {
      return get_sphere_ref ();
    }
    return get_sphere ();
  }

  bool sphere_vg::is_initialized () const
  {
    return _initialized_;
  }

  sphere_vg::sphere_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _sphere_.reset ();
    _sphere_ref_ = 0;
    _set_defaults_ ();
    return;
  }

  sphere_vg::~sphere_vg()
  {
    if (is_initialized ()) reset ();
    return;
  }

  void sphere_vg::initialize (const ::datatools::properties & setup_,
                              ::datatools::service_manager & /*service_manager_*/,
                              ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // parameters of the sphere vertex generator:
    int    mode = MODE_INVALID;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    double lunit = CLHEP::mm;
    std::string lunit_str;
    bool treat_mode           = false;
    bool treat_skin_skip      = false;
    bool treat_skin_thickness = false;

    if (_mode_ == MODE_INVALID) {
      if (setup_.has_key ("mode")) {
        std::string mode_str = setup_.fetch_string ("mode");
        if (mode_str == "surface") {
          mode = MODE_SURFACE;
        } else if (mode_str == "bulk") {
          mode = MODE_BULK;
        } else {
          DT_THROW_IF(true, std::logic_error, "Invalid mode '" << mode_str << "' !");
        }
        treat_mode = true;
      }
    }

    if (setup_.has_key ("length_unit")) {
      lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      if (! setup_.has_explicit_unit ("skin_skip")) skin_skip *= lunit;
      treat_skin_skip = true;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit ("skin_thickness")) skin_thickness *= lunit;
      treat_skin_thickness = true;
    }


    if (treat_mode) set_mode (mode);
    if (treat_skin_skip) set_skin_skip (skin_skip);
    if (treat_skin_thickness) set_skin_thickness (skin_thickness);

    if (! has_sphere_safe ()) {
      double sphere_r;
      datatools::invalidate(sphere_r);
      std::string sphere_user_ref = vg_tools::SHAPE_REF_NONE;
      std::string sphere_user_ref_name;

      if (setup_.has_key("sphere.radius")) {
        sphere_r = setup_.fetch_real("sphere.radius");
        if (! setup_.has_explicit_unit("sphere.radius")) sphere_r *= lunit;
      }

      geomtools::sphere sphere(sphere_r);
      set_sphere(sphere);
    }

    _init_ ();
    _initialized_ = true;
    return;
  }

  void sphere_vg::reset ()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void sphere_vg::_init_ ()
  {
    return;
  }

  void sphere_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }

  void sphere_vg::_set_defaults_ ()
  {
    _sphere_.reset ();
    _sphere_ref_ = 0;
    _mode_ = MODE_INVALID;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    return;
  }

  void sphere_vg::tree_dump (std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag;
    if (has_sphere_ref ()) {
      out_ << "External sphere : " << *_sphere_ref_ << " [" << _sphere_ref_ << ']';
    } else {
      out_ << "Embedded sphere : " << _sphere_;
    }
    out_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Mode :        " << _mode_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Skin skip:    " << _skin_skip_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Skin thickness: " << _skin_thickness_ << std::endl;
    return;
  }

  void sphere_vg::_shoot_vertex(::mygsl::rng & random_,
                                ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Not initialized !");
    geomtools::invalidate (vertex_);
    //double x = 0.0, y = 0.0, z = 0.0;
    const geomtools::sphere * the_sphere = &_sphere_;
    if (has_sphere_ref ()) {
      the_sphere = _sphere_ref_;
    }

    if (_mode_ == MODE_BULK) {
      double r_max = the_sphere->get_radius() - 0.5 * _skin_thickness_;
      randomize_sphere(random_,
                       0.0, r_max,
                       0.0, M_PI,
                       0.0, 2 * M_PI,
                       vertex_);
    }

    if (_mode_ == MODE_SURFACE) {
      double r_min = the_sphere->get_radius() + _skin_skip_;
      double r_max = r_min;
      //double delta_thick = 0.0;
      if (_skin_thickness_ > 0.0) {
        r_min -= 0.5 * _skin_thickness_;
        r_max += 0.5 * _skin_thickness_;
      }
      randomize_sphere(random_,
                       r_min, r_max,
                       0.0, M_PI,
                       0.0, 2 * M_PI,
                       vertex_);
     }

     return;
  }

  void randomize_sphere(mygsl::rng & random_,
                        double r1_, double r2_,
                        double theta1_, double theta2_,
                        double phi1_, double phi2_,
                        geomtools::vector_3d & vertex_)
  {
    double phi = random_.flat(phi1_, phi2_);
    double cost1 = std::cos(theta1_);
    double cost2 = std::cos(theta2_);
    double cost =  random_.flat(cost1, cost2);
    double sint = std::sqrt(1.0 - gsl_pow_2(cost));
    double r = r1_;
    if (r1_ != r2_) {
      double r13 = gsl_pow_3(r1_);
      double r23 = gsl_pow_3(r2_);
      double r3 = random_.flat(r13, r23);
      static const double one_third = 1.0 / 3.0;
      r = std::pow(r3, one_third);
    }
    vertex_.setX(r * sint * std::cos(phi));
    vertex_.setY(r * sint * std::sin(phi));
    vertex_.setZ(r * cost);
    return;
  }

} // end of namespace genvtx
