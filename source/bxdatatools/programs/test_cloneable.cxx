// -*- mode: c++; -*- 
// test_cloneable.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/i_cloneable.h>

using namespace std;

struct foo : public datatools::utils::i_cloneable
{
public:
  string prefix_;
  int    id_;
public:
  foo & prime ()
  {
    prefix_ = prefix_ + "'";
    return *this;
  }
  foo (const string & a_prefix, int a_id ) :
    prefix_ (a_prefix), id_ (a_id)
  {
  }

  DATATOOLS_CLONEABLE_DECLARATION(foo)

  /*
  virtual foo * clone (void) const
  {
    foo * f = new foo (*this);
    return f;
  }
  */

  void print () const
  {
    cout << "foo : prefix=\"" << prefix_ << "\" , " << "id=" << id_ << endl;
  }

};

DATATOOLS_CLONEABLE_IMPLEMENTATION(foo)

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'datatools::utils::i_cloneable' interface..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
	{
          string arg = argv_[iarg];
	  if ((arg == "-d") || (arg == "--debug")) debug = true;
	  iarg++;
	}
    
      {
	foo f1 ("f1", 1);
	foo f2 ("f2", 2);
	foo f3 ("f3", 3);
	vector<datatools::utils::i_cloneable * > v;
	v.push_back (&f1);
	v.push_back (&f2);
	v.push_back (&f3);
	dynamic_cast<foo*> (v[0])->print ();
	dynamic_cast<foo*> (v[1])->print ();
	dynamic_cast<foo*> (v[2])->print ();

	// populate using cloned objects:
	vector<datatools::utils::i_cloneable * > v2;
	v2.reserve (v.size ());
	v2.assign (v.size (), 0); // filled with NULL pointers
	for (int i = 0; i < v2.size (); i++)
	  {
	    v2[i] = v[i]->clone ();
	    dynamic_cast<foo*>(v2[i])->prime ().print ();
	    //	  dynamic_cast<foo*>(v2[i])->print ();
	  }

	// delete cloned objects:
	for (int i = 0; i < v2.size (); i++)
	  {
	    if (v2[i] != 0) delete v2[i];
	  }
      }
 
      {
	foo f4 ("f4", 4);
	f4.print ();
	foo * cf4 = datatools::utils::i_cloneable::clone_it (f4);
	cf4->prime ().print ();
	delete cf4;
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

// end of test_cloneable.cxx 
  
