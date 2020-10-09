// signal_data.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-28
 * Last modified : 2016-09-28
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <mctools/signal/signal_data.h>

namespace mctools {

  namespace signal {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (signal_data,"mctools::signal:signal_data")

    signal_data::signal_data()
    {
      return;
    }

    signal_data::~signal_data()
    {
      return;
    }

    void signal_data::set_auxiliaries(const datatools::properties & p)
    {
      _auxiliaries_ = p;
    }

    const datatools::properties & signal_data::get_auxiliaries () const
    {
      return _auxiliaries_;
    }

    datatools::properties & signal_data::grab_auxiliaries ()
    {
      return _auxiliaries_;
    }

    bool signal_data::has_signals(const std::string & a_category) const
    {
      return _signals_dict_.find(a_category) != _signals_dict_.end();
    }

    void signal_data::add_signals(const std::string & a_category, size_t a_capacity)
    {
      DT_THROW_IF (a_category.empty(),
                   std::logic_error,
                   "Invalid empty name for a collection of signals !");
      if (has_signals(a_category)) {
        return;
      }
      signal_handle_collection_type dummy;
      _signals_dict_[a_category] = dummy;
      if (a_capacity > 0) {
        _signals_dict_[a_category].reserve(a_capacity);
      }
      return;
    }

    void signal_data::remove_signals(const std::string & a_category)
    {
      signals_dict_type::iterator found = _signals_dict_.find(a_category);
      if (found != _signals_dict_.end()) {
        _signals_dict_.erase(found);
      }
      return;
    }

    size_t signal_data::get_number_of_signals(const std::string & a_category) const
    {
      size_t nbr_signals = 0;
      signals_dict_type::const_iterator found = _signals_dict_.find(a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of signals with category '" << a_category << "' !");
      nbr_signals = found->second.size ();
      return nbr_signals;
    }

    base_signal &
    signal_data::add_signal(const std::string & a_category)
    {
      base_signal * bsh = 0;
      if (!has_signals(a_category)) {
        add_signals(a_category, 4);
      }
      signals_dict_type::iterator found = _signals_dict_.find(a_category);
      // DT_THROW_IF(found == _signals_dict_.end(),
      //             std::logic_error,
      //             "No collection of signals with category '" << a_category << "' !");
      found->second.push_back(signal_handle_type(new base_signal));
      bsh = &found->second.back().grab();
      bsh->set_category(a_category);
      return *bsh;
    }

    void signal_data::remove_signal(const std::string & a_category, int a_hit_index)
    {
      signals_dict_type::iterator found
        = _signals_dict_.find(a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of handles of signals with category '" << a_category << "' !");
      signal_handle_collection_type & sig_col = found->second;
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= (int)sig_col.size (),
                   std::logic_error,
                   "Invalid signal index in category '" << a_category << "' !");
      sig_col.erase(sig_col.begin() + a_hit_index);
      return;
    }

    const base_signal &
    signal_data::get_signal(const std::string & a_category, int a_hit_index) const
    {
      const base_signal * bsh = 0;
      signals_dict_type::const_iterator found
        = _signals_dict_.find(a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of handles of signals with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= (int)found->second.size (),
                   std::logic_error,
                   "Invalid signal index in category '" << a_category << "' !");
      DT_THROW_IF (! found->second[a_hit_index].has_data (),
                   std::logic_error,
                   "Null signal handle at index " << a_hit_index << " in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index].get ();
      return *bsh;
    }

    base_signal &
    signal_data::grab_signal(const std::string & a_category, int a_hit_index)
    {
      base_signal * bsh = 0;
      signals_dict_type::iterator found
        = _signals_dict_.find (a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of handles of signals with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= (int)found->second.size (),
                   std::logic_error,
                   "Invalid signal index in category '" << a_category << "' !");
      DT_THROW_IF (! found->second[a_hit_index].has_data (),
                   std::logic_error,
                   "Null signal handle at index " << a_hit_index << " in category '" << a_category << "' !");
      bsh = &found->second[a_hit_index].grab ();
      return *bsh;
    }

    signal_data::signal_handle_collection_type &
    signal_data::grab_signals(const std::string & a_category)
    {
      signals_dict_type::iterator found = _signals_dict_.find (a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of signals with category '" << a_category << "' !");
      return found->second;
    }

    const signal_data::signal_handle_collection_type &
    signal_data::get_signals(const std::string & a_category) const
    {
      signals_dict_type::const_iterator found = _signals_dict_.find (a_category);
      DT_THROW_IF (found == _signals_dict_.end (),
                   std::logic_error,
                   "No collection of signals with category '" << a_category << "' !");
      return found->second;
    }

    void signal_data::build_list_of_categories(std::vector<std::string> & cats_) const
    {
      cats_.clear();
      for (const auto& cat_entry : _signals_dict_) {
        cats_.push_back(cat_entry.first);
      }
      return;
    }

    void signal_data::clear()
    {
      _signals_dict_.clear();
      _auxiliaries_.clear();
      return;
    }

    void signal_data::reset()
    {
      clear();
      return;
    }

    void signal_data::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Signal categories : " << '[' << _signals_dict_.size() << ']' << std::endl;
        std::size_t cat_counter = 0;
        for (const auto& cat_entry : _signals_dict_) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          if (++cat_counter != _signals_dict_.size()) {
            out_ << datatools::i_tree_dumpable::tag;
          } else {
            out_ << datatools::i_tree_dumpable::last_tag;
          }
          out_ << "Category '" << cat_entry.first << "' : ";
          out_ << '[' << cat_entry.second.size() << ']';
          out_ << std::endl;
        }
      }

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Auxiliaries : " << '[' << _auxiliaries_.size() << ']' << std::endl;
      if (_auxiliaries_.size() ){
        std::ostringstream indent2;
        indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        _auxiliaries_.tree_dump(out_, "", indent2.str());
      }
      return;
    }

  } // end of namespace signal

} // end of namespace mctools
