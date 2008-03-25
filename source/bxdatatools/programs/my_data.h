/* my_data.h */

#ifndef __datatools__test__my_data_h
#define __datatools__test__my_data_h 1

#include <vector>

#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/serialization.h>

#include <datatools/utils/i_tree_dump.h>

class data_t : public datatools::utils::i_tree_dumpable,
	       public datatools::serialization::i_serializable     
{
 public:
  static const std::string SERIAL_TAG;
  virtual const std::string & get_serial_tag();
 private:
  int8_t   __flags;
  int32_t  __ival;
  double   __dval;
  std::vector<double> __dvals;

public:
  void init()
  {
    __flags='a';
    __ival=666;
    __dval=3.14;
  }

  data_t()
  {
    init();
  }

  data_t( char f_ , int i_ , double d_ , size_t n_ ) 
  {
    init();
    __flags=f_;
    __ival=i_;
    __dval=d_;
    double v=10.;
    for ( int i=0 ; i<n_ ; i++ ) {
      __dvals.push_back(v);
      v *=3.;
    }
  }

  const int8_t  & flags() const { return __flags; }
  const int32_t & ival()  const { return __ival; }
  const double  & dval()  const { return __dval; }
  const std::vector<double> & dvals() const { return __dvals; }
  
 private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize( Archive            & ar_ , 
		  const unsigned int   version_ )
  {
    ar_ & boost::serialization::make_nvp("flags",__flags);
    ar_ & boost::serialization::make_nvp("ival",__ival);
    ar_ & boost::serialization::make_nvp("dval",__dval);
    ar_ & boost::serialization::make_nvp("dvals",__dvals);
  }

 public:

  virtual void tree_dump( std::ostream & out_ , 
		     const std::string & title_ ,
		     const std::string & indent_ ,
		     bool inherit_ = false ) const
    {
      using namespace datatools::utils;
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      if ( !title_.empty() ) {
	out_ << indent << title_ << std::endl;
      }
      out_ << indent << i_tree_dumpable::tag << "Address : [" << this << ']' << std::endl;
      out_ << indent << i_tree_dumpable::tag << "flags : " << __flags << std::endl;
      out_ << indent << i_tree_dumpable::tag << "ival  : "  << __ival << std::endl;
      out_ << indent << i_tree_dumpable::tag << "dval  : "  
	   << __dval << std::endl;
      out_ << indent << i_tree_dumpable::inherit_tag(inherit_) 
	   << "dvals[" << __dvals.size() << "] : ";
      for ( int i=0; i<__dvals.size(); i++ ) 
	out_ << __dvals[i] << ' ';
      out_ << std::endl;
    }

};

const std::string data_t::SERIAL_TAG = "__DATA__";
const std::string & data_t::get_serial_tag()
{
  return data_t::SERIAL_TAG;
}

/*********************************************/

class more_data_t : public data_t
{
public:
  static const std::string SERIAL_TAG;
  virtual const std::string & get_serial_tag();
private:
  std::string __name;

public:

  more_data_t() : data_t('z',-1,3.14159,3) 
  {
    __name="<anonymous>";
  }

  more_data_t( char c_ , int i_ , double d_ , size_t n_ , 
	       const std::string & name_ ) 
    : data_t(c_,i_,d_,n_)
  {
    __name=name_;
  }

  const std::string & name() const { return __name; }

private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize( Archive &          ar_ , 
		  const unsigned int version_ )
  {
    ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(data_t);
      //ar_ & boost::serialization::base_object<data_t>(*this);
    ar_ & boost::serialization::make_nvp("name",__name);
  }

 public:

  virtual void tree_dump( std::ostream & out_ , 
			  const std::string & title_ ,
			  const std::string & indent_ ,
			  bool inherit_ = false ) const
    {
      using namespace datatools::utils;
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      data_t::tree_dump(out_,title_,indent_,true);
      out_ << indent << i_tree_dumpable::inherit_tag(inherit_) 
	   << "name = "  << __name << std::endl;
    }

};

const std::string more_data_t::SERIAL_TAG = "__MORE_DATA__";
const std::string & more_data_t::get_serial_tag()
{
  return more_data_t::SERIAL_TAG;
}

#endif // __datatools__test__my_data_h

/* end of  my_data.h */
