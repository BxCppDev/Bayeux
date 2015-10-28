// Ourselves:
#include <datatools/ui/utils.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

int main(int /* argc_ */, char * /* argv_ */[])
{
  std::vector<std::string> names;
  names.push_back("");
  names.push_back("a");
  names.push_back("0a");
  names.push_back("a0");
  names.push_back(".a");
  names.push_back("a_");
  names.push_back("_a");
  names.push_back("a/b");

  for (int i = 0; i < (int) names.size(); i++) {
    std::clog << "Name '" << names[i] << "' is ";
    if (!datatools::ui::path::is_valid_name(names[i])) {
      std::clog << "not ";
    }
    std::clog << "valid." << std::endl;
  }
  std::clog << std::endl;

  std::vector<std::string> parent_paths;
  parent_paths.push_back("");
  parent_paths.push_back("/");
  parent_paths.push_back(".");
  parent_paths.push_back("./");
  parent_paths.push_back("..");
  parent_paths.push_back("../");
  parent_paths.push_back("~");
  parent_paths.push_back("~/");
  parent_paths.push_back("//");
  parent_paths.push_back("a/b/");
  parent_paths.push_back("/a/b/c");
  parent_paths.push_back("/a/b/c/");
  for (int j = 0; j < (int) parent_paths.size(); j++) {
    const std::string & parent_path = parent_paths[j];
    std::clog << "Parent path is '" << parent_path<< "' :" << std::endl;
    for (int i = 0; i < (int) names.size(); i++) {
      std::clog << "  For name '" << names[i] << "', child path is : ";
      std::string child_path;
      if (datatools::ui::path::build_leaf_full_path(parent_path,
                                                    names[i],
                                                    child_path)) {
        std::clog << "'" << child_path << "'";
      } else {
        std::clog << "<invalid>";
      }
      std::clog << std::endl;
    }
    std::clog << std::endl;
  }

  return EXIT_SUCCESS;
}
