#include <iostream>
#include <fstream>
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
    A a1 (1);
    A a2 (2);
    B b1 (a1);
    B b2 (a2);
    B b3 (a2);
    B b4;
    B b5 (a1);

    string fname = "mt0.xml";
    ofstream fout (fname.c_str ());
    
    boost::archive::xml_oarchive ar (fout);

    // store A's:
    ar & boost::serialization::make_nvp ("a1", a1);
    ar & boost::serialization::make_nvp ("a2", a2);

    // then store B's: 
    ar & boost::serialization::make_nvp ("b1", b1);
    ar & boost::serialization::make_nvp ("b2", b2);
    ar & boost::serialization::make_nvp ("b3", b3);
    ar & boost::serialization::make_nvp ("b4", b4);
    ar & boost::serialization::make_nvp ("b5", b5);
  }

  return 0;
}
