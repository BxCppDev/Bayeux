// test_histogram_pool.cxx
/*
 * Histogram pool sample program
 *
 *
 */

// Standard library:
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <mygsl/histogram_pool.h>
#include <mygsl/rng.h>

void test_1 ()
{
  std::clog << "==============> test_1" << std::endl;

  mygsl::histogram_pool HP ("A set of histograms");

  mygsl::histogram_1d & h1 = HP.add_1d ("h1", "The h1 1D-histogram");
  mygsl::histogram_1d & h2 = HP.add_1d ("h2", "The h2 1D-histogram");
  mygsl::histogram_2d & h3 = HP.add_2d ("h3", "The h3 2D-histogram");
  mygsl::histogram_2d & h4 = HP.add_2d ("h4", "The h4 2D-histogram");
  mygsl::histogram_1d & h5 = HP.add_1d ("h5", "The h5 1D-histogram");
  mygsl::histogram_1d & h6 = HP.add_1d ("h6", "The h6 1D-histogram");

  // Attach histograms to some specific group :
  HP.set_group ("h1", "grp1");
  HP.set_group ("h2", "grp1");
  HP.set_group ("h5", "grp1");
  HP.set_group ("h6", "grp2");

  // Set some individual traits for some histograms :
  h1.grab_auxiliaries ().set_flag ("F1");
  h2.grab_auxiliaries ().set_flag ("foo");
  h3.grab_auxiliaries ().set_flag ("foo");
  h4.grab_auxiliaries ().set_flag ("F1");
  h6.grab_auxiliaries ().set_flag ("foo");

  HP.tree_dump (std::clog, "HP: ", "INFO: ");

  h1.initialize (20, 0.0, 10.0);
  h2.initialize (10, 1.e-5, 1.e5, mygsl::BIN_MODE_LOG);
  h3.initialize (20, 0.0, 10.0, 10, 1.e-5, 1.e5,
                 mygsl::BIN_MODE_LINEAR,
                 mygsl::BIN_MODE_LOG);
  h4.initialize (20, 0.0, 10.0, 100, 0, 1.e5);
  h5.initialize (10, 0.0, 100.0);
  h6.initialize (100, 0.0, 4.0);
  std::string   random_id = mygsl::rng::DEFAULT_RNG_TYPE;
  unsigned long random_seed = 12345;
  mygsl::rng    random (random_id, random_seed);

  unsigned int nshoots = 100000;
  for (int i= 0; i < (int) nshoots; i++)
    {
      double x1 = random.exponential (3.3);
      h1.fill(x1);
      double x2 = std::exp (std::log (10.) * random.flat (-2.5, 2.5));
      h2.fill(x2);
      h3.fill(x1, x2);
      h4.fill(x1, x2);
      h5.fill(x1*x1);
      h6.fill(std::sqrt(x1));
    }
  h1.tree_dump (std::clog, "Histogram 'h1'");
  h2.tree_dump (std::clog, "Histogram 'h2'");
  h3.tree_dump (std::clog, "Histogram 'h3'");
  h4.tree_dump (std::clog, "Histogram 'h4'");
  h5.tree_dump (std::clog, "Histogram 'h5'");
  h6.tree_dump (std::clog, "Histogram 'h6'");

  h1.print (std::cout, 15);
  h2.print (std::cout, 15);

  std::vector<std::string> histo_names;
  HP.names (histo_names, "dim=1");
  std::clog << "\n1D-Histos are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  histo_names.clear ();
  HP.names (histo_names, "dim=2");
  std::clog << "\n2D-Histos are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  histo_names.clear ();
  HP.names (histo_names, "flag=F1");
  std::clog << "\nHistos with flag 'F1' are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  //histo_names.clear ();
  HP.names (histo_names, "group=grp1");
  std::clog << "\nHistos with flag 'F1' or in group 'grp1' are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  histo_names.clear ();
  HP.names (histo_names);
  std::clog << "\nAll histos are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  histo_names.clear ();
  HP.names (histo_names, "flag=foo", true);
  std::clog << "\nHistos without flag 'foo' are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;


  std::clog << "Histogram h1 :" << std::endl;
  h1.print_ascii (std::clog);
  std::clog << "Histogram h2 :" << std::endl;
  h2.print_ascii (std::clog);

  {
    std::clog << "INFO: "
              << "Test serialization..."
              << std::endl;
    std::string filename = "test_histogram_pool.xml";
    {
      std::clog << "INFO: "
                << "Test serialization: writer..."
                << std::endl;
      datatools::data_writer writer (filename);
      writer.store (HP);
    }

    {
      std::clog << "INFO: "
                << "Test serialization: reader..."
                << std::endl;
      mygsl::histogram_pool HP2;
      datatools::data_reader reader (filename);
      if (reader.has_record_tag ())
        {
          if (reader.record_tag_is (mygsl::histogram_pool::SERIAL_TAG))
            {
              reader.load (HP2);
              HP2.tree_dump (std::clog, "HP2 : ", "INFO: ");
            }
          else
            {
              std::cerr << "ERROR: "
                        << "Cannot load an histogram pool object !"
                        << std::endl;
            }
        }
      else
        {
          std::cerr << "ERROR: "
                    << "No object in the Boost archive !"
                    << std::endl;
        }
    }
  }
  return;
}

