// mygsl::multi_eval.cc 

#include <mygsl/multi_eval.h>

#include <sstream>
#include <stdexcept>
#include <functional>

namespace mygsl {
  
  size_t multi_eval::get_dimension () const
  {
    return __domains.size ();
  }
  
  multi_eval::multi_eval (size_t dimension_)
  {  
    __domains.assign (dimension_, interval::make_no_limit ());
  }
  
  multi_eval::~multi_eval ()
  {
    __domains.clear ();
  }

  bool multi_eval::is_valid (int i_, double x_) const
  {
    return __domains[i_].is_in (x_);
  }

  bool multi_eval::is_valid (const double * x_) const
  {
    for (int i = 0; i < __domains.size (); i++)
      {
	if (! is_valid (i, x_[i])) return false;
      }
    return true;
  }
  
  interval & multi_eval::get_domain (int i_)
  {
    return __domains[i_];
  }  
  
  const interval & multi_eval::get_domain (int i_) const
  {
    return __domains[i_];
  }  

  double multi_eval::operator () (const double * x_) const
  {
    return evaluate (x_);
  }

  void multi_eval::__check_dimension (size_t dim_) const
  {
    if (dim_ != get_dimension ())
      {
	throw runtime_error ("multi_eval::__check_dimension: Invalid dimension!");
      }
  }
  
  double multi_eval::evaluate (double x_) const
  {
    __check_dimension (1);
    double v[1];
    v[0] = x_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (double x_, double y_) const
  {
    __check_dimension (2);
    double v[2];
    v[0] = x_;
    v[1] = y_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (double x_, double y_, double z_) const
  {
    __check_dimension (3);
    double v[3];
    v[0] = x_;
    v[1] = y_;
    v[2] = z_;
    return evaluate (v);
  }
  
  double multi_eval::evaluate (const vector<double> & v_) const
  {
    const double * first_arg = 0;
    if (v_.size ())
      {
	first_arg = &(v_[0]);
      }
    return evaluate (first_arg);
    //    return evaluate (v_.data ());
  }
  
  double multi_eval::evaluate (const double *  x_) const
  {
    if (! is_valid (x_))
      {
	ostringstream message;
	message << "multi_eval::evaluate: "
		<< "value '" << x_ << "' is out of the evaluation domain!";
	throw runtime_error (message.str ());
      }
    return eval (x_);
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
    return __multi_eval->is_valid (__index, x_);
  }
 
  const double & unary_eval_from_multi::param (int i_) const
  {
    if ((i_ < 0) || (i_ >= __params.size ()))
      {
	ostringstream message;
	message << "unary_eval_from_multi::param: Invalid parameter index!";
	throw runtime_error (message.str ());
      }
    return __params[i_];
  }

  double & unary_eval_from_multi::param (int i_)
  {
    if ((i_ < 0) || (i_ >= __params.size ()))
      {
	ostringstream message;
	message << "unary_eval_from_multi::param: Invalid parameter index!";
	throw runtime_error (message.str ());
      }
    return __params[i_];
  }

  void unary_eval_from_multi::set_index (int i_)
  {
    if ((i_ < 0) || (i_ >= __multi_eval->get_dimension ()))
      {
	ostringstream message;
	message << "unary_eval_from_multi::set_index: Invalid parameter index!";
	throw runtime_error (message.str ());
      }
    __index = i_;
  }

  void unary_eval_from_multi::init (const multi_eval & multi_eval_, int i_, const vector<double> & params_)
  {
    __multi_eval = &multi_eval_;
    set_index (i_);
    if (params_.size () != __multi_eval->get_dimension ())
      {
	ostringstream message;
	message << "unary_eval_from_multi::init: Invalid const parameters dimension ";
	throw runtime_error (message.str ());
      }
    __index = i_;
    __params = params_;
  }
  
  unary_eval_from_multi::unary_eval_from_multi (const multi_eval & multi_eval_, 
						int i_, 
						const vector<double> & params_)
  {
    init (multi_eval_, i_, params_);
  }

  unary_eval_from_multi::unary_eval_from_multi (const multi_eval & multi_eval_, 
						int i_, 
						const double * params_)
  {
    vector<double> v;
    v.assign (multi_eval_.get_dimension (), 0.0);
    for (int i = 0; i < v.size (); i++)
      {
	v[i] = params_[i];
      }
    init (multi_eval_, i_, v);
  }

  unary_eval_from_multi::~unary_eval_from_multi ()
  {
    __params.clear ();
    __multi_eval = 0;
  }
  
  double unary_eval_from_multi::eval (double x_) const
  {
    double backup = __params [__index];
    unary_eval_from_multi * tmp = const_cast<unary_eval_from_multi *>(this);
    tmp->__params[__index] = x_;
    double res = __multi_eval->evaluate (__params);
    tmp->__params [__index] = backup;
    return res;
  }
  
}

// end of mygsl::multi_eval.cc
