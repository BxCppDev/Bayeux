// -*- mode: c++ ; -*- 
/* model_with_internal_items_tools.cc
 */

#include <geomtools/model_with_internal_items_tools.h>

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <datatools/units.h>

#include <geomtools/logical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {

  using namespace std;  

  model_with_internal_items_tools::item_type::item_type ()
  {
    _model_ = 0;
    return;
  }

  void model_with_internal_items_tools::item_type::set_label (const string & label_)
  {
    _label_ = label_;
    return;
  }

  const string & model_with_internal_items_tools::item_type::get_label () const
  {
    return _label_;
  }

  void model_with_internal_items_tools::item_type::set_model (const i_model & model_)
  {
    _model_ = &model_;
    return;
  }

  const i_model & model_with_internal_items_tools::item_type::get_model () const
  {
    if (_model_ == 0)
      {
        ostringstream message;
        message << "model_with_internal_items_tools::item_type::get_model: "
                << "Missing model !";
        throw runtime_error (message.str ());
      }
    return *_model_;
  }

  void model_with_internal_items_tools::item_type::set_placement (const placement & pl_)
  {
    _placement_ = pl_;
    return;
  }

  const placement & 
  model_with_internal_items_tools::item_type::get_placement () const
  {
    return _placement_;
  }

  const physical_volume & 
  model_with_internal_items_tools::item_type::get_physical_volume () const
  {
    return _phys_;
  }

  physical_volume & 
  model_with_internal_items_tools::item_type::get_physical_volume ()
  {
    return _phys_;
  }

  /************************************************/
    
  bool model_with_internal_items_tools::has_item (const string & label_) const
  {
    if (label_.length () == 0)
      {
        ostringstream message;
        message << "model_with_internal_items_tools::has_item: "
                << "Empty label !"
                << endl;
        throw logic_error (message.str ());
      }
    return _items_.find (label_) != _items_.end ();      
  }
    
  const model_with_internal_items_tools::item_type & 
  model_with_internal_items_tools::get_item (const string & label_) const
  {
    item_dict_type::const_iterator found = _items_.find (label_);
    if (found == _items_.end ())
      {
        ostringstream message;
        message << "model_with_internal_items_tools::get_item: "
                << "No item with label '" << label_ << "'"
                << endl;
        throw logic_error (message.str ());
      }
    return found->second;
  }

  model_with_internal_items_tools::item_type & 
  model_with_internal_items_tools::get_item (const string & label_)
  {
    item_dict_type::iterator found = _items_.find (label_);
    if (found == _items_.end ())
      {
        ostringstream message;
        message << "model_with_internal_items_tools::get_item: "
                << "No item with label '" << label_ << "' !"
                << endl;
        throw runtime_error (message.str ());
      }
    return found->second;
  }

  void model_with_internal_items_tools::add_item (const string & label_, 
                                                  const i_model & model_,
                                                  const placement & placement_)
  {
    /*
    for (item_dict_t::const_iterator i = _items_.begin ();
         i != _items_.end ();
         i++)
      {
        clog << "DEVEL: model_with_internal_items_tools::add_item: "
             << "Label = '" << i->first << "'"
             << endl;
      }
    */   
    if (has_item (label_))
      {
        ostringstream message;
        message << "model_with_internal_items_tools::add_item: "
                << "There is already an item with label '" << label_ << "' !"
                << endl;
        throw runtime_error (message.str ());
      }
      
    {
      item_type dummy;
      _items_[label_] = dummy;
    }
    item_type & the_item = _items_[label_];
    the_item.set_label (label_);
    the_item.set_model (model_);
    the_item.set_placement (placement_);
    return;
  }

  size_t model_with_internal_items_tools::get_number_of_items () const
  {
    return _items_.size ();
  }

  const model_with_internal_items_tools::item_dict_type & 
  model_with_internal_items_tools::get_items () const
  {
    return _items_;
  }

  model_with_internal_items_tools::item_dict_type & model_with_internal_items_tools::get_items ()
  {
    return _items_;
  }
 
  // ctor:
  model_with_internal_items_tools::model_with_internal_items_tools ()
  {
    return;
  }
  
  // dtor:
  model_with_internal_items_tools::~model_with_internal_items_tools ()
  {
    _items_.clear ();
    return;
  }

  void model_with_internal_items_tools::plug_internal_models (const datatools::properties & config_,
                                                              logical_volume & log_,
                                                              models_col_type * models_)
  {
    bool devel = false;
    double lunit = CLHEP::mm;
    vector<string> labels;
 
    if (config_.has_key ("length_unit"))
      {
        string length_unit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (length_unit_str);
      }

    if (config_.has_key ("internal_item.labels"))
      {
        config_.fetch ("internal_item.labels", labels);
      }

    if (labels.size () == 0)
      {
        // no internal item within the 'logical_volume' mother envelope:
        return;
      }

    if (! models_)
      {
        ostringstream message;
        message << "model_with_internal_items_tools::_at_construct: "
                << "Missing dictionary of models !"; 
        throw runtime_error (message.str ());
      }

    for (vector<string>::const_iterator i = labels.begin ();
         i != labels.end ();
         i++)
      {
        string item_label = *i;
        string item_model_name;
        const i_model * item_model = 0;
        placement item_placement;
        // extract placement:
        {
          string item_placement_str;
          ostringstream item_placement_oss;
          item_placement_oss << "internal_item.placement." << item_label;
          if (config_.has_key (item_placement_oss.str ()))
            {
              item_placement_str = config_.fetch_string (item_placement_oss.str ());
            }
          else
            {
              ostringstream message;
              message << "model_with_internal_items_tools::_at_construct: "
                      << "Missing '" << item_placement_oss.str () << "' property !"; 
              throw logic_error (message.str ());
            }
          // parse placement:
          bool res_parsing = true;
          try
            {
              res_parsing = placement::from_string (item_placement_str, item_placement);
            }
          catch (...)
            {
              res_parsing = false;
            }
          if (! res_parsing)
            {
              ostringstream message;
              message << "model_with_internal_items_tools::_at_construct: "
                      << "Item's placement parsing failed !"; 
              throw logic_error (message.str ());
            }
        }

        // extract model:
        {
          ostringstream item_model_key_oss;
          item_model_key_oss << "internal_item.model." << item_label;
          if (config_.has_key (item_model_key_oss.str ()))
            {
              item_model_name = config_.fetch_string (item_model_key_oss.str ());
            }
          else
            {
              ostringstream message;
              message << "model_with_internal_items_tools::_at_construct: "
                      << "Missing '" << item_model_key_oss.str () << "' property !"; 
              throw logic_error (message.str ());
            }
          {
            models_col_type::const_iterator found = 
              models_->find (item_model_name);
            if (found != models_->end ())
              {
                item_model = found->second;
              }
            else
              {
                ostringstream message;
                message << "model_with_internal_items_tools::_at_construct: "
                        << "Cannot find model with name '" 
                        << item_model_name << "' !";
                throw logic_error (message.str ());
              }
          }
        }

        //clog << "DEVEL: adding internal item : " << item_label << endl;
        add_item (item_label, *item_model, item_placement);
        physical_volume & item_phys = get_item (item_label).get_physical_volume ();
        const placement & item_plcmt = get_item (item_label).get_placement ();
        item_phys.set_name (i_model::make_physical_volume_name (item_label));
        item_phys.set_placement (item_plcmt);
        item_phys.set_logical (item_model->get_logical ());
        item_phys.set_mother (log_);
      }

    return;
  }

} // end of namespace geomtools

// end of model_with_internal_items_tools.cc
