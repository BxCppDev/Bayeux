// -*- mode: c++ ; -*-
/* plate_with_hole_model.cc
 */

#include <geomtools/plate_with_hole_model.h>

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

namespace geomtools {

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(plate_with_hole_model,"geomtools::plate_with_hole_model");

  std::string plate_with_hole_model::get_model_id () const
  {
    return "geomtools::plate_with_hole_model";
  }

  const std::string & plate_with_hole_model::get_material () const
  {
    return _material_;
  }

  const geomtools::subtraction_3d & plate_with_hole_model::get_solid () const
  {
    return _solid_;
  }

  // ctor:
  plate_with_hole_model::plate_with_hole_model () : i_model ("plate_with_hole_model")
  {
    _material_ = "";
    _box_hole_.reset ();
    _cyl_hole_.reset ();
    datatools::invalidate (_x_);
    datatools::invalidate (_y_);
    datatools::invalidate (_z_);
    datatools::invalidate (_r_hole_);
    datatools::invalidate (_x_hole_);
    datatools::invalidate (_y_hole_);
    datatools::invalidate (_x_pos_hole_);
    datatools::invalidate (_y_pos_hole_);
    return;
  }

  // dtor:
  plate_with_hole_model::~plate_with_hole_model ()
  {
    return;
  }

  void plate_with_hole_model::_at_construct (const std::string & name_,
                                             const datatools::properties & config_,
                                             geomtools::models_col_type * models_)
  {
    //set_name (name_);

    /*** parse properties ***/

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit"))
      {
        const std::string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    DT_THROW_IF (!config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in plate with hole model '" << name_ << "' !");
    _material_ = config_.fetch_string ("material.ref");

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property in plate with hole model '" << name_ << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) {
      x *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property in plate with hole model '" << name_ << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) {
      y *= lunit;
    }

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property in plate with hole model '" << name_ << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) {
      z *= lunit;
    }

    double r_hole;
    datatools::invalidate (r_hole);
    if (config_.has_key ("r_hole"))
      {
        r_hole = config_.fetch_real ("r_hole");
        if (! config_.has_explicit_unit ("r_hole")) {
          r_hole *= lunit;
        }
      }

    double x_hole;
    double y_hole;
    datatools::invalidate (x_hole);
    datatools::invalidate (y_hole);
    if (! datatools::is_valid (r_hole)) {
      DT_THROW_IF (! config_.has_key ("x_hole"), std::logic_error, "Missing 'x_hole' property in plate with hole model '" << name_ << "' !");
      x_hole = config_.fetch_real ("x_hole");
      if (! config_.has_explicit_unit ("x_hole")) {
        x_hole *= lunit;
      }
      DT_THROW_IF (! config_.has_key ("y_hole"), std::logic_error, "Missing 'y_hole' property in plate with hole model '" << name_ << "' !");
      y_hole = config_.fetch_real ("y_hole");
      if (! config_.has_explicit_unit ("y_hole")) {
        y_hole *= lunit;
      }
    }

    double x_pos_hole = 0.0;
    if (config_.has_key ("x_pos_hole")) {
      x_pos_hole = config_.fetch_real ("x_pos_hole");
      if (! config_.has_explicit_unit ("x_pos_hole")) {
        x_pos_hole *= lunit;
      }
    }

    double y_pos_hole = 0.0;
    if (config_.has_key ("y_pos_hole")) {
      y_pos_hole = config_.fetch_real ("y_pos_hole");
      if (! config_.has_explicit_unit ("y_pos_hole")) {
        y_pos_hole *= lunit;
      }
    }

    _x_ = x;
    _y_ = y;
    _z_ = z;
    _r_hole_ = r_hole;
    _x_hole_ = x_hole;
    _y_hole_ = y_hole;
    _z_hole_ = _z_ + CLHEP::micrometer;
    _x_pos_hole_ = x_pos_hole;
    _y_pos_hole_ = y_pos_hole;

    std::vector<double> xy_pos_hole;
    xy_pos_hole.push_back (_x_pos_hole_);
    xy_pos_hole.push_back (_y_pos_hole_);
    if (_x_pos_hole_ * _y_pos_hole_ != 0) {
      _solid_.properties ().store ("xy_pos_hole", xy_pos_hole);
    }
    // Checks :
    double hole_x_min = x_pos_hole;
    double hole_x_max = x_pos_hole;
    double hole_y_min = y_pos_hole;
    double hole_y_max = y_pos_hole;
    if (datatools::is_valid (_r_hole_)) {
      hole_x_min -= _r_hole_;
      hole_x_max += _r_hole_;
      hole_y_min -= _r_hole_;
      hole_y_max += _r_hole_;
    } else {
      hole_x_min -= 0.5 * _x_hole_;
      hole_x_max += 0.5 * _x_hole_;
      hole_y_min -= 0.5 * _y_hole_;
      hole_y_max += 0.5 * _y_hole_;
    }

    DT_THROW_IF ((hole_x_min < -0.5 * _x_) || (hole_x_max > 0.5 * _x_),
                 std::logic_error,
                 "Hole size is too large for mother box dimension in plate with hole model '" << name_ << "' !");

    DT_THROW_IF ((hole_y_min < -0.5 * _y_) || (hole_y_max > 0.5 * _y_),
                 std::logic_error,
                 "Hole size is too large for mother box dimension in plate with hole model '" << name_ << "' !");

    _mother_.set_x (_x_);
    _mother_.set_y (_y_);
    _mother_.set_z (_z_);
    DT_THROW_IF (! _mother_.is_valid (),
                 std::logic_error,
                 "Invalid dimension(s) for the mother box in plate with hole model '" << name_ << "' !");

    geomtools::placement hole_placement (_x_pos_hole_, _y_pos_hole_, 0.0, 0, 0, 0);
    if (datatools::is_valid (_r_hole_)) {
      _cyl_hole_.set_r (_r_hole_);
      _cyl_hole_.set_z (_z_hole_);
      _solid_.set_shapes (_mother_,
                          _cyl_hole_,
                          hole_placement);
    } else if (datatools::is_valid (_x_hole_) && datatools::is_valid (_y_hole_))
      {
        _box_hole_.set_x (_x_hole_);
        _box_hole_.set_y (_y_hole_);
        _box_hole_.set_z (_z_hole_);
        _solid_.set_shapes (_mother_,
                            _box_hole_,
                            hole_placement);
      } else {
      DT_THROW_IF (true, std::logic_error, "No defined shape for hole in plate with hole model '" << name_ << "' !");
    }

    // Install proposed 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5*_x_;
      sd_ptr->xmax = +0.5*_x_;
      sd_ptr->ymin = -0.5*_y_;
      sd_ptr->ymax = +0.5*_y_;
      sd_ptr->zmin = -0.5*_z_;
      sd_ptr->zmax = +0.5*_z_;
      _solid_.set_stackable_data (sd_ptr);
    }

