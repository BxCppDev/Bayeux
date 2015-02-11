/// \file geomtools/elliptical_tube.cc

// Ourselves:
#include <geomtools/elliptical_tube.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/cylinder.h>

namespace geomtools {

  using namespace std;

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(elliptical_tube,
                                             "geomtools::elliptical_tube");

  const std::string & elliptical_tube::elliptical_tube_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "elliptical_tube";
    }
    return label;
  }

  void elliptical_tube::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
  }

  double elliptical_tube::get_xmin () const
  {
    return -_x_radius_;
  }

  double elliptical_tube::get_xmax () const
  {
    return _x_radius_;
  }

  double elliptical_tube::get_ymin () const
  {
    return -_y_radius_;
  }

  double elliptical_tube::get_ymax () const
  {
    return _y_radius_;
  }

  double elliptical_tube::get_zmin () const
  {
    return -0.5*_z_;
  }

  double elliptical_tube::get_zmax () const
  {
    return +0.5*_z_;
  }

  double elliptical_tube::get_z () const
  {
    return _z_;
  }

  void elliptical_tube::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0,std::logic_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  double elliptical_tube::get_half_z () const
  {
    return 0.5 * _z_;
  }

  void elliptical_tube::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
    return;
  }

  double elliptical_tube::get_x_radius () const
  {
    return _x_radius_;
  }

  double elliptical_tube::get_y_radius () const
  {
    return _y_radius_;
  }

  void elliptical_tube::set_x_radius (double xr_)
  {
    DT_THROW_IF (xr_ < 0.0, std::logic_error, "Invalid inner radius '" << xr_ << "' !");
    _x_radius_ = xr_;
    return;
  }

  void elliptical_tube:: set_y_radius (double yr_)
  {
    DT_THROW_IF (yr_ < 0.0, std::logic_error, "Invalid inner radius '" << yr_ << "' !");
    _y_radius_ = yr_;
    return;
  }

  void elliptical_tube::set (double xr_, double yr_, double z_)
  {
    set_x_radius (xr_);
    set_y_radius (yr_);
    set_z (z_);
    return;
  }

  elliptical_tube::elliptical_tube ()
  {
    _set_default();
    return;
  }

  elliptical_tube::elliptical_tube (double xr_, double yr_, double z_)
  {
    _set_default();
    set (xr_, yr_, z_);
    return;
  }

  elliptical_tube::~elliptical_tube ()
  {
    reset();
    return;
  }

  double elliptical_tube::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    if ( mask_ & FACE_SIDE ) {
      // Ramanujan's approximation
      s += M_PI * ( 3 * (_x_radius_+_y_radius_)
                    - std::sqrt( (3*_x_radius_+_y_radius_)*(_x_radius_+3*_y_radius_) ) ) * _z_;
    }
    if ( mask_ & FACE_BOTTOM ) {
      s += M_PI * _x_radius_ * _y_radius_;
    }
    if ( mask_ & FACE_TOP ) {
      s += M_PI * _x_radius_ * _y_radius_;
    }
    return s;
  }

  double elliptical_tube::get_volume (uint32_t /*flags*/) const
  {
    return M_PI * _x_radius_ * _y_radius_ * _z_;
  }

  bool elliptical_tube::is_valid () const
  {
    return (_x_radius_ > 0.0 && _y_radius_ > 0.0 && _z_ > 0.0);
  }

  void elliptical_tube::_set_default()
  {
    datatools::invalidate (_x_radius_);
    datatools::invalidate (_y_radius_);
    datatools::invalidate (_z_);
    return;
  }

  void elliptical_tube::reset ()
  {
    unlock();

    _set_default();

    this->i_shape_3d::reset();
    return;
  }

  std::string elliptical_tube::get_shape_name () const
  {
    return elliptical_tube::elliptical_tube_label();
  }

  bool elliptical_tube::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;

    if ( (point_.x()*point_.x())/((_x_radius_-hskin)*(_x_radius_-hskin))
         + (point_.y()*point_.y())/((_y_radius_-hskin)*(_y_radius_-hskin)) < 1.
         && (std::abs(point_.z()) <= (0.5*_z_-hskin))
         ) return true;
    return false;
  }

  bool elliptical_tube::is_outside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;

    if ( (point_.x()*point_.x())/((_x_radius_+hskin)*(_x_radius_+hskin))
         + (point_.y()*point_.y())/((_y_radius_+hskin)*(_y_radius_+hskin)) > 1.
         || (std::abs(point_.z()) <= (0.5*_z_-hskin))
         ) return true;
    return false;
  }

  double elliptical_tube::get_parameter ( const std::string & flag_ ) const
  {
    if ( flag_ == "x_radius" ) return get_x_radius ();
    if ( flag_ == "y_radius" ) return get_y_radius ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::runtime_error, "Unknown flag !");
  }

  vector_3d elliptical_tube::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_SIDE))
      {
        normal.set (position_.x()/(_x_radius_*_x_radius_),
                    position_.y()/(_y_radius_*_y_radius_), 0.0);
      }
    else if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
    return (normal);
  }

  bool elliptical_tube::is_on_surface (const vector_3d & point_,
                            int mask_,
                            double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_SIDE)
      {
        if ( ( (point_.x()*point_.x())/((_x_radius_-0.5*skin)*(_x_radius_-0.5*skin))
               + (point_.y()*point_.y())/((_y_radius_-0.5*skin)*(_y_radius_-0.5*skin)) > 1. )
             && ( (point_.x()*point_.x())/((_x_radius_+0.5*skin)*(_x_radius_+0.5*skin))
                  + (point_.y()*point_.y())/((_y_radius_+0.5*skin)*(_y_radius_+0.5*skin)) < 1. )
             && ( std::abs (point_.z ()) < 0.5 * _z_ ) )
          return true;
      }
    if (mask & FACE_BOTTOM)
      {
        if ( ( (point_.x()*point_.x())/(_x_radius_*_x_radius_)
               + (point_.y()*point_.y())/(_y_radius_*_y_radius_) < 1. )
             && ( std::abs (point_.z () + 0.5 * _z_) < 0.5 * skin ) )
          return true;
      }
    if (mask & FACE_TOP)
      {
        if ( ( (point_.x()*point_.x())/(_x_radius_*_x_radius_)
               + (point_.y()*point_.y())/(_y_radius_*_y_radius_) < 1. )
             && ( std::abs (point_.z () - 0.5 * _z_) < 0.5 * skin ) )
          return true;
      }
    return false;
  }

  bool elliptical_tube::find_intercept (const vector_3d & /*from_*/,
                             const vector_3d & /*direction_*/,
                             intercept_t & intercept_,
                             double /*skin_*/) const
  {
    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    intercept_.reset ();
    return intercept_.is_ok ();
  }

  std::ostream & operator<< (std::ostream & out_, const elliptical_tube & t_)
  {
    out_ << '{' << elliptical_tube::elliptical_tube_label() << ' '
         << t_._x_radius_ << ' '
         << t_._y_radius_ << ' '
         << t_._z_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, elliptical_tube & t_)
  {
    t_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    std::string name;
    in_ >> name;
    if (name != elliptical_tube::elliptical_tube_label())
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double xr, yr, z;
    in_ >> xr >> yr >> z;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    c = 0;
    in_.get (c);
    if (c != '}')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    try
      {
        t_.set (xr, yr, z);
      }
    catch (...)
      {
        t_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void elliptical_tube::tree_dump (ostream & out_,
                        const string & title_,
                        const string & indent_,
                        bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius () / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;
    return;
  }

  void elliptical_tube::generate_wires (std::list<polyline_3d> & lpl_,
                             const placement & p_,
                             uint32_t /*options_*/) const
  {
    const int nsamples = 36;
    for (int j = 0; j < 2; j++)
      {
        vector_3d vertex[nsamples];
        for (int i = 0; i < nsamples; i++)
          {
            double thetai = i * 2. * M_PI/nsamples;
            vertex[i].set (_x_radius_ * std::cos (thetai),
                           _y_radius_ * std::sin (thetai),
                           -0.5 * get_z () + j * get_z ());
          }
        {
          polyline_3d dummy;
          lpl_.push_back (dummy);
        }
        polyline_3d & pl = lpl_.back ();
        pl.set_closed (true);
        for (int i = 0; i < 36; i++)
          {
            vector_3d v;
            p_.child_to_mother (vertex[i], v);
            pl.add (v);
          }


        for (int i = 0; i < nsamples; i++)
          {
            vector_3d vertex[2];
            double thetai = i * 2. * M_PI/nsamples;
            double x = _x_radius_ * std::cos (thetai);
            double y = _y_radius_ * std::sin (thetai);
            vertex[0].set (x, y, -0.5 * get_z ());
            vertex[1].set (x, y, +0.5 * get_z ());
            {
              polyline_3d dummy;
              lpl_.push_back (dummy);
              polyline_3d & pl = lpl_.back ();
              pl.set_closed (false);
              for (int i = 0; i < 2; i++)
                {
                  vector_3d v;
                  p_.child_to_mother (vertex[i], v);
                  pl.add (v);
                }
            }
          }
      }
    return;
  }

  void elliptical_tube::initialize(const datatools::properties & config_,
                                   const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    // double aunit = CLHEP::degree;
    // if (config_.has_key("angle_unit")) {
    //   const std::string aunit_str = config_.fetch_string("angle_unit");
    //   aunit = datatools::units::get_angle_unit_from(aunit_str);
    // }

    double x_radius;
    datatools::invalidate (x_radius);
    if (config_.has_key ("x_radius")) {
      x_radius = config_.fetch_real ("x_radius");
      if (! config_.has_explicit_unit ("x_radius")) {
        x_radius *= lunit;
      }
    } else if (config_.has_key ("x_diameter")) {
      x_radius = 0.5 * config_.fetch_real ("x_diameter");
      if (! config_.has_explicit_unit ("x_diameter")) {
        x_radius *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (x_radius), std::logic_error,
                 "Missing elliptical_tube 'x_radius' property !");

    double y_radius;
    datatools::invalidate (y_radius);
    if (config_.has_key ("y_radius")) {
      y_radius = config_.fetch_real ("y_radius");
      if (! config_.has_explicit_unit ("y_radius")) {
        y_radius *= lunit;
      }
    } else if (config_.has_key ("y_diameter")) {
      y_radius = 0.5 * config_.fetch_real ("y_diameter");
      if (! config_.has_explicit_unit ("y_diameter")) {
        y_radius *= lunit;
      }
    }
    DT_THROW_IF (! datatools::is_valid (y_radius), std::logic_error,
                 "Missing elliptical_tube 'y_radius' property !");

    DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                 "Missing elliptical_tube 'z' property !");
    double z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      z *= lunit;
    }
    set(x_radius, y_radius, z);
    lock();
    return;
  }

  // static
  void elliptical_tube::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the elliptical tube.\n"
                              )
        .add_example("Set the default length unit::       \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x_radius")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The x radius of the elliptical tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the x radius::                    \n"
                     "                                      \n"
                     "  x_radius : real as length = 2.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x_diameter")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The x diameter of the elliptical tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``x_radius`` property is set.")
        .add_example("Set the x diameter::                    \n"
                     "                                        \n"
                     "  x_diameter : real as length = 4.0 cm  \n"
                     "                                        \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_radius")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The y radius of the elliptical tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the y radius::                   \n"
                     "                                     \n"
                     "  y_radius : real as length = 5.0 cm \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_diameter")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The y diameter of the elliptical tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``y_radius`` property is set.")
        .add_example("Set the y diameter::                    \n"
                     "                                        \n"
                     "  y_diameter : real as length = 10.0 cm \n"
                     "                                        \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::elliptical_tube")
        .set_terse_description("The dimension of the elliptical tube along the Z axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Z dimension::          \n"
                     "                               \n"
                     "  z : real as length = 5.0 mm  \n"
                     "                               \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::elliptical_tube the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::elliptical_tube, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::elliptical_tube");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D elliptical_tube solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The elliptical_tube solid 3D-shape is defined by \n"
                               "its three dimensions.                            \n"
                               );

  // Populate the specific OCD :
  geomtools::elliptical_tube::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  x_radius : real as length = 50.0 mm            \n"
                               "  y_radius : real as length = 10.0 mm            \n"
                               "  z        : real           = 20.0 cm            \n"
                               "                                                 \n"
                               );

  /* Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides(see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::elliptical_tube' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::elliptical_tube, "geomtools::elliptical_tube")
