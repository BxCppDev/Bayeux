//

#include <iostream>
#include <fstream>
#include <exception>
#include <limits>

#include <boost/cstdint.hpp>
#include <boost/qpba/quasi_portable_binary_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/qpba/quasi_portable_binary_iarchive.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

struct A
{
  int32_t a;
  double  x;
  string  s;
  A () : a (0), x (0.0), s() {}
  A (int32_t aa, double xx, const string & ss) : a (aa), x(xx), s(ss) {}
 
private:
   
  friend class boost::serialization::access;
  template<class Archive>
  void serialize (Archive & ar, const unsigned int file_version)
  {
    //ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
    ar & boost::serialization::make_nvp("a", a);
    ar & boost::serialization::make_nvp("x", x);
    ar & boost::serialization::make_nvp("s", s);
    return;
  }
    
};

ostream & operator<< (ostream & out, const A & a)
{
  out << "{class=A: a=" << a.a << "; x=" << a.x<< "; s='" << a.s << "'}";
  return out;
}

struct B
{
  int16_t i;
  B () : i (-1) {}
  B (int16_t ii) : i (ii) {}

private:
   
  friend class boost::serialization::access;
  template<class Archive>
  void serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("i", i);
    return;
  }
    
};

ostream & operator<< (ostream & out, const B & b)
{
  out << "{class=B: b=" << b.i << "'}";
  return out;
}

int main (void)
{
  try
    {
      bool io = true;
      {
	A a0 (0, 0, "");
	A a1 (111, 3.14159, "pi");
	A a2 (222, +std::numeric_limits<double>::infinity (), "+infinity");
	A a3 (333,  std::numeric_limits<double>::quiet_NaN (), "Nan");
	A a4 (444, -std::numeric_limits<double>::infinity (), "-infinity");
	cout << "a0 = " << a0 << endl;
	cout << "a1 = " << a1 << endl;
	cout << "a2 = " << a2 << endl;
	cout << "a3 = " << a3 << endl;
	cout << "a4 = " << a4 << endl;

	if (io)
	  {
	    clog << "Storing..." << endl;
	    {
	      ofstream sf ("prg1.data");
	      boost::archive::quasi_portable_binary_oarchive oa (sf);
	      oa & a0;
	      oa & a1;
	      oa & a2;
	      oa & a3;
	      oa & a4;
	    }
	    {
	      ofstream sf ("prg1.xml");
	      boost::archive::xml_oarchive oa (sf);
	      oa & boost::serialization::make_nvp("a0",a0);
	      oa & boost::serialization::make_nvp("a1",a1);
	      oa & boost::serialization::make_nvp("a2",a2);
	      oa & boost::serialization::make_nvp("a3",a3);
	      oa & boost::serialization::make_nvp("a4",a4);
	    }
	    {
	      ofstream sf ("prg1.txt");
	      boost::archive::text_oarchive oa (sf);
	      oa & a0;
	      oa & a1;
	      oa & a2;
	      oa & a3;
	      oa & a4;
	    }
	  }
      }

      {
	A a0;
	A a1;
	A a2;
	A a3;
	A a4;
	if (io)
	  {
	    clog << endl << "Loading..." << endl;
	    ifstream lf ("prg1.data");
	    boost::archive::quasi_portable_binary_iarchive ia (lf);
	    ia & a0;
	    ia & a1;
	    ia & a2;
	    ia & a3;
	    ia & a4;
	  }
	cout << "a0 = " << a0 << endl;
	cout << "a1 = " << a1 << endl;
	cout << "a2 = " << a2 << endl;
	cout << "a3 = " << a3 << endl;
	cout << "a4 = " << a4 << endl;
     }

    }
  catch (exception &x)
    {
      cerr << "error: " << x.what () << endl;
      return 1;
    }
  return 0;
}
