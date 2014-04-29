// test_multidimensional_minimization.cxx
/*
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <ctime>
#include <limits>
#include <stdexcept>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_randist.h>

#include <mygsl/rng.h>
#include <mygsl/histogram.h>
#include <mygsl/multidimensional_minimization.h>
#include <mygsl/param_entry.h>
#include <mygsl/error.h>

using namespace std;

/**********************************************************/

double log_poisson (unsigned long k_, double mu_)
{
  double mu = mu_;
  if ((k_ > 0) && (mu_ < numeric_limits<double>::denorm_min ()))
    {
      mu = numeric_limits<double>::denorm_min ();
    }
  if ((k_ == 0) && (mu_ == 0.0))
    {
      return 0.0;
    }
  return -mu + k_ * log (mu) - gsl_sf_lnfact (k_);
}

/**********************************************************/

struct data
{
  static const double E1;
  static const double E2;
  static const double E3;
  static const double E4;
};
const double data::E1 = 540.; // keV
const double data::E2 = 980.; // keV
const double data::E3 = 720.; // keV
const double data::E4 = 1150.; // keV

/**********************************************************/

double sigma_resol (double e_, double r_)
{
  double f = 2. * sqrt (2. * log (2.));
  return r_ * sqrt (e_ / 1000.) * 1000. / f;
}

/**********************************************************/

class experiment
{
public:

  struct params_type
  {
    double a, b, r, p1, p2, p3, p4;

    void dump (ostream & out_) const
    {
      string tag  = "|-- ";
      string ltag = "`-- ";
      out_ << "experiment::params_type" << endl;
      out_ << tag << "a  = " << a << endl;
      out_ << tag << "b  = " << b << endl;
      out_ << tag << "r  = " << r << endl;
      out_ << tag << "p1 = " << p1 << endl;
      out_ << tag << "p2 = " << p2 << endl;
      out_ << tag << "p3 = " << p3 << endl;
      out_ << ltag << "p4 = " << p4 << endl;
    }

    static double complement (double pa_, double pb_, double pc_)
    {
      return 1.0 - pa_ - pb_ - pc_;
    }

  };

  double __e1;
  double __e2;
  double __e3;
  double __e4;
  double __sig_e1;
  double __sig_e2;
  double __sig_e3;
  double __sig_e4;
  size_t __nevents;
  unsigned long int __seed;

  double __r;
  double __a;
  double __b;
  double __p1;
  double __p2;
  double __p3;
  double __p4;

  void set (const params_type & params_,
            size_t nevents_,
            unsigned long int seed_)
  {
    __r  = params_.r;
    __a  = params_.a;
    __b  = params_.b;
    __p1 = params_.p1;
    __p2 = params_.p2;
    __p3 = params_.p3;
    __p4 = params_.p4;
    __sig_e1  = sigma_resol (__e1, __r);
    __sig_e2  = sigma_resol (__e2, __r);
    __sig_e3  = sigma_resol (__e3, __r);
    __sig_e4  = sigma_resol (__e4, __r);
    __nevents = nevents_;
    __seed    = seed_ ;
  }

  experiment (const params_type & params_,
              size_t nevents_         = 5000,
              unsigned long int seed_ = 0)
  {
    __e1 = data::E1;
    __e2 = data::E2;
    __e3 = data::E3;
    __e4 = data::E4;
    set (params_, nevents_, seed_);
  }

  void shoot (mygsl::histogram & h_)
  {
    h_.init (1024, 0., 1024.);
    mygsl::rng ran ("mt19937", __seed);
    for (int i = 0; i < (int) __nevents; i++)
      {
        double energy;
        double r = ran.uniform ();

        if (r < __p1)
          {
            energy = ran.gaussian (__e1, __sig_e1);
          }
        else if (r < (__p2 + __p1))
          {
            energy = ran.gaussian (__e2, __sig_e2);
          }
        else if (r < (__p3 + __p2 + __p1))
          {
            energy = ran.gaussian (__e3, __sig_e3);
          }
        else
          {
            energy = ran.gaussian (__e4, __sig_e4);
          }
        int ch = (int) ((energy - __b) / __a);
        h_.fill (ch);
      }
  }

};

/**********************************************************/

class test_multimin_system : public mygsl::multidimensional_minimization_system
{

public:
  enum mode_type
    {
      MODE_CHISQUARE  = 0,
      MODE_LIKELIHOOD = 1
    };

private:
  bool                       debug;
  int                      __mode;
  const mygsl::histogram * __hdata;
  int __min_channel;
  int __max_channel;

public:

