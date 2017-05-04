// mctools/mcnp/geometry/universe_store.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mctools/mcnp/geometry/universe_store.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      universe_store::universe_store()
      {
        return;
      }

      universe_store::~universe_store()
      {
        _universes_.clear();
        return;
      }

      int universe_store::_candidate_id()
      {
        int cand_id = universe::INVALID_ID;
        for (int id = universe::MIN_ID;
             id <= universe::MAX_ID;
             id++) {
          if (!has_universe(id)) {
            cand_id = id;
            break;
          }
        }
        return cand_id;
      }

      universe & universe_store::add_universe(int id_)
      {
        int id = id_;
        if (id < universe::MIN_ID) {
          id = _candidate_id();
        }
        DT_THROW_IF(id < 1, std::logic_error,
                    "Invalid universe identifier '" << id << "'!");
        universe_ptr_type ptr(new universe);
        universe & univ = *ptr.get();
        univ.set_id(id);
        // DT_THROW_IF(!univ_.is_valid(), std::logic_error,
        //             "Cannot add invalid universe '" << univ_.get_name() << "'!");
        DT_THROW_IF(has_universe(univ.get_id()),
                    std::logic_error,
                    "Universe store already contains universe with ID=[" << univ.get_id() << "]!");
        _universes_[univ.get_id()] = ptr;
        return univ;
      }

      void universe_store::remove_universe(int id_)
      {
        universe_dict_type::iterator found = _universes_.find(id_);
        if (found != _universes_.end()) {
          _universes_.erase(found);
        }
        return;
      }

      bool universe_store::has_universe(int id_) const
      {
        return _universes_.find(id_) != _universes_.end();
      }

      const universe_ptr_type & universe_store::get(int id_) const
      {
        universe_dict_type::const_iterator found = _universes_.find(id_);
        DT_THROW_IF(found == _universes_.end(), std::logic_error,
                    "No universe with identifier = [" << id_ << "]!");
        return found->second;
      }

      void universe_store::build_ids(std::set<int> & ids_) const
      {
        for (universe_dict_type::const_iterator iuniv = _universes_.begin();
             iuniv != _universes_.end();
             iuniv++) {
          ids_.insert(iuniv->first);
        }
        return;
      }

      uint32_t universe_store::size() const
      {
        return _universes_.size();
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools
