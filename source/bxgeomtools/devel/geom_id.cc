// -*- mode: c++; -*- 
/* geom_id.cc
 */

#include <GeomUtils/geom_id.h>

namespace geomutils {

  // static constants definitions:
  const char geom_id::TOKEN_CAT_INVALID        = '?';
  const char geom_id::TOKEN_ADD_INVALID        = '?';
  const char geom_id::TOKEN_ADD_SEP            = '.';
  const char geom_id::TOKEN_ID_OPEN            = '[';
  const char geom_id::TOKEN_ID_SEP             = ':';
  const char geom_id::TOKEN_ID_CLOSE           = ']';
  const int  geom_id::CAT_INVALID              = -1; 
  const int  geom_id::ADD_INVALID              = -1; 
  
  // static definitions:
  
  // getters/setters definitions:
  int geom_id::get_category() const
  {
    return _category;
  }
  
  void geom_id::set_category( int new_value_ )
  {
    _category=new_value_;
  }

  size_t geom_id::get_address_depth() const
  {
    return _address_depth;
  }

  const int * geom_id::get_address() const
  {
    return _address;
  }
  
  int geom_id::get_address_value( size_t pos_ ) const
  {
    return _address[pos_];
  }
  
  void geom_id::set_address_value( size_t pos_ , int val_ ) 
  {
    if ( pos_ >= _address_depth ) {
      std::ostringstream message;
      message << "geom_id::set_address_val: Invalid address position: pos='"
	      << pos_ << "' address depth='" << _address_depth << "'!";
      throw std::runtime_error(message.str());
    }
    if ( val_ < 0 ) _address[pos_]=ADD_INVALID;
    else _address[pos_]=val_;
  }

  void geom_id::__assert_address_value_makes_sense( size_t pos_ , int val_ ) const
  {
    if ( val_ != ADD_INVALID && pos_ >= _address_depth ) {
      std::ostringstream message;
      message << "geom_id::set_address_values: "
	      << "Attempt to set address value above max depth: pos='"
	      << pos_ << "' address depth='" << _address_depth << "'!";
      throw std::runtime_error(message.str());
    }
  }
    
  // How ugly it is!!! ;-)
  void geom_id::set_address_values( 
		     int val0_ , 
		     int val1_ , 
		     int val2_ , 
		     int val3_ , 
		     int val4_ ,
		     int val5_ , 
		     int val6_ , 
		     int val7_ , 
		     int val8_ ,
		     int val9_ )
  {
    _address[0]=val0_;
    if ( _address_depth>1 ) {
      _address[1]=val1_;
      __assert_address_value_makes_sense(2,val2_);
    if ( _address_depth>2 ) {
      _address[2]=val2_;
      __assert_address_value_makes_sense(3,val3_);
    if ( _address_depth>3 ) {
      _address[3]=val3_;
      __assert_address_value_makes_sense(4,val4_);
    if ( _address_depth>4 ) {
      _address[4]=val4_;
      __assert_address_value_makes_sense(5,val5_);
    if ( _address_depth>5 ) {
      _address[5]=val5_;
      __assert_address_value_makes_sense(6,val6_);
    if ( _address_depth>6 ) {
      _address[6]=val6_;
      __assert_address_value_makes_sense(7,val7_);
    if ( _address_depth>7 ) {
      _address[7]=val7_;
      __assert_address_value_makes_sense(8,val8_);
    if ( _address_depth>8 ) {
      _address[8]=val8_;
      __assert_address_value_makes_sense(9,val9_);
    if ( _address_depth>9 ) {
      _address[9]=val9_;
    }}}}}}}}}
  }
  
  // ctor/dtor:
  geom_id::geom_id()
  {
    _address_depth=0;
  }

  geom_id::geom_id( int cat_ , size_t add_sz_ )
  {
    set_category(cat_);
    init_address(add_sz_);
  }
  
