// -*- mode: C++; -*- 
/* foo.h */
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

#ifndef __foo_h
#define __foo_h 1

#include <string>
#include <iostream>
#include <limits>

#include <boost/cstdint.hpp> // use portable integers

using namespace std;

// define a dummy class to be serialized: 
class foo 
{
  uint32_t __count;
  float    __v1;
  double   __v2;
  string   __name;
  
 public:

  static const string SERIAL_TAG; // the serialization ID of the class

  foo ()
    {
      __count = 0;
      __v1 = 0.0F;
      __v2 = 0.0;
      __name = "King Arthur";
    }

  void randomize (bool with_nans_ = false)
    {
      // random initialization:
      double prob;
      __count = (int) (drand48 () * 100);

      __v1 = (float) (drand48 () * 1.e-10);
      __v2 = drand48 () * 1.e+6;
      if (with_nans_)
	{
	  prob = drand48 ();
	  if (prob < 0.5) inf ();
	  if (prob < 0.25) nan ();
	}
      prob = drand48 ();
      __name = "King Arthur";
      if (prob < 0.5) __name = "Sir Gawain";
      if (prob < 0.25) __name = "African swallow";
      if (prob < 0.10) __name = "Two african swallows";
      if (prob < 0.03) __name = "A coconut";
    }

  void inf ()
    {
      // force infinites floatings:
      __v1 = -std::numeric_limits<float>::infinity ();
      __v2 = std::numeric_limits<double>::infinity ();
    }

  void nan ()
    {
      // force NaN floatings:
      __v1 = std::numeric_limits<float>::quiet_NaN ();
      __v2 = std::numeric_limits<double>::quiet_NaN ();
    }

  int get_count () const
    {
      return __count;
    }

  double get_v1 () const
    {
      return __v1;
    }

  double get_v2 () const
    {
      return __v2;
    }

  const string & get_name () const
    {
      return __name;
    }

 private:
  friend class boost::serialization::access;
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::make_nvp ("count", __count);
      ar & boost::serialization::make_nvp ("v1", __v1);
      ar & boost::serialization::make_nvp ("v2", __v2);
      ar & boost::serialization::make_nvp ("name", __name);
    }
};

ostream & operator<< (ostream & out_, const foo & d_)
{
  out_ << "(count=" << d_.get_count () 
       << " v1=" << d_.get_v1 () 
       << " v2=" << d_.get_v2 ()
       << " name=`" << d_.get_name ()
       << "')";
  return out_;
}

const string foo::SERIAL_TAG = "__foo__";

#endif // __foo_h 1

/* end of foo.h */
