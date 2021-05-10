// -*- mode: c++ ; -*-
/* geomtools_test_model_1.cc
 */

#include <geomtools_test_model_1.h>

namespace geomtools {

  using namespace std;

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(test_model_1,"geomtools::test::test_model_1")

  const geomtools::box & test_model_1::get_solid () const
  {
    return _solid_;
  }

  string test_model_1::get_model_id () const
  {
    return "geomtools::test::test_model_1";
  }

  test_model_1::test_model_1 () : geomtools::i_model ()
  {
  }

  test_model_1::~test_model_1 ()
  {
  }

  void test_model_1::_at_construct (const datatools::properties & config_,
                                    models_col_type * /* models_ */)
  {
    bool devel = false;
    if (devel) clog << "DEVEL: test_model_1::_at_construct: Entering..." << endl;
    double width = 1.0 * CLHEP::mm;
    double height = 1.0 * CLHEP::mm;
    string material;

    if (config_.has_key ("width"))
      {
        if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'width'..." << endl;
        width = config_.fetch_real ("width");
        width *= CLHEP::mm;
      }
    if (config_.has_key ("height"))
      {
        if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'height'..." << endl;
        height = config_.fetch_real ("height");
        height *= CLHEP::mm;
      }
    if (config_.has_key ("material.ref"))
      {
        if (devel) clog << "DEVEL: test_model_1::_at_construct: key= 'material'..." << endl;
        material = config_.fetch_string ("material.ref");
      }
    else
      {
        ostringstream message;
        message << "test_model_1::_at_construct: "
                << "Missing 'material.ref' property !";
        throw std::runtime_error (message.str ());
      }

    _solid_.reset ();
    _solid_.set_x (width);
    _solid_.set_y (width);
    _solid_.set_z (height);
    _solid_.lock(); // Mandatory
    if (! _solid_.is_valid ())
      {
        throw runtime_error ("test_model_1::_at_construct: Invalid solid !");
      }

    // initialize the 'logical_volume' of this model:
    grab_logical ().set_name (i_model::make_logical_volume_name (get_name()));
    grab_logical ().set_shape (_solid_);
    grab_logical ().set_material_ref (material);

    if (devel) clog << "DEVEL: test_model_1::_at_construct: Exiting." << endl;
    return;
  }

  void test_model_1::tree_dump (ostream & out_,
                                const string & title_ ,
                                const string & indent_,
                                bool inherit_) const
  {
     string indent;
     if (! indent_.empty ()) indent = indent_;
     i_model::tree_dump (out_, title_, indent, true);

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
