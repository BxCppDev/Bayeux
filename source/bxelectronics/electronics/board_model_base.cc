// board_model_base.cc
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/electronics.
//
// Bayeux/electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/electronics. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <electronics/board_model_base.h>

// This project
#include <electronics/component_types.h>
#include <electronics/physical_component.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(board_model_base, "electronics::board_model_base");

  board_model_base::board_model_base()
  {
    set_type(TYPE_MODULE_BOARD);
    set_allow_embedded_components(true);
    _slot_width_ = 0;
    _max_number_of_mezzanine_boards_ = 0;
    return;
  }

  board_model_base::~board_model_base()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  uint32_t board_model_base::get_slot_width() const
  {
    return _slot_width_;
  }

  void board_model_base::set_slot_width(uint32_t sw_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is already initialized !");
    _slot_width_ = sw_;
    return;
  }

  uint32_t board_model_base::get_max_number_of_mezzanine_boards() const
  {
    return _max_number_of_mezzanine_boards_;
  }

  void board_model_base::set_max_number_of_mezzanine_boards(uint32_t n_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(_mezzanine_labels_.size() > 0,
                std::logic_error,
                "Board model '" << get_name() << "' already has mezzanine boards !");
    _max_number_of_mezzanine_boards_ = n_;
    return;
  }

  bool board_model_base::has_format() const
  {
    return ! _format_.empty();
  }

  const std::string & board_model_base::get_format() const
  {
    return _format_;
  }

  void board_model_base::set_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is already initialized !");
    _format_ = format_;
    return;
  }

  bool board_model_base::has_mezzanine_format() const
  {
    return ! _mezzanine_format_.empty();
  }

  const std::string & board_model_base::get_mezzanine_format() const
  {
    return _mezzanine_format_;
  }

  void board_model_base::set_mezzanine_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is already initialized !");
    _mezzanine_format_ = format_;
    return;
  }

  bool board_model_base::has_mezzanine_board(uint32_t mezzanine_slot_) const
  {
    return _mezzanine_labels_.find(mezzanine_slot_) != _mezzanine_labels_.end();
  }

  void board_model_base::remove_mezzanine_board(uint32_t mezzanine_slot_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is initialized !");
    DT_THROW_IF(!has_mezzanine_board(mezzanine_slot_),
                std::logic_error,
                "Board model '" << get_name() << "' does not have a mezzanine board at slot '"
                << mezzanine_slot_ << "' !");
    {
      std::string mezzanine_label = _mezzanine_labels_[mezzanine_slot_];
      // Explicit unregistration of the mezzanine label at its slot:
      _mezzanine_labels_.erase(_mezzanine_labels_.find(mezzanine_slot_));
      this->_remove_embedded_physical(mezzanine_label);
    }
    return;
  }

  physical_component &
  board_model_base::add_mezzanine_board(uint32_t mezzanine_slot_,
                                        const component_model_base & mezzanine_model_,
                                        const std::string & mezzanine_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board model '" << get_name() << "' is already initialized !");
    indexed_labels_dict_type::iterator found = _mezzanine_labels_.find(mezzanine_slot_);
    DT_THROW_IF(_max_number_of_mezzanine_boards_ == 0,
                std::logic_error,
                "Board model '" << get_name() << "' does not accept mezzanine boards !");
    DT_THROW_IF(mezzanine_slot_ >= _max_number_of_mezzanine_boards_,
                std::logic_error,
                "Board model '" << get_name() << "' : Invalid mezzanine slot '" << mezzanine_slot_ << "' !");
    DT_THROW_IF(has_mezzanine_board(mezzanine_slot_),
                std::logic_error,
                "Board model '" << get_name() << "' already has a mezzanine board at slot '" << mezzanine_slot_ << "' !");
    const board_model_base * mez_board = dynamic_cast<const board_model_base *>(&mezzanine_model_);
    DT_THROW_IF(mez_board == 0 || mezzanine_model_.get_type() != TYPE_MODULE_MEZZANINE_BOARD,
                std::logic_error,
                "Board model '" << get_name() << "' : Attempt to embed a non mezzanine component '"
                << mezzanine_model_.get_name()
                << "' at slot '" << mezzanine_slot_ << "' !");

    if (has_mezzanine_format() && mez_board->has_format()) {
      DT_THROW_IF(_mezzanine_format_ != mez_board->get_format(),
                  std::logic_error,
                  "Board model '" << get_name() << "' :  Attempt to embed a mezzanine board with incompatible format ('"
                  << mez_board->get_format()
                  << "') at slot '" << mezzanine_slot_ << "' !");
    }
    // Finally, insert the mezzanine as a embedded component:
    // Compute the mezzanine label:
    std::string mezzanine_label = mezzanine_label_;
    if (mezzanine_label.empty()) {
      if (_mezzanine_labels_.find(mezzanine_slot_) != _mezzanine_labels_.end()) {
        mezzanine_label = _mezzanine_labels_[mezzanine_slot_];
      } else {
        // Automated mezzanine label:
        std::ostringstream mod_label;
        mod_label << "mezzanine_" << mezzanine_slot_;
        mezzanine_label = mod_label.str();
      }
    }
    // Explicit registration of the mezzanine label at its slot:
    _mezzanine_labels_[mezzanine_slot_] = mezzanine_label;
    return this->_add_embedded_physical(mezzanine_label, mezzanine_model_.get_logical());
  }

  const physical_component &
  board_model_base::get_mezzanine_board(uint32_t mezzanine_slot_)
  {
    indexed_labels_dict_type::iterator found = _mezzanine_labels_.find(mezzanine_slot_);
    DT_THROW_IF(found == _mezzanine_labels_.end(),
                std::logic_error,
                "Board model '" << get_name() << "' has no embedded mezzanine board at slot '" << mezzanine_slot_ << "' !");
    return get_embedded_component(found->second);
  }

  void board_model_base::_at_initialize(const datatools::properties& config_,
                                        component_model_pool_type& components_)
  {
    this->_board_initialize(config_, components_);
    return;
  }

  void board_model_base::_at_reset()
  {
    this->_board_reset();
    return;
  }

  void board_model_base::_board_reset()
  {
    _mezzanine_labels_.clear();
    _slot_width_ = 0;
    _max_number_of_mezzanine_boards_ = 0;
    _mezzanine_format_.clear();
    _format_.clear();
    this->_component_reset();
    return;
  }

  void board_model_base::_board_initialize(const datatools::properties & config_,
                                           component_model_pool_type& component_models_)
  {
    this->_component_initialize(config_, component_models_);

    if (! has_format()) {
      if (config_.has_key("board.format")) {
        std::string bf = config_.fetch_string("board.format");
        set_format(bf);
      }
    }

    if (! has_mezzanine_format()) {
      if (config_.has_key("board.mezzanine_format")) {
        std::string mf = config_.fetch_string("board.mezzanine_format");
        set_mezzanine_format(mf);
      }
    }

    if (! _slot_width_) {
      if (config_.has_key("board.slot_width")) {
        int32_t sw = config_.fetch_integer("board.slot_width");
        DT_THROW_IF ((sw < 1 || sw > 64), std::domain_error,
                     "Invalid slot width '" << sw << "' !");
        uint32_t usw = sw;
        set_slot_width(usw);
      }
    }

    if (_max_number_of_mezzanine_boards_ == 0) {
      if (config_.has_key("board.max_number_of_mezzanine_boards")) {
        int32_t sw = config_.fetch_integer("board.slot_width");
        DT_THROW_IF ((sw < 1 || sw > 64), std::domain_error,
                     "Invalid slot width '" << sw << "' !");
        uint32_t usw = sw;
        set_slot_width(usw);
      }
    }

    if (_max_number_of_mezzanine_boards_ > 0) {
      for (int islot = 0; islot < _max_number_of_mezzanine_boards_; islot++) {
       // Attempt to find a mezzanine model at the visited slot:
        std::ostringstream mezzanine_slot_model_key;
        mezzanine_slot_model_key << "board.mezzanine_boards.slot_" << islot << ".model" ;
        if (config_.has_key(mezzanine_slot_model_key.str())) {
          std::string mezzanine_model = config_.fetch_string(mezzanine_slot_model_key.str());
          component_model_pool_type::iterator found = component_models_.find(mezzanine_model);
          DT_THROW_IF(found == component_models_.end(),
                      std::logic_error,
                      "Cannot find a component model named '" << mezzanine_model << "' !");
          component_model_handle_type & mezzanine_model_handle
            = found->second.grab_initialized_component_model_handle();
          std::ostringstream mezzanine_slot_label_key;
          mezzanine_slot_label_key << "board.mezzanine_boards.slot_" << islot << ".label";
          std::string mezzanine_label;
          // Attempt to find a mezzanine label:
          if (config_.has_key(mezzanine_slot_label_key.str())) {
            mezzanine_label = config_.fetch_string(mezzanine_slot_label_key.str());
          }
          add_mezzanine_board(islot, mezzanine_model_handle.get(), mezzanine_label);
        }
      }
      /*
      for (int islot = 0; islot < _max_number_of_mezzanine_boards_; islot++) {
        std::ostringstream mb_slot_key;
        mb_slot_key << "board.mezzanine_boards.slot_" << islot;
        if (config_.has_key(mb_slot_key.str())) {
          std::string mezzanine_model_name = config_.fetch_string(mb_slot_key.str());
          component_model_handle_type & mezzanine_handle
            = component_models_.find(mezzanine_model_name)->second.grab_initialized_component_model_handle();
          this->add_mezzanine_board(islot, mezzanine_handle.get());
        }
      }
      */
    }

    return;
  }

  /// Post remove
  void board_model_base::_post_remove_embedded_physical(const std::string & embedded_label_)
  {
    // Explicitely unregistration of a mezzanine board:
    indexed_labels_dict_type::iterator found
      = std::find_if(_mezzanine_labels_.begin(),
                     _mezzanine_labels_.end(),
                     std::bind2nd(map_data_compare<indexed_labels_dict_type>(),
                                  embedded_label_));
    if (found != _mezzanine_labels_.end()) {
      // If the removed embedded component is one of the registered mezzanine boards, it is unregistered:
      DT_LOG_TRACE(get_logging_priority(),
                   "Unregistering mezzanine board at slot='" << found->first << "'");
      _mezzanine_labels_.erase(found);
    }
    return;
  }

  void board_model_base::tree_dump(std::ostream& out_,
                             const std::string& title_,
                             const std::string& indent_,
                             bool inherits_) const
  {
    this->component_model_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Slot width : " << _slot_width_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherits_)
         << "Mezzanine boards : ";
    if (_max_number_of_mezzanine_boards_ == 0) {
      out_ << "<none>";
    }
    out_ << std::endl;
    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherits_)
         << i_tree_dumpable::tag
         << "Number of slots : "
         << _max_number_of_mezzanine_boards_ << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherits_)
         << i_tree_dumpable::tag
         << "Format : '" << _mezzanine_format_ << "'" << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherits_)
         << i_tree_dumpable::last_tag
         << "Mezzanine boards : ";
    if (_mezzanine_labels_.size() > 0) {
      out_ << _mezzanine_labels_.size();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    for (int islot = 0; islot < _max_number_of_mezzanine_boards_; islot++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherits_);
      out_ << i_tree_dumpable::last_skip_tag;
      if (islot == _max_number_of_mezzanine_boards_ - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Slot #" << islot << " : ";
      indexed_labels_dict_type::const_iterator found_at_slot = _mezzanine_labels_.find(islot);
      if (found_at_slot != _mezzanine_labels_.end()) {
        out_ << "Model '" << get_embedded_component(found_at_slot->second).get_logical().get_model().get_name() << "'";
      } else {
        out_ << "<empty>";
      }
      out_ << std::endl;
    }

    return;
  }

} // end of namespace electronics

// end of board_model_base.cc
