// -*- mode: c++ ; -*-
/* cylindric_extrusion_boxed_model.cc
 */

#include <geomtools/cylindric_extrusion_boxed_model.h>

#include <mygsl/min_max.h>

#include <datatools/units.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>


namespace geomtools {

  using namespace std;

  const geomtools::i_shape_3d & cylindric_extrusion_boxed_model::get_solid () const
  {
    return *_solid_;
  }

  void cylindric_extrusion_boxed_model::set_extrusion_radius (double r_)
  {
    assert_unconstructed("geomtools::cylindric_extrusion_boxed_model::set_extrusion_radius");

    if (r_ <= 0.0 * CLHEP::mm)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::set_extrusion_radius: "
                << "Invalid R " << r_ / CLHEP::mm << " mm !";
        throw logic_error (message.str ());
      }
    _extrusion_radius_ = r_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_x (double x_)
  {
    assert_unconstructed("geomtools::cylindric_extrusion_boxed_model::set_mother_x");

    if (x_ <= 0.0 * CLHEP::mm)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::set_mother_x: "
                << "Invalid X " << x_ / CLHEP::mm << " mm !";
        throw logic_error (message.str ());
      }
    _mother_x_ = x_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_y (double y_)
  {
    assert_unconstructed("geomtools::cylindric_extrusion_boxed_model::set_mother_y");

    if (y_ <= 0.0 * CLHEP::mm)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::set_mother_y: "
                << "Invalid Y " << y_ / CLHEP::mm << " mm !";
        throw logic_error (message.str ());
      }
    _mother_y_ = y_;
    return;
  }

  void cylindric_extrusion_boxed_model::set_mother_z (double z_)
  {
    assert_unconstructed("geomtools::cylindric_extrusion_boxed_model::set_mother_z");

    if (z_ <= 0.0 * CLHEP::mm)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::set_mother_z: "
                << "Invalid Z " << z_ / CLHEP::mm << " mm !";
        throw logic_error (message.str ());
      }
    _mother_z_ = z_;
    return;
  }

  double cylindric_extrusion_boxed_model::get_extrusion_radius () const
  {
    return _extrusion_radius_;
  }

  double cylindric_extrusion_boxed_model::get_mother_x () const
  {
    return _mother_x_;
  }

  double cylindric_extrusion_boxed_model::get_mother_y () const
  {
    return _mother_y_;
  }

  double cylindric_extrusion_boxed_model::get_mother_z () const
  {
    return _mother_z_;
  }

  void cylindric_extrusion_boxed_model::set_material_name (const string & mn_)
  {
    assert_unconstructed("geomtools::cylindric_extrusion_boxed_model::set_material_name");

    _material_name_ = mn_;
  }

  const string & cylindric_extrusion_boxed_model::get_material_name () const
  {
    return _material_name_;
  }

  string cylindric_extrusion_boxed_model::get_model_id () const
  {
    return "geomtools::cylindric_extrusion_boxed_model";
  }

  cylindric_extrusion_boxed_model::cylindric_extrusion_boxed_model ()
    : i_model ("geomtools::cylindric_extrusion_boxed_model")
  {
    _material_name_ = "";
    _mother_x_ = numeric_limits<double>::quiet_NaN ();
    _mother_y_ = numeric_limits<double>::quiet_NaN ();
    _mother_z_ = numeric_limits<double>::quiet_NaN ();
    _extrusion_radius_ = numeric_limits<double>::quiet_NaN ();
    return;
  }

  cylindric_extrusion_boxed_model::~cylindric_extrusion_boxed_model ()
  {
    return;
  }

  void cylindric_extrusion_boxed_model::_at_construct (const string & name_,
                                                       const datatools::properties & config_,
                                                       models_col_type * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: cylindric_extrusion_boxed_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double mother_x;
    double mother_y;
    double mother_z;
    double extrusion_radius;
    string material_name = material::constants::instance ().MATERIAL_REF_UNKNOWN;
    double lunit = CLHEP::mm;
    string lunit_str = "mm"; // default unit

    if (config_.has_key ("length_unit"))
      {
        lunit_str = config_.fetch_string ("length_unit");
      }
    lunit = datatools::units::get_length_unit_from (lunit_str);

    if (config_.has_key ("x"))
      {
        mother_x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) mother_x *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "Missing 'x' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("y"))
      {
        mother_y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) mother_y *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "Missing 'y' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        mother_z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) mother_z *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "Missing 'z' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("extrusion_radius"))
      {
        extrusion_radius = config_.fetch_real ("extrusion_radius");
        if (! config_.has_explicit_unit ("extrusion_radius")) {
          extrusion_radius *= lunit;
        }
      }
    else
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "Missing 'extrusion_radius' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("material.ref"))
      {
        material_name = config_.fetch_string ("material.ref");
      }
    else
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "Missing 'material.ref' property !";
        throw logic_error (message.str ());
      }

    if (extrusion_radius >= 0.5 * mother_x)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "extrusion radius ("
                << extrusion_radius / CLHEP::mm << " mm) is too large (X-axis)!";
        throw logic_error (message.str ());
      }
    if (extrusion_radius >= 0.5 * mother_y)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::_at_construct: "
                << "extrusion radius ("
                << extrusion_radius / CLHEP::mm << " mm) is too large (Y-axis) !";
        throw logic_error (message.str ());
      }

    set_material_name (material_name);
    set_mother_x (mother_x);
    set_mother_y (mother_y);
    set_mother_z (mother_z);
    set_extrusion_radius (extrusion_radius);

    _mother_box_.reset ();
    _mother_box_.set_x (get_mother_x ());
    _mother_box_.set_y (get_mother_y ());
    _mother_box_.set_z (get_mother_z ());
    if (! _mother_box_.is_valid ())
      {
        throw logic_error ("geomtools::cylindric_extrusion_boxed_model::_at_construct: Invalid box dimensions !");
      }

    _extrusion_cylinder_.set_diameter (2 * get_extrusion_radius ());
    double eps = 1.0e-5 * CLHEP::mm;
    _extrusion_cylinder_.set_z (get_mother_z () + eps);

    placement p1 (vector_3d (0, 0, 0), 0, 0, 0);
    placement p2 (vector_3d (0, 0, 0), 0, 0, 0);
    _extruded_solid_.set_shape1 (_mother_box_, p1);
    _extruded_solid_.set_shape2 (_extrusion_cylinder_, p2);
    if (devel)
      {
        _extruded_solid_.dump (std::clog);
      }

    _extruded_log_.set_name (i_model::make_logical_volume_name ("extruded_box"));
    _extruded_log_.set_shape (_extruded_solid_);
    _extruded_log_.set_material_ref (_material_name_);
    _extruded_placement_.set (vector_3d (0, 0, 0), 0, 0, 0);

    _extruded_phys_.set_name (i_model::make_physical_volume_name ("extruded_box"));
    _extruded_phys_.set_placement (_extruded_placement_);
    _extruded_phys_.set_logical (_extruded_log_);
    _extruded_phys_.set_mother (get_logical ());
    // Install default 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5 * _mother_x_;
      sd_ptr->xmax = +0.5 * _mother_x_;
      sd_ptr->ymin = -0.5 * _mother_y_;
      sd_ptr->ymax = +0.5 * _mother_y_;
      sd_ptr->zmin = -0.5 * _mother_z_;
      sd_ptr->zmax = +0.5 * _mother_z_;
      _extruded_solid_.set_stackable_data (sd_ptr);
      if (devel)
        {
          clog << "DEVEL: cylindric_extrusion_boxed_model::_at_construct: Entering..." << endl;
          sd_ptr->tree_dump (cerr,
                             "geomtools::cylindric_extrusion_boxed_model::_at_construct: Stackable data: ",
                             "DEVEL: ");
        }
    }
    _extruded_solid_.set_user_draw ((void *) &cylindric_extrusion_boxed_model::gnuplot_draw_user_function);
    _solid_ = &_extruded_solid_;

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_extruded_solid_);
    get_logical ().set_material_ref (material_name);

    if (devel) clog << "DEVEL: cylindric_extrusion_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void cylindric_extrusion_boxed_model::tree_dump (ostream & out_,
                                                   const string & title_ ,
                                                   const string & indent_,
                                                   bool inherit_) const
  {
    bool devel = false;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << i_tree_dumpable::tag
         << "Material name : '" << get_material_name () << "'" << endl;

    out_ << indent << i_tree_dumpable::tag
         << "Mother X : " << get_mother_x () / CLHEP::mm << " mm" << endl;

    out_ << indent << i_tree_dumpable::tag
         << "Mother Y : " << get_mother_y () / CLHEP::mm << " mm" << endl;

    out_ << indent << i_tree_dumpable::tag
         << "Mother Z : " << get_mother_z () / CLHEP::mm << " mm" << endl;

    out_ << indent << i_tree_dumpable::tag
         << "Extrusion radius : " << get_extrusion_radius () / CLHEP::mm << " mm" << endl;


    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _extruded_solid_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    return;
  }

  void cylindric_extrusion_boxed_model::gnuplot_draw_user_function (std::ostream & out_,
                                                                    const geomtools::vector_3d & position_,
                                                                    const geomtools::rotation_3d & rotation_,
                                                                    const geomtools::i_object_3d & obj_,
                                                                    void *)
  {
    const subtraction_3d * solid = dynamic_cast<const subtraction_3d *>(&obj_);
    if (solid == 0)
      {
        ostringstream message;
        message << "geomtools::cylindric_extrusion_boxed_model::gnuplot_draw_user_function: "
                << "3D-object of '" << obj_.get_shape_name () << "' shape type has not the right type !";
        throw logic_error (message.str ());
      }
    const i_composite_shape_3d::shape_type & s1 = solid->get_shape1 ();
    const i_composite_shape_3d::shape_type & s2 = solid->get_shape2 ();
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();

    // extract useful stuff (shapes and properties):
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);
    const geomtools::cylinder & extrusion_cylinder = dynamic_cast<const geomtools::cylinder &> (sh2);

    // draw first shape:
    {
      placement mother_world_placement;
      mother_world_placement.set_translation (position_);
      mother_world_placement.set_orientation (rotation_);

      placement world_item_placement;
      mother_world_placement.child_to_mother (s1.get_placement (),
                                              world_item_placement);
      const vector_3d   & sh1_pos = world_item_placement.get_translation ();
      const rotation_3d & sh1_rot = world_item_placement.get_rotation ();
      gnuplot_draw::draw_box (out_,
                              sh1_pos,
                              sh1_rot,
                              mother_box);
    }

    // draw second:
    {
      cylinder cyl (extrusion_cylinder.get_radius (), mother_box.get_z ());
      placement mother_world_placement;
      mother_world_placement.set_translation (position_);
      mother_world_placement.set_orientation (rotation_);

      placement world_item_placement;
      mother_world_placement.child_to_mother (s1.get_placement (),
                                              world_item_placement);
      const vector_3d   & sh1_pos = world_item_placement.get_translation ();
      const rotation_3d & sh1_rot = world_item_placement.get_rotation ();
      gnuplot_draw::draw_cylinder (out_,
                                   sh1_pos,
                                   sh1_rot,
                                   cyl);
    }
    return;
  }

  // registration :   
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(cylindric_extrusion_boxed_model,"geomtools::cylindric_extrusion_boxed_model");

} // end of namespace geomtools

// end of cylindric_extrusion_boxed_model.cc
