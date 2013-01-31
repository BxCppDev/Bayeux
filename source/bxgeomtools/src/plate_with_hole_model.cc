// -*- mode: c++ ; -*-
/* plate_with_hole_model.cc
 */

#include <geomtools/plate_with_hole_model.h>

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <datatools/utils.h>
#include <datatools/units.h>

namespace geomtools {

  using namespace std;

  // registration :   
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(plate_with_hole_model,"geomtools::plate_with_hole_model");

  string plate_with_hole_model::get_model_id () const
  {
    return "geomtools::plate_with_hole_model";
  }

  const string & plate_with_hole_model::get_material () const
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
    bool devel = g_devel;
    if (config_.has_flag ("devel"))
      {
        devel = true;
      }
    if (devel) std::clog << "DEVEL: plate_with_hole_model::_at_construct: Entering..." << std::endl;

    set_name (name_);

    double lunit = CLHEP::mm;
    double x;
    double y;
    double z;
    double r_hole;
    double x_hole;
    double y_hole;

    datatools::invalidate (r_hole);
    datatools::invalidate (x_hole);
    datatools::invalidate (y_hole);

    double x_pos_hole = 0.0;
    double y_pos_hole = 0.0;

    /*** parse properties ***/

    if (config_.has_key ("length_unit"))
      {
        std::string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("material.ref"))
      {
        _material_ = config_.fetch_string ("material.ref");
      }
    else
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Missing 'material.ref' property !";
        throw std::logic_error (message.str ());
      }

    if (config_.has_key ("x"))
      {
        x = config_.fetch_real ("x");
      }
    else
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Missing 'x' property !";
        throw std::logic_error (message.str ());
      }

    if (config_.has_key ("y"))
      {
        y = config_.fetch_real ("y");
      }
    else
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Missing 'y' property !";
        throw std::logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
      }
    else
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Missing 'z' property !";
        throw std::logic_error (message.str ());
      }

    x  *= lunit;
    y  *= lunit;
    z  *= lunit;

    if (config_.has_key ("r_hole"))
      {
        r_hole = config_.fetch_real ("r_hole");
        r_hole  *= lunit;
      }
    // else
    //   {
    //     std::ostringstream message;
    //     message << "geomtools::plate_with_hole_model::_at_construct: "
    //             << "Missing 'r_hole' property !";
    //     throw std::logic_error (message.str ());
    //   }

    if (! datatools::is_valid (r_hole))
      {
        if (config_.has_key ("x_hole"))
          {
            x_hole = config_.fetch_real ("x_hole");
          }
        else
          {
            std::ostringstream message;
            message << "geomtools::plate_with_hole_model::_at_construct: "
                    << "Missing 'x_hole' property !";
            throw std::logic_error (message.str ());
          }
        
        if (config_.has_key ("y_hole"))
          {
            y_hole = config_.fetch_real ("y_hole");
          }
        else
          {
            std::ostringstream message;
            message << "geomtools::plate_with_hole_model::_at_construct: "
                    << "Missing 'y_hole' property !";
            throw std::logic_error (message.str ());
          }
        x_hole  *= lunit;
        y_hole  *= lunit;
      }
    
    if (config_.has_key ("x_pos_hole"))
      {
        x_pos_hole = config_.fetch_real ("x_pos_hole");
        x_pos_hole *= lunit;
      }
    
    if (config_.has_key ("y_pos_hole"))
      {
        y_pos_hole = config_.fetch_real ("y_pos_hole");
        y_pos_hole  *= lunit;
      }
    
    if (devel) std::clog << "DEVEL: geomtools::plate_with_hole_model::_at_construct: Properties are parsed !" << std::endl;
    
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
    if (_x_pos_hole_ * _y_pos_hole_ != 0)
      {
        _solid_.properties ().store ("xy_pos_hole", xy_pos_hole);
        //_solid_.properties ().tree_dump (cerr, "Solid: ", "DEVEL: ");
      }
    // Checks :
    double hole_x_min = x_pos_hole;
    double hole_x_max = x_pos_hole;
    double hole_y_min = y_pos_hole;
    double hole_y_max = y_pos_hole;
    if (datatools::is_valid (_r_hole_))
      {
        hole_x_min -= _r_hole_;
        hole_x_max += _r_hole_;
        hole_y_min -= _r_hole_;
        hole_y_max += _r_hole_;
      }
    else
      {
        hole_x_min -= 0.5 * _x_hole_;
        hole_x_max += 0.5 * _x_hole_;
        hole_y_min -= 0.5 * _y_hole_;
        hole_y_max += 0.5 * _y_hole_;
      }
    
    if ((hole_x_min < -0.5 * _x_) || (hole_x_max > 0.5 * _x_))
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Hole size is too large for mother box dimension !";
        throw std::logic_error (message.str ());
      }
    
    if ((hole_y_min < -0.5 * _y_) || (hole_y_max > 0.5 * _y_))
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Hole size is too large for mother box dimension !";
        throw std::logic_error (message.str ());
      }
    
    _mother_.set_x (_x_);
    _mother_.set_y (_y_);
    _mother_.set_z (_z_);
    if (! _mother_.is_valid ())
      {
        ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "Invalid dimension(s) for the mother box !";
        throw logic_error (message.str ());
      }
           
    geomtools::placement hole_placement (_x_pos_hole_, _y_pos_hole_, 0.0, 
                                         0, 0, 0);
    if (datatools::is_valid (_r_hole_))
      {
        _cyl_hole_.set_r (_r_hole_);
        _cyl_hole_.set_z (_z_hole_);
        _solid_.set_shapes (_mother_, 
                            _cyl_hole_, 
                            hole_placement);
      }
    else if (datatools::is_valid (_x_hole_) && datatools::is_valid (_y_hole_))
      {
        _box_hole_.set_x (_x_hole_);
        _box_hole_.set_y (_y_hole_);
        _box_hole_.set_z (_z_hole_);
        _solid_.set_shapes (_mother_, 
                            _box_hole_, 
                            hole_placement);
      }
    else
      {
        ostringstream message;
        message << "geomtools::plate_with_hole_model::_at_construct: "
                << "No defined shape for hole !";
        throw logic_error (message.str ());
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
      // if ((_x_pos_hole_ * _y_pos_hole_) == 0.0)
      //   {
      //     sd_ptr->zmax = 0.0;
      //     sd_ptr->zmin = 0.0;
      //   }
      _solid_.set_stackable_data (sd_ptr);
      if (devel)
        {
          clog << "DEVEL: plate_with_hole_model::_at_construct: Entering..." << endl;
          sd_ptr->tree_dump (cerr, "plate_with_hole_model::_at_construct: Stackable data: ", "DEVEL: ");
        }
    }

    _solid_.set_user_draw ((void *) &plate_with_hole_model::gnuplot_draw_user_function);

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    get_logical ().set_material_ref (_material_);

    if (devel)
      {
        this->tree_dump (clog, "plate_with_hole_model::_at_construct: ", "DEVEL: ");
      }
    if (devel) clog << "DEVEL: plate_with_hole_model::_at_construct: Exiting." << endl;
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
    const subtraction_3d * solid = dynamic_cast<const geomtools::subtraction_3d *>(&obj_);
    if (solid == 0)
      {
        std::ostringstream message;
        message << "geomtools::plate_with_hole_model::gnuplot_draw_user_function: "
                << "3D-object of '" << obj_.get_shape_name () << "' shape type has not the right type !";
        throw std::logic_error (message.str ());
      }
    const i_composite_shape_3d::shape_type & s1 = solid->get_shape1 ();
    const i_composite_shape_3d::shape_type & s2 = solid->get_shape2 ();
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();

    // extract useful stuff (shapes and properties):
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);

    bool draw_mother = true;
    if (draw_mother)
      {
        // draw first shape:
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

    bool draw_hole = true;
    if (draw_hole)
      {
        // draw hole contour :
        placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        {
          placement c1_plcmt;
          c1_plcmt.set (hole_x_pos, hole_y_pos, hole_z_pos, 0., 0. , 0.);
          placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          if (sh2.get_shape_name () == "box")
            {
              const geomtools::box & hole_box 
                = dynamic_cast<const geomtools::box &> (sh2);
              gnuplot_draw::draw_box (out_,
                                      sh2_pos,
                                      sh2_rot,
                                      hole_box.get_x (),
                                      hole_box.get_y (),
                                      mother_box.get_z (),
                                      AXIS_Z, 8);
            }
          if (sh2.get_shape_name () == "cylinder")
            {
              const geomtools::cylinder & hole_cylinder 
                = dynamic_cast<const geomtools::cylinder &> (sh2);
              gnuplot_draw::draw_cylinder (out_,
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
