// -*- mode: c++ ; -*- 
/* box_vg.cc
 */

#include <genvtx/box_vg.h>

#include <stdexcept>
#include <sstream>

#include <datatools/utils/units.h>

namespace genvtx {

  bool box_vg::g_debug = false;

  void box_vg::_assert_lock_ (const string & where_)
  {
    if (_initialized_)
      {
        ostringstream message;
        message << "genvtx::box_vg::_assert_lock_: " << where_ << ": "
                << "Object is locked !";
        throw logic_error (message.str());
      }
    return;
  }

  bool box_vg::is_initialized () const
  {
    return _initialized_;
  }

  int box_vg::get_mode () const
  {
    return _mode_;
  }

  void box_vg::set_mode (int mode_)
  {
    if ((mode_ !=  MODE_BULK) && (mode_ !=  MODE_SURFACE))
      {
        throw logic_error ("genvtx::box_vg::set_mode: Invalid mode !");
      }
    _mode_ = mode_;
    return;
  }

  void box_vg::set_surface_mask (int surface_mask_)
  {
    _assert_lock_ ("genvtx::box_vg::set_surface_mask");
    _surface_mask_ = surface_mask_;
    return;
  }

  void box_vg::set_skin_skip (double skin_skip_)
  {
    _assert_lock_ ("genvtx::box_vg::set_surface_mask");
    _skin_skip_ = skin_skip_;
    return;
  }

