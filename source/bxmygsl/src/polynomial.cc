// mygsl::polynomial.cc

#include <mygsl/polynomial.h>

#include <stdexcept>
#include <sstream>
#include <cmath> // for std::abs

namespace mygsl {

  using namespace std;

  size_t polynomial::get_ncoeffs () const
  {
    return _c_.size ();
  }
  
  size_t polynomial::get_degree () const
  {
    return get_ncoeffs () - 1;
  }
  
  double polynomial::get_coeff (size_t i_) const
  {
    return _c_.at (i_);
  }
  
  void polynomial::set_coeff (size_t i_, double c_)
  {
    if (i_ > get_degree ())
      {
        size_t sz = _c_.size ();
        _c_.resize (i_ + 1);
        for (int i = sz; i < _c_.size (); i++)
          {
            _c_[i] = 0.0;
          }
      }
    _c_.at (i_) = c_;
    return;
  }
  
  polynomial::polynomial (size_t degree_)
  {
    _c_.assign (degree_ + 1, 0.0);
    _c_ [degree_] = 1.0;
    return;
  }
  
  polynomial::polynomial (double c0_)
  {
    _c_.assign (1, c0_);
    return;
  }
  
  polynomial::polynomial (double c0_, double c1_)
  {
    _c_.assign (2, 0.0);
    _c_ [0] = c0_;
    _c_ [1] = c1_;
    return;
  }
  
  polynomial::polynomial (double c0_, double c1_, double c2_)
  {
    _c_.assign (3, 0.0);
    _c_ [0] = c0_;
    _c_ [1] = c1_;
    _c_ [2] = c2_;
    return;
  }
  
  polynomial::polynomial (const vector<double> & c_)
  {
    _c_ = c_;
    return;
  }
  
  polynomial::polynomial (const polynomial & p_)
  {
    _c_ = p_._c_;
    return;
  }
  
  polynomial::~polynomial ()
  {
    _c_.clear ();
    return;
  }
   
  double polynomial::eval (double x_) const
  {
    size_t sz = _c_.size ();
    const double * first_arg = 0;
    if (sz > 0)
      {
        first_arg = &(_c_[0]);
      }
    return gsl_poly_eval (first_arg, sz, x_);
  }

  void polynomial::print (ostream & out_, int format_, bool eol_) const
  {
    string prod_sym = "×";
    string exp_sym  = "^";
    prod_sym = "*";
    exp_sym  = "**";
    if (format_ == 1)
      {
        prod_sym = "×";
        exp_sym  = "^";
      }
    bool first = true;
    for (int deg = 0; deg <= get_degree (); deg++)
      {
        if (_c_[deg] == 0.0)
          {
            continue;
          }
        else
          {
            if (! first)
              {
                out_ << (_c_[deg] > 0? " + ": " - ");
              }
            else
              {
                out_ << (_c_[deg] > 0? "": " - ");
                first = false;      
              }
            out_.unsetf (ios::showpos);
            // 2011-06-16 FM: add std::abs to remove template ambiguity for some compiler :
            out_ << std::abs(_c_[deg]);
            if (deg >= 1)
              {
                out_ << ' ' << prod_sym << ' ' << 'X';
              } 
            if (deg >= 2)
              {
                out_ << exp_sym << deg;
              } 
          }
      }
    if (eol_) out_ << endl;
  }

  bool polynomial::solve_linear (double p0_, 
                                 double p1_, 
                                 size_t & nsols_,
                                 double & x0_)
  {
    if (p1_ == 0.0)
      {
        if (p0_ == 0.0)
          {
            nsols_ = -1;
            return false;
          }
        else
          {
            nsols_ = 0;
            return false;
          }
        x0_ = GSL_NAN;
      }
    x0_ = -p0_ / p1_;
    nsols_ = 1;
    return true;
  }

  bool polynomial::solve_quadratic (double p0_, 
                                    double p1_, 
                                    double p2_, 
                                    size_t & nsols_,
                                    double & x0_, double &  x1_)
  {
    if (p2_ == 0)
      {
        x1_ = GSL_NAN;
        return solve_linear (p0_, p1_, nsols_, x0_);    
      }
    nsols_ = (size_t) gsl_poly_solve_quadratic (p2_, p1_, p0_, &x0_, &x1_);
    if (nsols_ < 1)
      {
        x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
        x1_ = GSL_NAN;
      }
    return nsols_ > 0;
  } 

  bool polynomial::solve_cubic (double p0_, 
                                double p1_, 
                                double p2_, 
                                size_t & nsols_,
                                double & x0_, 
                                double & x1_, 
                                double & x2_)
  {
    nsols_ = (size_t) gsl_poly_solve_cubic (p2_, p1_, p0_, &x0_, &x1_, &x2_);
    if (nsols_ < 1)
      {
        x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
        x1_ = GSL_NAN;
      }
    if (nsols_ < 3)
      {
        x2_ = GSL_NAN;
      }
    return nsols_ > 0;
  }

  bool polynomial::solve_cubic (double p0_, 
                                double p1_, 
                                double p2_,
                                double p3_,
                                size_t & nsols_,
                                double & x0_, 
                                double & x1_, 
                                double & x2_)
  {
    if (p3_ == 0)
      {
        x2_ = GSL_NAN;  
        return solve_quadratic (p0_, p1_, p2_, nsols_, x0_, x1_);
      }
    nsols_ = (size_t) solve_cubic (p2_ / p3_, p1_ / p3_, p0_ / p3_, nsols_, x0_, x1_, x2_);
    if (nsols_ < 1)
      {
        x0_ = GSL_NAN;
      }
    if (nsols_ < 2)
      {
        x1_ = GSL_NAN;
      }
    if (nsols_ < 3)
      {
        x2_ = GSL_NAN;
      }
    return nsols_ > 0;
  }

  /**************************************/

  void polynomial::solver::_init_ (size_t sz_)
  {
    _status_ = GSL_EFAILED;
    if (_ws_ != 0)
      {
        if (_sz_ >= sz_)
          {
            return;
          }
        else
          {
            _reset_ ();
          }
      }
    if (sz_ > 1)
      {
        _ws_ = gsl_poly_complex_workspace_alloc (sz_);
        _sz_ = sz_;
      }
    return;
  }

  void polynomial::solver::_reset_ ()
  {
    if (_ws_ != 0)
      {
        gsl_poly_complex_workspace_free (_ws_);
        _ws_ = 0;
        _sz_ = 0;
      }
    _status_ = GSL_EFAILED;
    return;
  }

  polynomial::solver::solver (size_t sz_)
  {
    _status_ = GSL_EFAILED;
    _sz_ = 0;
    _ws_ = 0;
    _init_ (sz_);
    return;
  }

  polynomial::solver::~solver ()
  {
    _reset_ ();
    return;
  }

  bool polynomial::solver::solve (const polynomial & p_)
  {
    size_t deg = p_.get_degree ();
    _init_ (deg + 1);
    {
      throw logic_error ("polynomial::solver::solve:  Not implemented yet!");
    }
    return _status_ == GSL_SUCCESS;
  }

}

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::polynomial.cc
