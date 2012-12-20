// -*- mode: c++ ; -*- 
// test_smart_ref.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
 
#include <datatools/smart_ref.h>

using namespace std;

/*** A dummy class to be referenced by a 'smart_ref' instance ***/
class dummy 
{
  int __a;
  int __b;
public:
  dummy (int a_ = 0, int b_ = 0)
  {
    __a = a_;
    __b = b_;
  }
  int get_a () const
  {
    return __a;
  }
  int get_b () const
  {
    return __b;
  }
  void print (ostream & out_) const
  {
    out_ << '(' << __a << ',' << __b << ')';
  }

  friend ostream & operator<< (ostream & out_, const dummy & d_);
};

ostream & operator<< (ostream & out_, const dummy & d_)
{
  d_.print (out_);
  return out_;
}

/***************************************/

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'smart_ref'!" << endl; 
  
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
    
      const size_t SZ = 10;
      dummy t[SZ];
      for (int i = 0; i < SZ; i++)
        {
          t[i] = dummy (i, 3*i);
          clog << t[i] << endl;
        }
      
      const size_t SZ2 = 4; 
      datatools::smart_ref<dummy> t2[SZ2];
      for (int i = 0; i < SZ2; i++)
        {
          t2[i].set (t[i + 1]);
          datatools::smart_ref<dummy> SR;
          SR.set (t[3]);
          t2[i].get_properties ().store ("color", "red");
          t2[i].get_properties ().store ("flavor", "charm");
          t2[i].get_properties ().store ("spin", 1);
          t2[i].get_properties ().store ("mass_GeV", 1.342);
          t2[i].get_properties ().store_flag ("beautiful");
          t2[i].get_properties ().store_flag ("dummy");
          cout << "Referenced dummy : " << t2[i].get () << endl;
          if ((t2[i].get ().get_a () % 2) == 0) 
            {
              t2[i].get_properties ().store_flag ("a_is_even");
            }
          else
            {
              t2[i].get_properties ().store_flag ("a_is_odd");
            }
        }

      datatools::smart_ref<dummy>::has_flag pred ("a_is_odd");
      for (int i = 0; i < SZ2; i++)
        {
          if (pred (t2[i]))
            {
              cout << "Referenced dummy with odd 'a': " << t2[i].get () << endl;              
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

// end of test_smart_ref.cxx
