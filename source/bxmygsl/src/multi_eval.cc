// mygsl::multi_eval.cc 

#include <mygsl/multi_eval.h>

#include <sstream>
#include <stdexcept>
#include <functional>

namespace mygsl {

  using namespace std;
  
  size_t multi_eval::get_dimension () const
  {
    return _domains_.size ();
  }
  
  multi_eval::multi_eval (size_t dimension_)
  {  
    _domains_.assign (dimension_, interval::make_no_limit ());
    return;
  }
  
  multi_eval::~multi_eval ()
  {
    _domains_.clear ();
    return;
  }

  bool multi_eval::is_valid (int i_, double x_) const
  {
    return _domains_[i_].is_in (x_);
  }

  bool multi_eval::is_valid (const double * x_) const
  {
    for (int i = 0; i < _domains_.size (); i++) {
      if (! is_valid (i, x_[i])) return false;
    }
    return true;
  }
  
  interval & multi_eval::get_domain (int i_)
  {
    return _domains_[i_];
  }  
  
  const interval & multi_eval::get_domain (int i_) const
  {
    return _domains_[i_];
  }  

  double multi_eval::operator () (const double * x_) const
  {
    return evaluate (x_);
  }

  void multi_eval::_check_dimension_ (size_t dim_) const
  {
    if (dim_ != get_dimension ()) {
      throw std::logic_error ("mygsl::multi_eval::_check_dimension_: Invalid dimension!");
    }
    return;
  }
  
  double multi_eval::evaluate (double x_) const
  {
    _check_dimension_ (1);
    double v[1];
    v[0] = x_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (double x_, double y_) const
  {
    _check_dimension_ (2);
    double v[2];
    v[0] = x_;
    v[1] = y_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (double x_, double y_, double z_) const
  {
    _check_dimension_ (3);
    double v[3];
    v[0] = x_;
    v[1] = y_;
    v[2] = z_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (const vector<double> & v_) const
  {
    const double * first_arg = 0;
    if (v_.size ()) {
      first_arg = &(v_[0]);
    }
    return evaluate (first_arg);
    //    return evaluate (v_.data ());
  }
  
  double multi_eval::evaluate (const double *  x_) const
  {
    if (! is_valid (x_)) {
      std::ostringstream message;
      message << "mygsl::multi_eval::evaluate: "
              << "value '" << x_ << "' is out of the evaluation domain!";
      throw std::domain_error (message.str ());
    }
    return _eval(x_);
  }

  // static:
  double multi_eval::g_function (const double * x_, void * functor_)
  {
    const multi_eval * an_eval = static_cast<const multi_eval *>(functor_);
    return (*an_eval) (x_);
  };

  /***************************************/

  bool unary_eval_from_multi::is_valid (double x_) const
  {
    return _multi_eval_->is_valid (_index_, x_);
  }
 
  const double & unary_eval_from_multi::param (int i_) const
  {
    if ((i_ < 0) || (i_ >= _params_.size ())) {
      ostringstream message;
      message << "mygsl::unary_eval_from_multi::param: Invalid parameter index!";
      throw domain_error (message.str ());
    }
    return _params_[i_];
  }

  double & unary_eval_from_multi::param (int i_)
  {
    if ((i_ < 0) || (i_ >= _params_.size ())) {
      ostringstream message;
      message << "mygsl::unary_eval_from_multi::param: Invalid parameter index!";
      throw domain_error (message.str ());
    }
    return _params_[i_];
  }

  void unary_eval_from_multi::set_index (int i_)
  {
    if ((i_ < 0) || (i_ >= _multi_eval_->get_dimension ())) {
      ostringstream message;
      message << "mygsl::unary_eval_from_multi::set_index: Invalid parameter index!";
      throw domain_error (message.str ());
    }
    _index_ = i_;
    return;
  }

  void unary_eval_from_multi::init (const multi_eval & multi_eval_, int i_, const vector<double> & params_)
  {
    _multi_eval_ = &multi_eval_;
    set_index (i_);
    if (params_.size () != _multi_eval_->get_dimension ()){
      ostringstream message;
      message << "mygsl::unary_eval_from_multi::init: Invalid const parameters dimension ";
      throw domain_error (message.str ());
    }
    _index_ = i_;
    _params_ = params_;
    return;
  }
  
  unary_eval_from_multi::unary_eval_from_multi (const multi_eval & multi_eval_, 
                                                int i_, 
                                                const vector<double> & params_)
  {
    init (multi_eval_, i_, params_);
    return;
  }

  unary_eval_from_multi::unary_eval_from_multi (const multi_eval & multi_eval_, 
                                                int i_, 
                                                const double * params_)
  {
    vector<double> v;
    v.assign (multi_eval_.get_dimension (), 0.0);
    for (int i = 0; i < v.size (); i++) {
      v[i] = params_[i];
    }
    init (multi_eval_, i_, v);
    return;
  }

  unary_eval_from_multi::~unary_eval_from_multi ()
  {
    _params_.clear ();
    _multi_eval_ = 0;
    return;
  }
  
  double unary_eval_from_multi::_eval (double x_) const
  {
    double backup = _params_ [_index_];
    unary_eval_from_multi * tmp = const_cast<unary_eval_from_multi *>(this);
    tmp->_params_[_index_] = x_;
    double res = _multi_eval_->evaluate (_params_);
    tmp->_params_ [_index_] = backup;
    return res;
  }
  
}

// end of mygsl::multi_eval.cc
