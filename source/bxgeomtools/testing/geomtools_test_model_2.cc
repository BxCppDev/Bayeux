// geomtools_test_model_2.cc

// Ourselves:
#include <geomtools_test_model_2.h>

namespace geomtools {

  using namespace std;

  // Registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(test_model_2,"geomtools::test::test_model_2")

  const geomtools::box & test_model_2::get_solid () const
  {
    return _solid_;
  }

  string test_model_2::get_model_id () const
  {
    return "geomtools::test::test_model_2";
  }

  test_model_2::test_model_2 () : geomtools::i_model ()
  {
    _sub1_model_ = 0;
    _sub2_model_ = 0;
    return;
  }

  test_model_2::~test_model_2 ()
  {
    return;
  }

  void test_model_2::_at_construct (const datatools::properties & config_,
                                    models_col_type * models_)
  {
    bool devel = false;
    if (devel) clog << "DEVEL: test_model_2::_at_construct: Entering..." << endl;
    double gas_pressure = 1. * CLHEP::atmosphere;
    double gas_temperature = 300. * CLHEP::kelvin;
    double distance = 50. * CLHEP::mm;
    double phi = 0. * CLHEP::degree;
    double theta = 0. * CLHEP::degree;
    string material = "air";
    string sub1_model_name;
    string sub2_model_name;

    if (config_.has_key ("gas_pressure"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'gas_pressure'..." << endl;
        gas_pressure = config_.fetch_real ("gas_pressure");
        gas_pressure *= CLHEP::atmosphere;
      }

    if (config_.has_key ("gas_temperature"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'gas_temperature'..." << endl;
        gas_temperature = config_.fetch_real ("gas_temperature");
        gas_temperature *= CLHEP::kelvin;
      }

    if (config_.has_key ("distance"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'distance'..." << endl;
        distance = config_.fetch_real ("distance");
        distance *= CLHEP::mm;
      }

    if (config_.has_key ("theta"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'theta'..." << endl;
        theta = config_.fetch_real ("theta");
        theta *= CLHEP::degree;
      }

    if (config_.has_key ("phi"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'phi'..." << endl;
        phi = config_.fetch_real ("phi");
        phi *= CLHEP::degree;
      }

    if (config_.has_key ("material.ref"))
      {
        if (devel) clog << "DEVEL: test_model_2::_at_construct: key= 'material'..." << endl;
        material = config_.fetch_string ("material.ref");
      }

    if (config_.has_key ("sub1_model"))
      {
        sub1_model_name = config_.fetch_string ("sub1_model");
      }
    else
      {
        ostringstream message;
        message << "test_model_2::_at_construct: "
                << "Missing 'sub1_model' property !";
        throw runtime_error (message.str ());
      }

    if (config_.has_key ("sub2_model"))
      {
        sub2_model_name = config_.fetch_string ("sub2_model");
      }
    else
      {
        ostringstream message;
        message << "test_model_2::_at_construct: "
                << "Missing 'sub2_model' property !";
        throw runtime_error (message.str ());
      }

    if (! models_)
      {
        ostringstream message;
        message << "test_model_2::_at_construct: "
                << "Missing logicals dictionary !";
        throw runtime_error (message.str ());
      }

    // Sub1:
    {
      models_col_type::const_iterator found = models_->find (sub1_model_name);
      if (found != models_->end ()) {
          _sub1_model_ = (dynamic_cast<const test_model_1 *> (found->second));
      } else {
        ostringstream message;
        message << "test_model_2::_at_construct: "
                << "Cannot find model with name '"
                << sub1_model_name << "' !";
        throw runtime_error (message.str ());
      }
    }

    // Sub2:
    {
      models_col_type::const_iterator found = models_->find (sub2_model_name);
      if (found != models_->end ())
        {
          _sub2_model_ = (dynamic_cast<const test_model_1 *> (found->second));
        }
      else
        {
          ostringstream message;
          message << "test_model_2::_at_construct: "
                  << "Cannot find model with name '"
                  << sub2_model_name << "' !";
          throw runtime_error (message.str ());
        }
    }

    _distance_ = distance;
    _phi_ = phi;
    _theta_ = theta;

    vector_3d sub1_pos;
    double r1 = 0.5 * distance + _sub1_model_->get_solid ().get_half_z ();
    create_spherical (sub1_pos, r1, phi, theta);
    _sub1_placement_.set_translation (sub1_pos);
    _sub1_placement_.set_orientation (phi, theta + M_PI, 0.0);

    vector_3d sub2_pos;
    double r2 = 0.5 * distance + _sub2_model_->get_solid ().get_half_z ();
    create_spherical (sub2_pos, r2, phi, theta + M_PI);
    _sub2_placement_.set_translation (sub2_pos);
    _sub2_placement_.set_orientation (phi, theta, 0.0);

    _solid_.reset ();
    double size =
      distance
      + 2 * _sub1_model_->get_solid ().get_x ()
      + 2 * _sub2_model_->get_solid ().get_x ();
    _solid_.set_x (size);
    _solid_.set_y (size);
    _solid_.set_z (size);
    _solid_.lock(); // Mandatory
    if (!_solid_.is_valid ())
      {
        throw runtime_error ("test_model_2::_at_construct: Invalid solid !");
      }

    // initialize the 'logical_volume' of this model:
    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (material);

    if (devel) clog << "DEVEL: test_model_2::_at_construct: Install physicals..." << endl;
    _sub1_phys_.set_name (i_model::make_physical_volume_name ("sub1"));
    _sub1_phys_.set_placement (_sub1_placement_);
    _sub1_phys_.set_logical (_sub1_model_->get_logical ());
    _sub1_phys_.set_mother (_logical);

    _sub2_phys_.set_name (i_model::make_physical_volume_name ("sub2"));
    _sub2_phys_.set_placement (_sub2_placement_);
    _sub2_phys_.set_logical (_sub2_model_->get_logical ());
    _sub2_phys_.set_mother (_logical);
    if (devel) clog << "DEVEL: test_model_2::_at_construct: Physicals are installed. " << endl;

    if (devel) clog << "DEVEL: test_model_2::_at_construct: Exiting." << endl;
    return;
  }

  void test_model_2::tree_dump (ostream & out_,
                                const string & title_ ,
                                const string & indent_,
                                bool inherit_) const
  {
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);


      {
        // Sub-model #1:
        if (_sub1_model_)
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Sub model 1 : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
              _sub1_model_->tree_dump (out_, "", indent_oss.str ());
            }
          }
        else
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Sub model 1 : " << "<missing>" << endl;
          }
      }

      {
        // Sub-model #2:
        if (_sub2_model_)
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Sub model 2 : " << endl;
            {
              ostringstream indent_oss;
              indent_oss << indent;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
              _sub2_model_->tree_dump (out_, "", indent_oss.str ());
            }
          }
        else
          {
            out_ << indent << datatools::i_tree_dumpable::tag
                 << "Sub model 2 : " << "<missing>" << endl;
          }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Placement 1 : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _sub1_placement_.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Placement 2 : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _sub2_placement_.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Sub physical 1 : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _sub1_phys_.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Sub physical 2 : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _sub2_phys_.tree_dump (out_, "", indent_oss.str ());
        }
      }

      {
        out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
             << "Solid : " << endl;
        {
          ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          _solid_.tree_dump (out_, "", indent_oss.str ());
        }
      }


     return;
  }

} // end of namespace geomtools

// end of geomtools_test_model_2.cc
