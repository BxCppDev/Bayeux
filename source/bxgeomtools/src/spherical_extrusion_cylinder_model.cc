// -*- mode: c++ ; -*-
/* spherical_extrusion_cylinder_model.cc
 */

#include <geomtools/spherical_extrusion_cylinder_model.h>

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <datatools/utils.h>
#include <datatools/units.h>

namespace geomtools {

  using namespace std;

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(spherical_extrusion_cylinder_model,"geomtools::spherical_extrusion_cylinder_model");

  string spherical_extrusion_cylinder_model::get_model_id () const
  {
    return "geomtools::spherical_extrusion_cylinder_model";
  }

  const string & spherical_extrusion_cylinder_model::get_material () const
  {
    return _material_;
  }

  const geomtools::subtraction_3d & spherical_extrusion_cylinder_model::get_solid () const
  {
    return _solid_;
  }

  // ctor:
  spherical_extrusion_cylinder_model::spherical_extrusion_cylinder_model () : i_model ("spherical_extrusion_cylinder_model")
  {
    _material_ = "";
    _bottom_ = false;
    datatools::invalidate (_r_);
    datatools::invalidate (_z_);
    datatools::invalidate (_r_extrusion_);
    datatools::invalidate (_r_sphere_);
    return;
  }

  // dtor:
  spherical_extrusion_cylinder_model::~spherical_extrusion_cylinder_model ()
  {
    return;
  }

