// -*- mode: c++ ; -*- 
/* replicated_model.cc
 */

#include <geomtools/replicated_model.h>

#include <exception>
#include <limits> 

#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>
#include <datatools/units.h>

namespace geomtools {

  using namespace std;
  
  const geomtools::box & replicated_model::get_box () const
  {
    return _solid_;
  }
  
  const geomtools::box & replicated_model::get_solid () const
  {
    return _solid_;
  }

  void replicated_model::set_number_of_items (size_t n_)
  {
    assert_unconstructed ("geomtools::replicated_model::set_number_of_items");
    _number_of_items_ = n_;
    return;
  }
  
  size_t replicated_model::get_number_of_items ()
  {
    return _number_of_items_;
  }
  
  double replicated_model::get_step () const
  {
    return _step_;
  }
  
  double replicated_model::get_x () const
  {
    return _x_;
  }
  
  double replicated_model::get_y () const
  {
    return _y_;
  }
  
  double replicated_model::get_z () const
  {
    return _z_;
  }
 
  void replicated_model::set_model (const i_model & model_)
  {
    assert_unconstructed ("geomtools::replicated_model::set_model");

    // check if model has a logical volume with a stackable shape:
    const i_shape_3d & shape = model_.get_logical ().get_shape ();
    // if (! i_shape_3d::is_stackable (model_.get_logical ().get_shape ()))
    //   {
    //     ostringstream message;
    //     message << "geomtools::replicated_model::set_model: "
    //             << "Model '" << model_.get_name () << "' is not stackable ! "
    //     throw logic_error (message.str ());
    //   }
    _model_ = &model_;
    return;
  }

  const i_model & replicated_model::get_model () const
  {
    return *_model_;
  }
 
  string replicated_model::get_model_id () const
  {
    return "geomtools::replicated_model";
  }
  
  replicated_model::replicated_model () : i_boxed_model ()
  {
    _model_ = 0;
    _x_ = numeric_limits<double>::quiet_NaN ();
    _y_ = numeric_limits<double>::quiet_NaN ();
    _z_ = numeric_limits<double>::quiet_NaN ();
    _number_of_items_ = 0;
    _step_ = numeric_limits<double>::quiet_NaN ();
    return;
  }
  
  replicated_model::~replicated_model ()
  {
    return;
  }
  
  void replicated_model::_at_construct (const string & name_,
                                              const datatools::properties & config_,
                                              models_col_type * models_)
  {
    bool devel = i_model::g_devel;
    if (devel) clog << "DEVEL: replicated_model::_at_construct: Entering..." << endl;
    set_name (name_);
    double x;
    double y;
    double z;
    string material_name = material::constants::instance ().MATERIAL_REF_DEFAULT;
    string model_name;
    size_t number_of_items = 0;
    string replicant_axis_label = "";
    string replicated_label = "replicated";
    double length_unit = CLHEP::mm;

    if (config_.has_flag ("replicated.force_stackable"))
      {
        datatools::properties stackable_config;
        config_.export_and_rename_starting_with(stackable_config,
                                                "replicated.force_stackable.",
                                                stackable::STACKABLE_PREFIX);
        if (! _sd_.initialize(stackable_config))
          {
            ostringstream message;
            message << "geomtools::replicated_model::_at_construct: "
                    << "Cannot build the stackable data !";
            throw logic_error (message.str ());
          }
      }  
 
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
        message << "geomtools::replicated_model::_at_construct: "
                << "Missing 'replicated.axis' property !"; 
        throw logic_error (message.str ());           
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
        message << "geomtools::replicated_model::_at_construct: "
                << "Missing 'replicated.number_of_items' property !"; 
        throw logic_error (message.str ());           
      }
     
    if (config_.has_key ("replicated.model"))
      {
        model_name = config_.fetch_string ("replicated.model");
      }  
    else if (config_.has_key ("model")) // Obsolete
      {
        model_name = config_.fetch_string ("model");
      }  
    else
      {
        ostringstream message;
        message << "geomtools::replicated_model::_at_construct: "
                << "Missing 'replicated.model' property !"; 
        throw logic_error (message.str ());   
      }

    if (config_.has_key ("length_unit"))
      {
        string lunit_str = config_.fetch_string ("length_unit");
        length_unit = datatools::units::get_length_unit_from (lunit_str);
      }
     
    if (config_.has_key ("replicated.step"))
      {
        _step_ = config_.fetch_real ("replicated.step");
        if (_step_ <= 0.0 ) {
          ostringstream message;
          message << "geomtools::replicated_model::_at_construct: "
                  << "Invalid value for 'replicated.step' property !"; 
          throw logic_error (message.str ());   
        }
        _step_ *= length_unit;
      }  
 
    if (number_of_items == 0)
      {
        ostringstream message;
        message << "geomtools::replicated_model::_at_construct: "
                << "Number of items is zero !"; 
        throw logic_error (message.str ());   
      }

