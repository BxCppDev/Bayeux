// test_serializable_3.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <boost/scoped_ptr.hpp>

#include <datatools/i_serializable.h>
#include <datatools/archives_instantiation.h>

// The serializable 'things' container :
#include <datatools/things.h>

// The serializable 'properties' container :
#include <datatools/properties.h>
#include <geomtools/geom_id.h>
#include <geomtools/line_3d.h>
#include <geomtools/helix_3d.h>
#include <geomtools/polyline_3d.h>

// Some pre-processor guards about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <geomtools/bio_guard.h>

#include <datatools/library_loader.h>

#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <datatools/i_serializable.ipp>

using namespace std;

/*** the serializable A sample class ***/

class A : DATATOOLS_SERIALIZABLE_CLASS
{

public:

  void set_value (double v)
  {
    value_ = v;
    return;
  }

  double get_value () const
  {
    return value_;
  }

  A ();

  A (double v_);

  virtual ~A ();

  void dump (ostream & = clog) const;


private :

  double value_;

  DATATOOLS_SERIALIZATION_DECLARATION()

};

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(A, "test_things::A")


/*** use some macros to implement serialization stuff for class A ***/
BOOST_CLASS_EXPORT_KEY2 (A, "test_things::A")

/***********************************************************
 * Boost/Serialization export/implement/instantiation code *
 * for the A                                               *
 ***********************************************************/


template<class Archive>
void A::serialize (Archive & ar, const unsigned int /* version */)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp ("value", value_);
  return;
}

/*** use some macros to implement serialization stuff for class A ***/
BOOST_CLASS_EXPORT_IMPLEMENT (A)

void A::dump (ostream & out) const
{
  out << "A::dump : value = " << value_ << endl;
  return;
}

A::A () : value_ (0.0)
{
  return;
}

A::A (double v) : value_ (v)
{
  return;
}

A::~A ()
{
}

/*** serializable B  sample class ***/

class B : DATATOOLS_SERIALIZABLE_CLASS
{
public:

  void set_index (int i)
  {
    index_ = i;
    return;
  }

  int32_t get_index () const
  {
    return index_;
  }

  B () : index_ (0)
  {
    return;
  }

  B (int i) : index_ (i)
  {
    return;
  }

  virtual ~B ()
  {
  }

  void dump (ostream & = clog) const;

private:

  int32_t index_;

  DATATOOLS_SERIALIZATION_DECLARATION()

};

DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(B, "test_things::B")

/*** use some macros to implement serialization stuff for class B ***/
BOOST_CLASS_EXPORT_KEY2 (B, "test_things::B")

template<class Archive>
void B::serialize (Archive & ar, const unsigned int /* version */)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp ("index", index_);
  return;
}

void B::dump (ostream & out) const
{
  out << "B::dump : index = " << index_ << endl;
  return;
}

/*** use some macros to implement serialization stuff for class B ***/
BOOST_CLASS_EXPORT_IMPLEMENT (B)

