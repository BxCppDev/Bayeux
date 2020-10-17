// -*- mode: c++; -*-
// test_cloneable_2.cxx
//! \file programs/test_cloneable_2.cxx
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-04-01
 * Last modified : 2011-04-01
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *  This programs test if the 'cloneable' to 'serializable' casting macros
 *  behaves well while manipulating some instances of a class that inherits
 *  both i_serializable and i_cloneable abstract interfaces.
 *
 */

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

// This Project:
#include <datatools/i_cloneable.h>
#include <datatools/i_serializable.h>

using namespace std;

struct data1 : public datatools::i_cloneable,
               public datatools::i_serializable
{
public:
  string prefix_;
  int    id_;
public:
  static const string  SERIAL_TAG;
  const string & get_serial_tag () const override
  {
    return SERIAL_TAG;
  }

  data1 & upgrade ()
  {
    prefix_ = prefix_ + "_upgraded";
    id_ += 1000;
    return *this;
  }
  data1 (const string & a_prefix, int a_id ) :
    prefix_ (a_prefix), id_ (a_id)
  {
  }
  virtual ~data1 ()
  {
    clog << "data1::dtor: Destroy data1 instance." << endl;
    return;
  }
  void print () const
  {
    cout << "data1 : prefix=\"" << prefix_ << "\" , " << "id=" << id_ << endl;
  }

public:
  DATATOOLS_CLONEABLE_DECLARATION(data1)
  DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_DECLARATION(data1)

};

const string data1::SERIAL_TAG = "data1";

DATATOOLS_CLONEABLE_IMPLEMENTATION(data1)
DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_IMPLEMENTATION(data1)

int main (int /*argc_*/ , char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test of the 'datatools::i_cloneable' interface..." << endl;
      /*
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];
          if ((arg == "-d") || (arg == "--debug")) debug = true;
          iarg++;
        }
      */

      {
        data1 d1 ("d1", 1);
        d1.print ();

        data1 * cd1 = datatools::i_cloneable::clone_it (d1);
        cd1->upgrade ().print ();
        if (cd1 != 0) delete cd1;

        datatools::i_caster<datatools::i_cloneable, datatools::i_serializable> * clone_caster
          = d1.get_cloneable_to_serializable_caster ();
        datatools::i_cloneable * d1_clone = d1.clone ();

        clog << "d1_clone typename is '" << typeid(d1_clone).name () << "' @ " << d1_clone << endl;

        datatools::i_serializable * d1_serial = clone_caster->cast (d1_clone);
        clog << "d1_serial typename is '" << typeid(d1_serial).name () << "'" << "' @ " << d1_serial << endl;
        clog << "d1_serial serial tag is '" << d1_serial->get_serial_tag () << "'" << endl;

        data1 * d1_cast = dynamic_cast<data1 *> (d1_serial);
        d1_cast->print ();
        clog << "d1_cast typename is '" << typeid(d1_cast).name () << "'" << "' @ " << d1_cast << endl;

        if (d1_clone != 0) delete d1_clone;
      }

    }
  catch (exception & x)
    {
      clog << "error: " << x.what () << endl;
      error_code =  EXIT_FAILURE;
    }
  catch (...)
    {
      clog << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}
