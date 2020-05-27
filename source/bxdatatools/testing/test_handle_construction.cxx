// Needed Stdlib/datatools items to perform by-hand testing
#include <iostream>
#include <exception>
#include "datatools/exception.h"

// Helpers for test cases
#include <vector>
#include <string>
#include <algorithm>

// Ourselves
#include "datatools/handle.h"

using datatools::handle;
using datatools::make_handle;

void test()
{
  // Test basic/copy construction/destruction
  {
      auto a = make_handle<int>(2);
      {
          auto b{a};
          DT_THROW_IF(a.use_count() != 2, std::logic_error, "Copy of handle does not increment use_count");
      }
      DT_THROW_IF(a.use_count() != 1, std::logic_error, "Destruction of handle does not decrement use_count");
  }

  // Test move construction
  {
      auto a = make_handle<double>(3.14);
      // auto b{std::move(a)};
      handle<double> b{std::move(a)};

      DT_THROW_IF(a, std::logic_error, "Moved handle still holds data");
      DT_THROW_IF(a.use_count() != 0, std::logic_error, "Moved handle has non-null use count");

      // DT_THROW_IF(!b, std::logic_error, "Move constructed handle does not hold data"); // Fails with g++ 4.9 and Boost 1.63
      DT_THROW_IF(!b.has_data(), std::logic_error, "Move constructed handle does not hold data");
      DT_THROW_IF(b.use_count() != 1, std::logic_error, "Move constructed handle has incorrect use count");
  }

  // Test move assignment
  {
      auto a = make_handle<std::string>("hello");
      // auto b = std::move(a);
      handle<std::string> b{std::move(a)};
      DT_THROW_IF(a, std::logic_error, "Moved handle still holds data");
      DT_THROW_IF(a.use_count() != 0, std::logic_error, "Moved handle has non-null use count");

      DT_THROW_IF(!b, std::logic_error, "Move assigned handle does not hold data");
      DT_THROW_IF(b.use_count() != 1, std::logic_error, "Move assigned handle has incorrect use count");
  }
}

int main()
{
    try 
    {
        test();
    }
    catch (std::exception& e)
    {
        std::cerr << "test_handle_construction::test() failed:\n" << e.what() << std::endl;
        return 1;
    }
    return 0;
}