/*** main ***/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::things' !" << endl;

      // bool debug = false;
      bool out   = true;
      bool in    = true;
      bool xml   = true;

      bool with_ab = true;
      bool with_gbio = true;
      bool with_dbio = true;
      bool load_gbio = false;

      string LL_config = "";
      datatools::library_loader LL (LL_config);
      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              // if ((option == "-d") || (option == "--debug"))
              //   {
              //     debug = true;
              //   }
              // else
              if ((option == "-O") || (option == "--no-out"))
                {
                  out = false;
                }
              else if ((option == "-I") || (option == "--no-in"))
                {
                  in = false;
                }
              else if ((option == "-x") || (option == "--xml"))
                {
                  xml = true;
                }
              else if ((option == "-X") || (option == "--no-xml"))
                {
                  xml = false;
                }
              else if ((option == "--no-ab"))
                {
                  with_ab = false;
                }
              else if ((option == "--no-dbio"))
                {
                  with_dbio = false;
                }
              else if ((option == "--no-gbio"))
                {
                  with_gbio = false;
                }
              else if ((option == "--load-geomtools-bio"))
                {
                  clog << "warning: Loading '" << "geomtools_bio" << "'..." << endl;
                  load_gbio = true;
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

      if (with_gbio)
        {
          load_gbio = true;
        }
      if (load_gbio)
        {
          LL.load ("geomtools_bio");
        }

      if (out)
        {
          // declare the 'bag' instance as a 'things' container:
          datatools::things bag ("bag1", "A bag with things in it");

          if (with_ab)
            {
              // add some objects of type 'A' and 'B' in it
              // perform some on-the-fly setter on some of them :
              bag.add<A> ("a1", "The a1 object").set_value (666.6666);
              bag.add<A> ("a2", "The a2 object").set_value (3.1415);
              bag.add<B> ("b1", "The b1 object").set_index (7654321);
              B & b2 = bag.add<B> ("b2", "A permanent object", datatools::things::constant);
              b2.set_index (6878);
              bag.add<B> ("b3");
              bag.add<A> ("a3", "The a3 object").set_value (42.0);
            }

          if (with_gbio)
            {
              bag.add<geomtools::geom_id> ("g0");

              geomtools::geom_id & gg1 = bag.add<geomtools::geom_id> ("g1");
              gg1.set_type (666);
              gg1.set_address (0, 1, 2);

              geomtools::geom_id & gg2 = bag.add<geomtools::geom_id> ("g2");
              gg2.set_type (999);
              gg2.set_address (4, 3, 2, 1, 0);
            }

          if (with_dbio)
            {
              bag.add<datatools::properties> ("p1", "A property store").set_description ("A list of properties");
            }

          if (with_gbio)
          {
            geomtools::line_3d & l1 = bag.add<geomtools::line_3d> ("l1", "A 3D line");
            l1.set_first (geomtools::vector_3d (0.,0.,0.));
            l1.set_last (geomtools::vector_3d (1.,1.,1.));
            geomtools::helix_3d & h1 = bag.add<geomtools::helix_3d> ("h1", "A 3D helix");
            h1.set_radius(1.0);
            h1.set_step(1.0);
            h1.set_t1(0.0);
            h1.set_t2(1.0);
            h1.set_center(0.0, 0.0, 0.0);
            bag.add<geomtools::polyline_3d> ("pl1", "A 3D polyline");
          }

          // basic dump :
          bag.dump (clog);

          if (with_ab)
            {
              // check if there is a stored object named 'a1',
              // if yes check if the stored object 'a1' is an instance of 'A' :
              if (bag.has ("a1") && bag.is_a<A> ("a1"))
                {
                  // if everything is ok, get a 'A' const reference to the 'a1' object :
                  const A & a1 = bag.get<A> ("a1");
                  a1.dump (clog);
                }
              if (bag.is_a<A> ("a2"))
                {
                  // get a 'A' const reference to the 'a2' object :
                  const A & a2 = bag.get<A> ("a2");
                  a2.dump (clog);
                }
              if (bag.has ("b1") && ! bag.is_a<A> ("b1"))
                {
                  clog << "As expected, the 'b1' object is not an instance of A !" << endl;
                }
              if (bag.is_a<B> ("b1"))
                {
                  // get a 'B' const reference to the 'b1' object :
                  const B & b1 = bag.get<B> ("b1");
                  b1.dump (clog);
                }

              if (bag.is_a<B> ("b2"))
                {
                  // grab a 'B' non-const reference to the 'b1' object :
                  try
                    {
                      B & b2 = bag.grab<B> ("b2");
                      // modify the 'b2' object that is stored in 'bag' :
                      b2.set_index (999);
                    }
                  catch (exception & x)
                    {
                      cerr << "Ooops ! Cannot get a non-const reference on 'b2' : " << x.what () << endl;
                      clog << "As expected, the 'b2' object is a const instance of A !" << endl;
                    }
                }
              if (bag.is_a<B> ("b3") && ! bag.is_constant ("b2"))
                {
                  B & b3 = bag.grab<B> ("b3");
                  b3.set_index (-123);
                }
              try
                {
                  /* this will failed  and should have been first
                   * checked with 'bag.is_a<A> ("b3")' :
                   */
                  const A & tmp = bag.get<A> ("b3");
                  clog << tmp.get_value() << '\n';
                }
              catch (datatools::bad_things_cast & x)
                {
                  clog << "As expected, the 'b3' object cannot be fetch as an instance of A !" << endl;
                }
              /* fetch the 'a3' object without any check :
               *   bag.has ("a3")
               *   bag.is_a<A> ("a3")
               */
              const A & a3 = bag.get<A> ("a3");
              clog << a3.get_value() << '\n';

              // add more empty objects and play with them :
              bag.add<A> ("a4", "another A instance");
              bag.grab<B> ("b3").set_index (7777);
              bag.grab<A> ("a4").set_value (1.6e-19);
              bag.add<B> ("b4", "a default B instance");
            }

          if (with_dbio)
          {
            // add some properties in the 'p1' object :
            datatools::properties & p1 = bag.grab<datatools::properties> ("p1");
            p1.store_flag ("test");
            p1.store ("version.major", 1);
            p1.store ("version.minor", 2);
            p1.store ("version.patch", 10);
            p1.store ("pi", 3.14159);
          }

          if (with_gbio)
          {
            const geomtools::geom_id & g0 = bag.get<geomtools::geom_id> ("g0");
            clog << "g0 = " << g0 << endl;

            geomtools::geom_id & g1 = bag.grab<geomtools::geom_id> ("g1");
            g1.set (2, 1000);
            clog << "g1 = " << g1 << endl;

            const geomtools::geom_id & g2 = bag.get<geomtools::geom_id> ("g2");
            clog << "g2 = " << g2 << endl;
          }

          // dump again the bag :
          bag.dump (clog);

          {
            // now we store the 'bag' contents within a Boost archive :
            string filename = "test_things.txt";
            clog << "Store 'things'..." << endl;
            if (xml)
              {
                filename = "test_things.xml";
              }
            std::ofstream ofile (filename.c_str ());
            if (! xml)
              {
                // text ASCII archive :
                boost::archive::text_oarchive ota (ofile);
                ota & bag;
              }
            else
              {
                // XML archive :
                boost::archive::xml_oarchive oxa (ofile);
                oxa & boost::serialization::make_nvp ("bag", bag);
              }
          }
          clog << "Done." << endl << endl << endl;
        }

      if (in)
        {
          // declare the 'bag' instance as an empty 'things' container:
          datatools::things bag;

          {
            // now we load the 'bag' from a Boost archive :
            string filename = "test_things.txt";
            clog << "Load 'things'..." << endl;
            if (xml)
              {
                filename = "test_things.xml";
              }
            std::ifstream ifile (filename.c_str ());
            if (! xml)
              {
                // text ASCII archive :
                boost::archive::text_iarchive ita (ifile);
                ita & bag;
              }
            else
              {
                // XML archive :
                boost::archive::xml_iarchive ixa (ifile);
                ixa & boost::serialization::make_nvp ("bag", bag);
              }
            clog << "Done." << endl;
          }

          // dump it and check that is has been properly restored
          // from the serialization stream :
          bag.tree_dump (clog, "Bag (loaded) : ");

          // play with the objects stored in 'bag' :
          clog << "Play..." << endl;
          if (with_ab)
            {
              if (bag.has ("a1"))
                {
                  clog << "Object named 'a1' exists in the bag !" << endl;
                }
              else
                {
                  clog << "Object named 'a1' does not exist in the bag !" << endl;
                }
              if (bag.has ("a1") && bag.is_a<A> ("a1") )
                {
                  const A & a1 = bag.get<A> ("a1");
                  a1.dump (clog);
                }
              if (bag.has ("a2") && bag.is_a<A> ("a2"))
                {
                  const A & a2 = bag.get<A> ("a2");
                  a2.dump (clog);
                }
              if (bag.has ("b1") && bag.is_a<B> ("b1"))
                {
                  const B & b1 = bag.get<B> ("b1");
                  b1.dump (clog);
                }
              if (bag.has ("b2") && bag.is_a<B> ("b2"))
                {
                  const B & b2 = bag.get<B> ("b2");
                  b2.dump (clog);
                }
              if (bag.has ("b3") && bag.is_a<B> ("b3"))
                {
                  const B & b3 = bag.get<B> ("b3");
                  b3.dump (clog);
                }
            }
          if (! bag.has ("X"))
            {
              clog << "As expected, there is no object named 'X' in the bag !" << endl;
            }

          /*
          if (with_dbio)
          {
          if (bag.has ("p1") && bag.is_a<datatools::properties> ("p1"))
          {
          clog << "Fetching 'p1'..." << endl;
          datatools::properties & p1 = bag.grab<datatools::properties> ("p1");
          p1.tree_dump(clog, "p1");
          }
}
          */

          if (with_gbio)
            {
              if (bag.has ("l1"))
                {
                  const geomtools::line_3d & l1 = bag.get<geomtools::line_3d> ("l1");
                  l1.tree_dump (clog, "3D-line 'l1':");
                }

              if (bag.has ("pl1"))
                {
                  const geomtools::polyline_3d & pl1 = bag.get<geomtools::polyline_3d> ("pl1");
                  pl1.tree_dump (clog, "3D-polyline 'pl1':");
                }
            }

          if (with_ab)
            {
              if (bag.has ("a1"))
                {
                  // steal the 'a1' object from the bag and put it
                  // in a scope pointer (with autodeletion)
                  boost::scoped_ptr<A> pa1 (bag.pop<A> ("a1"));
                  pa1->set_value (98765.43210);
                  pa1->dump (clog);
                } // a1 is delete here
            }

        } // bag is destroyed here with all objects therein

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

// end of test_serializable_3.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
