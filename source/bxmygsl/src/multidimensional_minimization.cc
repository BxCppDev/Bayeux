// multidimensional_minimization.cc

// Ourselves:
#include <mygsl/multidimensional_minimization.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>

namespace mygsl {

  const size_t multidimensional_minimization::DEFAULT_MAX_ITER    = 1000;
  const size_t multidimensional_minimization::DEFAULT_MODULO_ITER = 100;
  const double multidimensional_minimization::DEFAULT_EPSABS      = 1.e-3;
  const double multidimensional_minimization_system::DEFAULT_OUT_OF_LIMIT_SLOPE = 1.e10;

  void multidimensional_minimization_system::set_numeric_eval_df (bool use_)
  {
    _numeric_eval_df_ = use_;
    return;
  }

  bool multidimensional_minimization_system::is_numeric_eval_df () const
  {
    return _numeric_eval_df_;
  }

  multidimensional_minimization_system::multidimensional_minimization_system (double slope_,
                                                                              bool use_numeric_eval_)
  {
    _out_of_limit_slope_ = slope_;
    if (slope_ <= 0) {
      _out_of_limit_slope_ = DEFAULT_OUT_OF_LIMIT_SLOPE;
    }
    _numeric_eval_df_ = use_numeric_eval_;
    return;
  }

  multidimensional_minimization_system::~multidimensional_minimization_system ()
  {
    return;
  }

  double multidimensional_minimization_system::func_eval_f_MR  (double x_, void * params_)
  {
    func_eval_f_param * fefp = static_cast<func_eval_f_param *>(params_);
    int free_param_index = fefp->free_param_index;
    multidimensional_minimization_system * sys = fefp->sys;

    // backup former 'x' value:
    double save_x = sys->get_free_param (free_param_index).get_value ();

    // enforce 'x' value:
    sys->grab_free_param (free_param_index).set_value_no_check (x_);
    sys->init_params_values ();

    double result = std::numeric_limits<double>::quiet_NaN ();
    DT_THROW_IF (sys->_eval_f_MR_(result) != 0, std::logic_error, "Cannot evaluate function!");

    // restore former 'x' value:
    sys->grab_free_param (free_param_index).set_value_no_check (save_x);
    sys->init_params_values ();
    return result;
  }

  int multidimensional_minimization_system::_eval_f_MR_ (double & f_)
  {
    bool local_debug = false;
    int code = _eval_f (f_);
    if (code != 0) {
      return code;
    }

    // search for 'out of limits' free parameters:
    double slope = _out_of_limit_slope_;
    bool dump = false;
    for (size_t i = 0; i < get_number_of_free_params (); i++) {
      double dist = get_free_param (i).get_dist_to_limit ();
      double step = get_free_param (i).get_step ();
      // add arbitrary huge value:
      double df = slope * dist / step;
      if (local_debug) {
        if (df != 0.0) {
          DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "out of limits == " << get_free_param (i).get_name () << " : Delta(f) == " << df);
          dump = true; //print_status (clog);
        }
      }
      f_ += df;
    }

