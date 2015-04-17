// test_i_unary_function.cxx

// Ourselves:
#include <mygsl/i_unary_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>

/* A wrapper function for the std::cos function */
struct Cosine : public mygsl::i_unary_function
{
protected:
  virtual double _eval(double x_) const;
};

double Cosine::_eval(double x_) const
{
  return std::cos(x_);
}

/* A wrapper function for the std::sin function */
struct Sine : public mygsl::i_unary_function
{
protected:
  virtual double _eval(double x_) const;
};

double Sine::_eval(double x_) const
{
  return std::sin(x_);
}

/* A wrapper function for the std::sqrt function */
struct Sqrt : public mygsl::i_unary_function
{
protected:
  virtual bool has_explicit_domain_of_definition() const;
  virtual bool is_in_domain_of_definition(double x_) const;
  virtual double _eval(double x_) const;
};

bool Sqrt::has_explicit_domain_of_definition() const
{
  return true;
}

bool Sqrt::is_in_domain_of_definition(double x_) const
{
  return x_ >= 0.0;
}

double Sqrt::_eval(double x_) const
{
  return std::sqrt(x_);
}

int main(int /* argc_ */, char ** /* argv_ */)
{
  try {

    // Declare 3 functor :
    Cosine c;
    Sine s;
    Sqrt sr;

    // Parameters to write (x,y=f(x)) values in a file :
    std::string datafile = "test_i_unary_function.data";
    double xmin = -10.0;
    double xmax = +10.0;
    unsigned int nsamples = 1000;
    int x_precision = 8;
    int fx_precision = 16;

    // Write sampled values for all functors :
    c.write_ascii_file(datafile, xmin, xmax, nsamples,
                       x_precision, fx_precision,
                       mygsl::i_unary_function::wo_append
                       | mygsl::i_unary_function::wo_data_index);
    s.write_ascii_file(datafile, xmin, xmax, nsamples,
                       x_precision, fx_precision,
                       mygsl::i_unary_function::wo_append
                       | mygsl::i_unary_function::wo_data_index);
    sr.write_ascii_file(datafile, xmin, xmax, nsamples,
                        x_precision, fx_precision,
                        mygsl::i_unary_function::wo_append
                        | mygsl::i_unary_function::wo_data_index
                        | mygsl::i_unary_function::wo_skip_odod);

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
