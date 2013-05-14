// mygsl::linear_system_solver.cc

#include <mygsl/linear_system_solver.h>

#include <cmath>
#include <stdexcept>

namespace mygsl {

  using namespace std;

  linear_system_solver::linear_system_solver (size_t dimension_)
  {
    _dimension_ = 0;
    _p_ = 0;
    _x_ = 0;
    _vva_ = 0;
    _vvb_ = 0;
    if (dimension_ > 0) _init_ (dimension_);
    return;
  }

  linear_system_solver::~linear_system_solver ()
  {
    _reset_ ();
    return;
  }

  void linear_system_solver::_reset_ ()
  {
    if (_x_) gsl_vector_free (_x_);
    if (_p_) gsl_permutation_free (_p_);

    gsl_vector_free (_vva_);
    gsl_vector_free (_vvb_);
    _vva_ = 0;
    _vvb_ = 0;
    //_va_.clear ();
    //_vb_.clear ();
    _dimension_ = 0;
    return;
  }

  void linear_system_solver::_init_ (size_t dimension_)
  {
    if (dimension_ < 1)
      {
        throw domain_error ("linear_system_solver::_init_: Invalid dimension");
        return;
      }
    _dimension_ = dimension_;
    //_va_.assign (_dimension_ * _dimension_, 0.0);
    //_vb_.assign (_dimension_, 0.0);
    _vva_ = gsl_vector_calloc (_dimension_ * _dimension_);
    _vvb_ = gsl_vector_calloc (_dimension_);
    _m_ = gsl_matrix_view_array (_vva_->data, _dimension_, _dimension_);
    _b_ = gsl_vector_view_array (_vvb_->data, _dimension_);
    _p_ = gsl_permutation_alloc (_dimension_);
    _x_ = gsl_vector_alloc (_dimension_);
    return;
  }

  bool linear_system_solver::is_initialized () const
  {
    return (_dimension_ > 0);
  }

  size_t linear_system_solver::get_dimension () const
  {
    return _dimension_;
  }

  int linear_system_solver::solve (const vector<double> & a_,
                                   const vector<double> & b_,
                                   vector<double> & x_)
  {
    bool debug = false;
    // checks:
    size_t dim = b_.size ();
    if (a_.size () != (dim * dim))
      {
        std::cerr << "ERROR: linear_system_solver::solve: Invalid system dimensions!" << endl;
        return EXIT_FAILURE;
      }
    if (dim != _vvb_->size)
      {
        std::clog << "WARNING: linear_system_solver::solve: Redimensioning system!" << endl;
        _reset_ ();
        _init_ (dim);
      }
    // input values:
    for (int i = 0; i < a_.size (); i++)
      {
        gsl_vector_set (_vva_, (size_t) i, a_[i]);
      }
    for (int i = 0; i < b_.size (); i++)
      {
        gsl_vector_set (_vvb_, (size_t) i, b_[i]);
      }
    // computing:
    int s;
    if (gsl_linalg_LU_decomp (&_m_.matrix, _p_, &s))
      {
        std::cerr << "ERROR: linear_system_solver::solve: Cannot do LU decomposition!" << endl;
        return EXIT_FAILURE;
      }
    if (gsl_linalg_LU_solve (&_m_.matrix, _p_, &_b_.vector, _x_))
      {
        std::cerr << "ERROR: linear_system_solver::solve: Cannot do LU solving!" << endl;
        return EXIT_FAILURE;
      }
    if (debug)
      {
        clog << "DEBUG: linear_system_solver::solve: x = (";
        for (int i = 0; i < _dimension_; i++)
          {
            clog << ' ' << gsl_vector_get (_x_, i);
          }
        clog << ')' << endl;
      }
    // output values:
    x_.reserve (_dimension_);
    double check = 0.0;
    for (int i = 0; i < _dimension_; i++)
      {
        x_[i] = gsl_vector_get (_x_, i);
        check += x_[i];
      }
    if (! isnormal (check))
      {
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }

  /* Solve:
   *
   *  a1 x + b1 y = c1
   *  a2 x + b2 y = c2
   *
   */
  bool linear_system_2x2_solve (double a1_, double b1_, double c1_,
                                double a2_, double b2_, double c2_,
                                double & x_, double & y_)
  {
    double det = b1_ * a2_ - a1_ * b2_;
    if (abs (det) < 1.e-15)
      {
        return false;
      }
    x_ = (b1_ * c2_ - c1_ * b2_) / det;
    y_ = (c1_ * a2_ - c2_ * a1_) / det;
    return true;
  }

}

// end of mygsl::linear_system_solver.cc
