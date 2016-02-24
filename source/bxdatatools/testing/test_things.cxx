// test_things.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

// This Project:
#include <datatools/serialization_macros.h>
#include <datatools/i_serializable.h>
#include <datatools/archives_instantiation.h>
// The serializable 'things' container :
#include <datatools/things.h>
// The serializable 'properties' container :
#include <datatools/properties.h>
// Some pre-processor guard about Boost I/O usage and linkage :
//#include <datatools/bio_guard.h>

using namespace std;

/// \brief the serializable A sample class
class A : public datatools::i_serializable
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

public:

  static const string SERIAL_TAG;

  virtual const string & get_serial_tag () const;

private:
  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

private :

  double value_;

};

const string A::SERIAL_TAG = "test_things::A";

const string & A::get_serial_tag () const
{
  return A::SERIAL_TAG;
}

template<class Archive>
void A::serialize (Archive & ar, const unsigned int /*version*/)
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

/// \brief the serializable B sample class
class B : public datatools::i_serializable
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

public:

  static const string SERIAL_TAG;

  virtual const string & get_serial_tag () const;

private:

  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

private:

  int32_t index_;

};

const string B::SERIAL_TAG = "test_things::B";

const string & B::get_serial_tag () const
{
  return B::SERIAL_TAG;
}

template<class Archive>
void B::serialize (Archive & ar, const unsigned int /*version*/)
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

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'datatools::things' !" << endl;

      bool out   = true;
      bool in    = true;
      bool xml   = true;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
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

      if (out)
      {
        // declare the 'bag' instance as a 'things' container:
        datatools::things bag ("bag1", "A bag with things in it");

        // add some objects of type 'A' and 'B' in it
        // perform some on-the-fly setter on some of them :
        bag.add<A> ("a1", "The a1 object").set_value (666.6666);
        bag.add<A> ("a2", "The a2 object").set_value (3.1415);
        bag.add<B> ("b1", "The b1 object").set_index (7654321);
        B & b2 = bag.add<B> ("b2", "A permanent object", datatools::things::constant);
        b2.set_index (6878);
        bag.add<B> ("b3");
        bag.add<A> ("a3", "The a3 object").set_value (42.0);
        bag.add<datatools::properties> ("p1", "A property store").set_description ("A list of properties");
        // basic dump :
        bag.dump (clog);

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
        if (bag.has_serial_tag ("a2", "test_things::A"))
          {
            clog << "As expected, the 'a2' object has serial tag 'test_things::A' !" << endl;
          }
        if (! bag.has_serial_tag ("a2", "test_things::B"))
          {
            clog << "As expected, the 'a2' object has no serial tag 'test_things::B' !" << endl;
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
            // grab a 'B' non-const reference to the 'b2' object :
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
        if (bag.is_a<B> ("b3") && ! bag.is_constant ("b3"))
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

            clog << "tmp.value = " << tmp.get_value () << endl;
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
        clog << "a3.value = " << a3.get_value () << endl;

        // add more empty objects and play with them :
        bag.add<A> ("a4", "another A instance");
        bag.grab<B> ("b3").set_index (7777);
        bag.grab<A> ("a4").set_value (1.6e-19);
        bag.add<B> ("b4", "a default B instance");

        // add some propeties in the 'p1' object :
        datatools::properties & p1 = bag.grab<datatools::properties> ("p1");
        p1.store_flag ("test");
        p1.store ("version.major", 1);
        p1.store ("version.minor", 2);
        p1.store ("version.patch", 10);
        p1.store ("pi", 3.14159);

        // dump again the bag :
        bag.dump (clog);

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
        clog << "Done." << endl << endl << endl;
      }

      if (in)
      {
        // declare the 'bag' instance as an empty 'things' container:
        datatools::things bag;

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

        // dump it and check that is has been properly restored
        // from the serialization stream :
        bag.dump (clog);

        // play with the objects stored in 'bag' :
        clog << "Play..." << endl;
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
        if (! bag.has ("X"))
          {
            clog << "As expected, there is no object named 'X' in the bag !" << endl;
          }
        if (bag.has ("p1") && bag.is_a<datatools::properties> ("p1"))
          {
            clog << "Fetching 'p1'..." << endl;
            datatools::properties & p1 = bag.grab<datatools::properties> ("p1");
            p1.tree_dump(clog, "p1");
          }

        if (bag.has ("a1"))
          {
            // steal the 'a1' object from the bag and put it
            // in a scope pointer (with autodeletion)
            boost::scoped_ptr<A> pa1 (bag.pop<A> ("a1"));
            pa1->set_value (98765.43210);
            pa1->dump (clog);
          } // a1 is delete here

        bag.dump (clog);

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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
