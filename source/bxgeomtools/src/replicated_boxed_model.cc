// -*- mode: c++ ; -*- 
/* replicated_boxed_model.cc
 */

#include <geomtools/replicated_boxed_model.h>

#include <exception>
#include <limits> 

#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;
  
  const geomtools::box & replicated_boxed_model::get_box () const
  {
    return _solid_;
  }
  
  const geomtools::box & replicated_boxed_model::get_solid () const
  {
    return _solid_;
  }

  void replicated_boxed_model::set_number_of_items (size_t n_)
  {
    assert_unconstructed ("replicated_boxed_model::set_number_of_items");
    _number_of_items_ = n_;
    return;
  }
  
  size_t replicated_boxed_model::get_number_of_items ()
  {
    return _number_of_items_;
  }
  
  double replicated_boxed_model::get_x () const
  {
    return _x_;
  }
  
  double replicated_boxed_model::get_y () const
  {
    return _y_;
  }
  
  double replicated_boxed_model::get_z () const
  {
    return _z_;
  }
 
  void replicated_boxed_model::set_boxed_model (const i_model & model_)
  {
    assert_unconstructed ("replicated_boxed_model::set_boxed_model");

    // check if model has a logical volume with a box shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    if (shape.get_shape_name () != box::BOX_LABEL)
      {
        ostringstream message;
        message << "replicated_boxed_model::set_boxed_model: "
                << "Model has no 'box' shape ! "
                << "Found '" << shape.get_shape_name () << "' !";
        throw runtime_error (message.str ());
      }
    _boxed_model_ = &model_;
    return;
  }

  const i_model & replicated_boxed_model::get_boxed_model () const
  {
    return *_boxed_model_;
  }
 
  string replicated_boxed_model::get_model_id () const
  {
    return "geomtools::replicated_boxed_model";
  }
  
  replicated_boxed_model::replicated_boxed_model () : i_boxed_model ()
  {
    _boxed_model_ = 0;
    _x_ = numeric_limits<double>::quiet_NaN ();
    _y_ = numeric_limits<double>::quiet_NaN ();
    _z_ = numeric_limits<double>::quiet_NaN ();
    _number_of_items_ = 0;
    return;
  }
  
  replicated_boxed_model::~replicated_boxed_model ()
  {
    return;
  }
  
  void replicated_boxed_model::_at_construct (const string & name_,
                                              const datatools::properties & config_,
                                              models_col_type * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: replicated_boxed_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double x;
    double y;
    double z;
    string material_name = material::constants::instance ().MATERIAL_REF_DEFAULT;
    string boxed_model_name;
    size_t number_of_items = 0;
    string replicant_axis_label = "";
    string replicated_label = "replicated";

    if (config_.has_key ("replicated.label"))
      {
        replicated_label = config_.fetch_string ("replicated.label");
      }  
    else if (config_.has_key ("replicated_label")) // Obsolete
      {
        replicated_label = config_.fetch_string ("replicated_label");
      }  

    if (config_.has_key ("replicated.axis"))
      {
        replicant_axis_label = config_.fetch_string ("replicated.axis");
      }  
    else if (config_.has_key ("replicant_axis")) // Obsolete
      {
        replicant_axis_label = config_.fetch_string ("replicant_axis");
      }  
    else
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Missing 'replicated.axis' property !"; 
        throw runtime_error (message.str ());           
      }

    if (config_.has_key ("replicated.number_of_items"))
      {
        number_of_items = config_.fetch_integer ("replicated.number_of_items");
      }  
    else if (config_.has_key ("number_of_items")) // Obsolete
      {
        number_of_items = config_.fetch_integer ("number_of_items");
      }  
    else
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Missing 'replicated.number_of_items' property !"; 
        throw runtime_error (message.str ());           
      }
     
    if (config_.has_key ("replicated.model"))
      {
        boxed_model_name = config_.fetch_string ("replicated.model");
      }  
    else if (config_.has_key ("boxed_model")) // Obsolete
      {
        boxed_model_name = config_.fetch_string ("boxed_model");
      }  
    else
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Missing 'replicated.model' property !"; 
        throw runtime_error (message.str ());   
      }

    if (number_of_items == 0)
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Number of items is zero !"; 
        throw runtime_error (message.str ());   
      }

    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    if (! axis_ok)
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Invalid replicant axis !"; 
        throw runtime_error (message.str ());   
      }

    if (! models_)
      {
        ostringstream message;
        message << "replicated_boxed_model::_at_construct: "
                << "Missing logicals dictionary !"; 
        throw runtime_error (message.str ());
      }
    
    // Boxed model:
    {
      models_col_type::const_iterator found = 
        models_->find (boxed_model_name);
      if (found != models_->end ())
        {
          set_boxed_model (dynamic_cast<const i_model &>(*(found->second)));
        }
      else
        {
          ostringstream message;
          message << "replicated_boxed_model::_at_construct: "
                  << "Cannot find model with name '" 
                  << boxed_model_name << "' !";
          throw runtime_error (message.str ());
        }
    }
    
    const box & b = 
      dynamic_cast<const box &>(_boxed_model_->get_logical ().get_shape ());
    _x_ = b.get_x ();
    _y_ = b.get_y ();
    _z_ = b.get_z ();
    double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x") 
      {
        _boxed_replica_placement_.set_replicant_step_x (b.get_x ());
        _x_ = b.get_x ()* get_number_of_items ();
        x0 = -0.5 * _x_ + 0.5 * b.get_x ();
      }
    if (replicant_axis_label == "y") 
      {
        _boxed_replica_placement_.set_replicant_step_y (b.get_y ());
        _y_ = b.get_y ()* get_number_of_items ();
        y0 = -0.5 * _y_ + 0.5 * b.get_y ();
      }
    if (replicant_axis_label == "z") 
      {
        _boxed_replica_placement_.set_replicant_step_z (b.get_z ());
        _z_ = b.get_z () * get_number_of_items ();
        z0 = -0.5 * _z_ + 0.5 * b.get_z ();
      }
    _boxed_replica_placement_.set_number_of_items (get_number_of_items ());

    _solid_.reset ();
    _solid_.set_x (_x_);
    _solid_.set_y (_y_);
    _solid_.set_z (_z_);
    if (! _solid_.is_valid ())
      {
        throw runtime_error ("replicated_boxed_model::_at_construct: Invalid solid !");
      }
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    if (_boxed_model_->get_logical ().has_material_ref ())
      {
        material_name = _boxed_model_->get_logical ().get_material_ref ();
      }
    get_logical ().set_material_ref (material_name);

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    _boxed_replica_placement_.set_basic_placement (basic_p);
    _boxed_phys_.set_name (i_model::make_physical_volume_name (replicated_label, get_number_of_items ()));
    _boxed_phys_.set_placement (_boxed_replica_placement_);
    _boxed_phys_.set_logical (_boxed_model_->get_logical ());
    _boxed_phys_.set_mother (_logical);

    if (devel) clog << "DEVEL: replicated_boxed_model::_at_construct: Exiting." << endl;
    return;
  }

  void replicated_boxed_model::tree_dump (ostream & out_, 
                                          const string & title_ , 
                                          const string & indent_, 
                                          bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "X : " << get_x () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Y : " << get_y () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Number of replicated items : " << _number_of_items_ << endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag 
           << "Replicated placement (box) : " << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _boxed_replica_placement_.tree_dump (out_, "", indent_oss.str ());
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

  // registration :   
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_boxed_model,"geomtools::replicated_boxed_model");
      
} // end of namespace geomtools

// end of replicated_boxed_model.cc
