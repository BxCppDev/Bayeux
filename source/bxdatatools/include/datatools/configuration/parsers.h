/// \file datatools/configuration/parsers.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-26
 * Last modified : 2017-01-12
 *
 * Copyright (C) 2016-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Parsers for variant items.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_PARSERS_H
#define DATATOOLS_CONFIGURATION_PARSERS_H

// Third party:
// - Boost:
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>

namespace datatools {

  namespace configuration {

    /// \brief Parser for a registry name
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "_a"
    /// "a_"
    /// "_aaa"
    /// "aaa_"
    /// "_aaa0"
    /// "_0aaa"
    /// \endcode
    template <typename Iterator>
    struct registry_name_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      registry_name_instance_grammar()
        : registry_name_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= ( qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
    };

    /// \brief Parser for a variant name
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "_a"
    /// "a_"
    /// "_aaa"
    /// "aaa_"
    /// "_aaa0"
    /// "_0aaa"
    /// \endcode
    template <typename Iterator>
    struct variant_name_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      variant_name_instance_grammar()
        : variant_name_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= ( qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
    };

    /// \brief Parser for a variant parameter name
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "a[12]"
    /// "_a"
    /// "a_"
    /// "_aaa"
    /// "aaa_"
    /// "_aaa0"
    /// "_0aaa"
    /// \endcode
    template <typename Iterator>
    struct param_name_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      param_name_instance_grammar()
        : param_name_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= ( qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z0-9_"))
          >> -( qi::char_('[') >> +qi::digit >> qi::char_(']') );
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
    };

    /// \brief Parser for a variant parameter group name
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "_a"
    /// "a_"
    /// "_aaa"
    /// "aaa_"
    /// "_aaa0"
    /// "_0aaa"
    /// "_0aaa/bbb"
    /// "_0aaa/bbb/ccc"
    /// \endcode
    template <typename Iterator>
    struct group_name_instance_grammar
      : boost::spirit::qi::grammar<Iterator,
                                   std::string()>
    {
      group_name_instance_grammar()
        : group_name_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        // instance_name %= ( qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));
        start %= (
                  subgrpg
                  >> *( qi::char_('/') >> subgrpg)
                  );
        // start %= ( qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9"));
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      variant_name_instance_grammar<Iterator>          subgrpg;
      //boost::spirit::qi::rule<Iterator, std::string()> instance_name;
    };

