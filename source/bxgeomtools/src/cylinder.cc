// -*- mode: c++; -*-
/* cylinder.cc
 */

#include <string>

#include <geomtools/cylinder.h>

#include <datatools/exception.h>

namespace geomtools {

  using namespace std;

  const std::string & cylinder::cylinder_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "cylinder";
    }
    return label;
  }

  double cylinder::get_xmin () const
  {
    return -_radius_;
  }

  double cylinder::get_xmax () const
  {
    return +_radius_;
  }

  double cylinder::get_ymin () const
  {
    return -_radius_;
  }

  double cylinder::get_ymax () const
  {
    return +_radius_;
  }

  double cylinder::get_zmin () const
  {
    return -0.5*_z_;
  }

  double cylinder::get_zmax () const
  {
    return +0.5*_z_;
  }

  double
  cylinder::get_r () const
  {
    return _radius_;
  }

  void
  cylinder::set_r (double a_radius)
  {
    DT_THROW_IF (a_radius < 0.0,logic_error,
                 "Invalid '" << a_radius << "' R value !");
    _radius_ = a_radius;
    return;
  }

  double
  cylinder::get_radius () const
  {
    return get_r ();
  }

  double
  cylinder::get_diameter () const
  {
    return (_radius_ + _radius_);
  }

  void
  cylinder::set_diameter (double a_diameter)
  {
    set_r (a_diameter * 0.5);
    return;
  }

  double
  cylinder::get_z () const
  {
    return _z_;
  }

  void
  cylinder::set_z (double a_z)
  {
    DT_THROW_IF (a_z < 0.0, logic_error,
                 "Invalid '" << a_z << "' Z value !");
    _z_ = a_z;
    return;
  }

  double
  cylinder::get_half_z () const
  {
    return _z_ * 0.5;
  }

  void
  cylinder::set_half_z (double a_half_z)
  {
    set_z (a_half_z + a_half_z);
    return;
  }

  void
  cylinder::set (double a_radius, double a_z)
  {
    set_r (a_radius);
    set_z (a_z);
    return;
  }

  void
  cylinder::set_alternative (double a_diameter, double a_half_z)
  {
    set_diameter (a_diameter);
    set_half_z (a_half_z);
    return;
  }

  // ctor/dtor:
  cylinder::cylinder ()
  {
    reset ();
    return;
  }

  cylinder::cylinder (double a_radius, double a_z)
  {
    set (a_radius, a_z);
    return;
  }

  cylinder::~cylinder ()
  {
    return;
  }

  string
  cylinder::get_shape_name () const
  {
    return cylinder_label();
  }

  double
  cylinder::get_surface (uint32_t a_mask) const
  {
    double s = 0.0;
    int mask = a_mask;
    if ((int) a_mask == (int) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_SIDE)
      {
        s += 2 * M_PI * _radius_ * _z_;
      }
    if (mask & FACE_BOTTOM)
      {
        s += M_PI * _radius_ * _radius_;
      }
    if (mask & FACE_TOP)
      {
        s += M_PI * _radius_ * _radius_;
      }
    return s;
  }

  double
  cylinder::get_volume (uint32_t /*flags*/) const
  {
    return M_PI * _radius_ * _radius_ * _z_;
  }

  double
  cylinder::get_parameter ( const string & flag_ ) const
  {
    if ( flag_ == "r" ) return get_r ();
    if ( flag_ == "radius" ) return get_r ();
    if ( flag_ == "diameter" ) return get_diameter ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    DT_THROW_IF(true, logic_error, "Unknown flag '" << flag_ << "' !");
  }


  bool
  cylinder::is_valid () const
  {
    return (_radius_ > 0.0 && _z_ > 0.0);
  }


  void
  cylinder::init ()
  {
    return;
  }

  void
  cylinder::reset ()
  {
    _radius_ = -1.0;
    _z_ = -1.0;
    return;
  }

  bool
  cylinder::is_inside (const vector_3d & a_position, double a_skin) const
  {
    double skin = get_skin ();
    if (a_skin > USING_PROPER_SKIN) skin = a_skin;

    double r = hypot (a_position.x (), a_position.y ());
    if ( r > _radius_ + 0.5 * skin ) return false;
    if ( abs (a_position.z ()) > 0.5 *_z_ + 0.5 * skin ) return false;
    return true;
  }

  vector_3d
  cylinder::get_normal_on_surface (const vector_3d & a_position) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (a_position, FACE_SIDE))
      {
        double phi = a_position.phi ();
        normal.set (cos (phi), sin (phi), 0.0);
      }
    else if (is_on_surface (a_position, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (a_position, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
    return (normal);
  }

  bool
  cylinder::is_on_surface (const vector_3d & a_position ,
                           int    a_mask ,
                           double a_skin) const
  {
    double skin = get_skin ();
    if (a_skin > USING_PROPER_SKIN) skin = a_skin;

    int mask = a_mask;
    if (a_mask == (int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;
    double r = hypot (a_position.x (), a_position.y ());
    if (mask & FACE_BOTTOM)
      {
        if ((abs (a_position.z () + 0.5 * _z_) < hskin)
            && (r < (_radius_ + hskin))) return true;
      }
    if (mask & FACE_TOP)
      {
        if ((abs (a_position.z () - 0.5 * _z_) < hskin)
            && (r < (_radius_ + hskin))) return true;
      }
    if (mask & FACE_SIDE)
      {
        if ((abs (a_position.z ()) < (0.5 * _z_ + hskin))
            && (abs (r - _radius_) < hskin)) return true;
      }
    return false;
  }

  bool
  cylinder::find_intercept (const vector_3d & a_from,
                            const vector_3d & a_direction,
                            intercept_t & a_intercept,
                            double a_skin) const
  {
    bool debug = false;
    //debug = true;
    const unsigned int NFACES = 3;
    double t[NFACES];
    const int CYL_SIDE = 0;
    const int CYL_BOTTOM = 1;
    const int CYL_TOP = 2;
    t[CYL_SIDE] = -1.0; // side
    t[CYL_BOTTOM] = -1.0; // bottom
    t[CYL_TOP] = -1.0; // top
    double a, b, c;
    double x0 = a_from.x ();
    double y0 = a_from.y ();
    double dx = a_direction.x ();
    double dy = a_direction.y ();
    a = dx * dx + dy * dy;
    b = 2. * (dx * x0 + dy * y0);
    c = x0 * x0 + y0 * y0 - _radius_ * _radius_;
    double delta = b * b - 4. * a * c;
    double ts[2];
    if (debug)
      {
        clog << "DEVEL: cylinder::find_intercept: from= "
             << a_from
             << endl;
        clog << "DEVEL: cylinder::find_intercept: direction= "
             << a_direction
             << endl;
        clog << "DEVEL: cylinder::find_intercept: a= "
             << a
             << endl;
        clog << "DEVEL: cylinder::find_intercept: b= "
             << b
             << endl;
        clog << "DEVEL: cylinder::find_intercept: c= "
             << c
             << endl;
        clog << "DEVEL: cylinder::find_intercept: delta= "
             << delta
             << endl;
      }
    if (delta >= 0.0)
      {
        double q = sqrt (delta);
        double n = a + a;
        ts[0] = (- b + q) / n;
        ts[1] = (- b - q) / n;
      }
    if (debug)
      {
        clog << "DEVEL: cylinder::find_intercept: ts[" << 0 << "]= "
             << ts[0]
             << endl;
        clog << "DEVEL: cylinder::find_intercept: ts[" << 1 << "]= "
             << ts[1]
             << endl;
      }
    for (int i = 0; i < 2; i++)
      {
        double tsi = ts[i];
        if (isnormal (tsi) && (tsi > 0.0))
          {
            if (t[CYL_SIDE] < 0)
              {
                t[CYL_SIDE] = tsi;
              }
            else
              {
                if (tsi < t[CYL_SIDE])
                  {
                    t[CYL_SIDE] = tsi;
                  }
              }
          }
      }

    double z0 = a_from.z ();
    double dz = a_direction.z ();
    t[CYL_BOTTOM] = (-0.5 * _z_ - z0) / dz;
    t[CYL_TOP]    = (+0.5 * _z_ - z0) / dz;

    double t_min = -1.0;
    int face_min = 0;
    for (int i = 0; i < (int) NFACES; i++)
      {
        double ti = t[i];
        if (debug)
          {
            clog << "DEVEL: cylinder::find_intercept: t[" << i << "]= "
                 << ti << " t_min=" << t_min
                 << " face_min=" << face_min
                 << endl;
          }
        if (isnormal (ti) && (ti > 0.0))
          {
            int face_bit = (0x1 << i); // face mask
            vector_3d intercept = a_from + a_direction * ti;
            if (is_on_surface (intercept, face_bit, a_skin))
              {
                if ((t_min < 0.0) || (ti < t_min))
                  {
                    t_min = ti;
                    face_min = face_bit;
                  }
              }
          }
      }
    a_intercept.reset ();
    if (face_min > 0)
      {
        a_intercept.set (0, face_min, a_from + a_direction * t_min);
      }
    return a_intercept.is_ok ();
  }

  ostream & operator<< (ostream & a_out, const cylinder & a_cylinder)
  {
    a_out << '{' << cylinder::cylinder_label() << ' '
          << a_cylinder._radius_ << ' '
          << a_cylinder._z_ << '}';
    return a_out;
  }

  istream & operator>> ( istream & a_in, cylinder & a_cylinder)
  {
    a_cylinder.reset ();
    char c = 0;
    a_in.get (c);
    if (c != '{')
      {
        a_in.clear (ios_base::failbit);
        return a_in;
      }
    string name;
    a_in >> name;
    if (name != cylinder::cylinder_label())
      {
        a_in.clear (ios_base::failbit);
        return a_in;
      }
    double r, z;
    a_in >> r >> z;
    if (! a_in)
      {
        a_in.clear (ios_base::failbit);
        return a_in;
      }
    c = 0;
    a_in.get (c);
    if (c != '}')
      {
        a_in.clear (ios_base::failbit);
        return a_in;
      }
    try
      {
        a_cylinder.set (r,z);
      }
    catch (...)
      {
        a_cylinder.reset ();
        a_in.clear (ios_base::failbit);
      }
    return a_in;
  }

  void cylinder::tree_dump (ostream & a_out,
                            const string & a_title,
                            const string & a_indent,
                            bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ()) indent = a_indent;
    i_object_3d::tree_dump (a_out, a_title, a_indent, true);

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Radius : " << get_r () / CLHEP::mm << " mm" << endl;
    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Z : " << get_z () / CLHEP::mm << " mm" << endl;
    return;
  }

  void cylinder::generate_wires (std::list<polyline_3d> & lpl_,
                                 const placement & p_,
                                 uint32_t /*options_*/) const
  {
    const int nsamples = 36;
    for (int j = 0; j < 2; j++)
      {
        double z = -0.5 * get_z () + j * get_z ();
        for (int jj = 0; jj < 3; jj++)
          {
            double r = get_r ();
            r /= (1 + jj);
            vector_3d vertex[nsamples];
            for (int i = 0; i < nsamples; i++)
              {
                double thetai = i * 2. * M_PI/nsamples;
                vertex[i].set (r * std::cos (thetai),
                               r * std::sin (thetai),
                               z);
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
          }
        for (int k = 0; k < 2; k++)
          {
            vector_3d vertex[2];
            vertex[0].set (-(k%2)*get_r (), -((k+1)%2)*get_r (), z);
            vertex[1].set (+(k%2)*get_r (), +((k+1)%2)*get_r (), z);
            {
              polyline_3d dummy;
              lpl_.push_back (dummy);
            }
            polyline_3d & pl = lpl_.back ();
            pl.set_closed (false);
            for (int i = 0; i < 2; i++)
              {
                vector_3d v;
                p_.child_to_mother (vertex[i], v);
                pl.add (v);
              }
          }
      /*
        vector_3d vertex[nsamples];
        for (int i = 0; i < nsamples; i++)
          {
            double thetai = i * 2. * M_PI/nsamples;
            vertex[i].set (get_r () * std::cos (thetai),
                           get_r () * std::sin (thetai),
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
        */
      }
     for (int i = 0; i < nsamples; i++)
      {
        vector_3d vertex[2];
        double thetai = i * 2. * M_PI/nsamples;
        double x = get_r () * std::cos (thetai);
        double y = get_r () * std::sin (thetai);
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
    return;
  }

} // end of namespace geomtools

// end of cylinder.cc
