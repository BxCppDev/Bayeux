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
#include "electronics/board_model_base.h"

// This project
#include "electronics/component_types.h"

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(board_model_base, "electronics::board_model_base");

  board_model_base::board_model_base()
  {
    set_type(TYPE_MODULE_BOARD);
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

  uint32_t board_model_base::get_slot_with() const
  {
    return _slot_width_;
  }

  void board_model_base::set_slot_width(uint32_t sw_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board '" << get_name() << "' is already initialized !");
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
                "Board '" << get_name() << "' is already initialized !");
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
                "Board '" << get_name() << "' is already initialized !");
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
                "Board '" << get_name() << "' is already initialized !");
    _mezzanine_format_ = format_;
    return;
  }

  bool board_model_base::has_mezzanine_board(uint32_t mezzanine_slot_) const
  {
    return _mezzanine_boards_.find(mezzanine_slot_) != _mezzanine_boards_.end();
  }

  void board_model_base::remove_mezzanine_board(uint32_t mezzanine_slot_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board '" << get_name() << "' is already locked !");
    DT_THROW_IF(!has_mezzanine_board(mezzanine_slot_),
                std::logic_error,
                "Board '" << get_name() << "' does not have a mezzanine board at slot '"
                << mezzanine_slot_ << "' !");
    _mezzanine_boards_.erase(_mezzanine_boards_.find(mezzanine_slot_));
    {
      std::ostringstream mez_label;
      mez_label << "mezzanine_" << mezzanine_slot_;
      this->remove_embedded(mez_label.str());
    }
    return;
  }

  void board_model_base::add_mezzanine_board(uint32_t mezzanine_slot_,
                                       component_handle_type & mezzanine_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board '" << get_name() << "' is already locked !");
    indexed_component_dict_type::iterator found = _mezzanine_boards_.find(mezzanine_slot_);
    DT_THROW_IF(_max_number_of_mezzanine_boards_ == 0,
                std::logic_error,
                "Board '" << get_name() << "' does not accept mezzanine boards !");
    DT_THROW_IF(mezzanine_slot_ >= _max_number_of_mezzanine_boards_,
                std::logic_error,
                "Board '" << get_name() << "' : Invalid mezzanine slot '" << mezzanine_slot_ << "' !");
    DT_THROW_IF(has_mezzanine_board(mezzanine_slot_),
                std::logic_error,
                "Board '" << get_name() << "' already has a mezzanine board at slot '" << mezzanine_slot_ << "' !");
    DT_THROW_IF(! mezzanine_.has_data(),
                std::logic_error,
                "Board '" << get_name() << "' cannot embed a NULL mezzanine board at slot '" << mezzanine_slot_ << "' !");
    const board_model_base * mez_board = dynamic_cast<const board_model_base *>(&mezzanine_.get());
    DT_THROW_IF(mez_board == 0 || mez_board->get_type() != TYPE_MODULE_MEZZANINE_BOARD,
                std::logic_error,
                "Board '" << get_name() << "' : Attempt to embed a non mezzanine component '"
                <<  mezzanine_.get().get_name()
                << "' at slot '" << mezzanine_slot_ << "' !");

    if (has_mezzanine_format() && mez_board->has_format()) {
      DT_THROW_IF(_mezzanine_format_ != mez_board->get_format(),
                  std::logic_error,
                  "Board '" << get_name() << "' :  Attempt to embed a mezzanine board with incompatible format ('"
                  << mez_board->get_format()
                  << "') at slot '" << mezzanine_slot_ << "' !");
    }
    // Finally, insert the mezzanine board:
    {
      std::ostringstream mez_label;
      mez_label << "mezzanine_" << mezzanine_slot_;
      // ... as a embedded component:
      this->add_embedded(mez_label.str(), mezzanine_);
    }
    // ... as an explicit mezzanine at its slot:
    _mezzanine_boards_[mezzanine_slot_] = mezzanine_;
    return;
  }

  component_handle_type & board_model_base::grab_mezzanine_board(uint32_t mezzanine_slot_)
  {
    indexed_component_dict_type::iterator found = _mezzanine_boards_.find(mezzanine_slot_);
    DT_THROW_IF(found == _mezzanine_boards_.end(),
                std::logic_error,
                "Board '" << get_name() << "' has no embedded mezzanine board at slot '" << mezzanine_slot_ << "' !");
    return found->second;
  }

  const component_handle_type & board_model_base::get_mezzanine_board(uint32_t mezzanine_slot_) const
  {
    indexed_component_dict_type::const_iterator found = _mezzanine_boards_.find(mezzanine_slot_);
    DT_THROW_IF(found == _mezzanine_boards_.end(),
                std::logic_error,
                "Board '" << get_name() << "' has no embedded mezzanine board at slot '"
                << mezzanine_slot_ << "' !");
    return found->second;
  }

  void board_model_base::_board_reset()
  {
    _mezzanine_boards_.clear();
    _slot_width_ = 0;
    _max_number_of_mezzanine_boards_ = 0;
    _mezzanine_format_.clear();
    _format_.clear();
    this->_component_reset();
    return;
  }

  void board_model_base::_board_initialize(const datatools::properties & config_,
                                     component_pool_type& components_)
  {
    this->_component_initialize(config_, components_);

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
        std::ostringstream mb_slot_key;
        mb_slot_key << "board.mezzanine_boards.slot_" << islot;
        if (config_.has_key(mb_slot_key.str())) {
          std::string mezzanine_name = config_.fetch_string(mb_slot_key.str());
          component_handle_type & mezzanine_handle
            = components_.find(mezzanine_name)->second.grab_component_handle();
          add_mezzanine_board(islot, mezzanine_handle);
        }
      }
    }
    return;
  }

  void board_model_base::initialize(const datatools::properties& config_,
                              component_pool_type& components_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Board '" << get_name() << "' is already initialized !");

    this->_board_initialize(config_, components_);

    _set_initialized(true);
    return;
  }

  void board_model_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Board '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    this->_board_reset();
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
         << "Slot width : '" << _slot_width_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherits_)
         << "Mezzanine boards : ";
    if (_max_number_of_mezzanine_boards_ == 0) {
      out_ << "<none>";
    }
    out_ << std::endl;

    if (_max_number_of_mezzanine_boards_) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherits_)
           << i_tree_dumpable::tag
           << "Max number of mezzanine boards : '" << _max_number_of_mezzanine_boards_ << "'" << std::endl;
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherits_)
           << i_tree_dumpable::tag
           << "Mezzanine format : '" << _mezzanine_format_ << "'" << std::endl;
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherits_)
           << i_tree_dumpable::last_tag
           << "Mezzanine boards : " << _mezzanine_boards_.size() << std::endl;
      for (int islot = 0; islot < _max_number_of_mezzanine_boards_; islot++) {
        out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherits_)
             << i_tree_dumpable::last_skip_tag;
        if (islot == _max_number_of_mezzanine_boards_ - 1) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        out_ << "Slot #" << islot << " : ";
        if (has_mezzanine_board(islot)) {
          out_ << "'" << get_mezzanine_board(islot).get().get_name() << "'";
        } else {
          out_ << "<empty>";
        }
        out_ << std::endl;
      }
    }

    return;
  }

} // end of namespace electronics

// end of board_model_base.cc