  size_t number_of_data () const
  {
    return __max_channel - __min_channel - 1;
  }

  bool is_chisquare () const
  {
    return __mode == MODE_CHISQUARE;
  }

  bool is_likelihood () const
  {
    return __mode == MODE_LIKELIHOOD;
  }

  void set_debug (bool d_)
  {
    debug = d_;
  }

  test_multimin_system (const mygsl::histogram & h_,
                        int min_channel_,
                        int max_channel_,
                        int mode_ = MODE_CHISQUARE,
                        bool debug_ = false)
  {
    debug   = debug_;
    __mode  = mode_;
    __hdata = &h_;
    __min_channel = min_channel_;
    __max_channel = max_channel_;
  }

  double f_nth (double energy_) const
  {
    double e   = energy_;

    if (e < 1.e-300) return 0.0;

    double a   = get_param (0).get_value_safe ();
    double b   = get_param (1).get_value_safe ();
    double r   = get_param (2).get_value_safe ();
    double p1  = get_param (3).get_value_safe ();
    double p2  = get_param (4).get_value_safe ();
    double p3  = get_param (5).get_value_safe ();
    double p4  = get_param (6).get_value_safe ();

    double t   = 0.0;
    double e1  = data::E1;
    double e2  = data::E2;
    double e3  = data::E3;
    double e4  = data::E4;
    double sum = __hdata->sum (__min_channel, __max_channel);

    double sig_e1 = sigma_resol (e1, r);
    double sig_e2 = sigma_resol (e2, r);
    double sig_e3 = sigma_resol (e3, r);
    double sig_e4 = sigma_resol (e4, r);
    t += p1 * gsl_ran_gaussian_pdf (e - e1, sig_e1);
    t += p2 * gsl_ran_gaussian_pdf (e - e2, sig_e2);
    t += p3 * gsl_ran_gaussian_pdf (e - e3, sig_e3);
    t += p4 * gsl_ran_gaussian_pdf (e - e4, sig_e4);
    double res = t;
    res *= sum;
    res *= a;
    if (! isfinite (res))
      {
        string tag = "|-- ";
        string ltag = "`-- ";
        clog << "DEBUG: test_multimin_system::f_nth: dump:" << endl;
        clog << tag << "res = " << res << endl;
        clog << tag << "sum = " << sum << endl;
        clog << tag << "e = " << e << endl;
        clog << tag << "a = " << a << endl;
        clog << tag << "b = " << b << endl;
        clog << tag << "r = " << r << endl;
        clog << tag << "p1 = " << p1 << endl;
        clog << tag << "p2 = " << p2 << endl;
        clog << tag << "p3 = " << p3 << endl;
        clog << tag << "p4 = " << p4 << endl;
        clog << tag << "sig_e1 = " << sig_e1 << endl;
        clog << tag << "sig_e2 = " << sig_e2 << endl;
        clog << tag << "sig_e3 = " << sig_e3 << endl;
        clog << ltag << "sig_e4 = " << sig_e4 << endl;
        throw runtime_error ("f_nth: STOP");
      }
    return res;
  }

  void print_solution (ostream & out_) const
  {
    double a = get_param (0).get_value ();
    double b = get_param (1).get_value ();
    for (int ich = 0; ich < (int) __hdata->bins (); ich++)
      {
        double ipos = ich + 0.5;
        double energy = a * ipos + b;
        double nth    = f_nth (energy);
        out_ << ipos << ' ' << nth << endl;
      }
  }