  geom_id::~geom_id()
  {
    //reset();
  }

  void geom_id::init( int cat_ , size_t add_sz_ )
  {
    set_category(cat_);
    init_address(add_sz_);
  }

  void geom_id::init_address( size_t add_sz_ )
  {
    if ( add_sz_ > MAX_ADDRESS_DEPTH ) {
      std::ostringstream message;
      message << "geom_id::init_address: Invalid address depth '"
	      << add_sz_ << "' (>" << MAX_ADDRESS_DEPTH << ")!";
      throw std::runtime_error(message.str());
    }
    _address_depth=add_sz_;
    reset_address();
  }

  void geom_id::reset_address()
  {
    for ( int i=0 ; i<MAX_ADDRESS_DEPTH; i++ ) {
      _address[i]=ADD_INVALID;
    }    
  }

  void geom_id::reset()
  {
    _category=CAT_INVALID;
    init_address(0);
  }

  bool geom_id::is_valid() const
  {
    if ( _category==CAT_INVALID ) {
      return false;
    }
    else {
      if ( _address_depth == 0 ) return false;
      for ( int i=0; i<_address_depth; i++ ) {
	if ( _address[i] < 0 ) return false;
      }
      return true;
    }
  }

  bool geom_id::is_initialized() const
  {
    return _category!=CAT_INVALID && _address_depth>0;
  }

  bool geom_id::is_category( int cat_ ) const
  {
    return _category == cat_;
  }
  
  bool geom_id::operator==( const geom_id & id_ ) const
  {
    if ( !is_category(id_._category) ) return false;
    if ( _address_depth != id_._address_depth ) return false;
    for ( int i=0; i<_address_depth; i++ ) {
      if ( _address[i] != id_._address[i] )return false;
    } 
    return true;
  }

  void geom_id::encode_address( std::string & str_  , const int * add_ , size_t sz_ )
  {
    std::ostringstream oss;
    if ( sz_==0 ) oss << geom_id::TOKEN_ADD_INVALID;
    else {
      for ( int i=0; i<sz_ ; i++ ) {
	oss << std::dec << add_[i];
	if ( i<sz_-1 ) oss << geom_id::TOKEN_ADD_SEP; 
      }
    }     
    str_=oss.str();
  }

  bool geom_id::decode_address( const std::string & str_ , int * add_ , size_t sz_ )
  {
    std::list<std::string> tokens;
    std::string seps;
    seps.push_back(geom_id::TOKEN_ADD_SEP);
    snemo::string_tools::split_string(str_,seps,tokens);
    if ( sz_>0 && tokens.size() != sz_ ) {
      return false;
    }
    int count=0;
    for ( std::list<std::string>::const_iterator i=tokens.begin();
	  i!=tokens.end();
	  i++ ) {
      if ( i->empty() ) return false;
      std::istringstream iss(*i);
      int val;
      iss >> val;
      if ( !iss ) {
	return false;
      }
      add_[count]=val;
      count++;
    }
    return true;
  }

  std::ostream & operator<<( std::ostream & out_ , const geom_id & id_ )
  {
    out_ << geom_id::TOKEN_ID_OPEN 
	 << id_._category << geom_id::TOKEN_ID_SEP;
    std::string add_str;
    geom_id::encode_address(add_str,id_._address,id_._address_depth);
    out_ << add_str << geom_id::TOKEN_ID_CLOSE;
    return out_;
  }

  void geom_id::extract( const geom_id & from_id_ , 
			 geom_id & to_id_ )
  {
    if ( from_id_.get_address_depth() < to_id_.get_address_depth() ) {
      throw std::runtime_error("geom_id::extract_id: Incompatible depth!");
    }
    for ( size_t i=0; i<to_id_.get_address_depth(); i++ ) {
      to_id_.set_address_value(i,from_id_.get_address_value(i));
    }
  }
  
} // end of namespace geomutils

// end of geom_id.cc
