// mctools/mcnp/geometry/surface_store.cc
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
#include <mctools/mcnp/geometry/surface_store.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <mctools/mcnp/format.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      surface_store::surface_store()
      {
        return;
      }

      surface_store::~surface_store()
      {
        _surfaces_.clear();
        return;
      }

      int surface_store::_candidate_id()
      {
        int cand_id = surface::INVALID_ID;
        for (int id = surface::MIN_ID;
             id <= surface::MAX_ID;
             id++) {
          if (!has_surface(id)) {
            cand_id = id;
            break;
          }
        }
        return cand_id;
      }

      surface & surface_store::add_surface(int id_)
      {
        int id = id_;
        if (id < surface::MIN_ID) {
          id = _candidate_id();
        }
        DT_THROW_IF(id < 1, std::logic_error,
                    "Invalid surface identifier '" << id << "'!");
        surface_ptr_type ptr(new surface);
        surface & surf = *ptr.get();
        surf.set_id(id);
        // DT_THROW_IF(!surf_.is_valid(), std::logic_error,
        //             "Cannot add invalid surface '" << surf_.get_name() << "'!");
        DT_THROW_IF(has_surface(surf.get_id()),
                    std::logic_error,
                    "Surface store already contains surface with ID=[" << surf.get_id() << "]!");
        _surfaces_[surf.get_id()] = ptr;
        return surf;
      }

      void surface_store::remove_surface(int id_)
      {
        surface_dict_type::iterator found = _surfaces_.find(id_);
        if (found != _surfaces_.end()) {
          _surfaces_.erase(found);
        }
        return;
      }

      bool surface_store::has_surface(int id_) const
      {
        return _surfaces_.find(id_) != _surfaces_.end();
      }

      const surface_ptr_type & surface_store::get(int id_) const
      {
        surface_dict_type::const_iterator found = _surfaces_.find(id_);
        DT_THROW_IF(found == _surfaces_.end(), std::logic_error,
                    "No surface with identifier = [" << id_ << "]!");
        return found->second;
      }

      void surface_store::build_ids(std::set<int> & ids_) const
      {
        for (surface_dict_type::const_iterator isurf = _surfaces_.begin();
             isurf != _surfaces_.end();
             isurf++) {
          ids_.insert(isurf->first);
        }
        return;
      }

      uint32_t surface_store::size() const
      {
        return _surfaces_.size();
      }

      void surface_store::print_card(std::ostream & out_, uint32_t flags_) const
      {
        if (flags_ & format::CARD_COMMENT) {
          format::add_comment_line(out_, "List of surfaces:");
        }
        for (surface_dict_type::const_iterator isurf = _surfaces_.begin();
             isurf != _surfaces_.end();
             isurf++) {
          const surface & surf = *isurf->second.get();
          surf.print_card(out_, flags_);
        }
        return;
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools
