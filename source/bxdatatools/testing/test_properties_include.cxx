// Ourselves:
#include <datatools/properties.h>
#include <datatools/properties_config.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// This Project:
#include <datatools/clhep_units.h>

void tpi1();
void tpi2();

int main(void)
{
  std::clog << "Test of the 'properties' file inclusion functionality..." << std::endl;
  try {
    tpi1();
    tpi2();
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    return 1;
  }
  return 0;
}

void tpi1()
{
  using namespace std;

  std::ostringstream env_include_path_out;
  env_include_path_out << "${HOME}";
  env_include_path_out << ":/tmp";
  setenv("DATATOOLS_INCLUDE_PATH", env_include_path_out.str().c_str(), 1);

  std::string my_conf_path = "${HOME}/params1.conf";
  {
    datatools::fetch_path_with_env(my_conf_path);
    std::ofstream myconf(my_conf_path.c_str());
    myconf << "#@description Mass of the first black hole\n";
    myconf << "m1 : real as mass = 34.0e30 kg\n\n";
    myconf << "#@description Mass of the second black hole\n";
    myconf << "m2 : real as mass = 25.0e30 kg\n\n";
    myconf << "#@description Distance between the two black holes\n";
    myconf << "d : real as length = 1230000.0 km\n\n";
  }

  std::string my_conf_path2 = "/tmp/params2.conf";
  {
    datatools::fetch_path_with_env(my_conf_path2);
    std::ofstream myconf(my_conf_path2.c_str());
    myconf << "d : real as length = 4200000.0 km\n\n";
  }


  datatools::properties conf("Base configuration");
  std::string confPath = "${DATATOOLS_TESTING_DIR}/config/test_pi.conf";
  uint32_t reader_opts = datatools::properties_config::RESOLVE_PATH
    | datatools::properties_config::LOG_TRACE;
  datatools::properties_config reader(reader_opts);
  reader.read(confPath, conf);
  clog << endl << "read conf :" << endl;
  conf.print_tree(clog);

  return;
}

void tpi2()
{
  using namespace std;

  std::ostringstream env_include_path_out;
  env_include_path_out << "${HOME}";
  env_include_path_out << ":/tmp";
  setenv("DATATOOLS_INCLUDE_PATH", env_include_path_out.str().c_str(), 1);

  std::string my_conf_path = "${HOME}/params1.conf";
  {
    datatools::fetch_path_with_env(my_conf_path);
    std::ofstream myconf(my_conf_path.c_str());
    myconf << "#@description Mass of the first black hole\n";
    myconf << "m1 : real as mass = 34.0e30 kg\n\n";
    myconf << "#@description Mass of the second black hole\n";
    myconf << "m2 : real as mass = 25.0e30 kg\n\n";
    myconf << "#@description Distance between the two black holes\n";
    myconf << "d : real as length = 1230000.0 km\n\n";
  }

  std::string my_conf_path2 = "/tmp/params2.conf";
  {
    datatools::fetch_path_with_env(my_conf_path2);
    std::ofstream myconf(my_conf_path2.c_str());
    myconf << "d : real as length = 4200000.0 km\n\n";
  }

  datatools::file_include include_solver;
  include_solver.set_logging(datatools::logger::PRIO_DEBUG);
  include_solver.append_explicit_include_path("${DATATOOLS_TESTING_DIR}/config/defaults");
  include_solver.append_explicit_include_path("${DATATOOLS_TESTING_DIR}/config/user");
  include_solver.append_explicit_include_path("/etc/config");
  include_solver.set_allows_current_directory(true);
  include_solver.set_include_path_env_name("DATATOOLS_INCLUDE_PATH");
  include_solver.set_include_path_env_strategy(datatools::file_include::EV_PREPEND);

  uint32_t reader_opts = datatools::properties_config::RESOLVE_PATH
    | datatools::properties_config::LOG_TRACE;
  datatools::properties_config reader(reader_opts);
  reader.set_fi(include_solver);

  std::string confPath = "${DATATOOLS_TESTING_DIR}/config/test_pi_naked.conf";
  datatools::properties conf("Base configuration");
  reader.read(confPath, conf);
  clog << endl << "read conf :" << endl;
  conf.print_tree(clog);

  return;
}
