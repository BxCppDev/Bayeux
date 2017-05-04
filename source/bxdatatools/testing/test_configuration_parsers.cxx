/// \file datatools/test_configuration_parsers.cxx

// Ourselves:
#include <datatools/configuration/parsers.h>

// Standard library:
#include <cstdlib>
#include <iostream>

// - Bayeux:
#include <bayeux/bayeux.h>

// This project:
#include <datatools/configuration/variant_object_info.h>

void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test_voi0();

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {

    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test_voi0();

  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
    error_code = EXIT_FAILURE;
  } catch (...){
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

void test0()
{
  std::clog << "\nTest 0:\n";
  std::vector<std::string> tokens = { "", "a", "test", "0a", "_0a", "a_b0", "a.", "a[23]" };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::param_name_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> param name='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}

void test1()
{
  std::clog << "\nTest 1:\n";
  std::vector<std::string> tokens = { "", "a", "reg", "reg:", "0a", "_0a", "a_b0", "a.", "a[23]" };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::registry_name_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> registry name='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}

void test2()
{
  std::clog << "\nTest 2:\n";
  std::vector<std::string> tokens = {
    "",
    "a",
    "ab",
    "ab[56]",
    "foo/bar",
    "foo/bar/baz",
    "foo[0]/bar/baz",
    "foo/bar/baz[0]",
    "foo/bar[0]/baz",
    "foo[42]/bar/baz[12]",
    "foo[42]/bar/baz[12]/test/biz",
  };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::local_param_path_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> local param path='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}


void test3()
{
  std::clog << "\nTest 3:\n";
  std::vector<std::string> tokens = {
    "",
    "a",
    "ab",
    "a/b",
    "a/b/c",
    "a[56]/b",
    "foo/bar[12]",
    "foo/bar/baz[12]",
    "foo[0]/bar/baz",
    "foo/bar/baz[0]/vee",
    "foo[42]/bar/baz[12]/zoo",
  };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::local_variant_path_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> local variant path='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}


void test4()
{
  std::clog << "\nTest 4:\n";
  std::vector<std::string> tokens = {
    "",
    "name:",
    "a",
    "name:ab",
    "name:ab[56]",
    "name:foo/bar",
    "name:foo/bar/baz",
    "name:foo[0]/bar/baz",
    "name:foo/bar/baz[0]",
    "name:foo/bar[0]/baz",
    "name:foo[42]/bar/baz[12]",
    "name:foo[42]/bar/baz[12]/test/biz",
  };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::full_param_path_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> full variant param path='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}

void test5()
{
  std::clog << "\nTest 5:\n";
  std::vector<std::string> tokens = {
    "",
    "name:",
    "a",
    "name:ab",
    "name:ab[56]/c",
    "name:foo/bar/c",
    "name:foo/bar/baz/d",
    "name:foo[0]/bar/baz/ee",
    "name:foo/bar/baz[0]/ee",
    "name:foo/bar[0]/baz/ee",
    "name:foo[42]/bar/baz[12]",
    "name:foo[42]/bar/baz[12]/test/biz/ff",
  };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::full_variant_path_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> full variant path='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}

void test6()
{
  std::clog << "\nTest 6:\n";
  std::vector<std::string> tokens = {
    "",
    "name:a",
    "name:a@grp1",
    "name:a[12]",
    "name:a[12]@grp1",
    "name:a/b/c",
    "name:a/b/c@.grp",
    "name:a[12]/b/c",
    "name:a/b/c[12]@grp1",
    "name:a[12]/b/c[12]@grp1",
    "name:a/b/c[12]@grp1",
    "name:a[12]/b/c[12]/d/e@grp1",
    "name:a[12]/b/c[12]/d@grp1",
  };
  for (const auto & token : tokens) {
    std::string::const_iterator str_iter = token.begin();
    std::string::const_iterator end_iter = token.end();
    datatools::configuration::full_param_value_group_path_instance_grammar<std::string::const_iterator> g;
    std::string name;
    bool res = boost::spirit::qi::phrase_parse(str_iter,
                                               end_iter,
                                               g,
                                               boost::spirit::ascii::space,
                                               name);
    if (!res || str_iter != end_iter) {
      std::clog << "fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "ok   : token='" << token << "' -> full param value group path='" << name << "'" << std::endl;
    }
  }
  std::clog << std::endl;
  return;
}

void test_voi0()
{
  std::clog << "\nTest voi 0:\n";
  std::vector<std::string> tokens = {
    "",
    ":",
    "name",
    "name:",
    ":name",
    "name:a/if_foo",
    "name:a/if_foo/",
    ":a/if_foo",
    "a/if_foo",
    "a[42]/if_foo/bar[12]/if_baz",
    "a",
    "name:a",
    ":a",
    "a@Grp1",
    ":a@Grp1",
    ":@Grp1",
    "@Grp1",
    "a/b/c@Grp1",
    "name:a@Grp1",
    "name_reg:a@Grp1",
    "name_reg:a/b@Grp1",
    "name_reg:a[5]/b/c[12]@Grp1",
  };
  for (const auto & token : tokens) {
    datatools::configuration::variant_object_info voi;
    uint32_t parsing_flags = 0;
    parsing_flags |= datatools::configuration::variant_object_info::PARSE_NO_LOCAL;
    // parsing_flags |= datatools::configuration::variant_object_info::PARSE_DEBUG;
    if (!voi.parse_from_string(token, parsing_flags)) {
      std::clog << "global fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "global ok   : token='" << token << "' -> what='" << voi << "'" << std::endl;
    }
    std::clog << std::endl;
  }
  for (const auto & token : tokens) {
    datatools::configuration::variant_object_info voi;
    uint32_t parsing_flags = 0;
    parsing_flags |= datatools::configuration::variant_object_info::PARSE_NO_GLOBAL;
    // parsing_flags |= datatools::configuration::variant_object_info::PARSE_DEBUG;
    if (!voi.parse_from_string(token, parsing_flags)) {
      std::clog << "local fail : token='" << token << "'" << std::endl;
    } else {
      std::clog << "local ok   : token='" << token << "' -> what='" << voi << "'" << std::endl;
    }
    std::clog << std::endl;
  }
  std::clog << std::endl;
  return;
}
