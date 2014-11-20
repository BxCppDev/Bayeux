// test_stl_tools.cxx

// Standard library:
#include <streambuf>

// Third party:
// - Boost:
#include <boost/spirit/include/support_istream_iterator.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/tessellation.h>
#include <geomtools/stl_tools.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>

void test_vertex ()
{
  std::clog << "*** Parsing a STL vertex : " << std::endl;
  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef geomtools::stl::vertex_parser<iterator_type> vertex_parser_type;

  std::string vertex_token = " vertex  1.900000e+001 -2.000000e+001  1.000000e+001 ";
  vertex_parser_type          vtx_grammar;
  geomtools::stl::vertex vtx;
  iterator_type iter = vertex_token.begin();
  iterator_type end = vertex_token.end();
  bool r = boost::spirit::qi::phrase_parse(iter, end, vtx_grammar, boost::spirit::ascii::space, vtx);
  if (r && iter == end) {
    std::cout << boost::fusion::tuple_open('[');
    std::cout << boost::fusion::tuple_close(']');
    std::cout << boost::fusion::tuple_delimiter(", ");

    std::cout << "-------------------------\n";
    std::cout << "Parsing succeeded\n";
    std::cout << "got: " << boost::fusion::as_vector(vtx) << std::endl;
    std::cout << "\n-------------------------\n";
  } else {
    std::cout << "-------------------------\n";
    std::cout << "Parsing failed\n";
    std::cout << "-------------------------\n";
  }

  return;
}


void test_facet ()
{
  std::clog << "*** Parsing a STL facet : " << std::endl;
  using boost::spirit::ascii::space;
  //typedef boost::spirit::istream_iterator iterator_type;
  typedef std::string::const_iterator iterator_type;
  typedef geomtools::stl::facet_parser<iterator_type> facet_parser_type;

  std::string stl_file_path = "${GEOMTOOLS_TESTING_DIR}/data/test_stl_one_facet.stl";
  datatools::fetch_path_with_env (stl_file_path);
  std::cerr << "DEVEL: stl_file_path='" << stl_file_path <<"'\n";
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
  iterator_type facet_end  = facet_token.end();

  facet_parser_type     fct_grammar;
  geomtools::stl::facet fct;

  bool r = boost::spirit::qi::phrase_parse(facet_iter,
                                           facet_end,
                                           fct_grammar,
                                           boost::spirit::ascii::space,
                                           fct);
  if (r && facet_iter == facet_end) {
    std::cout << boost::fusion::tuple_open('[');
    std::cout << boost::fusion::tuple_close(']');
    std::cout << boost::fusion::tuple_delimiter(", ");

    std::cout << "-------------------------\n";
    std::cout << "Parsing succeeded\n";
    std::cout << "got: " << std::endl;
    std::cout << "     " << fct << '\n';
    std::cout << "\n-------------------------\n";
  } else {
    std::cout << "-------------------------\n";
    std::cout << "Parsing failed\n";
    std::cout << "-------------------------\n";
  }

  return;
}

