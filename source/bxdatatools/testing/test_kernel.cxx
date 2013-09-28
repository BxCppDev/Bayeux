// -*- mode: c++ ; -*-
// test_kernel.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project
#include <datatools/datatools.h>
#include <datatools/kernel.h>
#include <datatools/exception.h>
#include <datatools/library_info.h>

int main(int argc_, char * argv_[]) {
  DATATOOLS_INIT_MAIN(argc_, argv_);
  try {

    DT_THROW_IF(! datatools::kernel::is_instantiated(),
                std::runtime_error,
                "No datatools kernel is instantiated !");

    const datatools::kernel & dtkl = datatools::kernel::const_instance();
    dtkl.tree_dump(std::cout, "The datatools' kernel: ", "*** ");

    if (dtkl.has_library_info_register()) {
      const datatools::library_info & lib_info_reg
        = dtkl.get_library_info_register();
      lib_info_reg.tree_dump(std::cout, "Kernel's library info register: ");
    }

    std::cerr << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    return EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// end of test_kernel.cxx
