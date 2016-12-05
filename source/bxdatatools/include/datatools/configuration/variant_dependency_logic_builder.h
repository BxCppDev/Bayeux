/// \file datatools/configuration/variant_dependency_logic_builder.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-26
 * Last modified : 2016-10-26
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
 *   Build a dependency logic object from an abstract syntax tree build from a formula.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_BUILDER_H
#define DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_BUILDER_H

// This project (Bayeux/datatools):
#include <datatools/configuration/variant_dependency_logic.h>
#include <datatools/configuration/variant_dependency_logic_parsing.h>

namespace datatools {

  namespace configuration {

    class variant_dependency;

    /// \brief Dependency logic builder from an AST
    class variant_dependency_logic_builder
    {
    public:
      /// Constructor
      variant_dependency_logic_builder(const parsing::dependency_logic_ast & ast_,
                                       variant_dependency & dependency_);

      /// Build the dependency logic
      dependency_logic_ptr_type build();

    protected:

      /// Recursive logic build
      dependency_logic_ptr_type _build_node(const parsing::dependency_logic_ast_node & node_);

    private:
      const parsing::dependency_logic_ast & _ast_; //!< The source AST
      variant_dependency & _dependency_; //!< The target dependency from which dependees are published (through their slots)
    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_DEPENDENCY_LOGIC_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
