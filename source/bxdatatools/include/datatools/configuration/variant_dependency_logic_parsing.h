/// \file datatools/configuration/variant_dependency_logic_parsing.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-11-02
 * Last modified : 2016-11-02
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
 * Description:
 *
 *   Parsing tools (AST, grammar) for variant dependency logic formulas.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_PARSING_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_PARSING_H

// Standard library:
#include <iostream>
#include <string>
#include <vector>
#include <limits>

// Third party:
#include <boost/variant.hpp>

namespace datatools {

  namespace configuration {

    namespace parsing {

      struct dependency_logic_ast_op;
      struct dependency_logic_ast_slot;

      /// \brief  AST node:
      typedef boost::variant<
             boost::recursive_wrapper<dependency_logic_ast_op>
             , dependency_logic_ast_slot
             >
             dependency_logic_ast_node;
      void print(const dependency_logic_ast_node & node_, std::ostream & out_, int indent_ = 0);

      /// \brief AST leaf node:
      struct dependency_logic_ast_slot
      {
        unsigned int slot_id = std::numeric_limits<unsigned int>::max(); //!< Slot ID
        void print(std::ostream & out_, int indent_ = 0) const;
      };

      /// \brief AST general operator node:
      struct dependency_logic_ast_op
      {
        std::string op_symbol;                //!< Operator symbol
        std::vector<dependency_logic_ast_node> children; //!< Child nodes
        void print(std::ostream & out_, int indent_ = 0) const;
      };

      /// \brief Parsing AST:
      struct dependency_logic_ast
      {
        /// Default constructor
        dependency_logic_ast();
        dependency_logic_ast_node top; //!< Top node
        bool is_valid() const;
        void invalidate();
        void print(std::ostream & out_, int indent_ = 0) const;
        bool parse(const std::string & formula_);

      };

    }  // end of namespace parsing

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_PARSING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
