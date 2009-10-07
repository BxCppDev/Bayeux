// foo.hpp

#ifndef __foo_h__
#define __foo_h__ 1

#include <cstdlib>
#include <string>
#include <list>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>

using namespace std;

class fuu
{
  double __value;

public:

  fuu ()
  {
    __value = 0;
    clog << "fuu::fuu: constructor at " << this << " done." << endl;	
  }

  ~fuu ()
  {
    clog << "fuu::~fuu: destructor at " << this << " done." << endl;	
  }

  fuu (double v_)
  {
    set_value (v_);
  }

  void set_value (double v_)
  {
    __value = v_;
  }

  double get_value () const
  {
    return __value;
  }

private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("value", __value);
  }
};

ostream & operator<< (ostream & out_, const fuu & f_)
{
  out_ << "(value=" << f_.get_value () 
       << ")";
  return out_;
}

class foo
{
  list<fuu*> __fuus;
  double     __sum;
  fuu*       __max_fuu;
public:
  static std::map<fuu*,int> g_addresses;

  foo ()
  {
    __sum = 0;
    clog << "foo::foo: constructor done." << endl;	
  }

  void clear_fuus ()
  {
    for (list<fuu*>::iterator it = __fuus.begin ();
	 it != __fuus.end ();
	 it++)
      {
	if (*it != 0)
	  {
	    delete *it;
	    clog << "**** NOTICE: Address [" << *it << "] is deleted!" << endl;	    
	    *it = 0;
	  }
      }
    __fuus.clear ();
    __sum = 0;
    __max_fuu = 0;
    clog << "foo::clear_fuus: list of fuus has been deallocated and cleared." << endl;	 
  }

  ~foo ()
  {
    if (__fuus.size () > 0) clear_fuus ();
    clog << "foo::~foo: destructor done." << endl;	
  }

  double check_sum () const
  {
    double s = 0.;
    
    for (list<fuu*>::const_iterator it = __fuus.begin ();
	 it != __fuus.end ();
	 it++)
      {
	const fuu * f = *it;
	s += f->get_value ();
      }
    return s;
  }

  void init_fuus (int n_)
  {
    __sum = 0;
    __max_fuu = 0;
    double max_val = 0.0;
    for (int i = 0; i < n_; i++)
      {
	double val = drand48 ();
	fuu * f = new fuu (val);
	clog << "**** NOTICE: Address " << f << " is allocated!" << endl;
	if (g_addresses.find (f) != g_addresses.end ())
	  {
	    clog << "**** WARNING: Address [" << f << "] has already been used before!" << endl;
	    g_addresses[f]++;
	  }
	else
	  {
	    g_addresses[f] = 1;
	  }
	if (val > max_val)
	  {
	    max_val = val;
	    __max_fuu = f;
	  }
	__fuus.push_back (f);
	__sum += val; 
      }
  }

  const list<fuu*> & get_fuus () const
  {
    return __fuus;
  }

  list<fuu*> & get_fuus ()
  {
    return __fuus;
  }

  const fuu * get_max_fuu () const
  {
    return __max_fuu;
  }

  double get_sum () const
  {
    return __sum;
  }

  static void dynamic_memory_accounting ()
  {
  clog << "Dynamic memory accounting:" << endl;  
  bool multi_used = false;
  for (std::map<fuu*,int>::const_iterator i = g_addresses.begin ();
       i != g_addresses.end ();
       i++)
    {
      clog << "|-- Address [" << i->first << "]" << " = " << i->second << endl;
      if (i->second > 1) multi_used = true;
    }
  if (multi_used)
    {
      clog << "`-- Some address has been used many times." << endl;
    }
  else
    {
      clog << "`-- No address has been reused." << endl;
    }
  }

private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("fuus", __fuus);
    ar_ & boost::serialization::make_nvp ("sum", __sum);
    ar_ & boost::serialization::make_nvp ("max_fuu", __max_fuu);
  }
};

std::map<fuu*,int> foo::g_addresses;

ostream & operator<< (ostream & out_, const foo & f_)
{
  out_ << "(sum=" << f_.get_sum ();
  int count = 0;
  for (list<fuu*>::const_iterator it = f_.get_fuus ().begin ();
       it != f_.get_fuus ().end ();
       it++)
    {
      if (*it == 0) continue;
      //out_ << ",fuu[" << count << "]=";
      out_ << ",fuu[" << *it << "]=";
      out_ << **it;
      count++;
    }
  double sum = f_.get_sum ();
  double check_sum = f_.check_sum ();
  out_ << ",max_fuu=[" << f_.get_max_fuu () << ']';
  out_ << ",check_sum=" << check_sum;
  out_ << ",status=" << ((sum == check_sum) ? "OK" : "ERROR");
  out_ << ')';
  return out_;
}

#endif // __foo_h__

// end of foo.hpp
