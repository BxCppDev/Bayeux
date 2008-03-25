/* boost serialization test program */
/*
 * The simple 'data1' class will still be serialized:
 * The class 'data2' uses some pointer 
 * to some instance of this 'data1' class
 * and is also serialized.
 * The tracking is activated as mentionned when
 * the default trait 'track_selectively' is used.
 *
 * As the same address is used as a temporary instance
 * to write/read the data from the archive,
 * the serialization fail due to tracking by memory address.
 *
 * A trick to avoid tracking side-effect is to use
 * one archive per object to be serialized within a same
 * file as attached I/O stream.
 * Nevertheless, for text archive, one must add a blank 
 * or newline separator
 * after each archive in the output stream,
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

//#include <boost/serialization/export.hpp>

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
    for (int i=0; i<sz_; i++) 
      {
	__vals.push_back(i);
      }
  }

  friend std::ostream & operator<<( std::ostream & out_ , const data1 & d_ );

private:

  friend class boost::serialization::access; 

  template<class Archive>
  void serialize(Archive          & ar_ , 
		 const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp("dummy",__dummy);
    ar_ & boost::serialization::make_nvp("vals",__vals);
  }  

};

std::ostream & operator<<( std::ostream & out_ , const data1 & d_ )
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

class data2
{
private:
  data1 * __data1;
public:
  void link(data1 & d_)
  {
    __data1 = &d_;
  }
  data2() : __data1(0) 
  {
  }
  data2(data1 & d_) : __data1(0) 
  {
    link(d_);
  }

  friend std::ostream & operator<<( std::ostream & out_ , const data2 & d_ );

private:

  friend class boost::serialization::access; 

  template<class Archive>
  void serialize(Archive          & ar_ , 
		 const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp("data1",__data1);
  }  
  
};

std::ostream & operator<<( std::ostream & out_ , const data2 & d_ )
{
  out_ << "[data2.__data1=" << d_.__data1;
  out_ << "]@" << &d_;
  return out_;
}

/*****************/

void write_1(bool with_d2_)
{
  std::cerr << "write_1: entering..." << std::endl;
  bool with_d2=with_d2_;

  boost::filesystem::path       out_path("prg_bs2.out.txt"); 
  std::ofstream                 fout(out_path.string().c_str());

  boost::filesystem::path       xout_path("prg_bs2.out.xml"); 
  std::ofstream                 xfout(xout_path.string().c_str());
 
  const uint32_t ndata = 10;
  { 
    boost::archive::text_oarchive oar(fout);
    boost::archive::xml_oarchive  xoar(xfout);
    oar  << ndata;
    xoar << boost::serialization::make_nvp("ndata",ndata);
  }
  fout << std::endl;

  for (int i = 0; i < ndata; i++)
    {
      boost::archive::text_oarchive oar(fout);
      boost::archive::xml_oarchive  xoar(xfout);
      data1 d(i,(i%4)+1);
      std::cerr << "i=" << i << " data=" << d << std::endl;
      const data1 & d_ref = d;
      oar  << d_ref; 
      xoar << boost::serialization::make_nvp("data1",d_ref);
      if (with_d2)
	{
	  if (i == 0) 
	    {
	      data2 d2(d);
	      const data2 & d2_ref = d2;
	      oar  << d2_ref;
	      xoar << boost::serialization::make_nvp("data2",d2_ref);
	      std::cerr << "i=" << i << " data2=" << d2 << std::endl;
	    }
	}
      fout << std::endl;
    }
  fout.close();
  xfout.close();
  std::cerr << "write_1: exiting." << std::endl;
}

void read_1(bool with_d2_)
{
  std::cerr << "read_1: entering..." << std::endl;
  bool with_d2=with_d2_;
  boost::filesystem::path       in_path("prg_bs2.out.txt"); 
  std::ifstream                 fin(in_path.string().c_str());

  uint32_t ndata;
  { 
      boost::archive::text_iarchive iar(fin);
      iar >> ndata;
      std::cerr << "read_1: ndata=" << ndata << std::endl;
  }

  for (int i = 0; i < ndata; i++)
    {
      boost::archive::text_iarchive iar(fin);
      data1 d;
      iar >> d;
      if (d.val() != i)
	{
	  std::cerr << "read_1: ERROR: Incorrect read..." << std::endl;
	}
      std::cout << "i=" << i << " object="<< d << std::endl;
      if (with_d2)
	{
	  if (i == 0) 
	    {
	      data2 d2;
	      iar >> d2;
	      std::cout << "i=" << i << " object2="<< d2 << std::endl;
	    }
	}
    }
  std::cerr << "read_1: exiting." << std::endl;
}

void read_2(bool with_d2_)
{
  std::cerr << "read_2: entering..." << std::endl;
  bool with_d2=with_d2_;
  boost::filesystem::path       in_path("prg_bs2.out.xml"); 
  std::ifstream                 fin(in_path.string().c_str());

  uint32_t ndata;
  { 
      boost::archive::xml_iarchive iar(fin);
      iar >> boost::serialization::make_nvp("ndata",ndata);
      std::cerr << "read_2: ndata=" << ndata << std::endl;
  }
  if (!fin) 
    {
	  std::cerr << "read_2: ERROR: I/O failure!" << std::endl;
	  
    }
  for (int i = 0; i < ndata; i++)
    {
      boost::archive::xml_iarchive iar(fin);
      data1 d;
      iar >> boost::serialization::make_nvp("data1",d);
      if (d.val() != i)
	{
	  std::cerr << "read_2: ERROR: Incorrect read..." << std::endl;
	}
      std::cout << "i=" << i << " object="<< d << std::endl;
      if (with_d2)
	{
	  if (i == 0) 
	    {
	      data2 d2;
	      iar >> boost::serialization::make_nvp("data2",d2);
	      std::cout << "i=" << i << " object2="<< d2 << std::endl;
	    }
	}
    }
  std::cerr << "read_2: exiting." << std::endl;
}

int main(void)
{
  bool with_d2 = true;
  write_1(with_d2);
  read_1(with_d2);
  read_2(with_d2);
  return 0;
}

/* end */
