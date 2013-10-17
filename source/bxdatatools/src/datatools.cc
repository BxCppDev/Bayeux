/* i_named.cc */
// Ourselves
#include <datatools/datatools.h>

// This project
#include <datatools/kernel.h>

namespace datatools {

  void initialize(int argc_, char * argv_[])
  {
    if (! datatools::kernel::is_instantiated()) {
      kernel::instantiate();
    }
    /*
    static boost::scoped_ptr<datatools::kernel> _kernel_handler;
    if (! datatools::kernel::is_instantiated()) {
      if (! _kernel_handler) {
        // Allocate the new global kernel and initialize it
        _kernel_handler.reset(new datatools::kernel);
      }
    }
    */
    if (datatools::kernel::is_instantiated()) {
      datatools::kernel & krnl = datatools::kernel::instance();
      if (! krnl.is_initialized()) {
        if (argc_ && argv_) {
          krnl.initialize(argc_, argv_);
        } else {
          krnl.initialize(0, 0);
        }
      }
    }

    ::datatools::_special_initialize_impl();
    return;
  }

  void terminate()
  {
    ::datatools::_special_terminate_impl();
    if (datatools::kernel::is_instantiated()) {
      datatools::kernel & krnl = datatools::kernel::instance();
      if (krnl.is_initialized()) {
        krnl.shutdown();
      }
    }
    return;
  }

} // end of namespace datatools
