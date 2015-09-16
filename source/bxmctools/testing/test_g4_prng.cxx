// test_g4_prng.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <mctools/g4/g4_prng.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::g4::g4_prng'!" << std::endl;

    bool debug = false;
    std::string type = "taus2";

    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") ||(option == "--debug")) {
          debug = true;
        }
        else if ((option == "-t") ||(option == "--taus2")) {
          type = "taus2";
        }
        else if ((option == "-g") ||(option == "--gfsr4")) {
          type = "gfsr4";
        }
        else if ((option == "-m") ||(option == "--mt19937")) {
          type = "mt19937";
        }
        else if ((option == "-R") ||(option == "--mt19937")) {
          type = "mt19937";
        }
        else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      }
      else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    int seed = 123456;
    mygsl::rng random;
    random.init(type, seed);

    mctools::g4::g4_prng my_prng(random);

    for (int i = 0; i < 10; i++) {
      std::cout << my_prng.flat() << ' ';
    }
    std::cout << std::endl << std::endl;

    std::ostringstream ostatus;
    random.to_stream(ostatus);
    std::string status = ostatus.str();
    std::cout << "status = " << status << std::endl;

    std::cout << "a = ";
    double a[10];
    my_prng.flatArray(10, a);
    for (int i = 0; i < 10; i++) {
      std::cout << a[i] << ' ';
    }
    std::cout << std::endl << std::endl;

    std::cout << "reload the status..." << std::endl << "a = ";
    std::istringstream istatus(status);
    random.from_stream(istatus);
    my_prng.flatArray(10, a);
    for (int i = 0; i < 10; i++) {
      std::cout << a[i] << ' ';
    }
    std::cout << std::endl << std::endl;
    my_prng.showStatus();
    my_prng.saveStatus("test_g4_prng.status");

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
