// test_seed_manager.cxx
/*
 *
 * shell> MYGSL_SEED_MANAGER_INIT_SEED_FROM="current_time" ./__build-Linux-x86_64/testing/test_seed_manager
 * shell> MYGSL_SEED_MANAGER_INIT_SEED_FROM="current_pid" ./__build-Linux-x86_64/testing/test_seed_manager
 * shell> MYGSL_SEED_MANAGER_INIT_SEED_FROM="urandom" ./__build-Linux-x86_64/testing/test_seed_manager
 * shell> MYGSL_SEED_MANAGER_INIT_SEED_FROM="foo" ./__build-Linux-x86_64/testing/test_seed_manager
 * ...
 * WARNING: mygsl::seed_manager::_set_init_seed_flags_: Invalid \
 *   value ('foo') for the 'MYGSL_SEED_MANAGER_INIT_SEED_FROM' environment \
 *   variable !
 * ...
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>

// This project:
#include <mygsl/seed_manager.h>
#include <mygsl/random_utils.h>

using namespace std;

void test0()
{
  std::clog << "\n********** Test #0:\n";
  mygsl::seed_manager SM;
  SM.set_init_seed_flags (mygsl::seed_manager::INIT_SEED_FROM_URANDOM);
  SM.add_seed ("manager",           123456);
  SM.add_seed ("primary_generator", 314159);
  SM.add_seed ("vertex_generator",  2718);
  SM.add_seed ("dummy_generator",   mygsl::random_utils::SEED_TIME);
  SM.dump (clog);

  clog << endl << "Updating..." << endl;
  SM.update_seed ("manager", 271828182);
  SM.add_seed ("smearing_processor", 76543);
  SM.dump (clog);

  clog << endl << "Invalidating one..." << endl;
  SM.invalidate_seed ("primary_generator");
  SM.dump (clog);

  try {
    SM.add_seed ("smearing_processor", 666);
  } catch (std::exception & error) {
    std::cerr << "error: As expected: " << error.what() << std::endl;
  }

  try {
    SM.add_seed ("007", 42);
  } catch (std::exception & error) {
    std::cerr << "error: As expected: " << error.what() << std::endl;
  }

  try {
    SM.add_seed ("std::ran", 1234);
  } catch (std::exception & error) {
    std::cerr << "error: As expected: " << error.what() << std::endl;
  }

  clog << "SM = " << SM << endl;
  return;
}

void test1()
{
  std::clog << "\n********** Test #1:\n";
  string line = "{a=4; b=56; c=356; d=-1; e=0; f=4321}";
  clog << "Parse a streamed expression : " << line<< endl;
  istringstream iss (line);
  mygsl::seed_manager SM2;
  iss >> SM2;
  if (! iss)
    {
      throw runtime_error ("Invalid format !");
    }
  clog << "SM2 = " << SM2 << endl;
  SM2.dump (clog);

  clog << endl << "All time seeds..." << endl;
  SM2.all_time_seeds ();
  SM2.dump (clog);

  clog << endl << "Manually set some seeds..." << endl;
  SM2.update_seed ("a", 314159);
  SM2.update_seed ("b", 999);
  SM2.update_seed ("manager", 666);
  SM2.update_seed ("manager1", 666);
  SM2.update_seed ("manager2", 666);
  SM2.update_seed ("r0", 1000);
  SM2.update_seed ("r1", 1000);
  SM2.update_seed ("r2", 1000);
  SM2.dump (clog);

  clog << endl << "Transform time seeds..." << endl;
  SM2.transform_time_seeds (true);
  SM2.dump (clog);

  clog << endl << "Make sure they are all different..." << endl;
  SM2.transform_time_seeds ();
  SM2.dump (clog);

  clog << endl << "Force some duplicated seed..." << endl;
  SM2.update_seed ("r1", 1000);
  SM2.dump (clog);

  clog << endl << "Ensure no duplicated seeds..." << endl;
  SM2.ensure_different_seeds ();
  SM2.dump (clog);

  clog << endl << "All time seeds (again)..." << endl;
  SM2.all_time_seeds ();
  SM2.dump (clog);

  clog << endl << "Ensure no duplicated seeds (again)..." << endl;
  SM2.ensure_different_seeds ();
  SM2.dump (clog);

  vector<string> labels;
  SM2.get_labels (labels);
  clog << endl << "Seed for '" << labels.front () << "' is : " << SM2.get_seed (labels.front ()) << endl;
  SM2.clear ();
  return;
}

void test2()
{
  std::clog << "\n********** Test #2:\n";
  mygsl::seed_manager SM1;
  SM1.set_init_seed_flags (mygsl::seed_manager::INIT_SEED_FROM_CURRENT_TIME);
  SM1.add_seed ("s1", mygsl::random_utils::SEED_TIME);
  SM1.add_seed ("s2", mygsl::random_utils::SEED_TIME);
  SM1.dump (clog);

  mygsl::seed_manager SM2;
  SM2.set_init_seed_flags (mygsl::seed_manager::INIT_SEED_FROM_CURRENT_TIME);
  SM2.add_seed ("t1", mygsl::random_utils::SEED_TIME);
  SM2.add_seed ("t2", mygsl::random_utils::SEED_TIME);
  SM2.dump (clog);

  SM1.transform_time_seeds (true);
  SM1.dump (clog);

  SM2.transform_time_seeds (true);
  SM2.dump (clog);

  return;
}

void test3()
{
  std::clog << "\n********** Test #3:\n";
  mygsl::seed_manager SM1;
  SM1.set_init_seed_flags (mygsl::seed_manager::INIT_SEED_FROM_URANDOM);
  SM1.add_seed ("u1", mygsl::random_utils::SEED_TIME);
  SM1.add_seed ("u2", mygsl::random_utils::SEED_TIME);
  SM1.dump (clog);

  mygsl::seed_manager SM2;
  SM2.set_init_seed_flags (mygsl::seed_manager::INIT_SEED_FROM_URANDOM);
  SM2.add_seed ("v1", mygsl::random_utils::SEED_TIME);
  SM2.add_seed ("v2", mygsl::random_utils::SEED_TIME);
  SM2.dump (clog);

  SM1.transform_time_seeds (true);
  SM1.dump (clog);

  SM2.transform_time_seeds (true);
  SM2.dump (clog);

  return;
}

void test4()
{
  std::clog << "\n********** Test #4:\n";
  mygsl::seed_manager SM1;
  SM1.add_seed ("s1", mygsl::random_utils::SEED_TIME);
  SM1.add_seed ("s2", mygsl::random_utils::SEED_TIME);
  SM1.dump (clog);

  mygsl::seed_manager SM2;
  SM2.add_seed ("t1", mygsl::random_utils::SEED_TIME);
  SM2.add_seed ("t2", mygsl::random_utils::SEED_TIME);
  SM2.dump (clog);

  SM1.transform_time_seeds (true);
  SM1.dump (clog);

  SM2.transform_time_seeds (true);
  SM2.dump (clog);

  return;
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'mygsl::seed_manager'!" << endl;

      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else
                {
                  clog << "warning: ignoring option '" << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      if (debug) {
      }

      test0();
      test1();
      test2();
      test3();
      test4();

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_seed_manager.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
