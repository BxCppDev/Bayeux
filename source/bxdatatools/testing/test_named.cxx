// -*- mode: c++; -*-
// test_named.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

// This Project:
#include <datatools/i_named.h>

struct foo : public datatools::i_named
{
private:
  std::string  _prefix_;
  std::size_t  _id_;

public:
  foo(const std::string & prefix_ , std::size_t id_) :
    _prefix_(prefix_), _id_(id_)
  {
  }
  virtual std::string fetch_name() const
  {
    std::ostringstream name_ss;
    name_ss << _prefix_ << _id_;
    return name_ss.str();
  }
};

struct bar : public datatools::i_named
{
private:
  std::string _name_;

public:
  bar(const std::string & name_ = "") :
    _name_(name_)
  {
  }
  virtual std::string fetch_name() const
  {
    return _name_;
  }
};

int main(int /* argc_ */, char ** /* argv_ */ )
{
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Test of the 'i_named' interface..." << std::endl;

    foo t1("t",1);
    foo t2("t",2);
    foo t3("t",666);

    std::clog << "t1 is named '" << t1.fetch_name() << "'" <<  std::endl;
    std::clog << "t2 is named '" << t2.fetch_name() << "'" <<  std::endl;
    std::clog << "t3 is named '" << t3.fetch_name() << "'" <<  std::endl;

    bar b1("");
    bar b2("b2");
    bar b3(bar::explicit_anonymous_label());

    std::clog << "b1 is named '" << b1.fetch_name() << "'" <<  std::endl;
    std::clog << "b2 is named '" << b2.fetch_name() << "'" <<  std::endl;
    std::clog << "b3 is named '" << b3.fetch_name() << "'" <<  std::endl;

  } catch (std::exception & x) {
    std::clog << "error: " << x.what () << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
