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

  model_with_internal_items_tools::item_type::item_type ()
  {
    _model_ = 0;
    return;
  }

  void model_with_internal_items_tools::item_type::set_label (const std::string & label_)
  {
    _label_ = label_;
    return;
  }

  const std::string & model_with_internal_items_tools::item_type::get_label () const
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
    DT_THROW_IF (_model_ == 0, std::logic_error, "Missing model !");
    return *_model_;
  }

  void model_with_internal_items_tools::item_type::set_placement (const placement & pl_)
  {
    _placement_ = pl_;
    return;
  }

  const placement & model_with_internal_items_tools::item_type::get_placement () const
  {
    return _placement_;
  }

  const physical_volume & model_with_internal_items_tools::item_type::get_physical_volume () const
  {
    return _phys_;
  }

  physical_volume & model_with_internal_items_tools::item_type::grab_physical_volume ()
  {
    return _phys_;
  }

  /************************************************/

  bool model_with_internal_items_tools::has_item (const std::string & label_) const
  {
    DT_THROW_IF (label_.length () == 0, std::logic_error, "Empty label !");
    return _items_.find (label_) != _items_.end ();
  }

  const model_with_internal_items_tools::item_type &
  model_with_internal_items_tools::get_item (const std::string & label_) const
  {
    item_dict_type::const_iterator found = _items_.find (label_);
    DT_THROW_IF (found == _items_.end (), std::logic_error, "No item with label '" << label_ << "'");
    return found->second;
  }

  model_with_internal_items_tools::item_type &
  model_with_internal_items_tools::grab_item (const std::string & label_)
  {
    item_dict_type::iterator found = _items_.find (label_);
    DT_THROW_IF (found == _items_.end (), std::logic_error, "No item with label '" << label_ << "'");
    return found->second;
  }

  void model_with_internal_items_tools::add_item (const std::string & label_,
                                                  const i_model & model_,
                                                  const placement & placement_)
  {
    DT_THROW_IF (has_item (label_), std::logic_error,
                 "There is already an item with label '" << label_ << "' !");
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

  model_with_internal_items_tools::item_dict_type &
  model_with_internal_items_tools::grab_items ()
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
    std::vector<std::string> labels;
    if (config_.has_key ("internal_item.labels")) {
      config_.fetch ("internal_item.labels", labels);
    }

    if (labels.size () == 0) {
      // no internal item within the 'logical_volume' mother envelope:
      return;
    }

    DT_THROW_IF (! models_, std::logic_error, "Missing dictionary of models !");

    for (std::vector<std::string>::const_iterator i = labels.begin ();
         i != labels.end ();
         i++) {
        const std::string item_label = *i;
        placement item_placement;
        // extract placement:
        {
          std::ostringstream item_placement_oss;
          item_placement_oss << "internal_item.placement." << item_label;
          DT_THROW_IF (! config_.has_key (item_placement_oss.str ()),
                       std::logic_error,
                       "Missing '" << item_placement_oss.str () << "' property !");
          const std::string item_placement_str = config_.fetch_string (item_placement_oss.str ());
          // parse placement:
          bool res_parsing = true;
          try {
              res_parsing = placement::from_string (item_placement_str, item_placement);
            }
          catch (...) {
              res_parsing = false;
            }
          DT_THROW_IF (! res_parsing, std::logic_error, "Item's placement parsing failed !");
        }

        // extract model:
        const i_model * item_model = 0;
        {
          std::ostringstream item_model_key_oss;
          item_model_key_oss << "internal_item.model." << item_label;
          DT_THROW_IF (! config_.has_key (item_model_key_oss.str ()),
                       std::logic_error,
                       "Missing '" << item_model_key_oss.str () << "' property !");
          const std::string item_model_name = config_.fetch_string (item_model_key_oss.str ());
          models_col_type::const_iterator found = models_->find (item_model_name);
          DT_THROW_IF (found == models_->end (),
                       std::logic_error,
                       "Cannot find model with name '" << item_model_name << "' !");
          item_model = found->second;
        }

        add_item (item_label, *item_model, item_placement);
        physical_volume & item_phys = grab_item (item_label).grab_physical_volume ();
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
