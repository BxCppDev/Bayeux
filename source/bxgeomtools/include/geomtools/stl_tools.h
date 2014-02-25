// -*- mode: c++; -*-
/* stl_tools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-23
 * Last modified: 2013-06-11
 *
 * License:
 *
 * Description:
 *
 *  STL (STereoLithography) tools for STL ASCII formatted file import.
 *
 * Based on :
 *
 *    http://en.wikipedia.org/wiki/STL_%28file_format%29
 *    http://www.ennex.com/~fabbers/StL.asp
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_STL_TOOLS_H_
#define GEOMTOOLS_STL_TOOLS_H_ 1

#include <iostream>
#include <vector>
#include <string>

// Ignore unused parameter warning from transiently included
// qi/real.hpp
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/spirit/include/qi.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace geomtools {

  /// Nested namespace of the Bayeux/geomtools module library (STereoLithography)
  namespace stl {

    /*******************
     *   STL vertex    *
     *******************/

    /// \brief A STL vertex
    struct vertex
    {
      double x, y, z;

      vertex ();

      friend std::ostream & operator<<(std::ostream & out_,
                                       const vertex & vtx_);

      friend bool operator== (const vertex & vtx1_,
                              const vertex & vtx2_);

      friend bool operator< (const vertex & vtx1_,
                             const vertex & vtx2_);

    };


    /*******************
     *   STL facet     *
     *******************/

    /// \brief A STL facet
    struct facet
    {
      double nx, ny, nz;
      std::vector<vertex> vertices;

      facet ();

      friend std::ostream & operator<<(std::ostream & out_,
                                       const facet & fct_);

    };


    /*******************
     *   STL solid     *
     *******************/

    /// \brief A STL solid
    struct solid
    {
      std::string name;
      std::vector<facet> facets;
      std::string dummy_name;

      solid ();
      friend std::ostream & operator<<( std::ostream & out_, const solid & sld_);

    };

  } // end of namespace stl

} // end of namespace geomtools


/****************************
 *  Boost/Fusion interface  *
 ****************************/

BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::vertex,
                          (double, x)
                          (double, y)
                          (double, z)
                          )

BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::facet,
                          (double, nx)
                          (double, ny)
                          (double, nz)
                          (std::vector<geomtools::stl::vertex>,vertices)
                          )

BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::solid,
                          (std::string, name)
                          (std::vector<geomtools::stl::facet>,facets)
                          (std::string, dummy_name)
                          )

namespace geomtools {

  namespace stl {


    /****************************
     *   Boost/Spirit parsers   *
     ****************************/

    template <typename Iterator>
    struct vertex_parser :
      boost::spirit::qi::grammar<Iterator,
                                 vertex(),
                                 boost::spirit::qi::ascii::space_type>
    {
      vertex_parser () : vertex_parser::base_type(start)
      {
        start %=
          boost::spirit::qi::lit("vertex")
          > boost::spirit::qi::double_
          > boost::spirit::qi::double_
          > boost::spirit::qi::double_
          ;
        return;
      }

      // Attributes:
      boost::spirit::qi::rule<Iterator,
                              vertex(),
                              boost::spirit::qi::ascii::space_type> start;

    };

    template <typename Iterator>
    struct facet_parser : boost::spirit::qi::grammar<Iterator,
                                                     facet(),
                                                     boost::spirit::qi::locals<std::string>,
                                                     boost::spirit::qi::ascii::space_type>
    {
      facet_parser () : facet_parser::base_type(facet_grammar, "geomtools::stl::facet_grammar")
      {

        facet_grammar %=
          boost::spirit::qi::lit("facet")
          > boost::spirit::qi::lit("normal")
          > boost::spirit::qi::double_
          > boost::spirit::qi::double_
          > boost::spirit::qi::double_
          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]
          > boost::spirit::qi::lit("outer")
          > boost::spirit::qi::lit("loop")
          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]


