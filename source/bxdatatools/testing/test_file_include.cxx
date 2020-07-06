// This Project:
#include <datatools/file_include.h>

// Standard library:
#include <iostream>
#include <exception>
#include <cstdlib>
#include <set>

// Third parties:
#include <boost/filesystem.hpp>

// This Project:
#include <bayeux/bayeux.h>
#include <datatools/utils.h>

void test_fi();

int main(void)
{
  bayeux::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    
   test_fi();
    
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

struct dir_store
{
  ~dir_store()
  {
    for (std::set<boost::filesystem::path>::const_reverse_iterator it
           = _stored_paths_.rbegin();
         it != _stored_paths_.rend();
         it++) {
      const auto & bdp = *it;
      std::clog << "[info] removing directory " << bdp << std::endl; 
      boost::filesystem::remove(bdp);
    }
  }
  void create_dir(const std::string & dirpath_) {
    std::string dp = dirpath_;
    datatools::fetch_path_with_env(dp);
    boost::filesystem::path bdp(dp);
    if (not boost::filesystem::is_directory(bdp)) {
      std::clog << "[info] creating directory " << bdp << std::endl; 
      boost::filesystem::create_directory(bdp);
      _stored_paths_.insert(bdp);
    }
  }
  std::set<boost::filesystem::path> _stored_paths_;
};

void test_fi()
{
  dir_store ds;
  
  setenv("BAYEUX_GENERIC_INCLUDE_PATH", "/tmp/_foo/more:/tmp/_foo:${HOME}/_foo", 1);
  datatools::file_include fi;
  fi.set_logging(datatools::logger::PRIO_ERROR);
  fi.set_include_path_env_strategy(datatools::file_include::EV_PREPEND);
  // fi.set_allows_current_directory(false);
  fi.set_allows_current_directory(true);
  fi.set_include_path_env_name("BAYEUX_GENERIC_INCLUDE_PATH");
  fi.append_explicit_include_path("/etc");
  fi.append_explicit_include_path("/var");
  fi.prepend_explicit_include_path("${HOME}");
  fi.prepend_explicit_include_path("${HOME}/_myfoo");
  fi.prepend_explicit_include_path("@datatools:variants/models/basic/1.0");
  
  ds.create_dir("/tmp/_foo");
  ds.create_dir("/tmp/_foo/more");
  ds.create_dir("${HOME}");
  ds.create_dir("${HOME}/_foo");
  ds.create_dir("${HOME}/_myfoo");

  fi.print_tree(std::clog);

  std::vector<std::string> paths;
  paths.push_back("utils.def");
  paths.push_back("wgetrc");
  paths.push_back("profile");
  paths.push_back("passwd");
  paths.push_back("/vmlinuz");
  paths.push_back("/usr/include/math.h");
  paths.push_back("/usr/include/nomath.h");
  paths.push_back("~/.bashrc");
  paths.push_back("~johndoe/.bashrc");
  paths.push_back("/root/.bashrc");
  paths.push_back("${HOME}/.bashrc");

  for (const std::string & path : paths) {
    std::string resolved_path;
    std::clog << "path          = '" << path << "'" << std::endl;
    if (fi.resolve(path, resolved_path)) {
      std::clog << "[found] resolved_path = '" << resolved_path << "'" << std::endl;
    } else {
      std::cerr << "[error] could not resolve this path!" << std::endl;
    }
    std::cerr << std::endl;
  }

  return;
}
