// -*- mode: c++ ; -*-
// test_library_info.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <datatools/datatools.h>
#include <datatools/datatools_config.h>
#include <datatools/library_info.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/kernel.h>

void test_standalone_libinfo()
{
  datatools::library_info libinfo;
  libinfo.tree_dump(std::clog, "Standalone library info register: ");

  datatools::properties & foo_lib_infos
    = libinfo.registration("foo",
                           "A dummy library",
                           "3.14.159",
                           "/usr",
                           "/usr/lib",
                           "/usr/share/foo");
  if (libinfo.has("foo")) {
    try {
      libinfo.update("foo", "version", "3.14.160");
    } catch (std::exception & expected) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Expected error: " << expected.what());
    }
    libinfo.update("foo", "debug", "0", "boolean");
    libinfo.update("foo", "active_trace", "1", "boolean");
    libinfo.print("foo", std::cout);

    foo_lib_infos.store("debug_level", 3);
    foo_lib_infos.store_flag("test");
    foo_lib_infos.store_integer(datatools::library_info::keys::version_major(), 3);
    foo_lib_infos.store_integer(datatools::library_info::keys::version_minor(), 14);
    foo_lib_infos.store_integer(datatools::library_info::keys::version_patch(), 160);
    foo_lib_infos.store_boolean("with_bio", true);
    foo_lib_infos.store_boolean("with_reflection", true);
    libinfo.print("foo", std::cout);

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
      std::string path = "@foo:/data/values.data";
      std::clog << "Original path : '" << path << "'" << std::endl;
      if (datatools::fetch_path_with_env(path)) {
        std::clog << "Fetched path  : '" << path << "'" << std::endl;
      } else {
        std::clog << "Cannot fetch path : '" << path << "'" << std::endl;
      }
    }

    libinfo.tree_dump(std::clog, "Standalone library info register: ");
    libinfo.unregistration("foo");
  }
  libinfo.tree_dump(std::clog, "Standalone library info register: ");
  return;
}

void test_kernel_libinfo()
{
  datatools::library_info & libinfo = datatools::kernel::instance().grab_library_info_register();

  libinfo.tree_dump(std::clog, "Kernel's library info register: ");

  datatools::properties & foo_lib_infos
    = libinfo.registration("foo",
                           "A dummy library",
                           "3.14.159",
                           "/usr",
                           "/usr/lib",
                           "/usr/share/foo");
  if (libinfo.has("foo")) {
    try {
      libinfo.update("foo", "version", "3.14.160");
    } catch (std::exception & expected) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Expected error: " << expected.what());
    }
    libinfo.update("foo", "debug", "0", "boolean");
    libinfo.update("foo", "active_trace", "1", "boolean");
    libinfo.print("foo", std::cout);

    foo_lib_infos.store("debug_level", 3);
    foo_lib_infos.store_flag("test");
    foo_lib_infos.store_integer(datatools::library_info::keys::version_major(), 3);
    foo_lib_infos.store_integer(datatools::library_info::keys::version_minor(), 14);
    foo_lib_infos.store_integer(datatools::library_info::keys::version_patch(), 160);
    foo_lib_infos.store_boolean("with_bio", true);
    foo_lib_infos.store_boolean("with_reflection", true);
    libinfo.print("foo", std::cout);

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
      std::string path = "@foo:/data/values.data";
      std::clog << "Original path : '" << path << "'" << std::endl;
      if (datatools::fetch_path_with_env(path)) {
        std::clog << "Fetched path  : '" << path << "'" << std::endl;
      } else {
        std::clog << "Cannot fetch path : '" << path << "'" << std::endl;
      }
    }

    libinfo.tree_dump(std::clog, "Kernel's library info register: ");
    libinfo.unregistration("foo");
  }
  libinfo.tree_dump(std::clog, "Kernel's library info register: ");
  return;
}

int main(int argc_, char * argv_[]) {
  DATATOOLS_INIT_MAIN(argc_,argv_);
  int error_code = EXIT_SUCCESS;
  try {

    std::cerr << "\n\ntest_standalone_libinfo..." << std::endl;
    test_standalone_libinfo();

    std::cerr << "\n\ntest_kernel_libinfo..." << std::endl;
    test_kernel_libinfo();

    std::cerr << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  DATATOOLS_FINI();
  return error_code;
}
