// test_multiparameter_system.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <mygsl/multiparameter_system.h>

using namespace std;

class my_system : public mygsl::multiparameter_system
{
public:
  void compute_automatic_params ()
  {
    // compute the value of parameter 'Pi-A*lambda' from the current
    // values of the 'Pi', 'A' and 'lambda' parameters:
    double A = get_param_by_name ("A").get_value ();
    double lambda = get_param_by_name ("lambda").get_value ();
    double Pi = get_param_by_name ("Pi").get_value ();
    double autoval = Pi - A * lambda;
    grab_param_by_name ("Pi-A*lambda").set_value (autoval);

    /* here we deliberately forgot to compute the 'forgotten-auto'
     * parameter; this will trigger exception at 'init'.
     */
    grab_param_by_name ("forgotten-auto").set_value (0.0);
    grab_param_by_name ("forgotten-auto").set_auto_computed (true);
  }
};

int main(int argc_ , char ** argv_)
{
  try
    {
      // a multi-parameter system:
      my_system MS;

      // add some free parameters (ranged or not):
      MS.add_param (mygsl::param_entry::make_free_min ("A", 1.0, 1.1, 0.001));
      MS.add_param (mygsl::param_entry::make_free_range ("lambda", 0.01, 3.5, 0.5, 0.001));
      MS.add_param (mygsl::param_entry::make_free_max ("b", 10.0, 0.5, 0.001));
      MS.add_param (mygsl::param_entry::make_free ("K", 0.5, 0.001));

      // add a constant parameter:
      MS.add_param (mygsl::param_entry::make_const ("Pi", 3.14159));

      // add some automatically computed parameters:
      MS.add_param (mygsl::param_entry::make_auto ("Pi-A*lambda"));
      MS.add_param (mygsl::param_entry::make_auto ("forgotten-auto"));

      MS.print (clog, "Multi-parameter system before locking", "");
      clog << endl;

      // locking the system triggers the computation of
      // 'automatic' parameters which depend on the values of
      // the other parameters:
      MS.lock_params ();
      MS.print (clog, "Multi-parameter system after locking", "");
      clog << endl;

      MS.grab_param_by_name ("A").set_value (1.2345);
      MS.grab_param_by_name ("b").set_no_min ();
      MS.compute_automatic_params ();
      MS.print (clog, "Multi-parameter system after change in A", "");
      clog << endl;

      mygsl::best_value A_bv (5.0, 0.01, 0.95);
      clog << ">>> " << "A (best value) = " << A_bv << endl;

      // set the best value for parameter 'A':
      MS.grab_param_by_name ("A").set_best_value (A_bv);
      clog << endl;

      MS.print (clog, "Multi-parameter system after setting A best value", "");

      cerr << "Init..." << endl;
      // the system's initialization ensures the locking and check if all
      // automatic parameters have been computed :
      MS.init ();

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_multiparameter_system.cxx
