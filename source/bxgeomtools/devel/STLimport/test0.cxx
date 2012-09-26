//

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <limits>
#include <streambuf>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lambda/lambda.hpp>

//#include <boost/fusion/include/io.hpp>

#include <boost/spirit/include/support_istream_iterator.hpp>

/// http://irmatden.developpez.com/tutoriels/cpp/parsing-fichiers-rib-avec-boost-spirit/?page=sommaire

/// http://www.boost.org/doc/libs/1_51_0/libs/spirit/doc/html/spirit/qi/tutorials/employee___parsing_into_structs.html


#include <geomtools/tessellation.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/utils/temporary_files.h>


/****************
 *  STL vertex  *
 ****************/

namespace geomtools {
  namespace stl {
  
    struct vertex_t
    {
      double x, y, z;

      vertex_t ()
      {
        x = std::numeric_limits<double>::quiet_NaN ();
        y = std::numeric_limits<double>::quiet_NaN ();
        z = std::numeric_limits<double>::quiet_NaN ();
        return;
      }

      friend std::ostream & operator<<( std::ostream & out_, const vertex_t & vtx_)
      {
        out_ << "geomtools::stl::vertex{" << vtx_.x << ';'  << vtx_.y << ';'  << vtx_.z << '}' ;
        return out_;
      }

      friend bool operator== (const vertex_t & vtx1_, const vertex_t & vtx2_) 
      {
        return vtx1_.x == vtx2_.x && vtx1_.y == vtx2_.y && vtx1_.z == vtx2_.z; 
      }

      friend bool operator< (const vertex_t & vtx1_, const vertex_t & vtx2_)
      {
        if (vtx1_.z < vtx2_.z) return true;
        else if (vtx1_.z > vtx2_.z) return false;
        else if (vtx1_.y < vtx2_.y) return true;
        else if (vtx1_.y > vtx2_.y) return false;
        else if (vtx1_.x < vtx2_.x) return true;
        return false;
      }

    };

  }
}


BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::vertex_t,
                          (double, x)
                          (double, y)
                          (double, z)
                          )

namespace geomtools {
  namespace stl {
    
    template <typename Iterator>
    struct vertex_parser : boost::spirit::qi::grammar<Iterator, 
                                                      vertex_t(), 
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
      
      boost::spirit::qi::rule<Iterator, vertex_t(), boost::spirit::qi::ascii::space_type> start;
      
    };

  }
}

/****************
 *  STL facet   *
 ****************/

namespace geomtools {
  namespace stl {
  
    struct facet_t
    {
      double nx, ny, nz;
      std::vector<vertex_t> vertices;

      facet_t ()
      {
        nx = std::numeric_limits<double>::quiet_NaN ();
        ny = std::numeric_limits<double>::quiet_NaN ();
        nz = std::numeric_limits<double>::quiet_NaN ();
        vertices.reserve(3);
        return;
      }

      friend std::ostream & operator<<( std::ostream & out_, const facet_t & fct_)
      {
        out_ << "geomtools::stl::facet{normal={" << fct_.nx << ';'  << fct_.ny << ';' << fct_.nz
             << "};nvtx=" << fct_.vertices.size () << ";"
             << "v0=" << fct_.vertices[0] << ";" 
             << "v1=" << fct_.vertices[1] << ";"
             << "v2=" << fct_.vertices[2] << '}' ;
        return out_;
      }

    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::facet_t,
                          (double, nx)
                          (double, ny)
                          (double, nz)
                          (std::vector<geomtools::stl::vertex_t>,vertices)
                          )

namespace geomtools {
  namespace stl {
    
    template <typename Iterator>
    struct facet_parser : boost::spirit::qi::grammar<Iterator, 
                                                     facet_t(), 
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
                                         // [
                                         //  (std::cerr << "********* " << boost::lambda::_1 << '\n')
                                         //  boost::phoenix::push_back(boost::phoenix::at_c<3>(boost::spirit::qi::_val), boost::spirit::qi::_1)
                                         // ] 
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
           << boost::spirit::qi::_4  // what failed?
           << boost::phoenix::val(" here: \"")
           << boost::phoenix::construct<std::string>(boost::spirit::qi::_3, boost::spirit::qi::_2) // iterators to error-pos, end
           << boost::phoenix::val("\"")
           << std::endl
           );
                
        return;
      }
      
