// circle.cc

// Ourselves:
#include <geomtools/circle.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace geomtools {

  using namespace std;

  //DT_STATIC_CONST_STRING_REF_DEFINITION(circle, circle_label, "circle");
  const std::string & circle::circle_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "circle";
    }
    return label;
  }

  string circle::get_shape_name () const
  {
    return circle_label();
  }

  unsigned int circle::get_number_of_path() const
  {
    return 1;
  }

  double circle::get_length(uint32_t flags_) const
  {
    if (flags_ == PATH_NONE) return 0.0;
    return get_circumference();
  }

  double circle::get_r () const
  {
    return _radius_;
  }

  double circle::get_radius () const
  {
    return get_r ();
  }

  void circle::set_diameter (double a_diameter)
  {
    set_r (a_diameter * 0.5);
    return;
  }

  double circle::get_diameter () const
  {
    return (_radius_ + _radius_);
  }

  void circle::set_r (double a_radius)
  {
    DT_THROW_IF (a_radius < 0.0,logic_error,
                 "Invalid '" << a_radius << "' R value !");
    _radius_ = a_radius;
    return;
  }

  double circle::get_surface () const
  {
    return M_PI * _radius_ * _radius_;
  }

  double circle::get_circumference () const
  {
    return 2 * M_PI * _radius_;
  }

  bool circle::is_valid () const
  {
    return (_radius_ > 0.0);
  }

  // ctor:
  circle::circle ()
  {
    _radius_ = -1.0;
    return;
  }

  // ctor:
  circle::circle (double a_radius)
  {
    set_r (a_radius);
    return;
  }

  // dtor:
  circle::~circle ()
  {
    return;
  }

  void circle::tree_dump (ostream & a_out,
                          const string & a_title,
                          const string & a_indent,
                          bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ()) indent = a_indent;
    i_object_3d::tree_dump (a_out, a_title, indent, true);

    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
         << "Radius : " << get_r () / CLHEP::mm << " mm" << endl;
    return;
  }

  bool circle::is_on_curve (const vector_3d &,
                            double /*a_tolerance*/) const
  {
    bool on_curve = false;
    DT_THROW_IF(true, runtime_error,"Not implemented yet !");
    return on_curve;
  }

  vector_3d circle::get_direction_on_curve (const vector_3d & /*a_position*/) const
  {
    vector_3d dir;
    invalidate (dir);
    DT_THROW_IF(true, runtime_error,"Not implemented yet !");
    return dir;
  }

  void circle::generate_wires (std::list<polyline_3d> & lpl_,
                               const placement & p_,
                               uint32_t /*options_*/) const
  {
    const int nsamples = 36;
    vector_3d vertex[nsamples];
    for (int i = 0; i < nsamples; i++)
      {
        double thetai = i * 2. * M_PI/nsamples;
        vertex[i].set (get_r () * std::cos (thetai),
                       get_r () * std::sin (thetai),
                       0.0);
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
    return;
  }

} // end of namespace geomtools
