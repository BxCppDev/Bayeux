#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/archive/xml_oarchive.hpp>

using namespace std;

class A
{
  int __i;
public:
  A (int i_) : __i (i_) {}
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("i", __i);
  }
};

class B
{
  A * __p;
public:
  B () : __p (0) {}
  B (A & a_) : __p (&a_) {}
  void set (A & a_)
  {
    __p = &a_;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("p", __p);
  }
};

int main ()
{
  {
    string fname = "mt1.xml";
    ofstream fout (fname.c_str ());
    
    boost::archive::xml_oarchive ar (fout);

    // store A's:
    for (int i = 0; i < 1; i++)
      {
	A a (i+1);
	ostringstream name;
	name << "a" << (i+1);
	ar & boost::serialization::make_nvp (name.str ().c_str (), a);
      }

    // then store B's: 
    for (int i = 0; i < 5; i++)
      {
	B b;
	if ((i == 0) || (i == 4)) b.set (a1);
	if ((i == 1) || (i == 2)) b.set (a2);
	ostringstream name;
	name << "b" << (i+1);
	ar & boost::serialization::make_nvp (name.str ().c_str (), b);
      }
  }

  return 0;
}
