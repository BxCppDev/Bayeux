// -*- mode: C++; -*- 
/* foo2.h */
/* 
 * Author: F. Mauger <mauger@lpccaen.in2p3.fr>
 * Date:   2009-03-10
 *
 * Copyright (c) 2008 François Mauger
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy 
 * at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef __foo2_h
#define __foo2_h 1

#include <string>
#include <iostream>
#include <limits>

#include <boost/cstdint.hpp> // use portable integers

using namespace std;

// define a new user class to be serialized: 
class foo2 
{
  string __message;
  
 public:

  static const string SERIAL_TAG; // the serialization ID of the class

  foo2 ()
    {
      __message = "What is your name?";
    }

  void randomize ()
    {
      // random initialization:
      double prob;
      prob = drand48 ();
      __message = "What is your name?";
      if (prob < 0.5) __message = "What is your quest?";
      if (prob < 0.25) __message = "What is your favorite color?";
    }

  const string & get_message () const
    {
      return __message;
    }

 private:
  friend class boost::serialization::access;
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::make_nvp ("message", __message);
    }
};

ostream & operator<< (ostream & out_, const foo2 & d_)
{
  out_ << "(message='" << d_.get_message () 
       << "')";
  return out_;
}

const string foo2::SERIAL_TAG = "__foo2__";

#endif // __foo2_h 1

/* end of foo2.h */