      // Attributes: 
      geomtools::stl::vertex_parser<Iterator> vertex_grammar; 
      boost::spirit::qi::rule<Iterator,
                              facet_t(), 
                              boost::spirit::qi::locals<std::string>, 
                              boost::spirit::qi::ascii::space_type> facet_grammar;

    };

  }
}

/****************
 *  STL solid   *
 ****************/

namespace geomtools {
  namespace stl {
  
    struct solid_t
    {
      std::string name;
      std::vector<facet_t> facets;
      std::string dummy_name;

      solid_t ()
      {
        facets.reserve(1000);
        return;
      }

      friend std::ostream & operator<<( std::ostream & out_, const solid_t & sld_)
      {
        out_ << "geomtools::stl::solid{name='" << sld_.name << "'" 
             << ";#facets="  << sld_.facets.size()  
             << ";dummy_name='" << sld_.name << "'}";
        return out_;
      }

    };

  }
}

BOOST_FUSION_ADAPT_STRUCT(geomtools::stl::solid_t,
                          (std::string, name)
                          (std::vector<geomtools::stl::facet_t>,facets)
                          (std::string, dummy_name)
                          )


namespace geomtools {
  namespace stl {
    
    template <typename Iterator>
    struct solid_parser : boost::spirit::qi::grammar<Iterator, 
                                                     solid_t(), 
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
          > boost::spirit::qi::repeat(4, boost::spirit::qi::inf)
          [
           facet_grammar [
                          std::cerr << boost::lambda::_1 << '\n'
                          //boost::phoenix::push_back(boost::phoenix::at_c<1>(boost::spirit::qi::_val), boost::spirit::qi::_1)
                          ] 
             // > boost::spirit::qi::no_skip
             //    [
             //      boost::spirit::qi::omit[*boost::spirit::qi::blank] 
             //      > boost::spirit::qi::eol
             //    ]
           ]
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
                              solid_t(), 
                              boost::spirit::qi::locals<std::string>, 
                              boost::spirit::qi::ascii::space_type> solid_grammar;

    };

  }
}

namespace geomtools {
  namespace stl {

    class stl_to_geomtools_converter
    {
    public:
      stl_to_geomtools_converter ();
      int convert (const solid_t & solid_, tessellated_solid & ts_);
      static void fix_broken_facets (tessellated_solid & ts_);
    };

    stl_to_geomtools_converter::stl_to_geomtools_converter ()
    {
      return;
    }

    void stl_to_geomtools_converter::fix_broken_facets (tessellated_solid & ts_)
    {
      // Search for mis-referenced vertices :
      std::vector<int> suspicious_vertex_indexes;
      for (geomtools::tessellated_solid::vertices_col_t::const_iterator 
             i = ts_.vertices ().begin ();
           i != ts_.vertices ().end ();
           i++)
        {
          const geomtools::facet_vertex & fvtx = i->second;
          if (fvtx.ref_facets.size () < 3)
            {
              suspicious_vertex_indexes.push_back (i->first);
            }
        }

      for (int i = 0; i < suspicious_vertex_indexes.size (); i++)
        {
          std::clog << "WARNING: geomtools::stl::stl_to_geomtools_converter::fix_broken_facets: "
                    << "Suspicious vertex #" << suspicious_vertex_indexes[i]
                    << std::endl;
          
        }
      ts_.dump (std::clog);
      return;
    }
 
    int stl_to_geomtools_converter::convert (const solid_t & solid_, 
                                             tessellated_solid & ts_)
    {
      typedef std::map<vertex_t,int> vtx_dict_t;
      vtx_dict_t vertices;

      std::cerr << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "
                << "Number of facets = " << solid_.facets.size()
                << std::endl;
      for (int ifacet = 0; ifacet < solid_.facets.size(); ifacet++)
        {
          std::cerr << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "
                    << "Processing facets #" << ifacet << " ..." 
                    << std::endl;
          const facet_t & a_facet = solid_.facets[ifacet];
          int ivertices[3];
          std::cerr << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "
                    << "  Facets #" << ifacet << " has " 
                    << a_facet.vertices.size () << " vertices." 
                    << std::endl;
          for (int ivtx = 0; ivtx < a_facet.vertices.size (); ivtx++)
            {
              const vertex_t & a_vertex = a_facet.vertices[ivtx];
              double x, y, z;
              x = a_vertex.x;
              y = a_vertex.y;
              z = a_vertex.z;
              vtx_dict_t::const_iterator found = vertices.find (a_vertex);
              if (found == vertices.end ())
                {
                  int new_vertex_id = vertices.size ();
                  vertices[a_vertex] = new_vertex_id;
                  ivertices[ivtx] = new_vertex_id;
                  ts_.add_vertex (ivertices[ivtx], 
                                  a_vertex.x, 
                                  a_vertex.y, 
                                  a_vertex.z);
                }
              else
                {
                  ivertices[ivtx] = found->second;
                }
              std::cerr << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "
                        << "     vertex #" << ivertices[ivtx]
                        << std::endl;
              }
          
          ts_.add_facet (ifacet, ivertices[0], ivertices[1], ivertices[2]);
        }
      std::cerr << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "
                << "Number of vertices = " << vertices.size()
                << std::endl;

      fix_broken_facets (ts_);

      //ts_.lock ();
      return EXIT_SUCCESS;
    }

  }
}

