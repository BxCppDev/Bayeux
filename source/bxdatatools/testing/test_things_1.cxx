// test_things_1.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <vector>

#include <boost/foreach.hpp>

#include <datatools/serialization_macros.h>
#include <datatools/i_serializable.h>
#include <datatools/archives_instantiation.h>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

// The serializable 'things' container :
#include <datatools/things.h>
// The serializable 'properties' container :
#include <datatools/properties.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>

using namespace std;

/*** the serializable A sample class ***/

class A : public datatools::i_serializable
{
public:

  static const string SERIAL_TAG;

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

  virtual const string & get_serial_tag () const;

  void dump (ostream & = clog) const;

private:
  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

  private :

  double value_;

};

const string A::SERIAL_TAG = "test_things::A";

template<class Archive>
void A::serialize (Archive & ar, const unsigned int file_version)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp ("value", value_);
  return;
}

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

const string & A::get_serial_tag () const
{
  return A::SERIAL_TAG;
}

/*** serializable B  sample class ***/

class B : public datatools::i_serializable
{
public:

  static const string SERIAL_TAG;

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

  virtual const string & get_serial_tag () const;

private:

  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

  private:

  int32_t index_;

};

template<class Archive>
void B::serialize (Archive & ar, const unsigned int file_version)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp ("index", index_);
  return;
}

const string B::SERIAL_TAG = "test_things::B";

const string & B::get_serial_tag () const
{
  return B::SERIAL_TAG;
}

void B::dump (ostream & out) const
{
  out << "B::dump : index = " << index_ << endl;
  return;
}

/***********************************************************
 * Boost/Serialization export/implement/instantiation code *
 * for the A and B classes                                 *
 ***********************************************************/

/*** use some macros to implement serialization stuff for class A ***/
BOOST_CLASS_EXPORT_KEY2 (A, "test_things::A")
BOOST_CLASS_EXPORT_IMPLEMENT (A)
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(A)

/*** use some macros to implement serialization stuff for class B ***/
BOOST_CLASS_EXPORT_KEY2 (B, "test_things::B")
BOOST_CLASS_EXPORT_IMPLEMENT (B)
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(B)


/*** main ***/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::things' !" << endl;

      bool debug = false;
      bool out   = true;
      bool in    = true;
      string format = "text";
      string compression = "";

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
              else if ((option == "-O") || (option == "--no-out"))
                {
                  out = false;
                }
              else if ((option == "-I") || (option == "--no-in"))
                {
                  in = false;
                }
              else if ((option == "-x") || (option == "--xml"))
                {
                  format = "xml";
                }
              else if ((option == "-b") || (option == "--bin"))
                {
                  format = "binary";
                }
              else if ((option == "-z") || (option == "--gzip"))
                {
                  compression = ".gz";
                }
              else if ((option == "-B") || (option == "--bz2"))
                {
                  compression = ".bz2";
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

      // declare the 'bag' instance as a 'things' container:
      datatools::things bag ("bag1", "A bag with things in it");

      // add some objects of type 'A' and 'B' in it
      // perform some on-the-fly setter on some of them :
      bag.add<A> ("a1").set_value (666.6666);
      bag.add<A> ("a2").set_value (3.1415);
      bag.add<B> ("b1").set_index (7654321);
      bag.add<B> ("b2").set_index (1);
      bag.add<B> ("b3").set_index (2);
      bag.add<A> ("a3").set_value (42.0);
      bag.add<datatools::properties> ("p1").set_description ("A list of properties");
      bag.add<A> ("a4");
      bag.grab<B> ("b3").set_index (7777);
      bag.grab<A> ("a4").set_value (1.6e-19);
      bag.add<B> ("b4");
      // here we put a bag in the bag :
      bag.add<datatools::things> ("g1").set_name ("sub_bag").set_description ("A bag stored in another bag");

      // fetch the 'propeties' container stored with name 'p1' :
      datatools::properties & p1 = bag.grab<datatools::properties> ("p1");
      p1.store_flag ("test");
      p1.store ("version.major", 1);
      p1.store ("version.minor", 2);
      p1.store ("version.patch", 10);
      p1.store ("pi", 3.14159);

      // fetch the 'things' container stored with name 'g1' :
      datatools::things & g1 = bag.grab<datatools::things> ("g1");
      g1.add<A> ("x1").set_value (33.0);
      g1.add<A> ("x2").set_value (12.0);
      g1.add<B> ("y1").set_index (7);

      // dump the bag :
      bag.tree_dump (clog, "The bag:");

      // DO NOT COMPILE (NO PUBLIC COPY CONSTRUCTOR) :
      /*
      datatools::things bag2 = bag;
      */

      // DO NOT COMPILE (NO PUBLIC COPY CONSTRUCTOR) :
      /*
      vector<datatools::things> bags;
      datatools::things dummy;
      bags.push_back (dummy);
      for (int i = 0; i < bags.size (); i++)
        {
          bags[i].tree_dump ();
        }
      */

      {
        clog << endl << "A container of pointers to bags..." << endl;
        typedef vector<datatools::things *> bag_col_t;
        bag_col_t bags;
        bags.push_back (new datatools::things ("O1", "a bag"));
        bags.push_back (new datatools::things ("O2", "another bag"));
        bags.push_back (new datatools::things ("O3", "yet another bag"));

        BOOST_FOREACH(datatools::things * bag, bags)
          {
            if (bag != 0) bag->tree_dump (clog, "Stored bag: ");
          }

        clog << "Delete stored bags..." << endl;
        BOOST_FOREACH(datatools::things * bag, bags)
          {
            if (bag != 0) delete bag;
          }
      }

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

// end of test_things_1.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