          > boost::spirit::qi::repeat(3)[vertex_grammar
                                         > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank]
                                                                      > boost::spirit::qi::eol]
                                         ]

          > boost::spirit::qi::lit("endloop")
          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]
          > boost::spirit::qi::lit("endfacet")
          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]
          ;

        facet_grammar.name("geomtools::stl::facet_grammar");
        vertex_grammar.name("geomtools::stl::vertex_grammar");

        boost::spirit::qi::on_error<boost::spirit::qi::fail>
          (
           facet_grammar
           , std::cerr
           << boost::phoenix::val("Error! Expecting ")
           << boost::spirit::qi::_4
           << boost::phoenix::val(" here: \"")
           << boost::phoenix::construct<std::string>(boost::spirit::qi::_3,
                                                     boost::spirit::qi::_2)
           << boost::phoenix::val("\"")
           << std::endl
           );

        return;
      }

      // Attributes:
      geomtools::stl::vertex_parser<Iterator> vertex_grammar;
      boost::spirit::qi::rule<Iterator,
                              facet(),
                              boost::spirit::qi::locals<std::string>,
                              boost::spirit::qi::ascii::space_type> facet_grammar;

    };



    template <typename Iterator>
    struct solid_parser : boost::spirit::qi::grammar<Iterator,
                                                     solid(),
                                                     boost::spirit::qi::locals<std::string>,
                                                     boost::spirit::qi::ascii::space_type>
    {
      solid_parser () : solid_parser::base_type(solid_grammar, "geomtools::stl:solid_grammar")
      {

        name_grammar %=
          boost::spirit::qi::lexeme[ boost::spirit::qi::alpha >> *boost::spirit::qi::alnum]
          ;

        solid_grammar %=
          boost::spirit::qi::lit("solid")
          > name_grammar
          > boost::spirit::qi::omit[ boost::spirit::qi::lexeme[ *(boost::spirit::qi::char_ - boost::spirit::qi::eol) ]]

          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]
          > boost::spirit::qi::repeat(4, boost::spirit::qi::inf)[facet_grammar]
          > boost::spirit::qi::lit("endsolid")
          > name_grammar
          > boost::spirit::qi::omit[boost::spirit::qi::lexeme[ *(boost::spirit::qi::char_ - boost::spirit::qi::eol) ]]
          > boost::spirit::qi::no_skip[boost::spirit::qi::omit[*boost::spirit::qi::blank] > boost::spirit::qi::eol]
          ;

        solid_grammar.name("geomtools::stl::solid_grammar");
        facet_grammar.name("geomtools::stl::solid::facet_grammar");
        name_grammar.name("geomtools::stl::solid::name_grammar");

        boost::spirit::qi::on_error<boost::spirit::qi::fail>
          (
           solid_grammar
           , std::cerr
           << boost::phoenix::val("Error! Expecting ")
           << boost::spirit::qi::_4
           << boost::phoenix::val(" here: \"")
           << boost::phoenix::construct<std::string>(boost::spirit::qi::_3, boost::spirit::qi::_2)
           << boost::phoenix::val("\"")
           << std::endl
           );

        return;
      }

      // Attributes:
      boost::spirit:: qi::rule<Iterator, std::string(), boost::spirit::qi::ascii::space_type> name_grammar;
      geomtools::stl::facet_parser<Iterator> facet_grammar;
      boost::spirit::qi::rule<Iterator,
                              solid(),
                              boost::spirit::qi::locals<std::string>,
                              boost::spirit::qi::ascii::space_type> solid_grammar;

    };

  } // end of namespace stl

} // end of namespace geomtools


namespace geomtools {

  class tessellated_solid;

  namespace stl {


    /*************************
     *  STL solid converter  *
     *************************/

    /** Converter from the STL ASCII format (from CATIA software)
     *  to geomtools model for tessellated solid (class tessellated_solid).
     */
    class stl_to_geomtools_converter
    {
    public:

      bool is_debug () const;

      void set_debug (bool);

      bool is_check_normal () const;

      void set_check_normal (bool);

      bool is_fix_attempt () const;

      void set_fix_attempt (bool);

      bool is_lock_ts () const;

      void set_lock_ts (bool d_);

      double get_length_unit () const;

      void set_length_unit (double u_);

      stl_to_geomtools_converter ();

      int convert (const solid & solid_, tessellated_solid & ts_);

      static void fix_broken_facets (tessellated_solid & ts_, bool verbose_ = true);

    private:

      bool _debug_;  /// Debug flag
      bool _check_normal_; /// Flag to check to facet normal relative to the 3 vertices
      bool _fix_attempt_; /// Flag to fix possible broken vertices and facets
      bool _lock_ts_; /// Flag to lock the target tessellated_solid
      double _length_unit_; /// CLHEP length unit (mm)

    };

  } // end of namespace stl

} // end of namespace geomtools


#endif // GEOMTOOLS_STL_TOOLS_H_

// end of stl_tools.h