  void box_vg::set_skin_thickness (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::box_vg::set_skin_thickness");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  void box_vg::set_bulk (double skin_thickness_)
  {
    _assert_lock_ ("genvtx::box_vg::set_bulk");
    _mode_ = MODE_BULK;
    return;
  }

  void box_vg::set_surface (int surface_mask_)
  {
    _assert_lock_ ("genvtx::box_vg::set_surface");
    _mode_ = MODE_SURFACE;
    set_surface_mask (surface_mask_);
    return;
  }

  bool box_vg::has_box_ref () const
  {
    return _box_ref_ != 0;
  }

  void box_vg::set_box_ref (const geomtools::box & box_)
  {
    _assert_lock_ ("genvtx::box_vg::set_box_ref");
    _box_ref_ = &box_;
    return;
  }

  void box_vg::set_box (const geomtools::box & box_)
  {
    _assert_lock_ ("genvtx::box_vg::set_box");
    _box_ = box_;
    return;
  }

  const geomtools::box & box_vg::get_box_safe () const
  {
    if (has_box_ref ())
      {
        return get_box_ref ();
      }
    return get_box ();
  }

  const geomtools::box & box_vg::get_box_ref () const
  {
    return *_box_ref_;
  }

  const geomtools::box & box_vg::get_box () const
  {
    return _box_;
  }
  
  // ctor:
  box_vg::box_vg ()
  {
    _initialized_ = false;
    _box_ref_ = 0;
    _reset_ ();
    return;
  }
  
  // dtor:
  box_vg::~box_vg ()
  {
    return;
  }

  void box_vg::init ()
  {
    if (_initialized_)
      {
        throw logic_error ("genvtx::box_vg::init: Already initialized !");
      }
    _init_ ();
    _initialized_ = true;
    return;
  }

  void box_vg::reset ()
  {
    if (! _initialized_)
      {
        throw logic_error ("genvtx::box_vg::reset: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  void box_vg::_init_ ()
  {
    bool devel = g_debug;
    if (_mode_ == MODE_SURFACE) 
      {
        if (_surface_mask_ == 0)
          {
            throw logic_error ("genvtx::box_vg::_init_: Surface mask is null !");
          }
        double s = _box_.get_surface (_surface_mask_);
        if (devel) clog << "DEVEL: genvtx::box_vg::_init_: Total surface = " << s << endl;
        _sum_weight_[0] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_BACK);
        _sum_weight_[1] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_FRONT);
        _sum_weight_[2] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_LEFT);
        _sum_weight_[3] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_RIGHT);
        _sum_weight_[4] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_BOTTOM);
        _sum_weight_[5] = _box_.get_surface (_surface_mask_ & geomtools::box::FACE_TOP);
        for (int i = 0; i < 6; i++) 
          {
            _sum_weight_[i] /= s;
            if (i > 0) 
              {
                _sum_weight_[i] += _sum_weight_[i - 1];
              }
            if (devel) clog << "DEVEL: genvtx::box_vg::_init_: Surface weight [" << i << "] = " << _sum_weight_[i] << endl;
          }
      }
    return;
  }

  void box_vg::_reset_ ()
  {
    _mode_ = MODE_DEFAULT;
    _surface_mask_ = geomtools::box::FACE_ALL;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    for (int i = 0; i < 6; i++)
      {
        _sum_weight_[i] = 0.0;
      }
    return;
  }

  void box_vg::dump (ostream & out_) const
  {
    out_ << "genvtx::box_vg::dump: " << endl;
    if (has_box_ref ())
      {
        out_ << "|-- " << "Box ref: " << *_box_ref_ << " [" << _box_ref_ << ']' << endl;
      }
    else
      {
        out_ << "|-- " << "Box: " << _box_ << endl;
      }
    out_ << "|-- " << "Mode: " << _mode_ << endl;
    out_ << "|-- " << "Surface mask: " << _surface_mask_ << endl;
    out_ << "|-- " << "Skin skip: " << _skin_skip_ << endl;
    out_ << "`-- " << "Skin thickness: " << _skin_thickness_ << endl;
    return;
  }
  
  void box_vg::_shoot_vertex (mygsl::rng & random_, 
                              geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (! _initialized_)
      {
        init ();
      }
    geomtools::invalidate (vertex_);
    double x, y, z;
    const geomtools::box * the_box = &_box_;
    if (has_box_ref ())
      {
        the_box = _box_ref_;
      }

    if (_mode_ == MODE_BULK) 
      {
        if (devel) 
          {
            clog << "DEVEL: genvtx::box_vg::_shoot_vertex: "
                 << "Bulk mode..." << endl;
          }
        x = (random_.uniform () - 0.5) * (the_box->get_x () - _skin_thickness_);
        y = (random_.uniform () - 0.5) * (the_box->get_y () - _skin_thickness_);
        z = (random_.uniform () - 0.5) * (the_box->get_z () - _skin_thickness_);
      }

    if (_mode_ == MODE_SURFACE) 
      {
        if (devel) 
          {
            clog << "DEVEL: genvtx::box_vg::_shoot_vertex: "
                 << "Surface mode..." << endl;
          }
        double r0 = random_.uniform ();
        double r1 = random_.uniform ();
        double r2 = random_.uniform ();
        double r3 = 0.0;
        double delta_thick = 0.0;
        if (_skin_thickness_ > 0.0) 
          {
            r3 = random_.uniform ();
            delta_thick = (r3 - 0.5) * _skin_thickness_;
          }
        
        if (r0 < _sum_weight_[0]) 
          {
            y = (r1 - 0.5) * the_box->get_y ();
            z = (r2 - 0.5) * the_box->get_z ();
            x = -(the_box->get_half_x () + _skin_skip_ + delta_thick);
          }
        else if (r0 < _sum_weight_[1]) 
          {
            y = (r1 - 0.5) * the_box->get_y ();
            z = (r2 - 0.5) * the_box->get_z ();
            x = +(the_box->get_half_x () + _skin_skip_ + delta_thick);
          }
        else if (r0 < _sum_weight_[2]) 
          {
            x = (r1 - 0.5) * the_box->get_x ();
            z = (r2 - 0.5) * the_box->get_z ();
            y = -(the_box->get_half_y () + _skin_skip_ + delta_thick);
          }
        else if (r0 < _sum_weight_[3]) 
          {
            x = (r1 - 0.5) * the_box->get_x ();
            z = (r2 - 0.5) * the_box->get_z ();
            y = +(the_box->get_half_y () + _skin_skip_ + delta_thick);
          }     
        else if (r0 < _sum_weight_[4]) 
          {
            x = (r1 - 0.5) * the_box->get_x ();
            y = (r2 - 0.5) * the_box->get_y ();
            z = -(the_box->get_half_z () + _skin_skip_ + delta_thick);
          }
        else 
          {
            x = (r1 - 0.5) * the_box->get_x ();
            y = (r2 - 0.5) * the_box->get_y ();
            z = +(the_box->get_half_z () + _skin_skip_ + delta_thick);
          }     
      }
    vertex_.set (x,y, z);
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  box_vg::create (const properties & configuration_, void * user_)
  {
    //cerr << "DEVEL: genvtx::box_vg::create: Entering..." << endl;
    //configuration_.tree_dump (cerr, "genvtx::box_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the box vertex generator:
    double box_x, box_y, box_z;
    box_x = box_y = box_z = 1.0;
    int mode = box_vg::MODE_DEFAULT;
    double skin_skip = 0.0;
    double skin_thickness = 0.0;
    int surface_mask = geomtools::box::FACE_NONE;
    double lunit = CLHEP::mm;
    string lunit_str;

    string box_user_ref = vg_tools::SHAPE_REF_NONE;
    string box_user_ref_name;

    if (configuration_.has_key ("mode"))
      {
        string mode_str = configuration_.fetch_string ("mode");
        if (mode_str == "surface")
          {
            mode = MODE_SURFACE;
          }
        else if (mode_str == "bulk")
          {
            mode = MODE_BULK;
          }
        else
          {
            ostringstream message;
            message << "genvtx::box_vg::create: Invalid mode '"
                    << mode_str << "' !";
            throw logic_error (message.str ());
          }
      }

    if (configuration_.has_key ("box_ref"))
      {
        box_user_ref = configuration_.fetch_string ("box_ref");
      }

    if (box_user_ref == vg_tools::SHAPE_REF_NONE)
      {
        if (configuration_.has_key ("box.x"))
          {
            box_x = configuration_.fetch_real ("box.x");
          }
        
        if (configuration_.has_key ("box.y"))
          {
            box_y = configuration_.fetch_real ("box.y");
          }
        
        if (configuration_.has_key ("box.z"))
          {
            box_z = configuration_.fetch_real ("box.z");
          }     
      }

    if (box_user_ref == vg_tools::SHAPE_REF_GETTER)
      {
        if (configuration_.has_key ("box_ref_name"))
          {
            box_user_ref_name = configuration_.fetch_string ("box_ref_name");
          }
      }

    if (configuration_.has_key ("length_unit"))
      {
        lunit_str = configuration_.fetch_string ("length_unit");
        lunit = datatools::utils::units::get_length_unit_from (lunit_str);
      }

    if (configuration_.has_key ("skin_skip"))
      {
        skin_skip = configuration_.fetch_real ("skin_skip");
      }

    if (configuration_.has_key ("skin_thickness"))
      {
        skin_thickness = configuration_.fetch_real ("skin_thickness");
      }

    vector<string> surfaces;
    if (mode == MODE_SURFACE)
      {
        
        if (configuration_.has_key ("surfaces"))
          {
            configuration_.fetch ("surfaces", surfaces);
          }

        for (int i = 0; i < surfaces.size (); i++)
          {
            if (surfaces[i] == "all")
              {
                surface_mask = geomtools::box::FACE_ALL;
              }
            else if (surfaces[i] == "back")
              {
                surface_mask |= geomtools::box::FACE_BACK;
              }
            else if (surfaces[i] == "front")
              {
                surface_mask |= geomtools::box::FACE_FRONT;
              }
            else if (surfaces[i] == "left")
              {
                surface_mask |= geomtools::box::FACE_LEFT;
              }
            else if (surfaces[i] == "right")
              {
                surface_mask |= geomtools::box::FACE_RIGHT;
              }
            else if (surfaces[i] == "bottom")
              {
                surface_mask |= geomtools::box::FACE_BOTTOM;
              }
            else if (surfaces[i] == "top")
              {
                surface_mask |= geomtools::box::FACE_TOP;
              }
          }
        
      }

    // create a new parameterized 'box_vg' instance:
    box_vg * ptr = new box_vg;
    ptr->set_mode (mode);
    ptr->set_skin_skip (skin_skip);
    ptr->set_skin_thickness (skin_thickness);
    ptr->set_surface_mask (surface_mask);

    // use an embedded box:
    if (box_user_ref == vg_tools::SHAPE_REF_NONE)
      {
        box_x *= lunit;
        box_y *= lunit;
        box_z *= lunit;
        geomtools::box box (box_x, box_y, box_z);
        ptr->set_box (box);
      }
    else
      // use an referenced external box extracted from:
      {
        if (user_ == 0)
          {
            throw logic_error ("genvtx::box_vg::create: No user data is provided !");
          }

        const geomtools::box * ext_box_ref = 0;
            
        if (box_user_ref == vg_tools::SHAPE_REF_PLAIN)
          {
            try
              {
                ext_box_ref = static_cast<const geomtools::box *> (user_);
              }
            catch (...)
              {
                throw logic_error ("genvtx::box_vg::create: Cannot reference a 'geomtools::box' from 'user' pointer !");
              }
          }
        else if (box_user_ref == vg_tools::SHAPE_REF_GETTER)
          {
            geomtools::i_object_3d::i_getter * getter = 0;
            try
              {
                getter = static_cast<geomtools::i_object_3d::i_getter *> (user_);
              }
            catch (...)
              {
                throw logic_error ("genvtx::box_vg::create: Cannot reference a 'i_object_3d::i_getter' from 'user' pointer !");
              }

            const geomtools::i_object_3d * o3d = 0;
            try
              {
                o3d = getter->get (box_user_ref_name, configuration_);
              }
            catch (...)
              {
                throw logic_error ("genvtx::box_vg::create: Cannot reference a 'i_object_3d' object from the 'getter' instance !");
              }

            if (o3d->get_shape_name () != "box")
              {
                throw logic_error ("genvtx::box_vg::create: Reference 3D object is not a 'box' shape !");
              }

            try
              {
                ext_box_ref = dynamic_cast <const geomtools::box *> (o3d);
              }
            catch (...)
              {
                throw logic_error ("genvtx::box_vg::create: Cannot reference a 'geomtools::box' from the generic 3D shape !");
              }
          }
        else 
          {
            throw logic_error ("genvtx::box_vg::create: Unknown mode for external shape referencing !");            
          }

        if (ext_box_ref == 0)
          {
            throw logic_error ("genvtx::box_vg::create: Cannot reference a box from 'user' pointer !");
          }
        ptr->set_box_ref (*ext_box_ref);
      }

    return ptr; 
  }
        

  /*
    const geomtools::i_model * model = 0;
    if (box_user_ref == vg_tools::SHAPE_REF_MODEL)
    {
    try
    {
    model = static_cast<const geomtools::i_model *> (user_);
    }
    catch (...)
    {
    throw logic_error ("box_vg::create: Cannot reference a 'geomtools::i_model' from 'user' pointer !");
    }
    }
    else
    {
    const geomtools::models_col_t * models = 0;
    try
    {
    models = static_cast<const geomtools::models_col_t *> (user_);
    }
    catch (...)
    {
    throw logic_error ("box_vg::create: Cannot reference a 'geomtools::models_col_t' dictionary from 'user' pointer !");
    }
    geomtools::models_col_t::const_iterator found 
    = models.find (box_user_ref);
    if (found == models.end ())
    {
    ostringstream message:
    message << "box_vg::create: Cannot find a model with name '"
    << user_ref << "' in the dictionary of models !";
    throw logic_error (message.str ());
    }
    model = (found->second)
                

    }
    if (! model->is_constructed ())
    {
    throw logic_error ("box_vg::create: Extracted model is not constructed !");
    }
    if (! model->get_logical ().has_shape ())
    {
    throw logic_error ("box_vg::create: Logical from extracted model has no shape !");
    } 
    const geomtools::i_shape3d & shape = model->get_logical ().get_shape ();
    if (shape.get_shape_name () != "box")
    {
    throw logic_error ("box_vg::create: Extracted 3D shape is not a box !");            
    }
    ext_box_ref = static_cast<const geomtools::box *> (&shape);
    }
    else
    {
            
    /*
    {
    const geomtools::models_col_t * models = 0;
              
              
    }
  */

  string box_vg::vg_id () const
  {
    return "genvtx::box_vg";
  }

  vg_creator_type box_vg::vg_creator () const
  {
    return box_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<box_vg> box_vg::g_cr_;
  
} // end of namespace genvtx

// end of box_vg.cc