    // search for 'out of limits' auto parameters:
    for (size_t i = 0; i < get_number_of_auto_params (); i++) {
      double dist = get_auto_param (i).get_dist_to_limit ();
      double step = get_auto_param (i).get_step ();
      // add arbitrary huge value:
      double df = slope * dist / step;
      if (local_debug) {
        if (df != 0.0) {
          DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "out of limits == " << get_auto_param (i).get_name () << " : Delta(f) == " << df);
          dump = true; //print_status (clog);
        }
      }
      f_ += df;
    }
    if (dump) print_status (std::clog);
    return code;
  }

  int multidimensional_minimization_system::_eval_df_MR_ (double * gradient_)
  {
    int code = _eval_df (gradient_);
    if (code != 0) {
      return code;
    }

    // search for 'out of limits' free parameters:
    for (size_t i = 0; i < get_number_of_free_params (); i++) {
      double sign = get_free_param (i).get_sign_limit ();
      // add arbitrary derivative value:
      gradient_[i] += sign * _out_of_limit_slope_;
    }

    // search for 'out of limits' auto parameters:
    for (size_t i = 0; i < get_number_of_auto_params (); i++) {
      double sign = get_auto_param (i).get_sign_limit ();
      // add arbitrary derivative value:
      gradient_[i] += sign * _out_of_limit_slope_;
    }

    return code;
  }

  int multidimensional_minimization_system::_numerical_eval_df_MR_ (double * gradient_)
  {
    for (size_t i = 0; i < get_number_of_free_params (); i++) {
      double save_value = get_free_param (i).get_value ();
      double x = save_value;
      double h = get_free_param (i).get_step ();
      double df = 0.0;
      double df_err = 0.0;
      // Do not use the return type as it is always zero (see GSL deriv.c source file)
      // int res_deriv = GSL_SUCCESS;
      gsl_function func;
      func_eval_f_param fefp (i, *this);
      func.params = &fefp;
      func.function = multidimensional_minimization_system::func_eval_f_MR;
      if (get_free_param (i).is_in_safe_range ()) {
        // res_deriv =
        gsl_deriv_central (&func, x, h, &df, &df_err);
      } else if (get_free_param (i).is_in_range_but_close_to_min ()) {
        // res_deriv =
        gsl_deriv_forward (&func, x, h, &df, &df_err);
      } else if (get_free_param (i).is_in_range_but_close_to_max ()) {
        // res_deriv =
        gsl_deriv_backward (&func, x, h, &df, &df_err);
      } else {
        df = 0.0;
      }
      gradient_[i] = df;
    }
    return 0;
  }

  int multidimensional_minimization_system::_eval_df (double* /*gradient_*/)
  {
    datatools::logger::priority p= datatools::logger::PRIO_ERROR;
    DT_LOG_ERROR(p, "The 'multidimensional_minimization_system::_eval_df' method ");
    DT_LOG_ERROR(p, "must be overloaded for your own class !");
    DT_LOG_ERROR(p, "If not, you should consider to activate the ");
    DT_LOG_ERROR(p, "'use_numeric_eval_df(true)' method in order to perform");
    DT_LOG_ERROR(p, "automatically the numerical estimation of the derivatives");
    return 1;
  }

  double multidimensional_minimization_system::get_out_of_limit_slope () const
  {
    return _out_of_limit_slope_;
  }

  int multidimensional_minimization_system::eval_f (const double * x_,
                                                    double & f_)
  {
    from_double_star (x_, get_number_of_free_params ());
    int code = _eval_f_MR_ (f_);
    return code;
  }

  int multidimensional_minimization_system::eval_df (const double * x_,
                                                     double * gradient_)
  {
    from_double_star (x_, get_number_of_free_params ());
    int code = 0;
    if (! _numeric_eval_df_) {
      code = _eval_df_MR_ (gradient_);
    } else {
      code = multidimensional_minimization_system::_numerical_eval_df_MR_ (gradient_);
    }
    return code;
  }

  int multidimensional_minimization_system::eval_fdf (const double * x_,
                                                      double & f_,
                                                      double * gradient_)
  {
    int code = eval_f (x_, f_);
    if (code != 0) {
      return code;
    }
    return eval_df (x_, gradient_);
  }


  int multidimensional_minimization_system::init_params_values ()
  {
    //lock_params ();
    multiparameter_system::init ();
    return 0;
  }

  void multidimensional_minimization_system::to_double_star (double * pars_,
                                                             size_t dimension_) const
  {
    const size_t fd = get_number_of_free_params ();
    DT_THROW_IF (dimension_ != fd, std::range_error, "Invalid dimension!");
    for (size_t i = 0; i < fd; i++) {
      pars_[i] = get_free_param (i).get_value ();
    }
    return;
  }

  void multidimensional_minimization_system::from_double_star (const double * pars_,
                                                               size_t dimension_)
  {
    DT_THROW_IF (dimension_ != get_number_of_free_params (), std::range_error, "Invalid dimension!");
    for (size_t i = 0; i < get_number_of_free_params (); i++) {
      grab_free_param (i).set_value_no_check (pars_[i]);
    }
    init_params_values ();
    return;
  }

  /*
    void multidimensional_minimization_system::search_likelihood_sigma ()
    {
    for (int param_index = param_begin;
    param_index < param_end;
    param_index++)
    {
    const mygsl::param_entry & param = get_free_param (param_index);
    double h  = param.get_step ();
    mygsl::multidimensional_minimization_system::func_eval_f_param func;
    func.free_param_index = param_index;
    func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
    double sigma_low;
    double sigma_high;
    double p = param.get_value ();
    double fmin = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
    double h = param.get_step ();
    double dir = +1;
    while (true)
    {
    p += (dir * h);
    double f
    = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
    if (f > (fmin + 0.5))
    {
    dir = -1;
    h /= 20;
    }
    }
    while (
    }
    }
  */

  /************************************************************************/

  multidimensional_minimization_system::func_eval_f_param::func_eval_f_param (int free_param_index_,
                                                                              multidimensional_minimization_system & sys_)
  {
    free_param_index = free_param_index_;
    sys = &sys_;
    return;
  }

  double multidimensional_minimization_system::func_eval_f_param::_eval (double x_) const
  {
    return func_eval_f_MR(x_, const_cast<func_eval_f_param*> (this));
  }

  void multidimensional_minimization_system::plot_f (const std::string & prefix_, int /*mode_*/) const
  {
    std::string prefix = prefix_;
    std::string ext = ".data";
    if (prefix.empty ()) {
      prefix = "mms_param_";
    }
    int param_begin = 0;
    int param_end = get_number_of_free_params ();
    for (int param_index = param_begin;
         param_index < param_end;
         param_index++) {
      const mygsl::param_entry & param = get_free_param (param_index);
      // clog << "NOTICE: multidimensional_minimization_system::plot_f: "
      //      << "Plot function to be minimized for parameter '"
      //      << param.get_name ()
      //      << "' in the current parameters' region: " << endl;
      std::ostringstream fname;
      fname << prefix
            << param.get_name () << ext;
      std::ofstream f_param (fname.str ().c_str ());
      int count = 0;
      double h  = param.get_step ();
      double h2 = h / 50.;
      double dt = h;
      double min = param.get_min ();
      double max = param.get_max ();
      for (double p = (min - 5.5 * h);
           p < (max + 5.5 * h);
           p += dt) {
        if (p < (min - 1.5 * h)) {
          dt = h;
        } else if (p < (min + h)) {
          dt = h2;
        } else if (p < (max - 1.5 * h )) {
          dt = h;
        } else if (p < (max + h )) {
          dt = h2;
        } else {
          dt = h;
        }
        mygsl::multidimensional_minimization_system::func_eval_f_param
          func (param_index, const_cast<mygsl::multidimensional_minimization_system&>(*this));
        //func.free_param_index = param_index;
        //func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
        double f
          = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
        f_param.precision (14);
        f_param << p << ' ' << f << std::endl;
        //if ((count % 100) == 0) clog << "  Count #" << count << endl;
        count++;
      }
      f_param.close ();
    }
    return;
  }

  /**************************************************************************/

  double multidimensional_minimization::f (const gsl_vector * v_,
                                           void * params_)
  {
    multidimensional_minimization_system * mms
      = static_cast<multidimensional_minimization_system*> (params_);
    double f;
    mms->eval_f (v_->data, f);
    return f;
  }

  void multidimensional_minimization::df (const gsl_vector * v_,
                                          void * params_,
                                          gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_df (v_->data, gradient_->data);
    return;
  }

  void multidimensional_minimization::fdf (const gsl_vector * v_,
                                           void * params_,
                                           double * f_,
                                           gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_fdf (v_->data, *f_, gradient_->data);
    return;
  }

  multidimensional_minimization::default_step_action multidimensional_minimization::__default_step_action;

  void multidimensional_minimization::set_default_step_action ()
  {
    _at_step_action_ = &multidimensional_minimization::__default_step_action;
    return;
  }

  void multidimensional_minimization::unset_step_action ()
  {
    _at_step_action_ = 0;
    return;
  }

  void multidimensional_minimization::set_step_action (multidimensional_minimization::at_step_action & asd_)
  {
    _at_step_action_ = &asd_;
    return;
  }

  void multidimensional_minimization::default_step_action::action (int status_,
                                                                   size_t iter_,
                                                                   double * x_,
                                                                   size_t   dim_,
                                                                   double   f_)
  {
    int    status = status_;
    size_t iter   = iter_;
    double * x    = x_;
    size_t dim    = dim_;
    double   f    = f_;
    std::cout << iter << ' ' << dim << ' ';
    for (size_t i = 0; i < dim ; i++) {
      std::cout.precision (15);
      std::cout << x[i] << ' ';
    }
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
    return;
  }

  void multidimensional_minimization::print_types (std::ostream & out_)
  {
    out_ << "conjugate_fr" << ' ';
    out_ << "conjugate_pr" << ' ';
    out_ << "vector_bfgs" << ' ';
    out_ << "steepest_descent" << ' ';
    out_ << "nmsimplex";
    return;
  }

  bool multidimensional_minimization::name_is_valid (const std::string & name_)
  {
    if (name_ == "conjugate_fr") return true;
    if (name_ == "conjugate_pr") return true;
    if (name_ == "vector_bfgs" ) return true;
    if (name_ == "steepest_descent") return true;
    if (name_ == "nmsimplex"  ) return true;
    return false;
  }

  std::string multidimensional_minimization::get_name () const
  {
    if (_mode_ == MODE_FDF && _fdfmin_ != 0 ) {
      return std::string (gsl_multimin_fdfminimizer_name (_fdfmin_));
    }
    if (_mode_ == MODE_F && _fmin_ != 0) {
      return std::string (gsl_multimin_fminimizer_name (_fmin_));
    }
    return "";
  }

  void multidimensional_minimization::_init_algorithm_ (const std::string & name_)
  {
    DT_THROW_IF (! name_is_valid (name_), std::logic_error, "Invalid minimization algorithm '"
                 << name_ << "' !");

    if (name_ == "conjugate_fr") {
      _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_fr;
      _set_mode_ (MODE_FDF);
    }

    if (name_ == "conjugate_pr") {
      _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_pr;
      _set_mode_ (MODE_FDF);
    }

    if (name_ == "vector_bfgs") {
      _algo_fdf_ = gsl_multimin_fdfminimizer_vector_bfgs;
      _set_mode_ (MODE_FDF);
    }

    if (name_ == "steepest_descent") {
      _algo_fdf_ = gsl_multimin_fdfminimizer_steepest_descent;
      _set_mode_ (MODE_FDF);
    }

    if (name_ == "nmsimplex") {
      _algo_f_ = gsl_multimin_fminimizer_nmsimplex;
      _set_mode_ (MODE_F);
    }
    return;
  }

  void multidimensional_minimization::_set_mode_ (int mode_)
  {
    DT_THROW_IF (mode_ != MODE_F && mode_ != MODE_FDF, std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void multidimensional_minimization::devel_dump_x () const
  {
    if (_x_ == 0) {
      std::cerr << "DEVEL: _x_==0" << std::endl;
      return;
    }
    std::cerr << "DEVEL ================================= " << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.size  = " << _x_->size
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.stride  = " << _x_->stride
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.data  = " << _x_->data
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.owner  = " << _x_->owner
              << std::endl;
    for (size_t i = 0; i < _x_->size; i++) {
      std::cerr << "DEVEL: "
                << " _x_.data[" << i << "]  = " << _x_->data[i]
                << std::endl;
    }
    std::cerr << "DEVEL: "
              << " _x_.block.size  = " << _x_->block->size
              << std::endl;
    for (size_t i = 0; i < _x_->block->size; i++) {
      std::cerr << "DEVEL: "
                << " _x_.block[" << i << "]  = " << _x_->block->data[i]
                << std::endl;
    }
    std::cerr << "DEVEL ================================= " << std::endl;
    return;
  }

  void multidimensional_minimization::init (const std::string & name_,
                                            multidimensional_minimization_system & ms_)
  {
    _sys_ = &ms_;
    if (! _sys_->is_lock_params ()) {
      _sys_->lock_params ();
    }
    size_t n = _sys_->get_number_of_free_params ();
    _init_algorithm_ (name_);

    if (_mode_ == MODE_FDF) {
      _fdf_.f      = &multidimensional_minimization::f;
      _fdf_.df     = &multidimensional_minimization::df;
      _fdf_.fdf    = &multidimensional_minimization::fdf;
      _fdf_.n      = n;
      _fdf_.params = (void *) _sys_;
      _fdfmin_     = gsl_multimin_fdfminimizer_alloc (_algo_fdf_, _fdf_.n);
    }

    if (_mode_ == MODE_F) {
      _f_.f      = &multidimensional_minimization::f;
      _f_.n      = n;
      _f_.params = (void *) _sys_;
      _fmin_     = gsl_multimin_fminimizer_alloc (_algo_f_, _f_.n);
    }

    _x_ = gsl_vector_alloc (n);
    gsl_vector_set_zero (_x_);

    _sys_->to_double_star (_x_->data, n);

    if (_mode_ == MODE_FDF) {
      _fdf_step_size_ = 0.01;
      _fdf_tol_       = 1.e-4;
      gsl_multimin_fdfminimizer_set (_fdfmin_,
                                     &_fdf_,
                                     _x_,
                                     _fdf_step_size_,
                                     _fdf_tol_);
    }

    if (_mode_ == MODE_F) {
      _ss_ = gsl_vector_alloc (n);
      gsl_vector_set_zero (_ss_);
      for (size_t i = 0; i < _sys_->get_number_of_free_params (); i++) {
        gsl_vector_set (_ss_, i, _sys_->get_param (i).get_step ());
      }
      gsl_multimin_fminimizer_set (_fmin_, &_f_, _x_, _ss_);
    }
    return;
  }

  void multidimensional_minimization::reset ()
  {
    if (_fdfmin_ != 0) {
      gsl_multimin_fdfminimizer_free (_fdfmin_);
      _fdfmin_ = 0;
    }
    if (_fmin_ != 0) {
      gsl_multimin_fminimizer_free (_fmin_);
      _fmin_ = 0;
    }
    if (_x_ != 0) {
      gsl_vector_free (_x_);
      _x_ = 0;
    }
    if (_ss_ != 0) {
      gsl_vector_free (_ss_);
      _ss_ = 0;
    }
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _fdf_step_size_ = 0.01;
    _fdf_tol_       = 1.e-4;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;

    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _mode_           = MODE_F;
    _stopping_       = STOPPING_SIZE;
    _epsabs_         = DEFAULT_EPSABS;
    _at_step_action_ = 0;
    if (_sys_ != 0) {
      _sys_->unlock_params ();
      _sys_ = 0;
    }
    _n_iter_ = 0;
    _fval_   = 0.0;
    return;
  }

  void multidimensional_minimization::set_max_iter (size_t max_)
  {
    _max_iter_ = max_;
    return;
  }

  void multidimensional_minimization::set_modulo_iter (size_t modulo_)
  {
    _modulo_iter_ = modulo_;
    return;
  }

  size_t multidimensional_minimization::get_max_iter () const
  {
    return _max_iter_;
  }

  size_t multidimensional_minimization::get_modulo_iter () const
  {
    return _modulo_iter_;
  }

  size_t multidimensional_minimization::get_n_iter () const
  {
    return _n_iter_;
  }

  double multidimensional_minimization::get_fval () const
  {
    return _fval_;
  }

  multidimensional_minimization::multidimensional_minimization ()
  {
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _mode_     = MODE_F;
    _fdfmin_   = 0;
    _fmin_     = 0;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;
    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _x_              = 0;
    _ss_             = 0;
    _fdf_step_size_  = 0.01;
    _fdf_tol_        = 1.e-4;
    _max_iter_       = DEFAULT_MAX_ITER;
    _stopping_       = STOPPING_SIZE;
    _epsabs_         = 1.0;
    _at_step_action_ = 0;

    _n_iter_         = 0;
    _modulo_iter_    = DEFAULT_MODULO_ITER;
    return;
  }

  multidimensional_minimization::~multidimensional_minimization ()
  {
    reset ();
    return;
  }

  void multidimensional_minimization::_at_step_hook (int status_,
                                                     size_t iter_,
                                                     double * x_,
                                                     size_t dim_,
                                                     double f_)
  {
    DT_LOG_TRACE_ENTERING(logging);
    if (_at_step_action_ != 0) {
      (*_at_step_action_) (status_, iter_, x_, dim_, f_);
    }
    DT_LOG_TRACE_EXITING(logging);
    return;
  }

  double multidimensional_minimization::get_epsabs () const
  {
    return _epsabs_;
  }

  int multidimensional_minimization::minimize (double epsabs_)
  {
    DT_LOG_TRACE_ENTERING(logging);

    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double * x    = 0;
    _epsabs_      = epsabs_;
    if (epsabs_ < 0.0) {
      _epsabs_ = DEFAULT_EPSABS;
    }
    double fx;
    datatools::invalidate(fx);

    do {
      iter++;
      DT_LOG_DEBUG(logging, "Iteration #" << iter);
      bool break_me = false;
      
      if (_mode_ == MODE_F) {
        DT_LOG_DEBUG(logging, "-> MODE_F");
        dim = _f_.n;
        status = gsl_multimin_fminimizer_iterate (_fmin_);
        DT_LOG_DEBUG(logging, "-> status = " << status);
        if (status != 0) {
          break_me = true;
        }
        double size = gsl_multimin_fminimizer_size (_fmin_);
        DT_LOG_DEBUG(logging, "-> size = " << size);
        DT_LOG_DEBUG(logging, "-> epsabs = " << _epsabs_);
        status = gsl_multimin_test_size (size, _epsabs_);
        DT_LOG_DEBUG(logging, "-> status = " << status);
        x = _fmin_->x->data;
        fx = _fmin_->fval;
        DT_LOG_DEBUG(logging, "-> END MODE_F");
      }

      if (_mode_ == MODE_FDF) {
        DT_LOG_DEBUG(logging, "-> MODE_FDF");
        dim = _fdf_.n;
        status = gsl_multimin_fdfminimizer_iterate (_fdfmin_);
        DT_LOG_DEBUG(logging, "-> status = " << status);
        if (status != 0) {
          break_me = true;
        }
        status = gsl_multimin_test_gradient (_fdfmin_->gradient, _epsabs_);
        x = _fdfmin_->x->data;
        fx = _fdfmin_->f;
        DT_LOG_DEBUG(logging, "-> END MODE_FDF");
      }
      
      _at_step_hook(status, iter, x, dim, fx);
      _fval_ = fx;
      if (datatools::logger::is_debug(logging)) {
        if ((iter % _modulo_iter_) == 0 or iter < 10) {
          DT_LOG_DEBUG_SHORT(logging, "Iteration #" << iter << " Fval == " << _fval_);
          _sys_->print_status (std::cerr);
          std::cerr << std::endl;
        }
      }
      
      if (break_me) {
        DT_LOG_DEBUG(logging, "Iteration break!");
        break;
      }
      DT_LOG_DEBUG(logging, "Status       = " << status);
      DT_LOG_DEBUG(logging, "GSL_SUCCESS  = " << GSL_SUCCESS);
      DT_LOG_DEBUG(logging, "GSL_CONTINUE = " << GSL_CONTINUE);
      DT_LOG_DEBUG(logging, "Max iter     = " << _max_iter_);
      DT_LOG_DEBUG(logging, "iter         = " << iter);

    } while ((status == GSL_CONTINUE) && (iter < _max_iter_));

    _n_iter_ = iter;
    if (status == GSL_SUCCESS) {
      _sys_->from_double_star (x, dim);
    }

    DT_LOG_TRACE_EXITING(logging);
    return status;
  }

} // namespace mygsl
