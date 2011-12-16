#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class I
{
  int __i;
public:
  I (int i_) : __i(i_) {}
  int get_i () const 
  {
    return __i;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("i", __i);
  }
};

class A
{
  int       __n;
  vector<I> __v;
public:
  int get_n () const 
  {
    return __n;
  }
  A () : __n (0) 
  {
  }
  void set (int n_) 
  {
    __v.clear ();
    __n = n_;
    for (int i = 1; i <= __n; i++)
      {
	I ii (i);
	__v.push_back (ii);
      }
  }
  A (int n_) : __n (0)
  {
    set (n_);
  }
  int sum () const
  {
    int s = 0;
    for (int i = 0; i < (int) __v.size (); i++) s += __v[i].get_i ();
    return s;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("n", __n);
    ar_ & boost::serialization::make_nvp ("v", __v);
  }
};

int main ()
{

  string fname = "mte1b.xml";
  ofstream fout (fname.c_str ());
  
  boost::archive::xml_oarchive ar (fout);

  const size_t NA = 10000;

  for (int i = 0; i < (int) NA; i++)
  {
    A a;
    a.set (i);
    /*
    cout << "a[" << i << "] : n=" << a.get_n () 
	 << " sum=" << a.sum () << endl;
    */
    // store A's:
    ar & boost::serialization::make_nvp ("A", a);
  }

  return 0;
}