    /// \brief Parser for a local variant parameter path
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "a[12]"
    /// "a/b/c"
    /// "a[12]/b/c"
    /// "a/b/c[12]"
    /// "a[12]/b/c[12]"
    /// "a/b/c[12]"
    /// "a[12]/b/c[12]/d/e"
    /// \endcode
    template <typename Iterator>
    struct local_param_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      local_param_path_instance_grammar()
        : local_param_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (
                  pnig
                  >> *( qi::char_('/') >> vnig >> qi::char_('/') >> pnig)
                  );
        return;
      }
      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      param_name_instance_grammar<Iterator>            pnig;
      variant_name_instance_grammar<Iterator>          vnig;
    };


    /// \brief Parser for a local variant path
    ///
    /// Supported format:
    /// \code
    /// "a/b"
    /// "a[12]/b"
    /// "a/b"
    /// "a[12]/b"
    /// "a/b/c[12]/d"
    /// "a[12]/b/c[12]/d"
    /// "a/b/c[12]/d"
    /// "a[12]/b/c[12]/d/e/f"
    /// \endcode
    template <typename Iterator>
    struct local_variant_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      local_variant_path_instance_grammar()
        : local_variant_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (
                  (pnig >> qi::char_('/') >> vnig)
                  >> *( qi::char_('/') >> pnig >> qi::char_('/') >> vnig)
                  );
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      param_name_instance_grammar<Iterator>            pnig;
      variant_name_instance_grammar<Iterator>          vnig;

    };

    /// \brief Parser for a full variant parameter path
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "name:a"
    /// "name:a[12]"
    /// "name:a/b/c"
    /// "name:a[12]/b/c"
    /// "name:a/b/c[12]"
    /// "name:a[12]/b/c[12]"
    /// "name:a/b/c[12]"
    /// "name:a[12]/b/c[12]/d/e"
    /// \endcode
    template <typename Iterator>
    struct full_param_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      full_param_path_instance_grammar()
        : full_param_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (rnig >> qi::char_(':') >> lppig);
        return;
      }
      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      registry_name_instance_grammar<Iterator>         rnig;
      local_param_path_instance_grammar<Iterator>      lppig;
    };

    /// \brief Parser for a full variant path
    ///
    /// Supported format:
    /// \code
    /// "",
    /// "name:",
    /// "a",
    /// "name:ab",
    /// "name:ab[56]/c",
    /// "name:foo/bar/c",
    /// "name:foo/bar/baz/d",
    /// "name:foo[0]/bar/baz/ee",
    /// "name:foo/bar/baz[0]/ee",
    /// "name:foo/bar[0]/baz/ee",
    /// "name:foo[42]/bar/baz[12]",
    /// "name:foo[42]/bar/baz[12]/test/biz/ff",
    /// \endcode
    template <typename Iterator>
    struct full_variant_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      full_variant_path_instance_grammar()
        : full_variant_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (rnig >> qi::char_(':') >> lvpig);
        return;
      }
      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      registry_name_instance_grammar<Iterator>         rnig;
      local_variant_path_instance_grammar<Iterator>    lvpig;
    };

    /// \brief Parser for a full variant parameter value group path
    ///
    /// Supported format:
    /// \code
    /// "name:a"
    /// "name:a@grp1"
    /// "name:a[12]"
    /// "name:a[12]@grp1"
    /// "name:a/b/c"
    /// "name:a/b/c@.grp"
    /// "name:a[12]/b/c"
    /// "name:a/b/c[12]@grp1"
    /// "name:a[12]/b/c[12]@grp1"
    /// "name:a/b/c[12]@grp1"
    /// "name:a[12]/b/c[12]/d/e@grp1"
    /// \endcode
    template <typename Iterator>
    struct full_param_value_group_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      full_param_value_group_path_instance_grammar()
        : full_param_value_group_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (fppig >> qi::char_('@') >> gnig);
        return;
      }
      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      full_param_path_instance_grammar<Iterator>       fppig;
      group_name_instance_grammar<Iterator>            gnig;
    };


    /// \brief Parser for a enumerated string value representation
    ///
    /// Supported format:
    /// \code
    /// "a"
    /// "_a"
    /// "a_"
    /// "_aaa"
    /// "aaa_"
    /// "_aaa0"
    /// "_0aaa"
    /// \endcode
    template <typename Iterator>
    struct enumerated_string_value_repr_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      enumerated_string_value_repr_instance_grammar()
        : enumerated_string_value_repr_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        namespace bsa = boost::spirit::ascii;
        // Rule definitions:
        start %= ( +qi::char_ );
        // // Rule definitions:
        // start %= ( qi::char_("a-zA-Z_")
        //            | (
        //               qi::char_("a-zA-Z_")
        //               >> -(+qi::char_("a-zA-Z0-9_:.-/")
        //               >> qi::char_("a-zA-Z0-9_")
        //               )
        //            )
        //   );
        return;
      }

      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
    };

    /// \brief Parser for a full variant parameter value path
    ///
    /// Supported format:
    /// \code
    /// "name:a"
    /// "name:a=Foo"
    /// "name:a[12]"
    /// "name:a[12]=Bar"
    /// "name:a/b/c"
    /// "name:a/b/c=Foo.grp"
    /// "name:a[12]/b/c"
    /// "name:a/b/c[12]=Babaz"
    /// "name:a[12]/b/c[12]=Nada"
    /// "name:a/b/c[12]=John"
    /// "name:a[12]/b/c[12]/d/e=Doe"
    /// \endcode
    template <typename Iterator>
    struct full_param_enum_string_value_path_instance_grammar
      : boost::spirit::qi::grammar<Iterator, std::string()>
    {
      full_param_enum_string_value_path_instance_grammar()
        : full_param_enum_string_value_path_instance_grammar::base_type(start)
      {
        namespace qi = boost::spirit::qi;
        // Rule definitions:
        start %= (fppig >> qi::char_('=') >> esvrig);
        return;
      }
      // Rules:
      boost::spirit::qi::rule<Iterator, std::string()> start;
      full_param_path_instance_grammar<Iterator>       fppig;
      enumerated_string_value_repr_instance_grammar<Iterator> esvrig;
    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_PARSERS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