    _solid_.set_user_draw ((void *) &plate_with_hole_model::gnuplot_draw_user_function);

    grab_logical ().set_name (i_model::make_logical_volume_name (name_));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (_material_);

    return;
  }

  /*** dump ***/

  void plate_with_hole_model::tree_dump (std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Material : '" << _material_ << "'" << std::endl;

    {
      // Mother scintillator box:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Mother box : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mother_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Hole X-position : " << _x_pos_hole_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Hole Y-position : " << _y_pos_hole_ / CLHEP::mm << " mm" << std::endl;

    {
      // Hole:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Hole : " << std::endl;
      if (_box_hole_.is_valid ())
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _box_hole_.tree_dump (out_,"",indent_oss.str ());
        }
      if (_cyl_hole_.is_valid ())
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _cyl_hole_.tree_dump (out_,"",indent_oss.str ());
        }
    }

    {
      // Solid:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    return;
  }

  void plate_with_hole_model::gnuplot_draw_user_function (std::ostream & out_,
                                                          const geomtools::vector_3d & position_,
                                                          const geomtools::rotation_3d & rotation_,
                                                          const geomtools::i_object_3d & obj_,
                                                          void *)
  {
    //using namespace geomtools;
    const geomtools::subtraction_3d * solid = dynamic_cast<const geomtools::subtraction_3d *>(&obj_);
    DT_THROW_IF (solid == 0,
                 std::logic_error,
                 "3D-object of '" << obj_.get_shape_name () << "' shape type has not the right type !");
    const geomtools::i_composite_shape_3d::shape_type & s1 = solid->get_shape1 ();
    const geomtools::i_composite_shape_3d::shape_type & s2 = solid->get_shape2 ();
    const geomtools::i_shape_3d & sh1 = s1.get_shape ();
    const geomtools::i_shape_3d & sh2 = s2.get_shape ();

    // extract useful stuff (shapes and properties):
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);

    const bool draw_mother = true;
    if (draw_mother)
      {
        // draw first shape:
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        geomtools::placement world_item_placement;
        mother_world_placement.child_to_mother (s1.get_placement (),
                                                world_item_placement);
        const geomtools::vector_3d   & sh1_pos = world_item_placement.get_translation ();
        const geomtools::rotation_3d & sh1_rot = world_item_placement.get_rotation ();
        geomtools::gnuplot_draw::draw_box (out_,
                                           sh1_pos,
                                           sh1_rot,
                                           mother_box);
      }

    double hole_x_pos = 0.0;
    double hole_y_pos = 0.0;
    double hole_z_pos = 0.0; // Always ZERO
    if (solid->properties ().has_key ("xy_pos_hole"))
      {
        std::vector<double> xy_pos_hole;
        solid->properties ().fetch ("xy_pos_hole", xy_pos_hole);
        hole_x_pos = xy_pos_hole.at(0);
        hole_y_pos = xy_pos_hole.at(1);
      }

    const bool draw_hole = true;
    if (draw_hole)
      {
        // draw hole contour :
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        {
          geomtools::placement c1_plcmt;
          c1_plcmt.set (hole_x_pos, hole_y_pos, hole_z_pos, 0., 0. , 0.);
          geomtools::placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          if (sh2.get_shape_name () == "box")
            {
              const geomtools::box & hole_box = dynamic_cast<const geomtools::box &> (sh2);
              geomtools::gnuplot_draw::draw_box (out_,
                                                 sh2_pos,
                                                 sh2_rot,
                                                 hole_box.get_x (),
                                                 hole_box.get_y (),
                                                 mother_box.get_z (),
                                                 AXIS_Z, 8);
            }
          if (sh2.get_shape_name () == "cylinder")
            {
              const geomtools::cylinder & hole_cylinder = dynamic_cast<const geomtools::cylinder &> (sh2);
              geomtools::gnuplot_draw::draw_cylinder (out_,
                                                      sh2_pos,
                                                      sh2_rot,
                                                      hole_cylinder.get_r (),
                                                      mother_box.get_z ());
            }
        }

      }
    return;
  }

} // end of namespace geomtools

// end of plate_with_hole_model.cc
