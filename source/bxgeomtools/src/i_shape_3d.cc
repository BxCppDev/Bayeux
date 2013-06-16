// -*- mode: c++; -*-
/* i_shape_3d.cc
 */

#include <geomtools/i_shape_3d.h>
#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  const double i_shape_3d::DEFAULT_SKIN      = GEOMTOOLS_DEFAULT_TOLERANCE;
  const double i_shape_3d::USING_PROPER_SKIN = GEOMTOOLS_PROPER_TOLERANCE;

  // static
  double i_shape_3d::get_default_skin ()
  {
    return GEOMTOOLS_DEFAULT_TOLERANCE;
  }

  // static
  double i_shape_3d::get_proper_skin ()
  {
    return GEOMTOOLS_PROPER_TOLERANCE;
  }

  bool i_shape_3d::is_composite () const
  {
    return false;
  }

  bool i_shape_3d::is_stackable (const i_shape_3d & a_shape)
  {
    if (a_shape.has_stackable_data ())
      {
        if (a_shape.get_stackable_data ().is_valid ())
          {
            return true;
          }
      }
    const i_stackable * the_stackable
      = dynamic_cast<const i_stackable *> (&a_shape);
    if (the_stackable != 0)
      {
        return true;
      }
    return false;
  }

  bool i_shape_3d::is_xmin_stackable (const i_shape_3d & a_shape)
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

  bool i_shape_3d::is_ymin_stackable (const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_ymin (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_ymax_stackable (const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_ymax (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_zmin_stackable (const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_zmin (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::is_zmax_stackable (const i_shape_3d & a_shape)
  {
    if (is_stackable (a_shape)) return true;
    if (stackable::has_zmax (a_shape.get_properties ())) return true;
    return false;
  }

  bool i_shape_3d::pickup_stackable_with_properties (const i_shape_3d & a_shape,
                                                     stackable_data & a_stackable_data)
  {
    bool ok = false;
    ok = i_shape_3d::pickup_stackable (a_shape, a_stackable_data);
    if (ok)
      {
        if (stackable::has_xmin (a_shape.get_properties ()))
          {
            a_stackable_data.xmin = stackable::get_xmin (a_shape.get_properties (), -1.0);
          }
        if (stackable::has_xmax (a_shape.get_properties ()))
          {
            a_stackable_data.xmax = stackable::get_xmax (a_shape.get_properties (), -1.0);
          }
        if (stackable::has_ymin (a_shape.get_properties ()))
          {
            a_stackable_data.ymin = stackable::get_ymin (a_shape.get_properties (), -1.0);
          }
        if (stackable::has_ymax (a_shape.get_properties ()))
          {
            a_stackable_data.ymax = stackable::get_ymax (a_shape.get_properties (), -1.0);
          }
        if (stackable::has_zmin (a_shape.get_properties ()))
          {
            a_stackable_data.zmin = stackable::get_zmin (a_shape.get_properties (), -1.0);
          }
        if (stackable::has_zmax (a_shape.get_properties ()))
          {
            a_stackable_data.zmax = stackable::get_zmax (a_shape.get_properties (), -1.0);
          }
      }
    return ok;
  }

  bool i_shape_3d::pickup_stackable (const i_shape_3d & a_shape,
                                     stackable_data & a_stackable_data)
  {
    a_stackable_data.invalidate ();
    bool ok = false;
    if (a_shape.has_stackable_data ())
      {
        if (a_shape.get_stackable_data ().is_valid ())
          {
            a_stackable_data = a_shape.get_stackable_data ();
            ok =  true;
          }
      }
    if (! ok)
      {
        const i_stackable * the_stackable
          = dynamic_cast<const i_stackable *> (&a_shape);
        if (the_stackable != 0)
          {
            a_stackable_data.xmin = the_stackable->get_xmin ();
            a_stackable_data.xmax = the_stackable->get_xmax ();
            a_stackable_data.ymin = the_stackable->get_ymin ();
            a_stackable_data.ymax = the_stackable->get_ymax ();
            a_stackable_data.zmin = the_stackable->get_zmin ();
            a_stackable_data.zmax = the_stackable->get_zmax ();
            ok = true;
          }
      }
    return ok;
  }

  double i_shape_3d::get_skin () const
  {
    return get_tolerance ();
  }

  void i_shape_3d::set_skin (double a_skin)
  {
    set_tolerance (a_skin);
  }

  int i_shape_3d::get_dimensional () const
  {
    return DIMENSIONAL_3;
  }

  bool i_shape_3d::has_stackable_data () const
  {
    return _stackable_data_ != 0;
  }

  const stackable_data & i_shape_3d::get_stackable_data () const
  {
    return *_stackable_data_;
  }

  void i_shape_3d::set_stackable_data (const stackable_data & a_stackable_data)
  {
    if (_stackable_data_ != 0)
      {
        if (&a_stackable_data == _stackable_data_) return;
        if (_owns_stackable_data_)
          {
            delete _stackable_data_;
            _stackable_data_ = 0;
          }
      }
    _owns_stackable_data_ = false;
    _stackable_data_ = &a_stackable_data;
    return;
  }

  void i_shape_3d::set_stackable_data (const stackable_data * a_stackable_data)
  {
    if (_stackable_data_ != 0)
      {
        if (a_stackable_data == _stackable_data_) return;
        if (_owns_stackable_data_)
          {
            delete _stackable_data_;
            _stackable_data_ = 0;
          }
      }
    _owns_stackable_data_ = true;
    _stackable_data_ = a_stackable_data;
    return;
  }

  // ctor:
  i_shape_3d::i_shape_3d () : i_object_3d ()
  {
    _owns_stackable_data_ = false;
    _stackable_data_ = 0;
    return;
  }

  // ctor:
  i_shape_3d::i_shape_3d (double a_skin) : i_object_3d (a_skin)
  {
    _owns_stackable_data_ = false;
    _stackable_data_ = 0;
    /*
      clog << "DEVEL: i_shape_3d::i_shape_3d: i_object_3d::DEFAULT_TOLERANCE = "
      << i_object_3d::DEFAULT_TOLERANCE << endl;
      clog << "DEVEL: i_shape_3d::i_shape_3d: i_shape_3d::DEFAULT_SKIN = "
      << i_shape_3d::DEFAULT_SKIN << endl;
      clog << "DEVEL: i_shape_3d::i_shape_3d: a_skin = " << a_skin << endl;
    */
    return;
  }

  // dtor:
  i_shape_3d::~i_shape_3d ()
  {
    if (_stackable_data_ == 0)
      {
        if (_owns_stackable_data_)
          {
            delete _stackable_data_;
            _stackable_data_ = 0;
          }
      }
    return;
  }

  bool i_shape_3d::is_outside (const vector_3d & a_position,
                               double a_skin) const
  {
    return ! is_inside (a_position, a_skin)
      && ! is_on_surface (a_position, ALL_SURFACES, a_skin);
  }

  void i_shape_3d::tree_dump (ostream & a_out,
                              const string & a_title,
                              const string & a_indent,
                              bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ()) indent = a_indent;
    i_object_3d::tree_dump (a_out, a_title, a_indent, true);
    if (i_shape_3d::is_stackable (*this))
      {
        stackable_data SD;
        i_shape_3d::pickup_stackable (*this, SD);
        a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
              << "Stackable data : ";
        if (_stackable_data_ != 0)
          {
            a_out << "[plugged]";
          }
        else
          {
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

  bool i_shape_3d::find_intercept (const vector_3d & a_from,
                                   const vector_3d & a_direction,
                                   intercept_t & a_intercept,
                                   double a_skin) const
  {
    // temporary:
    // NOT IMPLEMENTED !
    // default: no intercept on any face of the 3D shape...
    a_intercept.reset ();
    return a_intercept.is_ok ();
  }

} // end of namespace geomtools

// end of i_shape_3d.cc