    set_number_of_items (number_of_items);

    bool axis_ok = false;
    if (replicant_axis_label == "x") axis_ok = true;
    else if (replicant_axis_label == "y") axis_ok = true;
    else if (replicant_axis_label == "z") axis_ok = true;
    if (! axis_ok)
      {
        ostringstream message;
        message << "geomtools::replicated_model::_at_construct: "
                << "Invalid replicant axis !"; 
        throw logic_error (message.str ());   
      }

    if (! models_)
      {
        ostringstream message;
        message << "geomtools::replicated_model::_at_construct: "
                << "Missing logicals dictionary !"; 
        throw logic_error (message.str ());
      }
    
    // Stackable model:
    {
      models_col_type::const_iterator found = 
        models_->find (model_name);
      if (found != models_->end ())
        {
          set_model (dynamic_cast<const i_model &>(*(found->second)));
        }
      else
        {
          ostringstream message;
          message << "geomtools::replicated_model::_at_construct: "
                  << "Cannot find model with name '" 
                  << model_name << "' !";
          throw logic_error (message.str ());
        }
    }

    double dx, dy, dz;
    datatools::invalidate(dx);
    datatools::invalidate(dy);
    datatools::invalidate(dz);
    const i_shape_3d & shape = _model_->get_logical ().get_shape ();
    stackable_data sd;
    i_shape_3d::pickup_stackable (shape, sd);
    dx = sd.get_xmax () - sd.get_xmin ();
    dy = sd.get_ymax () - sd.get_ymin ();
    dz = sd.get_zmax () - sd.get_zmin ();
    if (_sd_.is_valid_weak())
      {
        if (replicant_axis_label == "x") 
          {
            if (_sd_.is_valid_x())
              {
                dx = _sd_.get_xmax () - _sd_.get_xmin ();
              }
          }
        if (replicant_axis_label == "y") 
          {
            if (_sd_.is_valid_y())
              {
                dy = _sd_.get_ymax () - _sd_.get_ymin ();
              }
          }
        if (replicant_axis_label == "z") 
          {
            if (_sd_.is_valid_z())
              {
                dz = _sd_.get_zmax () - _sd_.get_zmin ();
              }
          }
      }
    _x_ = dx;
    _y_ = dy;
    _z_ = dz;
     double x0, y0, z0;
    x0 = y0 = z0 = 0.0;
    if (replicant_axis_label == "x") 
      {
        double step_x = dx;
        if (datatools::is_valid(_step_))
          {
            step_x = _step_;
          }
        _replica_placement_.set_replicant_step_x (step_x);
        _x_ = dx + step_x * (get_number_of_items () - 1);
        x0 = -0.5 * _x_ + 0.5 * dx;
      }
    if (replicant_axis_label == "y") 
      {
        double step_y = dy;
        if (datatools::is_valid(_step_))
          {
            step_y = _step_;
          }
        _replica_placement_.set_replicant_step_y (step_y);
        _y_ =  dy + step_y * (get_number_of_items () - 1);
        y0 = -0.5 * _y_ + 0.5 * dy;
      }
    if (replicant_axis_label == "z") 
      {
        double step_z = dz;
        if (datatools::is_valid(_step_))
          {
            step_z = _step_;
          }
        _replica_placement_.set_replicant_step_z (step_z);
        _z_ =  dz + step_z * (get_number_of_items () - 1);
        z0 = -0.5 * _z_ + 0.5 * dz;
      }
    _replica_placement_.set_number_of_items (get_number_of_items ());

    _solid_.reset ();
    _solid_.set_x (_x_);
    _solid_.set_y (_y_);
    _solid_.set_z (_z_);
    if (! _solid_.is_valid ())
      {
        throw logic_error ("geomtools::replicated_model::_at_construct: Invalid solid !");
      }
    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    if (_model_->get_logical ().has_material_ref ())
      {
        material_name = _model_->get_logical ().get_material_ref ();
      }
    get_logical ().set_material_ref (material_name);

    placement basic_p;
    basic_p.set (x0, y0, z0, 0, 0, 0);
    _replica_placement_.set_basic_placement (basic_p);
    _phys_.set_name (i_model::make_physical_volume_name (replicated_label, get_number_of_items ()));
    _phys_.set_placement (_replica_placement_);
    _phys_.set_logical (_model_->get_logical ());
    _phys_.set_mother (_logical);

    if (devel) clog << "DEVEL: replicated_model::_at_construct: Exiting." << endl;
    return;
  }

  void replicated_model::tree_dump (ostream & out_, 
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
        _replica_placement_.tree_dump (out_, "", indent_oss.str ());
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
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(replicated_model,"geomtools::replicated_model");
      
} // end of namespace geomtools

// end of replicated_model.cc