void test_solid (bool draw_, std::string stlfp_)
{
  std::clog << "*** Parsing a STL solid : " << std::endl;

  bool draw = draw_;

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef geomtools::stl::solid_parser<iterator_type> solid_parser_type;

  std::string stl_file_path = "${GEOMTOOLS_TESTING_DIR}/data/test_stl_import.stl";
  if (! stlfp_.empty ()) {
    stl_file_path = stlfp_;
  }
  datatools::fetch_path_with_env (stl_file_path);
  std::cerr << "DEVEL: stl_file_path='" << stl_file_path <<"'\n";
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
    fin.seekg(0, std::ios::beg);
    std::cerr << "length : " << length << '\n';
    solid_token.reserve(length);
    fin.seekg(0, std::ios::beg);
    solid_token.assign((std::istreambuf_iterator<char>(fin)),
                       (std::istreambuf_iterator<char>()));
  }

  //std::cerr << "solid_token : \n------------------\n" << solid_token << "\n------------------\n";

  iterator_type solid_iter = solid_token.begin();
  iterator_type solid_end  = solid_token.end();

  solid_parser_type     sld_grammar;
  geomtools::stl::solid sld;

  bool r = boost::spirit::qi::phrase_parse(solid_iter,
                                           solid_end,
                                           sld_grammar,
                                           boost::spirit::ascii::space,
                                           sld);
  if (r && solid_iter == solid_end) {
    std::cout << boost::fusion::tuple_open('[');
    std::cout << boost::fusion::tuple_close(']');
    std::cout << boost::fusion::tuple_delimiter(", ");

    std::cout << "-------------------------\n";
    std::cout << "Parsing succeeded\n";
    std::cout << "got: " << std::endl;
    std::cout << "     " << sld << '\n';
    std::cout << "\n-------------------------\n";

    geomtools::tessellated_solid gtts;
    geomtools::stl::stl_to_geomtools_converter stl2gt;
    stl2gt.set_fix_attempt (true);
    /* int error = */ stl2gt.convert (sld, gtts);

    gtts.dump (std::clog);

    gtts.lock ();

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw) {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_stl_import");

      datatools::temp_file tmp_file_s;
      tmp_file_s.set_remove_at_destroy (true);
      tmp_file_s.create ("/tmp", "test_stl_import_s");

      geomtools::vector_3d pos;
      pos.set (0., 0., 0.);
      std::clog << "Vector: " << pos << std::endl;

      geomtools::rotation rot;
      int rotation_axis = geomtools::ROTATION_AXIS_Z;
      double rotation_angle = 0 * CLHEP::degree;
      geomtools::create_rotation (rot, rotation_axis, rotation_angle);
      geomtools::tree_dump (rot, std::clog, "Rotation matrix:");

      geomtools::gnuplot_draw::draw_tessellated (tmp_file.out (), pos, rot, gtts);
      tmp_file.close ();
      usleep (200);

      gtts.print_xyz (tmp_file_s.out ());
      tmp_file_s.close ();
      usleep (200);

      {
        Gnuplot g1 ("lines");
        {
          std::ostringstream title;
          title << "set title '";
          title << "Tessellated solid from CATIA/STL export";
          title << " (";
          title <<  gtts.vertices ().size () << " vertices, " << gtts.facets ().size () << " facets)";
          g1.cmd (title.str ());
        }
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -30");
        g1.set_xrange (-30, +30).set_yrange (-30, +30).set_zrange (-30, +30);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");

        g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }


      {
        Gnuplot g1 ("pm3d");
        {
          std::ostringstream title;
          title << "set title '";
          title << "Tessellated solid from CATIA/STL export";
          title << " (";
          title <<  gtts.vertices ().size () << " vertices, " << gtts.facets ().size () << " facets)";
          g1.cmd (title.str ());
        }
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -30");
        g1.set_xrange (-30, +30).set_yrange (-30, +30).set_zrange (-30, +30);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        g1.cmd ("set pm3d at s depthorder corners2color geomean hidden3d 100 ");
        g1.cmd ("set style line 100 lt 1 lw 0.5 ");
        g1.cmd ("unset hidden3d");
        g1.cmd ("unset surf");
        g1.cmd ("set colorbox vertical user front");
        g1.cmd ("set palette model RGB");
        g1.cmd ("set palette defined ( \
 0 \"yellow\"   ,                           \
 15 \"red\" )");
        g1.cmd ("show palette");

        g1.plotfile_xyzo (tmp_file_s.get_filename (), 1, 2, 3, 4, "3D view");
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      // g1.plotfile_xy (tmp_file.get_filename (), 1, 2, "2D view");
      // g1.showonscreen (); // window output
      // geomtools::gnuplot_drawer::wait_for_key ();
      // usleep (200);

    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  } else {
    std::cout << "-------------------------\n";
    std::cout << "Parsing failed\n";
    std::cout << "-------------------------\n";
  }
  return;
}


int main(int argc_, char **argv_)
{
  // bool debug = false;
  bool draw = false;
  std::string fn;

  int iarg = 1;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-D" || arg == "--draw") {
      draw = true;
    } else if (arg == "-f" || arg == "--filename") {
      fn = argv_[++iarg];
    }
    iarg++;
  }

  try {
    test_vertex ();

    test_facet ();

    std::clog << "NOTICE: fn = '" << fn << "'\n";
    test_solid (draw, fn);
  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << '\n';
    return 1;
  }
  catch (...) {
    std::cerr << "ERROR: " << "An unexpected error occured !" << '\n';
    return 1;
  }

  return 0;
}
