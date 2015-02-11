// spherical_extrusion_box_model.cc

// Ourselves:
#include <geomtools/spherical_extrusion_box_model.h>

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(spherical_extrusion_box_model,"geomtools::spherical_extrusion_box_model");

  std::string spherical_extrusion_box_model::get_model_id () const
  {
    return "geomtools::spherical_extrusion_box_model";
  }

  const std::string & spherical_extrusion_box_model::get_material () const
  {
    return _material_;
  }

  const geomtools::subtraction_3d & spherical_extrusion_box_model::get_solid () const
  {
    return _solid_;
  }

  spherical_extrusion_box_model::spherical_extrusion_box_model () : i_model("spherical_extrusion_box_model")
  {
    _material_ = "";
    _bottom_ = false;
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    datatools::invalidate(_z_);
    datatools::invalidate(_r_extrusion_);
    datatools::invalidate(_r_sphere_);
    datatools::invalidate(_h_);
    return;
  }

  spherical_extrusion_box_model::~spherical_extrusion_box_model ()
  {
    return;
  }

  void spherical_extrusion_box_model::_at_construct (const std::string & name_,
                                                     const datatools::properties & config_,
                                                     geomtools::models_col_type * /*models_*/)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");

    /*** parse properties ***/

    double lunit = CLHEP::mm;
    if (config_.has_key ("length_unit")) {
      const std::string lunit_str = config_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property in spherical extrusion box model '" << name_ << "' !");
    _material_ = config_.fetch_string ("material.ref");

    DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property in spherical extrusion box model '" << name_ << "' !");
    double x = config_.fetch_real ("x");
    if (! config_.has_explicit_unit ("x")) x *= lunit;

    DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property in spherical extrusion box model '" << name_ << "' !");
    double y = config_.fetch_real ("y");
    if (! config_.has_explicit_unit ("y")) y  *= lunit;

    DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property in spherical extrusion box model '" << name_ << "' !");
    double z = config_.fetch_real ("z");
    if (! config_.has_explicit_unit ("z")) z *= lunit;

    DT_THROW_IF (! config_.has_key ("r_extrusion"), std::logic_error, "Missing 'r_extrusion' property in spherical extrusion box model '" << name_ << "' !");
    double re = config_.fetch_real ("r_extrusion");
    if (! config_.has_explicit_unit ("r_extrusion")) re *= lunit;

    DT_THROW_IF (! config_.has_key ("r_sphere"), std::logic_error, "Missing 'r_sphere' property in spherical extrusion box model '" << name_ << "' !");
    double rs = config_.fetch_real ("r_sphere");
    if (! config_.has_explicit_unit ("r_sphere")) rs *= lunit;

    DT_LOG_TRACE (get_logging_priority (), "Properties are parsed !");

    _x_ = x;
    _y_ = y;
    _z_ = z;
    _r_extrusion_ = re;
    _r_sphere_    = rs;
    if (config_.has_flag ("bottom")) {
      _bottom_ = true;
    }

    DT_THROW_IF (_r_extrusion_ > _x_ || _r_extrusion_ > _y_,
                 std::logic_error,
                 "Extrusion radius is larger than mother box dimension in spherical extrusion box model '" << name_ << "' !");

    DT_THROW_IF (_r_extrusion_ > _r_sphere_,
                 std::logic_error,
                 "Extrusion radius is larger than the spherical radius in spherical extrusion box model '" << name_ << "' !");

    _mother_.set_x (_x_);
    _mother_.set_y (_y_);
    _mother_.set_z (_z_);
    _mother_.lock();
    DT_THROW_IF (! _mother_.is_valid (), std::logic_error, "Invalid dimension(s) for the mother box in spherical extrusion box model '" << name_ << "' !");

    _extrusion_.set (_r_sphere_);
    _extrusion_.lock();

    const double a = std::sqrt (_r_sphere_ * _r_sphere_
                                - _r_extrusion_ * _r_extrusion_);
    const double c = _r_sphere_ - a;
    DT_THROW_IF (c > _z_, std::logic_error, "Mother box is not long enough (Z ) to host the extrusion in spherical extrusion box model '" << name_ << "' !");
    _h_ = _z_ - c;
    double zsphere = 0.5 * _z_ + a;
    if (_bottom_) zsphere *= -1;
    DT_LOG_TRACE (get_logging_priority (), "x       =" << x / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "y       =" << y / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "z       =" << z / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "re      =" << re / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "rs      =" << rs / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "h       =" << _h_ / CLHEP::mm << " mm");
    DT_LOG_TRACE (get_logging_priority (), "zsphere =" << zsphere / CLHEP::mm << " mm");

    const geomtools::placement sphere_extrusion_placement (0, 0, zsphere, 0, 0, 0);
    _solid_.set_shapes (_mother_, _extrusion_, sphere_extrusion_placement);

    // Install proposed 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -0.5*_x_;
      sd_ptr->xmax = +0.5*_x_;
      sd_ptr->ymin = -0.5*_y_;
      sd_ptr->ymax = +0.5*_y_;
      if (! _bottom_) {
        sd_ptr->zmax = +0.5 * z - c;
        sd_ptr->zmin = -0.5 * z;
      } else {
        sd_ptr->zmin = -0.5 * z + c;
        sd_ptr->zmax = +0.5 * z;
      }
      _solid_.set_stackable_data (sd_ptr);
      DT_LOG_TRACE (get_logging_priority (), "Stackable data:");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        sd_ptr->tree_dump (std::cerr);
      }
    }

    // Install a dedicated drawer:
    _drawer_.reset(new wires_drawer(*this, _h_, _bottom_));
    _solid_.set_wires_drawer(*_drawer_);
    _solid_.lock();

    grab_logical ().set_name (i_model::make_logical_volume_name (name_));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (_material_);

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void spherical_extrusion_box_model::tree_dump (std::ostream & out_,
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

    {
      // Spherical extrusion:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Spherical extrusion : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _extrusion_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Bottom : '" << _bottom_ << "'" << std::endl;

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

  spherical_extrusion_box_model::wires_drawer::wires_drawer(const spherical_extrusion_box_model & model_,
                                                            double h_,
                                                            bool bottom_)
  {
    _model_ = &model_;
    _h_ = h_;
    _bottom_ = bottom_;
    return;
  }

  spherical_extrusion_box_model::wires_drawer::~wires_drawer()
  {
    return;
  }

  void spherical_extrusion_box_model::wires_drawer::generate_wires(std::ostream & out_,
                                                                   const geomtools::vector_3d & position_,
                                                                   const geomtools::rotation_3d & rotation_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    const geomtools::subtraction_3d & solid = _model_->get_solid();
    const geomtools::i_composite_shape_3d::shape_type & s1 = solid.get_shape1 ();
    const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2 ();
    const geomtools::i_shape_3d & sh1 = s1.get_shape ();
    const geomtools::i_shape_3d & sh2 = s2.get_shape ();

    // extract useful stuff (shapes and properties):
    const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);
    const geomtools::sphere & extrusion_sphere = dynamic_cast<const geomtools::sphere &> (sh2);
    bool bottom = _bottom_;
    const double h = _h_;

    const bool draw_mother = true;
    if (draw_mother) {
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

    const double zcyl = mother_box.get_z ();
    const double c = zcyl - h;
    const double rs = extrusion_sphere.get_r ();
    const double a = rs - c;
    const double re = sqrt (rs * rs - a * a);
    DT_LOG_TRACE (local_priority, "zcyl = " << zcyl);
    DT_LOG_TRACE (local_priority, "rs = " << rs);
    DT_LOG_TRACE (local_priority, "c = " << c);
    DT_LOG_TRACE (local_priority, "a = " << a);
    DT_LOG_TRACE (local_priority, "h = " << h);

    const bool draw_extrusion = true;
    if (draw_extrusion)
      {
        // draw extrusion limit:
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        // surface extrusion circle:
        {
          double z = 0.5 * zcyl;
          if (bottom) z *= -1.0;
          geomtools::placement c1_plcmt;
          c1_plcmt.set (0., 0., z, 0., 0., 0.);
          geomtools::placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          geomtools::gnuplot_draw::draw_circle (out_,
                                                sh2_pos,
                                                sh2_rot,
                                                re);
        }

        // extrusion arcs:
        {
          double z = 0.5 * zcyl;
          double theta0 = asin (re / rs);
          size_t nsamples = 20;
          double dt = 2 * theta0 / nsamples;
          double zs = z - c + rs;
          if (bottom) zs *= -1.0;
          geomtools::gnuplot_draw::polyline_type arc1;
          geomtools::gnuplot_draw::polyline_type arc2;
          for (size_t i = 0; i <= nsamples; i++)
            {
              const double theta = -theta0 + i * dt;
              double dz = -rs * cos (theta);
              if (bottom) dz *= -1.0;
              const double dt = rs * sin (theta);
              geomtools::vector_3d P1 (dt, 0.0, zs + dz);
              arc1.push_back (P1);
              geomtools::vector_3d P2 (0.0, dt, zs + dz);
              arc2.push_back (P2);
            }
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  position_,
                                                  rotation_,
                                                  arc1);
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  position_,
                                                  rotation_,
                                                  arc2);
        }

        double rfactor[2];
        rfactor[0]= 0.5;
        rfactor[1]= 0.25;

        for (size_t i = 0; i < 2; i++)
          {
            const double c2 = rfactor[i] * c;
            double z = 0.5 * mother_box.get_z () - c + c2;
            if (bottom) z *= -1.0;
            const double rs = extrusion_sphere.get_r ();
            const double a2 = rs - c2;
            const double r2 = sqrt (rs *rs - a2 * a2);

            geomtools::placement c1_plcmt;
            c1_plcmt.set (0., 0., z, 0., 0. , 0.);
            geomtools::placement world_item_placement;
            mother_world_placement.child_to_mother (c1_plcmt,
                                                    world_item_placement);
            const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
            const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
            geomtools::gnuplot_draw::draw_circle (out_,
                                                  sh2_pos,
                                                  sh2_rot,
                                                  r2);
          }
      }
    return;
  }

} // end of namespace geomtools
