// test_parameter_store.cxx

// Ourselves:
#include <mygsl/parameter_store.h>

// Standard library:
#include <string>
#include <cstdlib>
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

//! \brief A subscriber object
class subscriber : public mygsl::parameter_store::i_subscriber
{
public:

  //! Set an external parameter store
  void set_parameter_store(const mygsl::parameter_store & store_);

  //! Reset an external parameter store
  void reset_parameter_store();

  //! Default constructor
  subscriber(const std::string & name_);

  /// Destructor
  virtual ~subscriber();

  /// Update parameters action
  virtual void update_parameters();

  /// Print
  void print(std::ostream & out_ = std::clog) const;

private:

  std::string _name_; //!< Name of the subscriber object
  const mygsl::parameter_store * _params_; //!< Address of an external store of parameters
  int    _iA_; //!< Index of the 'a' parameter extracted from the store
  int    _iB_; //!< Index of the 'b' parameter extracted from the store
  int    _iN_; //!< Index of the 'n' parameter extracted from the store
  double _a_;  //!< Fetched value of the 'a' parameter
  double _b_;  //!< Fetched value of the 'b' parameter
  int    _n_;  //!< Fetched value of the 'n' parameter

};

void subscriber::set_parameter_store(const mygsl::parameter_store & store_)
{
  _params_ = &store_;
  const_cast<mygsl::parameter_store*>(_params_)->register_subscriber(*this);
  _iA_ = _params_->get_parameter_index("a");
  _iB_ = _params_->get_parameter_index("b");
  _iN_ = _params_->get_parameter_index("n");
  update_parameters();
  return;
}

void subscriber::reset_parameter_store()
{
  datatools::invalidate(_a_);
  datatools::invalidate(_b_);
  _n_ = -1;
  _iA_ = mygsl::parameter_store::INVALID_INDEX;
  _iB_ = mygsl::parameter_store::INVALID_INDEX;
  _iN_ = mygsl::parameter_store::INVALID_INDEX;
  if (_params_) {
    const_cast<mygsl::parameter_store*>(_params_)->unregister_subscriber(*this);
  }
  _params_ = 0;
  return;
}

subscriber::subscriber(const std::string & name_)
{
  _name_ = name_;
  _iA_ = mygsl::parameter_store::INVALID_INDEX;
  _iB_ = mygsl::parameter_store::INVALID_INDEX;
  _iN_ = mygsl::parameter_store::INVALID_INDEX;
  datatools::invalidate(_a_);
  datatools::invalidate(_b_);
  _n_ = -1;
  return;
}

subscriber::~subscriber()
{
  reset_parameter_store();
  return;
}

void subscriber::update_parameters()
{
  std::clog << "TRACE: subscriber::update_parameters(): Entering..." << std::endl;
  if (_params_->get(_iA_).is_value_set()) {
    std::clog << "TRACE: subscriber::update_parameters(): Fetching 'a'..." << std::endl;
    _a_ = _params_->get(_iA_).to_real();
  }
  if (_params_->get(_iB_).is_value_set()) {
    std::clog << "TRACE: subscriber::update_parameters(): Fetching 'b'..." << std::endl;
    _b_ = _params_->get(_iB_).to_real();
  }
  if (_params_->get(_iN_).is_value_set()) {
    std::clog << "TRACE: subscriber::update_parameters(): Fetching 'n'..." << std::endl;
    _n_ = _params_->get(_iN_).to_integer();
  }
  return;
}

void subscriber::print(std::ostream & out_) const
{
  out_ << "Subscriber '" << _name_ << "' : " << std::endl;
  out_ << "|-- " << "a = (" << _a_ << ") from index=[" << _iA_ << "]" << std::endl;
  out_ << "|-- " << "b = (" << _b_ << ") from index=[" << _iB_ << "]" << std::endl;
  out_ << "`-- " << "n = [" << _n_ << "] from index=[" << _iN_ << "]" << std::endl;
  return;
}

int main(/* int argc_ , char ** argv_ */)
{
  try {

    mygsl::parameter_store ps;
    ps.add("a", datatools::TYPE_REAL, "A");
    ps.add("b", datatools::TYPE_REAL, "B");
    ps.add("n", datatools::TYPE_INTEGER, "N");
    ps.tree_dump(std::clog, "Parameter store: ");

    subscriber s1("subscriber_1");
    s1.set_parameter_store(ps);
    s1.print();

    subscriber s2("subscriber_2");
    s2.set_parameter_store(ps);
    s2.print();

    ps.grab("a").set_real_value(2.31, false);
    ps.grab("b").set_real_value(0.53, false);
    ps.grab("n").set_integer_value(2, false);
    ps.tree_dump(std::clog, "Parameter store: ");
    ps.update();

    s1.print();
    s2.print();

    // Change one parameter:
    ps.grab("a").set_real_value(4.62);
    s1.print();
    s2.print();

    // Detach the store from a subscriber:
    s2.reset_parameter_store();

    // Change another parameter:
    ps.grab("b").set_real_value(1.06);
    s1.print();
    s2.print();

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
