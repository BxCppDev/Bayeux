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
  libinfo.path_registration("bar", "data", "/data/bar/raw/dataset3");
  libinfo.path_registration("bar", "resources", "/etc/bar/conf.d");
  libinfo.path_registration("baz", "data", "/data/baz/raw/dataset3");
  libinfo.path_registration("faz", "data", "/data/faz/runs/002/files");

  libinfo.tree_dump(std::clog, "Standalone library info register: ");
  std::clog << std::endl;

  {
    std::string p("@bar:run_0.zip");
    std::clog << "[info] Resolving path = " << p << std::endl;
    std::string rp = p;
    std::string errmsg;
    if (libinfo.resolve_path(p, rp, errmsg)) {
      std::clog << "[info] Resolved path    = " << rp << std::endl;
    } else {
      std::clog << "[error] Unresolved path = " << p << ": " << errmsg << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::string p("@bar.data:run_1.zip");
    std::clog << "[info] Resolving path = " << p << std::endl;
    std::string rp = p;
    std::string errmsg;
    if (libinfo.resolve_path(p, rp, errmsg)) {
      std::clog << "[info] Resolved path    = " << rp << std::endl;
    } else {
      std::clog << "[error] Unresolved path = " << p << ": " << errmsg << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::string p("@faz.data:truc/run_42.data");
    std::clog << "[info] Resolving path = " << p << std::endl;
    std::string rp = p;
    std::string errmsg;
    if (libinfo.resolve_path(p, rp, errmsg)) {
      std::clog << "[info] Resolved path    = " << rp << std::endl;
    } else {
      std::clog << "[error] Unresolved path = " << p << ": " << errmsg << std::endl;
    }
    std::clog << std::endl;
  }

  {
    std::string p("@faz.dummy:blah.txt");
    std::clog << "[info] Resolving path = " << p << std::endl;
    std::string rp = p;
    std::string errmsg;
    if (libinfo.resolve_path(p, rp, errmsg)) {
      std::clog << "[info] Resolved path    = " << rp << std::endl;
    } else {
      std::clog << "[error] Unresolved path = " << p << ": " << errmsg << std::endl;
    }
    std::clog << std::endl;
  }

  std::vector<std::string> mountDirectives = {
    "foo@/tmp/bar",
    "foo.resources@/tmp/bar",
    "foo.data@${HOME}",
    "foo.libraries@~/lib/x86_64",
    "foo.bad@arrrrrgh",
  };
  for (std::string mountDirective : mountDirectives) {
    std::string theLibname;
    std::string theTopic;
    std::string thePath;
    std::string errMsg;
    bool parsed = datatools::library_info::parse_path_registration_directive(mountDirective,
                                                                             theLibname,
                                                                             theTopic,
                                                                             thePath,
                                                                             errMsg);
    if (parsed) {
      std::clog << "[info] Parsed of '" << mountDirective << "' is ok: " << std::endl;
      std::clog << "[info]   Library name : " << theLibname << std::endl;
      std::clog << "[info]   Topic        : " << theTopic << std::endl;
      std::clog << "[info]   Path         : " << thePath << std::endl;
    } else {
      std::clog << "[error] Parsed failed: " << errMsg << std::endl;
    }
    std::clog << std::endl;
  }

  libinfo.unregistration("bar");
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
  libinfo.path_registration("bar", "data", "/data/bar/raw/dataset3");
  libinfo.path_registration("bar", "resources", "/data/bar/config");
  libinfo.path_registration("baz", "data", "/data/baz/raw/dataset3");
  libinfo.path_registration("faz", "data", "/data/faz/runs/002/files");
  libinfo.tree_dump(std::clog, "Kernel's library info register: ");
  std::clog << std::endl;

  {
    std::string p("@bar:run_0.zip");
    std::clog << "Resolving path = " << p << std::endl;
    std::string rp = p;
    datatools::fetch_path_with_env(rp);
    std::clog << "Resolved path  = " << rp << std::endl;
    std::clog << std::endl;
  }

  {
    std::string p("@bar:run_1.zip");
    std::clog << "Resolving path = " << p << std::endl;
    std::string rp = p;
    datatools::fetch_path_with_env(rp);
    std::clog << "Resolved path  = " << rp << std::endl;
    std::clog << std::endl;
  }

  {
    std::string p("@faz.data:truc/run_42.data");
    std::clog << "Resolving path = " << p << std::endl;
    std::string rp = p;
    datatools::fetch_path_with_env(rp);
    std::clog << "Resolved path  = " << rp << std::endl;
    std::clog << std::endl;
  }

  {
    std::string p("@faz:truc/run_42.data");
    std::clog << "Resolving path = " << p << std::endl;
    std::string rp = p;
    std::string errmsg;
    if (datatools::fetch_path(rp, errmsg)) {
      std::clog << "Resolved path  = " << rp << std::endl;
    } else {
      rp.clear();
      std::cerr << "[error] As expected: " << errmsg << std::endl;
    }
    std::clog << std::endl;
  }


  return;
}

int main(int argc_, char * argv_[]) {
  datatools::initialize(argc_,argv_);
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "\nEntering test_library_info...\n" << std::endl;

    std::clog << "\n\ntest_standalone_libinfo..." << std::endl;
    test_standalone_libinfo();

    std::clog << "\n\ntest_kernel_libinfo..." << std::endl;
    test_kernel_libinfo();

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  datatools::terminate();
  return error_code;
}
