//! \file ui/foo-reflect.h

#ifndef DATATOOLS_TESTING_UI_FOO_REFLECT_H
#define DATATOOLS_TESTING_UI_FOO_REFLECT_H

// Standard Library:
#include "foo.h"

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// Export the introspection code:
DR_CLASS_EXPORT(::foo, "foo")
DR_TYPE_EXPORT(::foo::dummy_type, "foo::dummy_type")

int f0(int a, int b)
{
  return a + b;
}

void test_func1(int i1_, int i2_)
{
  std::cerr << "DEVEL: test_func1: Entering...\n";
  int s = i1_ + i2_;
  std::cerr << "DEVEL: test_func1: s = " << s << "\n";
  std::cerr << "DEVEL: test_func1: Exiting.\n";
}

void test_func2(const std::vector<int> & v1_, const std::vector<int> & v2_)
{
  std::cerr << "DEVEL: test_func2: Entering...\n";
  int s = 0;
  for (auto v : v1_) s += v;
  for (auto v : v2_) s += v;
  std::cerr << "DEVEL: test_func2: s = " << s << "\n";
  std::cerr << "DEVEL: test_func2: Exiting.\n";
}

void set_sum_func(foo & f_, const std::vector<int> & v_)
{
  std::cerr << "DEVEL: set_sum_func: Entering...\n";
  f_.set_sum(v_);
  std::cerr << "DEVEL: set_sum_func: Exiting.\n";
}

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::foo::dummy_type, DT_UNUSED(tag_) )
      {
        DR_ENUM_DECLARE(::foo::dummy_type)
          .value("none",  ::foo::dummy_type::none)
          .value("one",   ::foo::dummy_type::one)
          .value("two",   ::foo::dummy_type::two)
          .value("three", ::foo::dummy_type::three)
          ;
        return;
      }

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(foo, DT_UNUSED(tag_) )
      {

        DR_CLASS_DECLARE (foo)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_PROPERTY_GETTER_SETTER("name",
                                           ::foo::get_name,
                                           ::foo::set_name)
          .DR_CLASS_PROPERTY_GETTER_SETTER("flag",
                                           ::foo::is_flag,
                                           ::foo::set_flag)
          .DR_CLASS_PROPERTY_GETTER_SETTER("value",
                                           ::foo::get_value,
                                           ::foo::set_value)
          .DR_CLASS_PROPERTY_GETTER_SETTER("x",
                                           ::foo::get_x,
                                           ::foo::set_x)
          .DR_CLASS_PROPERTY_GETTER_SETTER("mass",
                                           ::foo::get_mass,
                                           ::foo::set_mass)
          .DR_CLASS_PROPERTY_GETTER_SETTER("time",
                                           ::foo::get_time,
                                           ::foo::set_time)
          // Not supported by CAMP:
          // .DR_CLASS_PROPERTY_GETTER_SETTER("b",
          //                                  ::foo::get_b,
          //                                  ::foo::set_b)
          // .DR_CLASS_PROPERTY_GETTER("b",
          //                           ::foo::get_b)
          .DR_CLASS_PROPERTY_GETTER_SETTER("logging",
                                           ::foo::get_logging,
                                           ::foo::set_logging)
          // Print:
          .DR_CLASS_VOID_METHOD_CONST("print",
                                      ::foo,
                                      ::foo::print)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Print the current status of the object")

          // Reset:
          .DR_CLASS_VOID_METHOD("reset",
                                ::foo,
                                ::foo::reset)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Reset the object")

          // Set name:
          .DR_CLASS_VOID_METHOD1("set_name",
                                 ::foo,
                                 ::foo::set_name,
                                 std::string)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the name")
          .DR_CLASS_TAG("arg[0].name", "name")

          // Get name:
          .DR_CLASS_METHOD_CONST("get_name",
                                 ::foo,
                                 ::foo::get_name,
                                 std::string)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Get the name")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("returned.name", "name")

          // Set flag:
          .DR_CLASS_VOID_METHOD1("set_flag",
                                 ::foo,
                                 ::foo::set_flag,
                                 bool)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the flag boolean value")
          .DR_CLASS_TAG("arg[0].name", "flag")

          // Get flag:
          .DR_CLASS_METHOD_CONST("is_flag",
                                 ::foo,
                                 ::foo::is_flag,
                                 bool)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Check the flag boolean value")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("returned.name", "flag")

          // Set value:
          .DR_CLASS_VOID_METHOD1("set_value",
                                 ::foo,
                                 ::foo::set_value,
                                 int)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("arg[0].name", "value")
          .DR_CLASS_TAG("description", "Set the integer value")
          .DR_CLASS_TAG("arg[0].description", "the integer value")

          // Get value:
          .DR_CLASS_METHOD_CONST("get_value",
                                 ::foo,
                                 ::foo::get_value,
                                 int)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Get the integer value")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("returned.name", "value")
          .DR_CLASS_TAG("returned.description", "the returned value")

          // Set x:
          .DR_CLASS_VOID_METHOD1("set_x",
                                 ::foo,
                                 ::foo::set_x,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the dimensionless x real value")
          .DR_CLASS_TAG("arg[0].name", "x")
          .DR_CLASS_TAG("arg[0].description", "the x value")

          // Get x:
          .DR_CLASS_METHOD_CONST("get_x",
                                 ::foo,
                                 ::foo::get_x,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("description", "Get the dimensionless x real value")
          .DR_CLASS_TAG("returned.name", "x")
          .DR_CLASS_TAG("returned.description", "the returned x value")

          // Set mass:
          .DR_CLASS_VOID_METHOD1("set_mass",
                                 ::foo,
                                 ::foo::set_mass,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the mass value")
          .DR_CLASS_TAG("arg[0].name", "mass")
          .DR_CLASS_TAG("arg[0].description", "the mass value")
          .DR_CLASS_TAG("arg[0].explicit_unit_dimension", "mass")
          .DR_CLASS_TAG("arg[0].preferred_unit", "mg")

          // Get mass:
          .DR_CLASS_METHOD_CONST("get_mass",
                                 ::foo,
                                 ::foo::get_mass,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("description", "Return the mass")
          .DR_CLASS_TAG("returned.name", "mass")
          .DR_CLASS_TAG("returned.description", "the returned mass value")
          .DR_CLASS_TAG("returned.explicit_unit_dimension", "mass")
          .DR_CLASS_TAG("returned.preferred_unit", "mg")

          // Set time:
          .DR_CLASS_VOID_METHOD1("set_time",
                                 ::foo,
                                 ::foo::set_time,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the current time")
          .DR_CLASS_TAG("arg[0].name", "time")
          .DR_CLASS_TAG("arg[0].description", "the time value")
          .DR_CLASS_TAG("arg[0].implicit_unit", "ms")

          // Get time:
          .DR_CLASS_METHOD_CONST("get_time",
                                 ::foo,
                                 ::foo::get_time,
                                 double)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Get the current time")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("returned.name", "time")
          .DR_CLASS_TAG("returned.description", "the returned time value")
          .DR_CLASS_TAG("returned.implicit_unit", "ms")

          // Zero:
          .DR_CLASS_VOID_METHOD("zero",
                                ::foo,
                                ::foo::zero)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Force the integer value to zero")

          // Incr:
          .DR_CLASS_VOID_METHOD("incr",
                                ::foo,
                                ::foo::incr)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Increment the integer value by one")

          // Decr:
          .DR_CLASS_VOID_METHOD("decr",
                                ::foo,
                                ::foo::decr)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Decrement the integer value by one")

          // Get count:
          .DR_CLASS_METHOD1_CONST("get_count",
                                  ::foo,
                                  ::foo::get_count,
                                  int,
                                  int)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("description", "Return a value from the counts array")
          .DR_CLASS_TAG("arg[0].name", "index")
          .DR_CLASS_TAG("arg[0].description", "the index of the value to be fetched from the counts array")
          .DR_CLASS_TAG("returned.name", "value")
          .DR_CLASS_TAG("returned.description", "the returned time value")

          // Add count:
          .DR_CLASS_VOID_METHOD1("add_count",
                                 ::foo,
                                 ::foo::add_count,
                                 int)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Add a value in the counts array")
          .DR_CLASS_TAG("arg[0].name", "value")
          .DR_CLASS_TAG("arg[0].description", "the value to be appended to the counts array")

          // Set logging:
          .DR_CLASS_VOID_METHOD1("set_logging",
                                 ::foo,
                                 ::foo::set_logging,
                                 ::datatools::logger::priority)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the logging enum value")
          .DR_CLASS_TAG("arg[0].name", "prio")
          .DR_CLASS_TAG("arg[0].type", "datatools::logger::priority")
          .DR_CLASS_TAG("arg[0].description", "the logging priority value")

          // Get logging:
          .DR_CLASS_METHOD_CONST("get_logging",
                                 ::foo,
                                 ::foo::get_logging,
                                 ::datatools::logger::priority)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("description", "Get the logging priority enum value")
          .DR_CLASS_TAG("returned.name", "prio")
          .DR_CLASS_TAG("returned.type", "datatools::logger::priority")
          .DR_CLASS_TAG("returned.description", "the returned logging priority value")

          // Set b:
          .DR_CLASS_VOID_METHOD1("set_b",
                                 ::foo,
                                 ::foo::set_b,
                                 ::foo::dummy_type)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG("description", "Set the b enum value")
          .DR_CLASS_TAG("arg[0].name", "b")
          .DR_CLASS_TAG("arg[0].type", "foo::dummy_type")
          .DR_CLASS_TAG("arg[0].description", "the b value")

          // Get x:
          .DR_CLASS_METHOD_CONST("get_b",
                                 ::foo,
                                 ::foo::get_b,
                                 ::foo::dummy_type)
          .DR_CLASS_TAG_FLAG("builder")
          .DR_CLASS_TAG_FLAG("const")
          .DR_CLASS_TAG("description", "Get the b enum value")
          .DR_CLASS_TAG("returned.name", "b")
          .DR_CLASS_TAG("returned.type", "foo::dummy_type")
          .DR_CLASS_TAG("returned.description", "the returned b value")

          // Unsupported by CAMP: Set sum:
          // .function("set_sum",
          //           boost::function<void(foo&, const std::vector<int> &)>(boost::bind(set_sum_func, _1, _2))
          //           )
          //    boost::function<void(foo&, const std::vector<int> &)>(boost::bind(&foo::set_sum, _1, _2 )))
          //           boost::function<void (foo&, std::vector<int>)>(boost::bind(&foo::set_sum, _1, _2 ))
          //           // boost::function<void (foo&, int)>(boost::bind(&foo::set_sum, _1, _2 ))
          //           )
          // .DR_CLASS_VOID_METHOD1("set_sum",
          //                        ::foo,
          //                        ::foo::set_sum,
          //                        const std::vector<int> &)
          //.DR_CLASS_TAG_FLAG("builder")
          //.DR_CLASS_TAG("description", "Set sum")
          ;
        return;
      }

      // Test
      class X
      {
      public:
        void setA(int a) { std::clog << "a=" << a << std::endl; }
        void setB(std::vector<int> v){std::clog << "v.sz=" << v.size() << std::endl; }
        void setC(const std::vector<int> & v){std::clog << "v.sz=" << v.size() << std::endl; }
      };

      void test0()
      {
        std::cerr << "\ntest0: Begin..." << std::endl;
        boost::function<void(X&, int)> f1(boost::bind(&X::setA, _1, _2 ));
        boost::function<void(X&, std::vector<int>)> f2(boost::bind(&X::setB, _1, _2 ));
        boost::function<void(X&, const std::vector<int> &)> f3(boost::bind(&X::setC, _1, _2 ));

        X x;
        int a = 42;
        f1(x, a);
        std::vector<int> b = {1, 2, 3};
        f2(x, b);
        b.push_back(4);
        f3(x, b);

        {
          int i1 = 12;
          int i2 = 42;
          int r = boost::bind(f0, _1, _2)(i1, i2);
          std::cerr << "test0: r=" << r << std::endl;
          boost::bind(test_func1, _1, _2)(i1, i2);
        }

        {
          std::vector<int> v1 = {1, 2, 3};
          std::vector<int> v2 = {1, 2, 3};
          boost::bind(test_func2, _1, _2)(boost::cref(v1), boost::cref(v2));
        }

        {
          foo f;
          set_sum_func(f, b);
          f.print();
          f.reset();
          f.print();
          b = {1, 2, 3, 4, 3, 2, 1};
          boost::bind(set_sum_func, _1, _2)(f, boost::cref(b));
          f.print();
        }

        {
          std::cerr << "test0: foo::set_sum:" << std::endl;
          foo f;
          b = {10, 20, 30};
          boost::bind(&foo::set_sum, _1, _2)(f, boost::cref(b));
          f.print();
        }

        std::cerr << "test0: End." << std::endl << std::endl;
        return;
      }

      class Xinit
      {
        Xinit() { test0(); }
        static Xinit _instance;
      };
      Xinit Xinit::_instance;

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_TESTING_UI_FOO_REFLECT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
