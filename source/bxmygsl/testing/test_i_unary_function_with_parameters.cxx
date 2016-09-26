// test_i_unary_function_with_parameters.cxx

// Ourselves:
#include <mygsl/i_unary_function_with_parameters.h>

// Standard library:
#include <cstdlib>
#include <iostream>

// Third party:

// This project:
#include <mygsl/parameter_store.h>

//! \brief A function that depends on external parameters
class my_functor : public mygsl::i_unary_function_with_parameters
{
public:

  //! Default constructor:
  my_functor();

  //! Destructor:
  virtual ~my_functor();

  //! Initialization
  virtual void initialize(const datatools::properties & config_,
                          mygsl::unary_function_dict_type & functors_);

  //! Reset
  virtual void reset();

  //! Check initialization status
  virtual bool is_initialized() const;

  //! Smart printing
  virtual void tree_dump(std::ostream & out_ = std::clog,
                         const std::string & title_  = "",
                         const std::string & indent_ = "",
                         bool inherit_ = false) const;

protected:

  //! Evaluation from parameters
  double _eval_from_parameters(double x_) const;

  //! Update parameter
  void _at_parameters_change();

  //! Set default attributes
  void _set_defaults();

private:

  int _iA_; //!< Index of the 'a' parameter extracted from a store
  int _iB_; //!< Index of the 'b' parameter extracted from a store
  int _iN_; //!< Index of the 'n' parameter extracted from a store
  double _a_;
  double _b_;
  int    _n_;

};

void my_functor::_set_defaults()
{
  _iA_ = mygsl::parameter_store::INVALID_INDEX;
  _iB_ = mygsl::parameter_store::INVALID_INDEX;
  _iN_ = mygsl::parameter_store::INVALID_INDEX;
  datatools::invalidate(_a_);
  datatools::invalidate(_b_);
  _n_ = -1;
  return;
}

bool my_functor::is_initialized() const
{
  return this->i_unary_function_with_parameters::is_initialized()
    && (_iA_ != mygsl::parameter_store::INVALID_INDEX)
    && (_iB_ != mygsl::parameter_store::INVALID_INDEX)
    && (_iN_ != mygsl::parameter_store::INVALID_INDEX);
}

void my_functor::reset()
{
  _set_defaults();
  this->i_unary_function_with_parameters::reset();
  return;
}

void my_functor::_at_parameters_change()
{
  // Parameters:
  if (is_parameter_set(_iA_)) {
    fetch_parameter(_iA_, _a_);
  }
  if (is_parameter_set(_iB_)) {
    fetch_parameter(_iB_, _b_);
  }
  if (is_parameter_set(_iN_)) {
    fetch_parameter(_iN_, _n_);
  }
  std::clog << "DEBUG: my_functor::_at_parameters_change: Ok!" << std::endl;
  return;
}

void my_functor::initialize(const datatools::properties & config_,
                            mygsl::unary_function_dict_type & functors_)
{
  this->i_unary_function_with_parameters::initialize(config_, functors_);

  _iA_ = get_parameter_index("a");
  _iB_ = get_parameter_index("b");
  _iN_ = get_parameter_index("n");
  _at_parameters_change();
  return;
}

my_functor::my_functor()
{
  _set_defaults();
  return;
}

my_functor::~my_functor()
{
  return;
}

double my_functor::_eval_from_parameters(double x_) const
{
  return _a_ * std::pow(x_, _n_) + _b_;
}

void my_functor::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
{
  this->i_unary_function_with_parameters::tree_dump(out_, title_, indent_, true);

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Index for A: " << _iA_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::tag
       << "Index for B: " << _iB_ << std::endl;

  out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
       << "Index for N: " << _iN_ << std::endl;

  return;
}

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::parameter_store ps;
    ps.add("a", datatools::TYPE_REAL, "A");
    ps.add("p0", datatools::TYPE_REAL, "P0");
    ps.add("b", datatools::TYPE_REAL, "B");
    ps.add("n", datatools::TYPE_INTEGER, "N");
    ps.add("k", datatools::TYPE_INTEGER, "K");
    ps.grab("a").set_real_value(2.31);
    ps.grab("p0").set_real_value(-1.23);
    ps.grab("b").set_real_value(0.42);
    ps.grab("n").set_integer_value(2);
    ps.grab("k").set_integer_value(127);
    ps.tree_dump(std::clog, "Parameter store: ");

    my_functor f;
    f.register_requested_parameter("a", datatools::TYPE_REAL);
    f.register_requested_parameter("b", datatools::TYPE_REAL);
    f.register_requested_parameter("n", datatools::TYPE_INTEGER);
    f.set_parameter_store(ps);
    std::clog << f.get_epsilon() << std::endl;
    f.initialize_simple();
    f.tree_dump(std::clog, "My functor with parameters: ");

    {
      double dx = 0.05;
      std::cout << "# my functor with parameters (1):" << std::endl;
      for (double x = 0.0; x <= 2.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << f(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

    ps.grab("a").set_real_value(1.78);
    ps.grab("b").set_real_value(1.33);

    {
      double dx = 0.05;
      std::cout << "# my functor with parameters (2):" << std::endl;
      for (double x = 0.0; x <= 2.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << f(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

    ps.grab("a").set_real_value(1.22);
    ps.grab("b").set_real_value(3.33);

    {
      double dx = 0.05;
      std::cout << "# my functor with parameters (3):" << std::endl;
      for (double x = 0.0; x <= 2.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << f(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
