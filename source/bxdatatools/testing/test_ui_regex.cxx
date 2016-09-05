// Copyright Vladimir Prus 2002-2004.
// Copyright F. Mauger 2016.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// This example shows how a user-defined class can be parsed using
// specific mechanism -- not the iostream operations used by default.
//
// A new class 'magic_number' is defined and the 'validate' method is overloaded
// to validate the values of that class using Boost.Regex.
// To test, run
//
//    regex -m 123-456
//    regex -m 123-4567
//
// The first invocation should output:
//
//   The magic is "456"
//
// and the second invocation should issue an error message.
//
// New magic_real and magic_real_with_unit classes are also processed with their own validation functions.

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

/* Define a completely non-sensical class. */
struct magic_number {
public:
  magic_number(int n_) : n(n_) {}
  int n;
};

/* Define a completely non-sensical class. */
struct magic_real {
public:
  magic_real(double r_) : r(r_) {}
  double r;
};

/* Define a completely non-sensical class. */
struct magic_real_with_unit {
public:
  magic_real_with_unit(double r_ = 0.0, std::string u_ = "") : r(r_) , u(u_) {}
  double r;
  std::string u;
};

/* Overload the 'validate' function for the user-defined class.
   It makes sure that value is of form XXX-XXX
   where X are digits and converts the second group to an integer.
   This has no practical meaning, meant only to show how
   regex can be used to validate values.
*/
void validate(boost::any& v,
              const std::vector<std::string>& values,
              magic_number*, int)
{
  static boost::regex r("\\d\\d\\d-(\\d\\d\\d)");

  // Make sure no previous assignment to 'a' was made.
  boost::program_options::validators::check_first_occurrence(v);
  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const std::string& s = boost::program_options::validators::get_single_string(values);

  // Do regex match and convert the interesting part to int.
  boost::smatch match;
  if (boost::regex_match(s, match, r)) {
    v = boost::any(magic_number(boost::lexical_cast<int>(match[1])));
  } else {
    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
  }
}

/* Overload the 'validate' function for the user-defined class.
   It makes sure that value is of form XXX-XXX
   where X are digits and converts the second group to an integer.
   This has no practical meaning, meant only to show how
   regex can be used to validate values.
*/
void validate(boost::any& v,
              const std::vector<std::string>& values,
              magic_real*, int)
{
  // Make sure no previous assignment to 'v' was made.
  boost::program_options::validators::check_first_occurrence(v);
  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const std::string& s = boost::program_options::validators::get_single_string(values);

  double r = 0.0;
  std::istringstream in(s);
  in >> r;
  if (in) {
    v = boost::any(magic_real(r));
  } else {
    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
  }
}

/* Overload the 'validate' function for the user-defined class.
   It makes sure that value is of form XXX-XXX
   where X are digits and converts the second group to an integer.
   This has no practical meaning, meant only to show how
   regex can be used to validate values.
*/
void validate(boost::any& v,
              const std::vector<std::string>& values,
              magic_real_with_unit*, int)
{
  // Make sure no previous assignment to 'v' was made.
  boost::program_options::validators::check_first_occurrence(v);
  // Extract the first string from 'values'. If there is more than
  // one string, it's an error, and exception will be thrown.
  const std::string& s = boost::program_options::validators::get_single_string(values);

  typedef boost::tokenizer<boost::char_separator<char>> tokenizer_type;
  boost::char_separator<char> sep(" ~_");
  tokenizer_type tokens(s, sep);
  std::string value_token;
  std::string unit_token;

  int count = 0;
  for (tokenizer_type::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter, ++count) {
    std::string tok = *tok_iter;
    if (value_token.empty()) {
      value_token = *tok_iter;
    } else if (unit_token.empty()) {
      unit_token = *tok_iter;
    } else {
      throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
    }
  }
  if (count != 2) {
    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
   }
  double val = 0.0;
  std::istringstream in(value_token);
  in >> val;
  if (!in) {
    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
  }
  v = boost::any(magic_real_with_unit(val, unit_token));
}


int main(int ac, char* av[])
{
  try {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce a help screen")
      ("version,v", "print the version number")
      ("magic,m", boost::program_options::value<magic_number>(),
       "magic value (in NNN-NNN format)")
      ("real,r", boost::program_options::value<magic_real>(),
       "magic real")
      ("real-unit,u", boost::program_options::value<magic_real_with_unit>(),
       "magic real with unit")
      ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

    if (vm.count("help")) {
      std::cout << "Usage: regex [options]\n";
      std::cout << desc;
      return 0;
    }
    if (vm.count("version")) {
      std::cout << "Version 1.\n";
      return 0;
    }
    if (vm.count("magic")) {
      std::cout << "The magic is \""
                << vm["magic"].as<magic_number>().n << "\"\n";
    }
    if (vm.count("real")) {
      std::cout << "The real is \""
                << vm["real"].as<magic_real>().r << "\"\n";
    }
    if (vm.count("real-unit")) {
      std::cout << "The real is \""
                << vm["real-unit"].as<magic_real_with_unit>().r
                << "\" with unit = \""
                << vm["real-unit"].as<magic_real_with_unit>().u
                << "\"\n";
    }
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