  double compute_likelihood ()
  {
    bool local_debug = false;
    //local_debug = true;

    double likelihood = 0.0;

    size_t ch_min  = __min_channel;
    size_t ch_max  = __max_channel;

    double a  = get_param (0).get_value_safe ();
    double b  = get_param (1).get_value_safe ();
    double r  = get_param (2).get_value_safe ();
    // double p1 = get_param (3).get_value_safe ();
    // double p2 = get_param (4).get_value_safe ();
    // double p3 = get_param (5).get_value_safe ();
    // double p4 = get_param (6).get_value_safe ();

    for (int ich = ch_min; ich < (int) ch_max; ich++)
      {
        double nexp         = __hdata->at (ich);
        double energy       = a * (ich + 0.5) + b;
        double nth          = f_nth (energy);
        unsigned int sz_exp = (unsigned int) nexp;
        double lp = log_poisson (sz_exp, nth);
        likelihood -= lp;
        if (! isfinite (nth))
          {
            clog << "ERROR: NOT FINITE: " << endl;
            clog << "ERROR:   a      => " << a << endl;
            clog << "ERROR:   b      => " << b << endl;
            clog << "ERROR:   r      => " << r << endl;
            clog << "ERROR:   energy => " << energy << endl;
            clog << "ERROR:   nth    => " << nth << endl;
            clog << "ERROR:   sz_exp => " << sz_exp << endl;
            clog << "ERROR:   lp     => " << lp << endl;
            clog << "ERROR:   likelihood => " << likelihood << endl;
          }
        if (local_debug)
          {
            clog << "DEBUG: test_multimin_system::compute_likelihood: "
                 << " ich="  << ich
                 << " nth="  << nth
                 << " sz_exp=" << sz_exp
                 << " lp=" << lp
                 << " likelihood=" << likelihood << endl;
          }
      }

    if (local_debug)
      {
        clog << "DEBUG: test_multimin_system::compute_likelihood: "
             << "likelihood=" << likelihood << endl;
      }

    return likelihood;
  }

  double compute_chi_square ()
  {
    bool local_debug = false;
    //local_debug = true;
    if (local_debug) clog << "DEBUG: test_multimin_system::compute_chi_square: entering... " << endl;

    double chi2   = 0.0;

    size_t ch_min = __min_channel;
    size_t ch_max = __max_channel;

    double a  = get_param (0).get_value_safe ();
    double b  = get_param (1).get_value_safe ();
    // double r  = get_param (2).get_value_safe ();
    // double p1 = get_param (3).get_value_safe ();
    // double p2 = get_param (4).get_value_safe ();
    // double p3 = get_param (5).get_value_safe ();
    // double p4 = get_param (6).get_value_safe ();

    // double bv  = get_param (1).get_value ();

    for (int ich = ch_min; ich < (int) ch_max; ich++)
      {
        double nexp       = __hdata->at (ich);
        double sigma_nexp = sqrt (nexp);
        if (nexp < 2) sigma_nexp = 1.0;
        double energy    = a * (ich + 0.5) + b;
        double nth       = f_nth (energy);
        if (local_debug)
          {
            clog << "DEBUG: test_multimin_system::compute_chi_square: LOOP ich=" << ich << endl;
            clog << "DEBUG: test_multimin_system::compute_chi_square: nexp=" << nexp << endl;
            clog << "DEBUG: test_multimin_system::compute_chi_square: sigma_nexp=" << sigma_nexp << endl;
            clog << "DEBUG: test_multimin_system::compute_chi_square: energy=" << energy << endl;
            clog << "DEBUG: test_multimin_system::compute_chi_square: nth=" << nth << endl;
          }
        chi2 += gsl_pow_2 ((nexp - nth) / sigma_nexp);
      }
    if (local_debug) clog << "DEBUG: test_multimin_system::compute_chi_square: chi2=" << chi2 << endl;

    return chi2;
  }

  void compute_automatic_params ()
  {
    // extract current parameters:
    double p1 = get_param (3).get_value_safe ();
    double p3 = get_param (5).get_value_safe ();
    double p4 = get_param (6).get_value_safe ();
    p1 = get_param (3).get_value ();
    p3 = get_param (5).get_value ();
    p4 = get_param (6).get_value ();

    // recompute auto parameters 'p2':
    double p2 = experiment::params_type::complement (p1, p3, p4);
    grab_param (4).set_value_no_check (p2);

    // recompute auto parameters 'p1+p3':
    double p13 = p1 + p3;
    grab_param (7).set_value_no_check (p13);

    // recompute auto parameters 'p1+p4':
    double p14 = p1 + p4;
    grab_param (8).set_value_no_check (p14);

    // recompute auto parameters 'p3+p4':
    double p34 = p3 + p4;
    grab_param (9).set_value_no_check (p34);

    // recompute auto parameters 'p1-p3':
    double p1_3 = p1 - p3;
    grab_param (10).set_value_no_check (p1_3);

    // recompute auto parameters 'p2-p4':
    double p2_4 = p2 - p4;
    grab_param (11).set_value_no_check (p2_4);

  }

protected:

  int _eval_f (double & f_)
  {
    try
      {
        if (is_chisquare ())
          {
            f_ = compute_chi_square ();
          }

        if (is_likelihood ())
          {
            f_ = compute_likelihood ();
          }
      }
    catch (exception & x)
      {
        cerr << "ERROR: test_multimin_system::_eval_f: " << x.what () << endl;
        return 1;
      }
    return 0;
  }

};

