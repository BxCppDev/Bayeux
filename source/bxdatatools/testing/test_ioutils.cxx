// test_ioutils.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <datatools/ioutils.h>
#include <datatools/clhep_units.h>

void test_indent();
void test_indenter();
void test_to_binary();
void test_boolean();
void test_integer();
void test_real();
void test_real_with_unit();
void test_string();

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'ioutils' class..." << std::endl;

    bool color = false;

    // int iarg = 1;
    // while (iarg <  argc_) {
    //   std::string arg = argv_[iarg];
    //   iarg++;
    // }

    test_indent();
    test_indenter();
    test_to_binary();
    test_boolean();
    test_integer();
    test_real();
    test_real_with_unit();
    test_string();

  } catch ( std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_indent()
{
  std::clog << "\ntest_indent:\n";

  std::clog << datatools::io::indent() << datatools::io::warning << "This is a warning print ! "
            << datatools::io::normal << std::endl;
  datatools::io::indent()++;
  std::clog << datatools::io::indent() << datatools::io::notice << "This is a notice print ! "
            << datatools::io::normal << std::endl;
  std::clog << datatools::io::indent() << datatools::io::debug << "This is a debug print ! "
            << datatools::io::normal << std::endl;
  datatools::io::indent()++;
  std::clog << datatools::io::indent() << datatools::io::normal << datatools::io::notice << "More print ! "
            << datatools::io::normal << std::endl;
  std::clog << datatools::io::indent() << datatools::io::normal << datatools::io::notice << "Yet another print ! "
            << datatools::io::normal << std::endl;
  datatools::io::indent()--;
  std::clog << datatools::io::indent() << datatools::io::normal << datatools::io::notice << "Back to margin ! "
            << datatools::io::normal << std::endl;
  datatools::io::indent()(0);
  std::clog << datatools::io::indent() << datatools::io::devel << "Done."
            << datatools::io::normal << std::endl;
  datatools::io::indent()(6);
  std::clog << datatools::io::indent() << datatools::io::error << "Ooops !"
            << datatools::io::normal << std::endl;
  return;
}


void test_indenter()
{
  std::clog << "\ntest_indenter:\n";
  datatools::io::indenter ind;
  ind++;
  std::cout << ind << "Hello, World !" << std::endl;
  ind++;
  std::cout << ind << "How do you feel ?" << std::endl;
  ind(6);
  std::cout << ind << "Well, I'm fine." << std::endl;
  ind--;
  std::cout << ind << "Good ! Let's go !" << std::endl;
  ind--;
  std::cout << ind << "Back !" << std::endl;
  ind--;
  std::cout << ind << "Back !" << std::endl;
  ind--;
  std::cout << ind << "Back !" << std::endl;
  ind--;
  std::cout << ind << "And back again !" << std::endl;
  ind--;
  std::cout << ind << "The end." << std::endl;
  return;
}

void test_to_binary()
{
  std::clog << "\ntest_to_binary:\n";
  uint32_t value = 54;
  std::cout << "value (dec) = " << value << std::endl;
  std::cout << "value (bin) = " << datatools::io::to_binary (value) << std::endl << std::endl;
  value = 55;
  std::cout << "value (dec) = " << value << std::endl;
  std::cout << "value (bin) = " << datatools::io::to_binary (value) << std::endl;
  return;
}

void test_boolean()
{
  std::clog << "\ntest_boolean:\n";

  std::vector<std::string> reprs = { "0" , "1", "T", "t" , "f ", "false", "yes", "no", "bad", "2", "true", "1 34", "012" };
  for (auto repr : reprs) {
    uint32_t reader_flags = 0;
    bool value;
    if (datatools::io::read_boolean(repr, value, reader_flags)) {
      std::clog << "Boolean from  '" << repr << "' = ";
      datatools::io::write_boolean(std::clog, value, true);
      std::clog << "\n";
    } else {
      std::cerr << "Cannot read a boolean from '" << repr << "'.\n";
    }
  }
  return;
}

void test_integer()
{
  std::clog << "\ntest_integer:\n";

  std::vector<std::string> reprs = { "0" , "1", "2", "-1" , "false", "1.3", "bad" };
  for (auto repr : reprs) {
    uint32_t reader_flags = 0;
    int value;
    if (datatools::io::read_integer(repr, value, reader_flags)) {
      std::clog << "Integer from  '" << repr << "' = ";
      datatools::io::write_integer(std::clog, value);
      std::clog << "\n";
    } else {
      std::cerr << "Cannot read an integer from '" << repr << "'.\n";
    }
  }
  return;
}

void test_real()
{
  std::clog << "\ntest_real:\n";

  std::vector<std::string> reprs = { "0" , "1.2", "1.2e+3", "nan", "-inf", "inf", "NaN" , "+infinity", "-3.14159", "bad", "01.2", "0.12", "2.3 mm" };
  for (auto repr : reprs) {
    uint32_t reader_flags = 0;
    double value;
    bool normal_value = false;
    if (datatools::io::read_real_number(repr,
                                        value,
                                        normal_value, reader_flags)) {
      std::clog << "Real from  '" << repr << "' = ";
      datatools::io::write_real_number(std::clog, value);
      std::clog << "\n";
    } else {
      std::cerr << "Cannot read a real from '" << repr << "'.\n";
    }
  }
  return;
}

void test_real_with_unit()
{
  std::clog << "\ntest_real_with_unit:\n";
  {
    std::clog << "Real value = ";
    double length = 0.142 * CLHEP::m;
    datatools::io::write_real_number(std::clog,
                                     length,
                                     15,
                                     "mm",
                                     "length");
    std::clog << std::endl;

    std::clog << "Real value = ";
    uint32_t writer_flags
      = datatools::io::writer_strict_quoting
      | datatools::io::writer_tilda_sep;
    datatools::io::write_real_number(std::clog,
                                     length,
                                     15,
                                     "mm",
                                     "length",
                                     1.0,
                                     writer_flags);
    std::clog << std::endl;
  }
  return;
}

void test_string()
{
  std::clog << "\ntest_string:\n";
  std::vector<std::string> reprs = { "\"abc\"" , "'abc'", "'a b c'", "abc", "a b c", " a b c", "\"a b c\"", "\" a b c\"", "", "''", "'", "\"'\"", "\"'", "' '", "\"\"", "\"   \"" };
  for (auto repr : reprs) {
    uint32_t reader_flags = 0;
    std::string value;
    if (datatools::io::read_quoted_string(repr,value)) {
      std::clog << "String from [" << repr << "] = ";
      datatools::io::write_quoted_string(std::clog, value);
      std::clog << "\n";
    } else {
      std::cerr << "Cannot read a quoted string from [" << repr << "].\n";
    }
  }
  return;
  return;
}