void test_2 ()
{
  std::clog << "==============> test_2" << std::endl;
  mygsl::histogram_pool HP ("A set of histograms");
  HP.set_logging_priority(datatools::logger::PRIO_NOTICE);

  HP.load ("${MYGSL_TESTING_DIR}/config/test_histogram_pool.conf");

  HP.tree_dump (std::clog, "HP : ", "INFO: ");
  std::clog << std::endl;

  std::vector<std::string> histo_names;
  HP.names (histo_names, "*");
  std::clog << "\nHistograms are: \n";
  for (std::vector<std::string>::size_type i = 0; i < histo_names.size (); i++)
    {
      std::clog << histo_names[i] << ' ';
    }
  std::clog << std::endl << std::endl;

  if (HP.has ("h1", mygsl::histogram_pool::HISTOGRAM_DIM_1D))
    {
      mygsl::histogram & h1 = HP.grab_1d ("h1");
      h1.tree_dump (std::clog, "'h1' a beautiful 1D-histogram (empty) :");
      std::clog << std::endl;
   }

  if (HP.has_1d ("h2"))
    {
      mygsl::histogram & h2 = HP.grab_1d ("h2");

      for (int i = 0; i < 1000; i++)
        {
          double r = -50 * std::log (drand48 ());
          h2.fill (r);
        }
      h2.print_ascii (std::clog);
      h2.tree_dump (std::clog, "'h2' another beautiful 1D-histogram :");
      std::clog << std::endl;
    }

  if (HP.has ("h3", mygsl::histogram_pool::HISTOGRAM_DIM_2D))
    {
      mygsl::histogram_2d & h3 = HP.grab_2d ("h3");
      h3.tree_dump (std::clog, "'h3' a beautiful 2D-histogram (empty) :");
      std::clog << std::endl;
    }

  if (HP.has_1d ("h4"))
    {
      mygsl::histogram_1d & h4 = HP.grab_1d ("h4");
      h4.tree_dump (std::clog, "'h4' a beautiful 1D-histogram :");
      std::clog << std::endl;
    }

  HP.tree_dump (std::clog, "My pool of histograms : ");

  return;
}


void test_3 ()
{
  std::clog << "==============> test_3" << std::endl;

  datatools::object_configuration_description OCD;
  if ( !datatools::load_ocd<mygsl::histogram_pool>(OCD)) {
    std::clog << "Cannot find OCD support for the 'mygsl::histogram_pool' class."
              << std::endl;
    return;
  }
  OCD.print(std::clog, "*** ");
  OCD.dump(std::clog, "OCD: ");
  std::ofstream fscf ("test_OCD_histogram_pool.sample.conf");
  OCD.generate_sample_configuration(fscf, "the configuration of a 'mygsl::histogram_pool' object");

  return;
}


void test_4 ()
{
  std::clog << "==============> test_4" << std::endl;

  datatools::object_configuration_description OCD;
  if ( !datatools::load_ocd<mygsl::histogram_1d>(OCD)) {
    std::clog << "Cannot find OCD support for the 'mygsl::histogram_1d' class."
              << std::endl;
    return;
  }
  OCD.print(std::clog, "*** ");
  OCD.dump(std::clog, "OCD: ");
  std::ofstream fscf ("test_OCD_histogram_1d.sample.conf");
  OCD.generate_sample_configuration(fscf, "the configuration of a 'mygsl::histogram_1d' object");

  return;
}


void test_5 ()
{
  std::clog << "==============> test_5" << std::endl;

  datatools::object_configuration_description OCD;
  if ( !datatools::load_ocd<mygsl::histogram_2d>(OCD)) {
    std::clog << "Cannot find OCD support for the 'mygsl::histogram_2d' class."
              << std::endl;
    return;
  }
  OCD.print(std::clog, "*** ");
  OCD.dump(std::clog, "OCD: ");
  std::ofstream fscf ("test_OCD_histogram_2d.sample.conf");
  OCD.generate_sample_configuration(fscf, "the configuration of a 'mygsl::histogram_2d' object");

  return;
}



int main (int /* argc_ */, char ** /* argv_ */)
{
  long seed48 = 314159;
  srand48 (seed48);

  try {
      std::clog << "NOTICE: test program for class 'histogram_pool'..."
                << std::endl;

      std::clog << "NOTICE: Running test #1..." << std::endl;
      test_1 ();

      std::clog << "NOTICE: Running test #2..." << std::endl;
      test_2 ();

      std::clog << "NOTICE: Running test #3..." << std::endl;
      test_3 ();

      std::clog << "NOTICE: Running test #4..." << std::endl;
      test_4 ();

      std::clog << "NOTICE: Running test #5..." << std::endl;
      test_5 ();

      std::clog << "NOTICE: The end." << std::endl;

    }
  catch(std::exception & x) {
      std::cerr << "ERROR: " << x.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}