/*************************************************************************************/

int main(int argc_, char **argv_)
{
  bool draw = true;
  std::string stl_file_path = "test/test_export.stl";
  std::ifstream fin (stl_file_path.c_str ());

  /******************************************/

  {
    std::clog << "*** Parsing a STL vertex : " << std::endl;
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef geomtools::stl::vertex_parser<iterator_type> vertex_parser_t;

    std::string vertex_token = " vertex  1.900000e+001 -2.000000e+001  1.000000e+001 ";
    vertex_parser_t          vtx_grammar;
    geomtools::stl::vertex_t vtx;
    iterator_type iter = vertex_token.begin();
    iterator_type end = vertex_token.end();
    bool r = boost::spirit::qi::phrase_parse(iter, end, vtx_grammar, boost::spirit::ascii::space, vtx); 
    if (r && iter == end)
      {
        std::cout << boost::fusion::tuple_open('[');
        std::cout << boost::fusion::tuple_close(']');
        std::cout << boost::fusion::tuple_delimiter(", ");
        
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "got: " << boost::fusion::as_vector(vtx) << std::endl;
        std::cout << "\n-------------------------\n";
      }
    else
      {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
      }
    
  }

  /******************************************/

  {
    std::clog << "*** Parsing a STL facet : " << std::endl;
    using boost::spirit::ascii::space;
    //typedef boost::spirit::istream_iterator iterator_type;
    typedef std::string::const_iterator iterator_type;
    typedef geomtools::stl::facet_parser<iterator_type> facet_parser_t;

    std::string stl_file_path = "tests/one_facet3.stl";
    std::ifstream fin (stl_file_path.c_str (), std::ios::binary);

    // read the full file in the string :
    std::string facet_token;

    /*
    {
    std::istreambuf_iterator<char> inputIt(fin), emptyInputIt;
    std::back_insert_iterator< std::string> stringInsert(facet_token);
    std::copy(inputIt, emptyInputIt, stringInsert);
    }
    */

    {
      fin.seekg(0, std::ios::end);
      std::streampos length = fin.tellg();
      fin.seekg(0,std::ios::beg);
      std::cerr << "length : " << length << '\n';
      facet_token.reserve(length);
      fin.seekg(0, std::ios::beg);    
      facet_token.assign((std::istreambuf_iterator<char>(fin)),
                         (std::istreambuf_iterator<char>()));
    }

    std::cerr << "facet_token : \n------------------\n" << facet_token << "\n------------------\n";

    iterator_type facet_iter = facet_token.begin();
    iterator_type facet_end = facet_token.end();
    
    facet_parser_t          fct_grammar;
    geomtools::stl::facet_t fct;
    
    bool r = boost::spirit::qi::phrase_parse(facet_iter, 
                                             facet_end, 
                                             fct_grammar, 
                                             boost::spirit::ascii::space, 
                                             fct); 
    if (r && facet_iter == facet_end)
      {
        std::cout << boost::fusion::tuple_open('[');
        std::cout << boost::fusion::tuple_close(']');
        std::cout << boost::fusion::tuple_delimiter(", ");
        
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "got: " << std::endl; 
        std::cout << "     " << fct << '\n';
        std::cout << "\n-------------------------\n";
      }
    else
      {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
      }

  }

  /******************************************/

  {
    std::clog << "*** Parsing a STL solid : " << std::endl;
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef geomtools::stl::solid_parser<iterator_type> solid_parser_t;

    std::string stl_file_path = "tests/test_export.stl";
    std::ifstream fin (stl_file_path.c_str (), std::ios::binary);

    // read the full file in the string :
    std::string solid_token;

    /*
    {
    std::istreambuf_iterator<char> inputIt(fin), emptyInputIt;
    std::back_insert_iterator< std::string> stringInsert(solid_token);
    std::copy(inputIt, emptyInputIt, stringInsert);
    }
    */

    {
      fin.seekg(0, std::ios::end);
      std::streampos length = fin.tellg();
      fin.seekg(0,std::ios::beg);
      std::cerr << "length : " << length << '\n';
      solid_token.reserve(length);
      fin.seekg(0, std::ios::beg);    
      solid_token.assign((std::istreambuf_iterator<char>(fin)),
                         (std::istreambuf_iterator<char>()));
    }

    //std::cerr << "solid_token : \n------------------\n" << solid_token << "\n------------------\n";

    iterator_type solid_iter = solid_token.begin();
    iterator_type solid_end = solid_token.end();
    
    solid_parser_t          sld_grammar;
    geomtools::stl::solid_t sld;
    
    bool r = boost::spirit::qi::phrase_parse(solid_iter, 
                                             solid_end, 
                                             sld_grammar, 
                                             boost::spirit::ascii::space, 
                                             sld); 
    if (r && solid_iter == solid_end)
      {
        std::cout << boost::fusion::tuple_open('[');
        std::cout << boost::fusion::tuple_close(']');
        std::cout << boost::fusion::tuple_delimiter(", ");
        
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "got: " << std::endl;
        std::cout << "     " << sld << '\n';
        std::cout << "\n-------------------------\n";

        geomtools::stl::stl_to_geomtools_converter stl2gt;
        geomtools::tessellated_solid gtts;
        int error = stl2gt.convert (sld, gtts);
        //gtts.dump (std::clog);
        //gtts.lock ();
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        if (draw)
          {
            datatools::utils::temp_file tmp_file;
            tmp_file.set_remove_at_destroy (true);
            tmp_file.create ("/tmp", "test_stl_import");

            datatools::utils::temp_file tmp_file_s;
            tmp_file_s.set_remove_at_destroy (true);
            tmp_file_s.create ("/tmp", "test_stl_import_s");
            
            /*
            // Search for mis-referenced vertices :
            for (geomtools::tessellated_solid::vertices_col_t::const_iterator 
                   i = gtts.vertices ().begin ();
                 i != gtts.vertices ().end ();
                 i++)
              {
                const geomtools::facet_vertex & fvtx = i->second;
                if (fvtx.ref_facets.size () < 3)
                  {
                    fvtx.print_xyz (tmp_file_s.out ());
                  }
              }
            */

            geomtools::vector_3d pos;
            pos.set (0., 0., 0.);
            std::clog << "Vector: " << pos << std::endl;
            
            geomtools::rotation rot;
            int rotation_axis = geomtools::ROTATION_AXIS_Z;
            double rotation_angle = 90 * CLHEP::degree;
            geomtools::create_rotation (rot, rotation_axis, rotation_angle);
            geomtools::tree_dump (rot, std::clog, "Rotation matrix:");
            
            geomtools::gnuplot_draw::draw_tessellated (tmp_file.out (), pos, rot, gtts);
            tmp_file.close ();
            usleep (200);
            
            Gnuplot g1 ("lines"); 
            g1.cmd ("set grid");
            g1.set_xrange (-30, +30).set_yrange (-30, +30).set_zrange (-30, +30);
            g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
            g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view")
              .set_style ("points")
              .plotfile_xyz (tmp_file_s.get_filename (), 1, 2, 3, "")
              .set_style ("lines")
              .plotfile_xyz ("tests/ttt.data", 1, 2, 3, "");
            g1.showonscreen (); // window output
            geomtools::gnuplot_drawer::wait_for_key ();
            usleep (200);

            g1.plotfile_xy (tmp_file.get_filename (), 1, 2, "2D view")
              .set_style ("points")
              .plotfile_xy (tmp_file_s.get_filename (), 1, 2, "")
              .set_style ("points")
              .plotfile_xy ("tests/ttt.data", 1, 2, "");
            g1.showonscreen (); // window output
            geomtools::gnuplot_drawer::wait_for_key ();
            usleep (200);

          }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
      }
    else
      {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
      }

  }

  return 0;
}
