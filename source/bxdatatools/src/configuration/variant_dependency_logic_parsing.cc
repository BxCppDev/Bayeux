// datatools/configuration/variant_dependency_logic_parsing.cc
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
#include <datatools/configuration/variant_dependency_logic_parsing.h>

// Third party:
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace datatools {

  namespace configuration {

    namespace parsing {

      void print(const dependency_logic_ast_node & node_, std::ostream & out_, int indent_)
      {
        if (const dependency_logic_ast_op * op_node = boost::get<dependency_logic_ast_op>(&node_)) {
          op_node->print(out_, indent_ + 1);
        } else if (const dependency_logic_ast_slot * slot_node = boost::get<dependency_logic_ast_slot>(&node_)) {
          slot_node->print(out_, indent_ + 1);
        }
        return;
      }

      void dependency_logic_ast_op::print(std::ostream & out_, int indent_) const
      {
        for (int i=0; i < indent_; i++) {
          out_ << "  ";
        }
        out_ << "* Operator '" << op_symbol << "' : ";
        out_ << std::endl;
        for (const auto & child : children) {
          datatools::configuration::parsing::print(child, out_, indent_ + 1);
        }
      }

      void dependency_logic_ast_slot::print(std::ostream & out_, int indent_) const
      {
        for (int i=0; i < indent_; i++) {
          out_ << "  ";
        }
        out_ << "* Slot[#" << slot_id << "]";
        out_ << std::endl;
      }

      void dependency_logic_ast::print(std::ostream & out_, int indent_ ) const
      {
        out_ << "Logic AST: " << std::endl;
        datatools::configuration::parsing::print(top, out_, indent_);
        return;
      }

      bool dependency_logic_ast::is_valid() const
      {
        if (const dependency_logic_ast_slot * slot_node = boost::get<dependency_logic_ast_slot>(&top)) {
          return slot_node->slot_id != std::numeric_limits<unsigned int>::max();
        } else if (const dependency_logic_ast_op * op_node = boost::get<dependency_logic_ast_op>(&top)) {
          return op_node->children.size() && !op_node->op_symbol.empty();
        }
        return false;
      }

      void dependency_logic_ast::invalidate()
      {
        dependency_logic_ast_slot dummy;
        top = dummy;
        return;
      }

      dependency_logic_ast::dependency_logic_ast()
      {
        invalidate();
        return;
      }

    } // end of parsing

  }  // end of namespace configuration

}  // end of namespace datatools

BOOST_FUSION_ADAPT_STRUCT(
                          datatools::configuration::parsing::dependency_logic_ast_op,
                          (std::string, op_symbol)
                          (std::vector<datatools::configuration::parsing::dependency_logic_ast_node>, children)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          datatools::configuration::parsing::dependency_logic_ast_slot,
                          (unsigned int, slot_id)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          datatools::configuration::parsing::dependency_logic_ast,
                          (datatools::configuration::parsing::dependency_logic_ast_node, top)
                          )

namespace datatools {

  namespace configuration {

    namespace parsing {

      namespace qi = boost::spirit::qi;

      template <typename Iterator>
      struct dependency_logic_ast_grammar : boost::spirit::qi::grammar<Iterator, dependency_logic_ast(), boost::spirit::qi::ascii::space_type>
      {
        dependency_logic_ast_grammar() : dependency_logic_ast_grammar::base_type(start)
        {
          using boost::phoenix::at_c;

          gslot = ('['
                   >> qi::uint_   [at_c<0>(qi::_val) = qi::_1]
                   >> ']') |
            qi::uint_   [at_c<0>(qi::_val) = qi::_1];

          gop_symbol = qi::no_case [qi::string("and") | qi::string("or") | qi::string("xor") | qi::string("not")];

          gop = gop_symbol [at_c<0>(qi::_val) = qi::_1]
            >> '('
            // >> +gnode     [push_back(at_c<1>(qi::_val), qi::_1)]
            >> (gnode % ',') [at_c<1>(qi::_val) = qi::_1]
            >> ')';

          gnode = ( gop | gslot) [qi::_val = qi::_1];

          start = gnode;
          return;
        }

        qi::rule<Iterator, dependency_logic_ast_slot(), qi::ascii::space_type> gslot;
        qi::rule<Iterator, std::string()>                                      gop_symbol;
        qi::rule<Iterator, dependency_logic_ast_op(),   qi::ascii::space_type> gop;
        qi::rule<Iterator, dependency_logic_ast_node(), qi::ascii::space_type> gnode;
        qi::rule<Iterator, dependency_logic_ast(),      qi::ascii::space_type> start;

      };

      // int test_dependency_logic_parsing(const std::string & formula_)
      // {
      //   int error_code = 0;
      //   parsing::dependency_logic_ast ast;
      //   std::string::const_iterator iter = formula_.begin();
      //   std::string::const_iterator end = formula_.end();
      //   typedef parsing::dependency_logic_ast_grammar<std::string::const_iterator> grammar_type;
      //   grammar_type grammar; // Our grammar
      //   bool r = boost::spirit::qi::phrase_parse(iter,
      //                                            end,
      //                                            grammar,
      //                                            boost::spirit::qi::ascii::space,
      //                                            ast);
      //   if (r && iter == end) {
      //     std::clog << "Parsing succeeded:\n";
      //     ast.print(std::clog);
      //   } else {
      //     std::cerr << "Parsing failed at : " << *iter << "\n";
      //     error_code = 1;
      //   }
      //   return error_code;
      // }

      bool dependency_logic_ast::parse(const std::string & formula_)
      {
        int error_code = 0;
        this->invalidate();
        std::string::const_iterator iter = formula_.begin();
        std::string::const_iterator end = formula_.end();
        typedef parsing::dependency_logic_ast_grammar<std::string::const_iterator> grammar_type;
        grammar_type grammar; // Our grammar
        bool r = boost::spirit::qi::phrase_parse(iter,
                                                 end,
                                                 grammar,
                                                 boost::spirit::qi::ascii::space,
                                                 *this);
        if (!r || iter != end) {
          // std::cerr << "Parsing failed at : " << *iter << "\n";
          this->invalidate();
          error_code = 1;
        }
        return error_code == 0;
      }

    } // end of parsing

  }  // end of namespace configuration

}  // end of namespace datatools