  void spherical_extrusion_cylinder_model::_at_construct (const string & name_,
                                                          const datatools::properties & config_,
                                                          geomtools::models_col_type * models_)
  {
    bool devel = g_devel;
    if (config_.has_flag ("devel"))
      {
        devel = true;
      }
    if (devel) clog << "DEVEL: spherical_extrusion_cylinder_model::_at_construct: Entering..." << endl;

    set_name (name_);

    double lunit = CLHEP::mm;
    double r;
    double z;
    double re;
    double rs;

    /*** parse properties ***/

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

    if (config_.has_key ("material.ref"))
      {
        _material_ = config_.fetch_string ("material.ref");
      }
    else
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Missing 'material.ref' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) z *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Missing 'z' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("r"))
      {
        r = config_.fetch_real ("r");
        if (! config_.has_explicit_unit ("r")) r *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Missing 'r' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("r_extrusion"))
      {
        re = config_.fetch_real ("r_extrusion");
        if (! config_.has_explicit_unit ("r_extrusion")) re *= lunit;
     }
    else
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Missing 'r_extrusion' property !";
        throw logic_error (message.str ());
      }

    if (config_.has_key ("r_sphere"))
      {
        rs = config_.fetch_real ("r_sphere");
        if (! config_.has_explicit_unit ("r_sphere")) rs *= lunit;
      }
    else
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Missing 'r_sphere' property !";
        throw logic_error (message.str ());
      }


    if (devel) clog << "DEVEL: geomtools::spherical_extrusion_cylinder_model::_at_construct: Properties are parsed !" << endl;

    _r_ = r;
    _z_ = z;
    _r_extrusion_ = re;
    _r_sphere_    = rs;
    if (config_.has_flag ("bottom"))
      {
        _bottom_ = true;
      }

    if (_r_extrusion_ > _r_)
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Extrusion radius is larger than mother cylinder radius !";
        throw logic_error (message.str ());
      }

    if (_r_extrusion_ > _r_sphere_)
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Extrusion radius is larger than the spherical radius !";
        throw logic_error (message.str ());
      }

    _mother_.set_r (_r_);
    _mother_.set_z (_z_);
    if (! _mother_.is_valid ())
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
                << "Invalid dimension(s) for the mother cylinder !";
        throw logic_error (message.str ());
      }

    _extrusion_.set (_r_sphere_);

    double a = std::sqrt (_r_sphere_ * _r_sphere_
                          - _r_extrusion_ * _r_extrusion_);
    double c = _r_sphere_ - a;
    if (c > _z_)
      {
        throw logic_error ("geomtools::spherical_extrusion_cylinder_model::_at_construct: Mother cylinder is not long enough (Z ) to host the extrusion !");
      }
    double h = _z_ - c;
    double zsphere = 0.5 * _z_ + a;
    if (_bottom_) zsphere *= -1;
    if (devel)
      {
        cerr << "DEVEL: " << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
             << "z=" << z / CLHEP::mm << " mm" << endl;
        cerr << "DEVEL: " << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
             << "r=" << r / CLHEP::mm << " mm" << endl;
        cerr << "DEVEL: " << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
             << "re=" << re / CLHEP::mm << " mm" << endl;
        cerr << "DEVEL: " << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
             << "rs=" << rs / CLHEP::mm << " mm" << endl;
        cerr << "DEVEL: " << "geomtools::spherical_extrusion_cylinder_model::_at_construct: "
             << "h=" << h / CLHEP::mm << " mm" << endl;
      }

    geomtools::placement sphere_extrusion_placement (0, 0, zsphere, 0, 0, 0);
    if (devel) clog << "DEVEL: geomtools::spherical_extrusion_cylinder_model::_at_construct: zsphere=" << zsphere / CLHEP::mm << " mm" << endl;
    _solid_.set_shapes (_mother_,
                        _extrusion_,
                        sphere_extrusion_placement);

    _solid_.properties ().store ("h", h);
    if (_bottom_)
      {
        _solid_.properties ().store_flag ("bottom");
      }
    // Install proposed 'stackable data' pointer in the shape:
    {
      geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
      sd_ptr->xmin = -_r_;
      sd_ptr->xmax = +_r_;
      sd_ptr->ymin = -_r_;
      sd_ptr->ymax = +_r_;
      if (! _bottom_)
        {
          sd_ptr->zmax = +0.5 * z - c;
          sd_ptr->zmin = -0.5 * z;
        }
      else
        {
          sd_ptr->zmin = -0.5 * z + c;
          sd_ptr->zmax = +0.5 * z;
        }
      _solid_.set_stackable_data (sd_ptr);
      if (devel)
        {
          clog << "DEVEL: spherical_extrusion_cylinder_model::_at_construct: Entering..." << endl;
          sd_ptr->tree_dump (cerr, "spherical_extrusion_cylinder_model::_at_construct: Stackable data: ", "DEVEL: ");
        }
    }

    _solid_.set_user_draw ((void *) &spherical_extrusion_cylinder_model::gnuplot_draw_user_function);

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    get_logical ().set_material_ref (_material_);

    if (devel)
      {
        this->tree_dump (clog, "spherical_extrusion_cylinder_model::_at_construct: ", "DEVEL: ");
      }
    if (devel) clog << "DEVEL: spherical_extrusion_cylinder_model::_at_construct: Exiting." << endl;
    return;
  }

  /*** dump ***/

  void spherical_extrusion_cylinder_model::tree_dump (ostream & out_,
                                                      const string & title_,
                                                      const string & indent_,
                                                      bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Material : '" << _material_ << "'" << std::endl;

    {
      // Mother scintillator box:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Mother cylinder : " << std::endl;
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

  void spherical_extrusion_cylinder_model::gnuplot_draw_user_function (std::ostream & out_,
                                                                       const geomtools::vector_3d & position_,
                                                                       const geomtools::rotation_3d & rotation_,
                                                                       const geomtools::i_object_3d & obj_,
                                                                       void *)
  {
    using namespace geomtools;
    const subtraction_3d * solid = dynamic_cast<const geomtools::subtraction_3d *>(&obj_);
    if (solid == 0)
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
                << "3D-object of '" << obj_.get_shape_name () << "' shape type has not the right type !";
        throw logic_error (message.str ());
        //return;
      }
    const i_composite_shape_3d::shape_type & s1 = solid->get_shape1 ();
    const i_composite_shape_3d::shape_type & s2 = solid->get_shape2 ();
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();

    // extract useful stuff (shapes and properties):
    const geomtools::cylinder & mother_cylinder = dynamic_cast<const geomtools::cylinder &> (sh1);
    const geomtools::sphere & extrusion_sphere = dynamic_cast<const geomtools::sphere &> (sh2);
    bool bottom = false;
    if (solid->properties ().has_flag ("bottom"))
      {
        bottom = true;
      }
    if (! solid->properties ().has_key ("h"))
      {
        ostringstream message;
        message << "geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
                << "Missing 'h' property in the shape !";
        throw logic_error (message.str ());
      };

    double h = solid->properties ().fetch_real ("h");

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
        gnuplot_draw::draw_cylinder (out_,
                                     sh1_pos,
                                     sh1_rot,
                                     mother_cylinder);
      }

    double zcyl = mother_cylinder.get_z ();
    double rcyl = mother_cylinder.get_r ();
    double c = zcyl - h;
    double rs = extrusion_sphere.get_r ();
    double a = rs - c;
    double re = sqrt (rs * rs - a * a);
    /*
    cerr << "DEVEL : geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
         << " zcyl = " << zcyl << endl;
    cerr << "DEVEL : geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
         << " rs = " << rs << endl;
    cerr << "DEVEL : geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
         << " c = " << c << endl;
    cerr << "DEVEL : geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
         << " a = " << a << endl;
    cerr << "DEVEL : geomtools::spherical_extrusion_cylinder_model::gnuplot_draw_user_function: "
         << " h = " << h << endl;
    */
    bool draw_extrusion = true;
    if (draw_extrusion)
      {
        // draw extrusion limit:
        placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        // surface extrusion circle:
        {
          double z = 0.5 * zcyl;
          if (bottom) z *= -1.0;
          placement c1_plcmt;
          c1_plcmt.set (0., 0., z, 0. , 0. , 0.);
          placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          gnuplot_draw::draw_circle (out_,
                                     sh2_pos,
                                     sh2_rot,
                                     re);
        }

        // extrusion arcs:
        {
          double z = 0.5 * zcyl;
          gnuplot_draw::polyline_type arc1;
          gnuplot_draw::polyline_type arc2;
          double theta0 = asin (re / rs);
          size_t nsamples = 20;
          double dt = 2 * theta0 / nsamples;
          double zs = z - c + rs;
          if (bottom) zs *= -1.0;
          for (int i = 0; i <= nsamples; i++)
            {
              double theta = -theta0 + i * dt;
              double dz = -rs * cos (theta);
              if (bottom) dz *= -1.0;
              double dt = rs * sin (theta);
              vector_3d P1 (dt, 0.0, zs + dz);
              arc1.push_back (P1);
              vector_3d P2 (0.0, dt, zs + dz);
              arc2.push_back (P2);
            }
          gnuplot_draw::draw_polyline (out_,
                                       position_,
                                       rotation_,
                                       arc1);
          gnuplot_draw::draw_polyline (out_,
                                       position_,
                                       rotation_,
                                       arc2);
        }

        double rfactor[2];
        rfactor[0]= 0.5;
        rfactor[1]= 0.25;

        for (int i = 0; i < 2; i++)
          {
            double c2 = rfactor[i] * c;
            double z = 0.5 * mother_cylinder.get_z () - c + c2;
            if (bottom) z *= -1.0;
            double rs = extrusion_sphere.get_r ();
            double a2 = rs - c2;
            double r2 = sqrt (rs *rs - a2 * a2);

            placement c1_plcmt;
            c1_plcmt.set (0., 0., z, 0. , 0. , 0.);
            placement world_item_placement;
            mother_world_placement.child_to_mother (c1_plcmt,
                                                    world_item_placement);
            const vector_3d   & sh2_pos = world_item_placement.get_translation ();
            const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
            gnuplot_draw::draw_circle (out_,
                                       sh2_pos,
                                       sh2_rot,
                                       r2);
          }
      }
    return;
  }



} // end of namespace geomtools

// end of spherical_extrusion_cylinder_model.cc
