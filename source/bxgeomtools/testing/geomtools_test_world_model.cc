// -*- mode: c++ ; -*-
/* geomtools_test_world_model.cc
 */

#include <geomtools_test_world_model.h>

namespace geomtools {

  using namespace std;

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(test_world_model,"geomtools::test::test_world_model")

  const geomtools::box & test_world_model::get_solid () const
  {
    return __solid;
  }

  string test_world_model::get_model_id () const
  {
    return "geomtools::test::test_world_model";
  }

    // ctor:
  test_world_model::test_world_model () : geomtools::i_model ()
  {
    __setup_model = 0;
  }

  test_world_model::~test_world_model ()
  {
  }

  void test_world_model::_at_construct (const datatools::properties & config_,
                                        models_col_type * models_)
  {
    bool devel = false;
    if (devel) clog << "DEVEL: test_world_model::_at_construct: Entering..." << endl;
    string material = "vacuum";
    string setup_model_name;
    double phi = 0. * CLHEP::degree;
    double theta = 0. * CLHEP::degree;

    if (config_.has_key ("theta"))
      {
         if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'theta'..." << endl;
        theta = config_.fetch_real ("theta");
        theta *= CLHEP::degree;
      }

    if (config_.has_key ("phi"))
      {
         if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'phi'..." << endl;
        phi = config_.fetch_real ("phi");
        phi *= CLHEP::degree;
      }

    if (material::has_key (config_, material::make_key (material::material_ref_property())))
      {
        if (devel) clog << "DEVEL: test_world_model::_at_construct: key= 'material'..." << endl;
        material = config_.fetch_string (material::make_key (material::material_ref_property()));
      }

    if (config_.has_key ("setup_model"))
      {
        setup_model_name = config_.fetch_string ("setup_model");
      }
    else
      {
        ostringstream message;
        message << "test_world_model::_at_construct: "
                << "Missing 'setup_model' property !";
        throw runtime_error (message.str ());
      }

    if (! models_)
      {
        ostringstream message;
        message << "test_world_model::_at_construct: "
                << "Missing logicals dictionary !";
        throw runtime_error (message.str ());
      }

    // Setup model:
    {
      models_col_type::const_iterator found = models_->find (setup_model_name);
      if (found != models_->end ())
        {
          //__setup_model = (dynamic_cast<const test_model_2 *> (found->second));
          __setup_model = found->second;
        }
      else
        {
          ostringstream message;
          message << "test_world_model::_at_construct: "
                  << "Cannot find model with name '"
                  << setup_model_name << "' !";
          throw runtime_error (message.str ());
        }
    }

    vector_3d setup_pos;
    create_xyz (setup_pos, 0, 0, 0);
    __setup_placement.set_translation (setup_pos);
    __setup_placement.set_orientation (phi, theta, 0.0);

    __solid.reset ();

    double setup_x = 1000.;
    double setup_y = 1000.;
    double setup_z = 1000.;
    if (__setup_model->get_logical ().get_shape ().get_shape_name () == "box")
      {
        const geomtools::box * b = 0;
        b = dynamic_cast<const geomtools::box *> (&(__setup_model->get_logical ()).get_shape ());
        setup_x = b->get_x ();
        setup_y = b->get_y ();
        setup_z = b->get_z ();
      }

    double size = setup_x;
    if (setup_y > size) size = setup_y;
    if (setup_z > size) size = setup_z;
    size *= 2;
    __solid.set_x (size);
    __solid.set_y (size);
    __solid.set_z (size);
    if (!__solid.is_valid ())
      {
        throw runtime_error ("test_world_model::_at_construct: Invalid solid !");
      }

    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (__solid);
    grab_logical ().set_material_ref (material);

    if (devel) clog << "DEVEL: test_world_model::_at_construct: Install physicals..." << endl;
    __setup_phys.set_name (i_model::make_physical_volume_name ("setup"));
    __setup_phys.set_placement (__setup_placement);
    __setup_phys.set_logical (__setup_model->get_logical ());
    __setup_phys.set_mother (_logical);
    if (devel) clog << "DEVEL: test_world_model::_at_construct: Physicals are installed. " << endl;

    if (devel) clog << "DEVEL: test_world_model::_at_construct: Exiting." << endl;
    return;
  }

  void test_world_model::tree_dump (ostream & out_,
                                const string & title_ ,
                                const string & indent_,
                                bool inherit_) const
  {
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

     {
       // Setup model:
        if (__setup_model)
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Setup model : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
              __setup_model->tree_dump (out_, "", indent_oss.str ());
            }
          }
        else
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Setup model : " << "<missing>" << endl;
          }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Setup placement: " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          __setup_placement.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Setup physical : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          __setup_phys.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
             << "Solid : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          __solid.tree_dump (out_, "", indent_oss.str ());
        }
      }

     return;
  }

} // end of namespace geomtools

// end of geomtools_test_world_model.cc
