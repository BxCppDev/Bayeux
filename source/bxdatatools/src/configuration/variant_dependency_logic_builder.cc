// datatools/configuration/variant_dependency_logic_builder.cc
/*
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
#include <datatools/configuration/variant_dependency_logic_builder.h>

// Third party:
#include <boost/algorithm/string.hpp>

// This project (Bayeux/datatools):
#include <datatools/exception.h>
#include <datatools/configuration/variant_dependency.h>

namespace datatools {

  namespace configuration {

    variant_dependency_logic_builder::variant_dependency_logic_builder(const parsing::dependency_logic_ast & ast_,
                                                                       variant_dependency & dependency_)
      : _ast_(ast_)
      , _dependency_(dependency_)
    {
      return;
    }

    dependency_logic_ptr_type variant_dependency_logic_builder::build()
    {
      return _build_node(_ast_.top);
    }

    dependency_logic_ptr_type
    variant_dependency_logic_builder::_build_node(const parsing::dependency_logic_ast_node & node_)
    {
      dependency_logic_ptr_type log_ptr;
      // Visit the boost::variant AST node:
      if (const parsing::dependency_logic_ast_slot * slot_node
          = boost::get<parsing::dependency_logic_ast_slot>(&node_)) {
        // A leaf node:
        log_ptr.reset(new slot_logic(_dependency_));
        DT_THROW_IF(!_dependency_.has_dependee(slot_node->slot_id),
                    std::logic_error,
                    "Dependency with depender '" << _dependency_.get_depender().get_full_path() << "' "
                    << "has no dependee slot [#" << slot_node->slot_id << "]!");
        // ... is mapped to a SLOT logic object:
        slot_logic * sl = dynamic_cast<slot_logic*>(log_ptr.get());
        sl->set_dependee_slot(slot_node->slot_id);
      } else if (const parsing::dependency_logic_ast_op * op_node
                 = boost::get<parsing::dependency_logic_ast_op>(&node_)) {
        // An operator node:
        std::string op_symbol = op_node->op_symbol;
        boost::algorithm::to_lower(op_symbol);
        dependency_logic_factory_dict_type & facts = dependency_logic_factories();
        DT_THROW_IF(!facts.count(op_symbol), std::logic_error, "No logic with operator GUID='" << op_symbol << "'!");
        dependency_logic_factory_type fact = facts.find(op_symbol)->second;
        // ... is mapped to an OPERATOR logic object (AND, OR, XOR, NOT):
        log_ptr.reset(fact(_dependency_));
        for (std::size_t islot = 0; islot < op_node->children.size(); islot++) {
          const parsing::dependency_logic_ast_node & child_node = op_node->children[islot];
          dependency_logic_ptr_type child_logic = _build_node(child_node);
          log_ptr->connect(islot, child_logic);
        }
      }
      return log_ptr;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
