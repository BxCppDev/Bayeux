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

#include <datatools/utils/units.h>

#include <geomtools/logical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {

  using namespace std;  

  model_with_internal_items_tools::item::item ()
  {
    __model = 0;
  }

  void model_with_internal_items_tools::item::set_label (const string & label_)
  {
    __label = label_;
  }

  const string & model_with_internal_items_tools::item::get_label () const
  {
    return __label;
  }

  void model_with_internal_items_tools::item::set_model (const i_model & model_)
  {
    __model = &model_;
  }

  const i_model & model_with_internal_items_tools::item::get_model () const
  {
    if (__model == 0)
      {
	ostringstream message;
	message << "model_with_internal_items_tools::item::get_model: "
		<< "Missing model !";
	throw runtime_error (message.str ());
      }
    return *__model;
  }

  void model_with_internal_items_tools::item::set_placement (const placement & pl_)
  {
    __placement = pl_;
  }

  const placement & 
  model_with_internal_items_tools::item::get_placement () const
  {
    return __placement;
  }

  const physical_volume & 
  model_with_internal_items_tools::item::get_physical_volume () const
  {
    return __phys;
  }

  physical_volume & 
  model_with_internal_items_tools::item::get_physical_volume ()
  {
    return __phys;
  }

  /************************************************/
    
  bool model_with_internal_items_tools::has_item (const string & label_) const
  {
    return __items.find (label_) != __items.end ();      
  }
    
  const model_with_internal_items_tools::item & 
  model_with_internal_items_tools::get_item (const string & label_) const
  {
    item_dict_t::const_iterator found = __items.find (label_);
    if (found == __items.end ())
      {
	ostringstream message;
	message << "model_with_internal_items_tools::get_item: "
		<< "No item with label '" << label_ << "'"
		<< endl;
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  model_with_internal_items_tools::item & 
  model_with_internal_items_tools::get_item (const string & label_)
  {
    item_dict_t::iterator found = __items.find (label_);
    if (found == __items.end ())
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
    if (has_item (label_))
      {
	ostringstream message;
	message << "model_with_internal_items_tools::add_item: "
		<< "There is already an item with label '" << label_ << "' !"
		<< endl;
	throw runtime_error (message.str ());
      }
      
    __items[label_] = item ();
    item & the_item = __items[label_];
    the_item.set_label (label_);
    the_item.set_model (model_);
    the_item.set_placement (placement_);
    return;
  }

  size_t model_with_internal_items_tools::get_number_of_items () const
  {
    return __items.size ();
  }

  const model_with_internal_items_tools::item_dict_t & 
  model_with_internal_items_tools::get_items () const
  {
    return __items;
  }

  model_with_internal_items_tools::item_dict_t & model_with_internal_items_tools::get_items ()
  {
    return __items;
  }
 
  // ctor:
  model_with_internal_items_tools::model_with_internal_items_tools ()
  {
  }
  
  // dtor:
  model_with_internal_items_tools::~model_with_internal_items_tools ()
  {
    __items.clear ();
  }

  void model_with_internal_items_tools::plug_internal_models (const datatools::utils::properties & config_,
							      logical_volume & log_,
							      models_col_t * models_)
  {
    bool devel = false;
    double lunit = CLHEP::mm;
    vector<string> labels;
 
    if (config_.has_key ("length_unit"))
      {
	string length_unit_str = config_.fetch_string ("length_unit");
	lunit = datatools::utils::units::get_length_unit_from (length_unit_str);
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
		<< "Missing logicals dictionary !"; 
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
	      throw runtime_error (message.str ());
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
	      throw runtime_error (message.str ());
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
	      throw runtime_error (message.str ());
	    }
	  {
	    models_col_t::const_iterator found = 
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
		throw runtime_error (message.str ());
	      }
	  }
	}

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
