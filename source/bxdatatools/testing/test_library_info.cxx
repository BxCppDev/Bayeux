// -*- mode: c++ ; -*-
// test_library_info_manager.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project
#include <datatools/datatools_config.h>
#include <datatools/library_info.h>
#include <datatools/logger.h>
#include <datatools/properties.h>

int main (int argc_, char ** argv_) {
  try {
    datatools::library_info::status(std::clog);

    datatools::properties & foo_lib_infos
      = datatools::library_info::registration("foo",
                                              "A dummy library",
                                              "3.14.159",
                                              "/usr",
                                              "/usr/lib",
                                              "/usr/share/foo");
    if (datatools::library_info::has("foo")) {
      try {
        datatools::library_info::update("foo", "version", "3.14.160");
      } catch (std::exception & expected) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                     "Expected error: " << expected.what());
      }
      datatools::library_info::update("foo", "debug", "0", "boolean");
      datatools::library_info::update("foo", "active_trace", "1", "boolean");
      datatools::library_info::print("foo", std::cout);

      foo_lib_infos.store("debug_level", 3);
      foo_lib_infos.store_flag("test");
      foo_lib_infos.store_integer(datatools::library_info::keys::version_major(), 3);
      foo_lib_infos.store_integer(datatools::library_info::keys::version_minor(), 14);
      foo_lib_infos.store_integer(datatools::library_info::keys::version_patch(), 160);
      foo_lib_infos.store_boolean("with_bio", true);
      foo_lib_infos.store_boolean("with_reflection", true);
      datatools::library_info::print("foo", std::cout);

      if (foo_lib_infos.has_key("version")) {
        std::string ver = foo_lib_infos.fetch_string("version");
        std::clog << "foo version : '" << ver << "'"
                  << std::endl;
      }

      if (foo_lib_infos.has_key(datatools::library_info::keys::install_resource_dir())) {
        std::string resdir =
          foo_lib_infos.fetch_string(datatools::library_info::keys::install_resource_dir());
        std::clog << "Found foo resource dir : '" << resdir << "'"
                  << std::endl;
      }

      {
        std::clog << std::endl;
        std::string path = "@foo:/data/values.data";
        std::clog << "Original path : '" << path << "'" << std::endl;
        datatools::fetch_path_with_env(path);
        std::clog << "Fetched path  : '" << path << "'" << std::endl;
      }

      datatools::library_info::status(std::clog);

      datatools::library_info::unregistration("foo");
    }

    datatools::library_info::status(std::clog);
  }
  catch (std::exception & x)
  {
    std::cerr << "error: " << x.what () << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// end of test_datatools.cxx