int main (int argc_, char ** argv_)
{
  try
    {
      int mode   = test_multimin_system::MODE_CHISQUARE;
      bool debug = false;
      bool time_seed = false;
      size_t nevents  = 5000;
      unsigned long int seed = 1;
      bool use_default_step_action = false;
      string method_name = "nmsimplex";
      bool plot_f = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg[0] == '-')
            {
              if (arg == "--debug" || arg == "-d")
                {
                  debug = true;
                }

              else if (arg == "--plot-f" || arg == "-f")
                {
                  plot_f = true;
                }

              else if (arg == "--nmsimplex" || arg == "-S")
                {
                  method_name = "nmsimplex";
                }

              else if (arg == "--conjugate-fr" || arg == "-F")
                {
                  method_name = "conjugate_fr";
                }

              else if (arg == "--conjugate-pr" || arg == "-P")
                {
                  method_name = "conjugate_pr";
                }

              else if (arg == "--vector-bfgs" || arg == "-B")
                {
                  method_name = "vector_bfgs";
                }

              else if (arg == "--steepest-descent" || arg == "-D")
                {
                  method_name = "steepest_descent";
                }

              else if (arg == "--use-default-step-action" || arg == "-u" )
                {
                  use_default_step_action = true;
                }

              else if (arg == "--time-seed" || arg == "-t" )
                {
                  time_seed = true;
                }

              else if (arg == "--seed" || arg == "-s" )
                {
                  istringstream iss (argv_[++iarg]);
                  iss >> seed;
                  if (! iss)
                    {
                      throw runtime_error ("Invalid seed format !");
                    }
                }
              else if (arg == "--likelihood" || arg == "-l")
                {
                  mode = test_multimin_system::MODE_LIKELIHOOD;
                }

              else if (arg == "--chisquare" || arg == "-c")
                {
                  mode = test_multimin_system::MODE_CHISQUARE;
                }

              else if (arg == "--50")
                {
                  nevents = 50;
                }

              else if (arg == "--5000")
                {
                  nevents = 5000;
                }

              else if (arg == "--50000")
                {
                  nevents = 50000;
                }

              else if (arg == "--500000")
                {
                  nevents = 500000;
                }

              else if (arg == "--50000000")
                {
                  nevents = 50000000;
                }
              else
                {
                  clog << "WARNING: Ignoring option '" << arg << "'!" << endl;
                }
            }
          else
            {
              clog << "WARNING: Ignoring argument '" << arg << "'!" << endl;
            }
          iarg++;
        }

      mygsl::histogram h;

      experiment::params_type exp_params;
      exp_params.a  = 3.0;
      exp_params.b  = 50.0;
      exp_params.r  = 0.05;
      exp_params.p1 = 0.30;
      exp_params.p2 = 0.45;
      exp_params.p3 = 0.10;
      exp_params.p4 = experiment::params_type::complement (exp_params.p1,
                                                    exp_params.p2,
                                                    exp_params.p3);
      exp_params.dump (clog);

      if (time_seed)
        {
          time_t tt;
          tt = time (NULL);
          srand ((unsigned int) tt);
          seed = rand ();
        }

      clog << "NOTICE: seed=" << seed << endl;
      clog << "NOTICE: nevents=" << nevents << endl;

      experiment e (exp_params, nevents, seed);
      e.shoot (h);

      ofstream fhist ("test_multimin.his");
      h.print (fhist);
      fhist.close ();

      experiment::params_type start_params;

      start_params = exp_params;
      start_params.a  = 2.9;
      start_params.b  = 40.0;
      start_params.r  = 0.09;
      start_params.p1 = 0.30;
      start_params.p2 = 0.50;
      start_params.p3 = 0.10;
      start_params.p4 = experiment::params_type::complement (start_params.p1,
                                                          start_params.p2,
                                                          start_params.p3);

      // debug:
      test_multimin_system sys (h, 0, h.bins (), mode, debug);

      typedef mygsl::param_entry pe;
      sys.add_param (pe::make_free_range ("a",
                                          2.0,
                                          6.0,
                                          start_params.a,
                                          0.005),
                     "Energy calibration coefficient (keV/channel)");

      // 10.
      sys.add_param (pe::make_free_range ("b",
                                      0.,
                                      200.,
                                      start_params.b,
                                      1.),
                     "Energy calibration pedestal (keV)");

      // 0.005
      sys.add_param (pe::make_free_range ("r",
                                      0.02,
                                      0.15,
                                      start_params.r,
                                      0.001),
                     "Energy resolution coefficient (FWHM @ 1 MeV)");

      sys.add_param (pe::make_free_range ("p1",
                                      0.,
                                      1.,
                                      start_params.p1,
                                      0.005),
                     "Proportion of 540 keV peak");

      sys.add_param (pe::make_auto_range ("p2",
                                           0.0,
                                           1.0),
                     "Proportion of 980 keV peak");

      sys.add_param (pe::make_free_range ("p3",
                                      0.,
                                      1.,
                                      start_params.p3,
                                      0.005),
                     "Proportion of 720 keV peak");

      sys.add_param (pe::make_free_range ("p4",
                                      0.,
                                      1.,
                                      start_params.p4,
                                      0.005),
                     "Proportion of 1150 keV peak");

      sys.add_param (pe::make_auto_range ("p1+p3",
                                           0.0,
                                           1.0),
                     "Constraint on probability sum");

      sys.add_param (pe::make_auto_range ("p1+p4",
                                           0.0,
                                           1.0),
                     "Constraint on probability sum");

      sys.add_param (pe::make_auto_range ("p3+p4",
                                           0.0,
                                           1.0),
                     "Constraint on probability sum");

      sys.add_param (pe::make_auto_min ("p1-p3",
                                        0.0),
                     "Constraint on peak 1/peak3 branching ratio");

      sys.add_param (pe::make_auto_min ("p2-p4",
                                        0.0),
                     "Constraint on peak 2/peak4 branching ratio");

      sys.add_param (pe::make_const ("pi",
                                     M_PI),
                     "Constant Pi");


      sys.init_params_values ();
      sys.print (clog, "multidimensional minimization system");

      if (plot_f)
        {
          sys.plot_f ("test_multimin_f_init_");
        }

      clog << "Save initial model: " << endl;
      ofstream fini ("test_multimin.ini");
      sys.print_solution (fini);

      clog << "Available algorithms: ";
      mygsl::multidimensional_minimization::print_types (clog);
      clog << endl;

      mygsl::multidimensional_minimization mm;
      if (use_default_step_action) mm.set_default_step_action ();
      mygsl::error::off ();
      if (method_name != "nmsimplex")
        {
          sys.set_numeric_eval_df (true);
        }
      mm.set_max_iter (1000);
      mm.set_modulo_iter (10);
      mm.init (method_name, sys);

      clog << "Init done." << endl;

      double epsabs = 1.0;
      epsabs = 1.0e-2;
      //epsabs = 1.0e-3;
      if (mm.minimize (epsabs) == GSL_SUCCESS)
        {
          clog << "Found minimum!" << endl;

          sys.init_params_values ();
          for (int i = 0; i < (int) sys.get_number_of_params (); i++)
            {
              mygsl::best_value bv (sys.get_param (i).get_value ());
              cerr << "TEST: " << sys.get_param (i).get_name ()
                   << " ==> " << bv << endl;
              sys.grab_param (i).set_best_value (bv);
            }
          sys.print (clog, "Solution");
          sys.print_status (clog);

          clog << "Save best model: " << endl;
          ofstream fsol ("test_multimin.sol");
          sys.print_solution (fsol);

          clog << "Iterations = " << mm.get_n_iter () << endl;
          double fval = mm.get_fval ();
          if (sys.is_chisquare ())
            {
              clog << "Chi2 = " << fval << endl;
              double ndof = sys.number_of_data () - sys.get_number_of_free_params ();
              clog << "ndof = " << ndof << endl;
              clog << "Chi2/ndof= " << fval / ndof << endl;
            }
          else
            {
              clog << "Likelihood = " << fval << endl;
            }

          ofstream fpar ("test_multimin.pars");
          sys.print_line (fpar);
          fpar << fval << endl;

          sys.store_parameters ("test_multimin.lop");
          if (plot_f)
            {
              sys.plot_f ("test_multimin_f_sol_");
            }

          /*
          // check the re-loading of parameters
          mygsl::multiparameter_system_test MPS;
          MPS.load_parameters ("test_multimin.lop");
          MPS.print (clog, "Reloaded multi-parameter system");
          */

        }
      else
        {
          clog << "Cannot find minimum!" << endl;
          double fval = mm.get_fval ();
          clog << "Last fval = " << fval << endl;
        }
    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_multidimensional_minimization.cxx
