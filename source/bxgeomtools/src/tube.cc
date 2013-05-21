// -*- mode: c++; -*-
/* tube.cc
 */

#include <geomtools/tube.h>

#include <cmath>
#include <stdexcept>
#include <sstream>

#include <datatools/exception.h>

#include <geomtools/cylinder.h>

namespace geomtools {

  using namespace std;

  const std::string tube::TUBE_LABEL = "tube";

    double tube::get_xmin () const
    {
      return -_outer_r_;
    }

    double tube::get_xmax () const
    {
      return +_outer_r_;
    }

    double tube::get_ymin () const
    {
      return -_outer_r_;
    }

    double tube::get_ymax () const
    {
      return +_outer_r_;
    }

    double tube::get_zmin () const
    {
      return -0.5*_z_;
    }

    double tube::get_zmax () const
    {
      return +0.5*_z_;
    }

  double tube::get_z () const
  {
    return _z_;
  }

  void tube::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0,std::logic_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  double tube::get_half_z () const
  {
    return 0.5 * _z_;
  }

  void tube::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
    return;
  }

  bool tube::is_extruded () const
  {
    return _inner_r_ > 0.0;
  }

  double tube::get_inner_r () const
  {
    return _inner_r_;
  }

  double tube::get_outer_r () const
  {
    return _outer_r_;
  }

  void tube::set_radii (double inner_r_, double outer_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::logic_error, "Invalid inner radius '" << inner_r_ << "' !");
    _inner_r_ = inner_r_;
    DT_THROW_IF (outer_r_ < 0.0, std::logic_error, "Invalid outer radius '" << outer_r_ << "' !");
    _outer_r_ = outer_r_;
    DT_THROW_IF (_outer_r_ <= _inner_r_,
                 std::logic_error,
                 "Outer radius '"
                 << _outer_r_ << "' <= inner radius '"
                 << _inner_r_ << "'!");
    return;
  }

  void tube::set (double ir_, double or_, double z_)
  {
    set_radii (ir_ ,or_);
    set_z (z_);
    return;
  }

  void tube::set_half (double ir_, double or_, double hz_)
  {
    set_radii (ir_, or_);
    set_half_z (hz_);
    return;
  }

  // ctor:
  tube::tube ()
  {
    reset ();
    return;
  }

  // ctor:
  tube::tube (double ir_, double or_, double z_)
  {
    set (ir_, or_, z_);
    return;
  }

  // dtor:
  tube::~tube ()
  {
    return;
  }

  double tube::get_surface ( int mask_ ) const
  {
    double s = 0.0;
    if (mask_ & FACE_INNER_SIDE)
      {
        s += 2.0 * M_PI * _inner_r_ * _z_;
      }
    if ( mask_ & FACE_OUTER_SIDE )
      {
        s += 2.0 * M_PI * _outer_r_ * _z_;
      }
    if ( mask_ & FACE_BOTTOM )
      {
        s += M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_);
      }
    if ( mask_ & FACE_TOP )
      {
        s += M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_);
      }
    return s;
  }

  double tube::get_volume () const
  {
    return M_PI * (_outer_r_ * _outer_r_ - _inner_r_ * _inner_r_ ) * _z_;
  }

  bool tube::is_valid () const
  {
    return ((_inner_r_ > 0.0) && (_outer_r_ > _inner_r_) && _z_ > 0.0);
  }

  void tube::reset ()
  {
    _inner_r_ = -1.0;
    _outer_r_ = -1.0;
    _z_       = -1.0;
  }

  std::string tube::get_shape_name () const
  {
    return tube::TUBE_LABEL;
  }

  bool tube::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    double r = hypot (point_.x (), point_.y ());
    if (r > _outer_r_ + 0.5 * skin
        || r < _inner_r_ - 0.5 * skin) return false;
    if (std::abs (point_.z ()) > 0.5 * _z_ + 0.5 * skin ) return false;
    return true;
  }

  double tube::get_parameter ( const std::string & flag_ ) const
  {
    if ( flag_ == "inner.r" ) return get_inner_r ();
    if ( flag_ == "outer.r" ) return get_outer_r ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner.side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer.side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::runtime_error, "Unknown flag !");
  }

  vector_3d tube::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_OUTER_SIDE))
      {
        double phi = position_.phi ();
        normal.set (std::cos (phi), std::sin (phi), 0.0);
      }
    else if (is_on_surface (position_, FACE_INNER_SIDE))
      {
        double phi = position_.phi ();
        normal.set (std::cos (phi), std::sin (phi), 0.0);
        normal *= -1.;
      }
    else if (is_on_surface (position_, FACE_BOTTOM)) normal.set (0.0, 0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP)) normal.set (0.0, 0.0, +1.0);
    return (normal);
  }

  bool tube::is_on_surface (const vector_3d & point_,
                            int mask_,
                            double skin_) const
  {
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double r = hypot (point_.x (), point_.y ());
    if (mask & FACE_BOTTOM)
      {
        if ( (std::abs (point_.z () + 0.5 * _z_) < 0.5 * skin)
             && (r < _outer_r_ && r > _inner_r_)) return true;
      }
    if (mask & FACE_TOP)
      {
        if ( ( std::abs (point_.z () - 0.5 * _z_) < 0.5 * skin)
             && (r < _outer_r_ && r > _inner_r_)) return true;
      }
    if (mask & FACE_OUTER_SIDE)
      {
        if ( (std::abs (point_.z ()) < 0.5 * _z_)
             && (std::abs (r - _outer_r_) < 0.5 * skin)) return true;
      }
    if (mask & FACE_INNER_SIDE)
      {
        if ( (std::abs (point_.z ()) < 0.5 * _z_)
             && (std::abs (r - _inner_r_) < 0.5 * skin)) return true;
      }
    return false;
  }

  bool tube::find_intercept (const vector_3d & from_,
                             const vector_3d & direction_,
                             intercept_t & intercept_,
                             double skin_) const
  {
    intercept_.reset ();
    return intercept_.is_ok ();
  }

  std::ostream & operator<< (std::ostream & out_, const tube & t_)
  {
    out_ << '{' << tube::TUBE_LABEL << ' '
         << t_._inner_r_ << ' '
         << t_._outer_r_ << ' '
         << t_._z_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, tube & t_)
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
    if (name != tube::TUBE_LABEL)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double in_r, out_r, z;
    in_ >> in_r >> out_r >> z;
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
        t_.set (in_r, out_r, z);
      }
    catch (...)
      {
        t_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void tube::compute_inner_cylinder (cylinder & ic_)
  {
    ic_.reset ();
    DT_THROW_IF (! is_valid (),logic_error, "Tube is not valid !");
    ic_.set (get_inner_r (), get_z ());
    return;
  }

  void tube::compute_outer_cylinder (cylinder & oc_)
  {
    oc_.reset ();
    DT_THROW_IF (! is_valid (),logic_error, "Tube is not valid !");
    oc_.set (get_outer_r (), get_z ());
    return;
  }

  void tube::tree_dump (ostream & out_,
                        const string & title_,
                        const string & indent_,
                        bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(internal) : " << get_inner_r () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "R(external) : " << get_outer_r () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;
    return;
  }

  void tube::generate_wires (std::list<polyline_3d> & lpl_,
                             const placement & p_,
                             uint32_t options_) const
  {
    const int nsamples = 36;
    double r[2];
    r[0]=get_inner_r ();
    r[1]=get_outer_r ();
    for (int k = 0; k < 2; k++)
      {
        for (int j = 0; j < 2; j++)
          {
            vector_3d vertex[nsamples];
            for (int i = 0; i < nsamples; i++)
              {
                double thetai = i * 2. * M_PI/nsamples;
                vertex[i].set (r[k] * std::cos (thetai),
                               r[k] * std::sin (thetai),
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
          }

        for (int i = 0; i < nsamples; i++)
          {
            vector_3d vertex[2];
            double thetai = i * 2. * M_PI/nsamples;
            double x = r[k] * std::cos (thetai);
            double y = r[k] * std::sin (thetai);
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

} // end of namespace geomtools

// end of tube.cc
