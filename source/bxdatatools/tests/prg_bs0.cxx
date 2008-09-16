/* boost serialization test program */
/*
 * A simple 'data1' class to be serialized:
 * no pointer to some instance of this class
 * is serialized, so it is not tracked
 * for the trait 'track_selectively' is used by default.
 *
 * It is equivalent to the use of:
 * BOOST_CLASS_TRACKING(data1, boost::serialization::track_never)
 * within the program.
 *
 * This program run as expected without data corruption.
 *
 */

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <boost/cstdint.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/serialization/list.hpp>

class data1
{
private:

  int32_t            __dummy;
  std::list<int32_t> __vals;
public:

  int32_t val() const { return __dummy; }

  data1(int32_t dummy_ = 0, uint32_t sz_ = 0) : __dummy(dummy_) 
  {
    for (int i = 0; i < sz_; i++) 
      {
	__vals.push_back(i);
      }
  }

  friend std::ostream & operator<<(std::ostream & out_, const data1 & d_);

private:

  friend class boost::serialization::access; 

  template<class Archive>
  void serialize(Archive & ar_ , 
		 const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp("dummy", __dummy);
    ar_ & boost::serialization::make_nvp("vals", __vals);
  }  

};

std::ostream & operator<<(std::ostream & out_, const data1 & d_)
{
  out_ << "[data1.__dummy=" << d_.__dummy;
  out_ << ",data1.__vals={";
  size_t count=0;
  for (std::list<int32_t>::const_iterator i = d_.__vals.begin();
       i != d_.__vals.end();
       i++)
    {
      count++;
      out_ << *i;
      if (count != d_.__vals.size()) out_ << ',';
    }
  out_ << '}';
  out_ << "]@" << &d_;
  return out_;
}

/*****************/

void write_1()
{
  std::cerr << "write_1: entering..." << std::endl;

  boost::filesystem::path       out_path("prg_bs0.out.txt"); 
  std::ofstream                 fout(out_path.string().c_str());
  boost::archive::text_oarchive oar(fout);

  boost::filesystem::path       xout_path("prg_bs0.out.xml"); 
  std::ofstream                 xfout(xout_path.string().c_str());
  boost::archive::xml_oarchive  xoar(xfout);
 
  const uint32_t ndata = 10;
  oar  << ndata;
  xoar << boost::serialization::make_nvp("ndata",ndata);
  for (int i = 0; i < ndata; i++)
    {
      data1 d(i,(i%4)+1);
      std::cerr << "i=" << i << " data=" << d << std::endl;
      const data1 & d_ref = d;
      oar  << d_ref; 
      xoar << boost::serialization::make_nvp("data1",d_ref);
    }
  fout.close();
  xfout.close();
  std::cerr << "write_1: exiting." << std::endl;
}

void read_1()
{
  std::cerr << "read_1: entering..." << std::endl;

  boost::filesystem::path       in_path("prg_bs0.out.txt"); 
  std::ifstream                 fin(in_path.string().c_str());
  boost::archive::text_iarchive iar(fin);

  uint32_t ndata;
  iar >> ndata;
  for (int i = 0; i < ndata; i++)
    {
      data1 d;
      iar >> d;
      if (d.val() != i)
	{
	  std::cerr << "ERROR: Incorrect read..." << std::endl;
	}
      std::cout << "i=" << i << " object="<< d << std::endl;
    }
  std::cerr << "read_1: exiting." << std::endl;
}

int main(void)
{
  write_1();
  read_1();
  return 0;
}

/* end */
