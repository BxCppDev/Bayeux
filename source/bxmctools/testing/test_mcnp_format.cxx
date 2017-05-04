// test_mcnp_format.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// // - Bayeux/mygsl:
// #include <mygsl/rng.h>

// This project:
#include <mctools/mcnp/format.h>

int main(int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::mcnp::format'!" << std::endl;

    std::string short_line = "aaaaaaaaaaaaa";
    mctools::mcnp::format::format_line(std::clog, short_line);
    std::clog << std::endl;

    std::string long_line = "aaaaaaaaaa";
    long_line += "bbbbbbbbbb";
    long_line += "cccccccccc";
    long_line += "dddddddddd";
    long_line += "eeeeeeeeee";
    long_line += "ffffffffff";
    long_line += "gggggggggg";
    long_line += "hhhhhhhhhh";
    long_line += "iiiiiiiiii";
    mctools::mcnp::format::format_line(std::clog, long_line);
    std::clog << std::endl;

    std::string very_long_line = long_line + long_line + long_line;
    mctools::mcnp::format::format_line(std::clog, very_long_line);
    std::clog << std::endl;

    std::string comment_line = "This is a comment";
    mctools::mcnp::format::add_comment_line(std::clog, comment_line);
    std::clog << std::endl;

  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
