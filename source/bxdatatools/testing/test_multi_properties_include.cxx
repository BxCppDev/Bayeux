// test_multi_properties_include.cxx

// Ourselves:
#include <datatools/multi_properties.h>
#include <datatools/multi_properties_config.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

void mpi();

int main (int argc_ , char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test of the 'multi_properties' file inclusion functionality..." << endl;
    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      // if (arg == "-d" || arg == "--debug") debug = true;
      iarg++;
    }

    mpi();

  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void mpi()
{
  using namespace std;

  std::ostringstream env_include_path_out;
  env_include_path_out << '.';
  env_include_path_out << ":${HOME}";
  env_include_path_out << ":/tmp";

  std::string my_conf_path = "${HOME}/bar.conf";
  {
    datatools::fetch_path_with_env(my_conf_path);
    std::ofstream myconf(my_conf_path.c_str());
    myconf << "#@description My config\n";
    myconf << "#@key_label \"name\"\n";
    myconf << "#@meta_label \"type\"\n\n";
    myconf << "[name=\"params2\" type=\"section\"]\n";
    myconf << "m1 : real as mass = 34.0 kg\n\n";
  }

  std::string my_conf_path2 = "/tmp/baz.conf";
  {
    datatools::fetch_path_with_env(my_conf_path2);
    std::ofstream myconf(my_conf_path2.c_str());
    myconf << "#@description My config\n";
    myconf << "#@key_label \"name\"\n";
    myconf << "#@meta_label \"type\"\n\n";
    myconf << "[name=\"config\" type=\"section\"]\n";
    myconf << "pi : const real = 3.14159\n\n";
  }

  setenv("DATATOOLS_INCLUDE_PATH", env_include_path_out.str().c_str(), 1);

  datatools::multi_properties conf("name", "type", "Base configuration");
  std::string confPath = "${DATATOOLS_TESTING_DIR}/config/test_mpi.conf";
  uint32_t read_opts = datatools::multi_properties_config::RESOLVE_PATH
    | datatools::multi_properties_config::LOG_DEBUG;
  datatools::multi_properties::read_config(confPath, conf, read_opts);
  clog << endl << "read conf :" << endl;
  conf.print_tree(clog);

  return;
}
