/// \file geomtools/i_shape_3d.cc

// Ourselves:
#include <geomtools/i_shape_3d.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  const double i_shape_3d::DEFAULT_SKIN      = GEOMTOOLS_DEFAULT_TOLERANCE;
  const double i_shape_3d::ZERO_SKIN         = GEOMTOOLS_ZERO_TOLERANCE;
  const double i_shape_3d::USING_PROPER_SKIN = GEOMTOOLS_PROPER_TOLERANCE;

  // static
  const std::string i_shape_3d::volume_key()
  {
    static std::string _key("shape_3d.volume");
    return _key;
  }

  // static
  const std::string i_shape_3d::surface_key()
  {
    static std::string _key("shape_3d.surface");
    return _key;
  }

  // static
  double i_shape_3d::get_default_skin()
  {
    return GEOMTOOLS_DEFAULT_TOLERANCE;
  }

  // static
  double i_shape_3d::get_zero_skin()
  {
    return GEOMTOOLS_ZERO_TOLERANCE;
  }

  // static
  double i_shape_3d::get_proper_skin()
  {
    return GEOMTOOLS_PROPER_TOLERANCE;
  }

  bool i_shape_3d::has_number_of_faces() const
  {
    return get_number_of_faces() > 0;
  }

  unsigned int i_shape_3d::get_number_of_faces() const
  {
    return 0;
  }

  bool i_shape_3d::has_volume(uint32_t flags_) const
  {
    if (get_properties().has_key(volume_key())) {
      double v = get_properties().fetch_real(volume_key());
      if (! get_properties().has_explicit_unit(volume_key())) {
        // v *= 1.0; // No default volume unit;
      }
      return datatools::is_valid(v);
    }
    return datatools::is_valid(get_volume(flags_));
  }

  double i_shape_3d::get_volume(uint32_t /*flags_*/) const
  {
    if (get_properties().has_key(volume_key())) {
      double v = get_properties().fetch_real(volume_key());
      if (! get_properties().has_explicit_unit(volume_key())) {
        // v *= 1.0; // No default volume unit;
      }
      return v;
    }
    return std::numeric_limits<double>::quiet_NaN();
  }

  bool i_shape_3d::has_surface(uint32_t flags_) const
  {
    if (get_properties().has_key(surface_key())) {
      double v = get_properties().fetch_real(surface_key());
      if (! get_properties().has_explicit_unit(surface_key())) {
        //v *= 1.0; // No default surface unit;
      }
      return datatools::is_valid(v);
    }
    return datatools::is_valid(get_surface(flags_));
  }

  double i_shape_3d::get_surface(uint32_t /*flags_*/) const
  {
    if (get_properties().has_key(surface_key())) {
      double v = get_properties().fetch_real(surface_key());
      if (! get_properties().has_explicit_unit(surface_key())) {
        // v *= 1.0; // No default surface unit;
      }
      return v;
    }
    return std::numeric_limits<double>::quiet_NaN();
  }

  bool i_shape_3d::is_composite() const
  {
    return false;
  }

  bool i_shape_3d::is_stackable(const i_shape_3d & a_shape)
  {
    if (a_shape.has_stackable_data ()) {
      if (a_shape.get_stackable_data ().is_valid ()) {
        return true;
      }
    }
    const i_stackable * the_stackable
      = dynamic_cast<const i_stackable *> (&a_shape);
    if (the_stackable != 0) {
      return true;
    }
    return false;
  }

  bool i_shape_3d::is_xmin_stackable(const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_xmin (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_xmax_stackable (const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_xmax (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_ymin_stackable(const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_ymin (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_ymax_stackable(const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_ymax (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_zmin_stackable(const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_zmin (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_zmax_stackable(const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_zmax (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::pickup_stackable_with_properties(const i_shape_3d & a_shape,
                                                     stackable_data & a_stackable_data)
  {
    bool ok = false;
    ok = i_shape_3d::pickup_stackable (a_shape, a_stackable_data);
    if (ok) {
      if (stackable::has_xmin (a_shape.get_properties ())) {
        a_stackable_data.xmin = stackable::get_xmin (a_shape.get_properties (), -1.0);
      }
      if (stackable::has_xmax (a_shape.get_properties ())) {
        a_stackable_data.xmax = stackable::get_xmax (a_shape.get_properties (), -1.0);
      }
      if (stackable::has_ymin (a_shape.get_properties ())) {
        a_stackable_data.ymin = stackable::get_ymin (a_shape.get_properties (), -1.0);
      }
      if (stackable::has_ymax (a_shape.get_properties ())) {
        a_stackable_data.ymax = stackable::get_ymax (a_shape.get_properties (), -1.0);
      }
      if (stackable::has_zmin (a_shape.get_properties ())) {
        a_stackable_data.zmin = stackable::get_zmin (a_shape.get_properties (), -1.0);
      }
      if (stackable::has_zmax (a_shape.get_properties ())) {
        a_stackable_data.zmax = stackable::get_zmax (a_shape.get_properties (), -1.0);
      }
    }
    return ok;
  }

  bool i_shape_3d::pickup_stackable(const i_shape_3d & a_shape,
                                     stackable_data & a_stackable_data)
  {
    a_stackable_data.invalidate();
    bool ok = false;
    if (a_shape.has_stackable_data()) {
      if (a_shape.get_stackable_data().is_valid()) {
        a_stackable_data = a_shape.get_stackable_data();
        ok =  true;
      }
    }
    if (! ok) {
      const i_stackable * the_stackable
        = dynamic_cast<const i_stackable *> (&a_shape);
      if (the_stackable != 0) {
        a_stackable_data.xmin = the_stackable->get_xmin();
        a_stackable_data.xmax = the_stackable->get_xmax();
        a_stackable_data.ymin = the_stackable->get_ymin();
        a_stackable_data.ymax = the_stackable->get_ymax();
        a_stackable_data.zmin = the_stackable->get_zmin();
        a_stackable_data.zmax = the_stackable->get_zmax();
        ok = true;
      }
    }
    return ok;
  }

  double i_shape_3d::get_skin(double a_skin) const
  {
    DT_THROW_IF(std::isinf(a_skin), std::logic_error,
                "Invalid infinite skin thickness for shape '" << get_shape_name() << "' !");
    if (a_skin < 0.0 || std::isnan(a_skin)) {
      return get_tolerance();
    } else {
      return a_skin;
    }
  }

  double i_shape_3d::get_skin() const
  {
    return get_tolerance();
  }

  void i_shape_3d::set_skin(double a_skin)
  {
    set_tolerance(a_skin);
  }

  int i_shape_3d::get_dimensional() const
  {
    return DIMENSIONAL_3;
  }

  bool i_shape_3d::has_stackable_data() const
  {
    return _stackable_data_ != 0;
  }

  bool i_shape_3d::owns_stackable_data () const
  {
    return has_stackable_data() && _owns_stackable_data_;
  }

  const stackable_data & i_shape_3d::get_stackable_data() const
  {
    return *_stackable_data_;
  }

  void i_shape_3d::reset_stackable_data()
  {
    if (_stackable_data_ != 0) {
      if (_owns_stackable_data_) {
        delete _stackable_data_;
        _stackable_data_ = 0;
        _owns_stackable_data_ = false;
      }
    }
    return;
  }

  void i_shape_3d::set_stackable_data(const stackable_data & a_stackable_data)
  {
    if (&a_stackable_data == _stackable_data_) return;
    reset_stackable_data();
    _owns_stackable_data_ = false;
    _stackable_data_ = &a_stackable_data;
    return;
  }

  void i_shape_3d::set_stackable_data(const stackable_data * a_stackable_data)
  {
    if (a_stackable_data == _stackable_data_) return;
    reset_stackable_data();
    _owns_stackable_data_ = true;
    _stackable_data_ = a_stackable_data;
    return;
  }

  i_shape_3d::i_shape_3d() : i_object_3d()
  {
    _owns_stackable_data_ = false;
    _stackable_data_ = 0;
    return;
  }

  i_shape_3d::i_shape_3d(double a_skin) : i_object_3d(a_skin)
  {
    _owns_stackable_data_ = false;
    _stackable_data_ = 0;
    return;
  }

  i_shape_3d::~i_shape_3d ()
  {
    reset_stackable_data();
    return;
  }

  void i_shape_3d::initialize(const datatools::properties & config_)
  {
    this->i_object_3d::initialize(config_);
    return;
  }

  void i_shape_3d::reset()
  {
    reset_stackable_data();
    this->i_object_3d::reset();
    return;
  }

  shape_domain_flags_type i_shape_3d::where_is(const vector_3d & position_, double a_skin) const
  {
    try {
      if (is_inside(position_, a_skin)) {
        return SHAPE_DOMAIN_INSIDE;
      } else if (is_on_surface(position_, FACE_ALL_BITS, a_skin)) {
        return SHAPE_DOMAIN_ON_SURFACE;
      } else {
        return SHAPE_DOMAIN_OUTSIDE;
      }
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot determine the domain for position " << position_ << " w/r to shape '"
                   << this->get_shape_name() << "' : "
                   << error.what());
    } catch(...) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot determine the domain for position " << position_ << " w/r to shape '"
                   << this->get_shape_name() << "' !");
    }
    return SHAPE_DOMAIN_NONE;
  }

  bool i_shape_3d::is_outside(const vector_3d & a_position,
                              double a_skin) const
  {
    if (a_skin == get_zero_skin()) {
      return ! is_inside(a_position, a_skin);
    }
    return ! is_inside(a_position, a_skin)
      && ! is_on_surface(a_position, ALL_SURFACES, a_skin);
  }

  bool i_shape_3d::is_inside_or_surface (const vector_3d & a_position,
                                         double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const
  {
    return ! is_outside(a_position, a_skin);
  }

  void i_shape_3d::tree_dump(ostream & a_out,
                             const string & a_title,
                             const string & a_indent,
                             bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ()) indent = a_indent;
    i_object_3d::tree_dump (a_out, a_title, a_indent, true);
    if (i_shape_3d::is_stackable (*this)) {
      stackable_data SD;
      i_shape_3d::pickup_stackable (*this, SD);
      a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
            << "Stackable data : ";
      if (_stackable_data_ != 0) {
        a_out << "[plugged]";
      } else {
        a_out << "[native]";
      }
      a_out << endl;
      ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
      SD.tree_dump (a_out, "", indent_oss.str ());
    }
    /*
      a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
      << "Stackable_data : " << (_stackable_data_ != 0? "Yes": "No") << endl;
    */
    return;
  }

  bool i_shape_3d::find_intercept (const vector_3d & /*a_from*/,
                                   const vector_3d & /*a_direction*/,
                                   intercept_t & a_intercept,
                                   double /*a_skin*/) const
  {
    // temporary:
    // NOT IMPLEMENTED !
    // default: no intercept on any face of the 3D shape...
    a_intercept.reset();
    return a_intercept.is_ok();
  }

  // static
  void i_shape_3d::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_object_3d::init_ocd(ocd_);


    return;
  }

} // end of namespace geomtools
