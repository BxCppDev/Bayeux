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

bool test_make_handle()
{
  // Creation of a simple type works
  {
    handle<int> x = make_handle<int>(42);
    DT_THROW_IF( 42 != x.get(), std::logic_error,
                 "make_handle int instance does not have required value 42" );
    DT_THROW_IF( !x.unique(), std::logic_error,
                 "make_handle created a non-unique int instance" );
  }

  // Creation of a standard type works
  {
    using MyType = std::vector<std::string>;
    handle<MyType> x = make_handle<MyType>(3, "foo");

    DT_THROW_IF( 3 != x.get().size(), std::logic_error,
                 "make_handle did not create a 3 element vector" );
    DT_THROW_IF( !x.unique(), std::logic_error,
                 "make_handle created a non-unique std::vector<std::string> instance" );
    DT_THROW_IF( "foo" != x.get().at(2), std::logic_error,
                 "make_handle did not create vector with correct elements" );
  }

  return true;
}

bool test_handle_indirect_operator()
{
  // Test a handle on a trivial type...
  {
    handle<int> x = make_handle<int>(-13);

    // ... Extract its value
    int extracted = *x;
    DT_THROW_IF( extracted != -13, std::logic_error,
                 "extracted int value from handle != expected value -13");

    // ... Set its value
    (*x) = 42;
    DT_THROW_IF( 42 != (*x), std::logic_error,
                 "handle does not hold expected value 42");

    // ... reset and confirm it throws
    x.reset();
    bool gotException{false};
    try {
      (*x) = 24;
    }
    catch (std::logic_error& e) {
      //o.k., we've caught the expected error
      gotException = true;
    }
    DT_THROW_IF( !gotException, std::logic_error,
                 "handle operator*() did not throw exception on null data");
  }

  // Test a handle on a std type...
  {
    using MyType = std::vector<std::string>;
    handle<MyType> x = make_handle<MyType>(3, "foo");

    // Check its interfaces
    // ... basic
    DT_THROW_IF( 3 != (*x).size(), std::logic_error,
                 "handle does not hold a vector of size 3");

    // ... iterators
    for(const auto& v : *x) {
      DT_THROW_IF( "foo" != v, std::logic_error,
                 "handle does not hold a vector with elements `foo`");
    }
  }

  return true;
}

// Test usage of handleInstance->heldMemberFunction();
bool test_handle_deref_operator()
{
  // Test a handle on a std type...
  {
    using MyType = std::vector<std::string>;
    handle<MyType> x = make_handle<MyType>(3, "foo");

    // Check its interfaces
    // ... basic
    DT_THROW_IF( 3 != x->size(), std::logic_error,
                 "handle does not hold a vector of size 3");

    // ... iterators
    auto f = [](const MyType::value_type& v) {
      DT_THROW_IF( "foo" != v, std::logic_error,
                   "handle does not hold a vector with elements `foo`");};
    std::for_each(x->begin(), x->end(), f);

    // ... reset and confirm it throws
    x.reset();
    bool gotException{false};
    try {
      x->size();
    }
    catch (std::logic_error& e) {
      //o.k., we've caught the expected error
      gotException = true;
    }
    DT_THROW_IF( !gotException, std::logic_error,
                 "handle operator->() did not throw exception on null data");
  }

  return true;
}

int main()
{
  DT_THROW_IF( !test_make_handle(), std::logic_error,
               "test_make_handle failed" );
  DT_THROW_IF( !test_handle_indirect_operator(), std::logic_error,
               "test_handle_deref_operator failed" );
  DT_THROW_IF( !test_handle_deref_operator(), std::logic_error,
               "test_handle_deref_operator failed" );

  return 0;
}

