// -*- mode: c++; -*- 
/* properties.cc
 */

#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    bool properties::g_debug = false;

    properties::default_key_validator properties::g_default_key_validator;

    std::string properties::data::get_error_message( int error_code_ )
    {
      if ( error_code_ == ERROR_SUCCESS ) return "success";
      if ( error_code_ == ERROR_FAILURE ) return "generic error";
      if ( error_code_ == ERROR_BADTYPE ) return "invalid type";
      if ( error_code_ == ERROR_RANGE   ) return "invalid index";
      if ( error_code_ == ERROR_LOCK    ) return "lock error";
      return "generic error";
    }

    void properties::data::__clear_values()
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::__clear_values: entering..." << std::endl;
      }
      __boolean_values.clear();
      __integer_values.clear();
      __real_values.clear();
      __string_values.clear();
    }

    int properties::data::__init_values( char type_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::__init_values: entering..." << std::endl;
      }
      size_t size = size_;
      if ( size < 2 ) {
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: SCALAR..." << std::endl;
	}
	size=1;
	__desc &= ~MASK_VECTOR;
      }
      else {
	__desc |= MASK_VECTOR;
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: VECTOR... size=" << size << std::endl;
	}
      }
      __desc &= ~MASK_TYPE;
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::__init_values: desc=" << std::hex << (int) __desc 
		  << std::dec << std::endl;
      }
    
      if ( type_ == TYPE_BOOLEAN_SYMBOL ) {
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: BOOLEAN..." << std::endl;
	}
	__desc|=TYPE_BOOLEAN;
	__boolean_values.assign(size,false);
      }
      if ( type_ == TYPE_INTEGER_SYMBOL ) {
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: INTEGER..." 
		    << " TYPE_INTEGER=" << (int) TYPE_INTEGER << std::endl;
	}
	__desc|=TYPE_INTEGER;
	__integer_values.assign(size,0);
      }
      if ( type_ == TYPE_REAL_SYMBOL ) {
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: REAL..." << std::endl;
	}
	__desc|=TYPE_REAL;
	__real_values.assign(size,0.0);
      }
      if ( type_ == TYPE_STRING_SYMBOL ) {
	if ( properties::g_debug ) {
	  std::cerr << "DEBUG: properties::data::__init_values: STRING..." << std::endl;
	}
	__desc|=TYPE_STRING;
	__string_values.assign(size,"");
      }
      return ERROR_SUCCESS;
    }

    int properties::data::boolean( size_t size_ )
    {
      __clear_values();
      return __init_values(TYPE_BOOLEAN_SYMBOL,size_);
    }

    int properties::data::integer( size_t size_ )
    {
      __clear_values();
      return __init_values(TYPE_INTEGER_SYMBOL,size_);
    }

    int properties::data::real( size_t size_ )
    {
      __clear_values();
      return __init_values(TYPE_REAL_SYMBOL,size_);
    }

    int properties::data::string( size_t size_ )
    {
      __clear_values();
      return __init_values(TYPE_STRING_SYMBOL,size_);
    }

    /******/

    /*
      int properties::data::scalar()
      {
      __desc^=MASK_VECTOR;
      return ERROR_SUCCESS;
      }

      int properties::data::vector()
      {
      __desc|=MASK_VECTOR;
      return ERROR_SUCCESS;
      }
    */

    /******/

    bool properties::data::has_type() const
    {
      return (__desc & MASK_TYPE) != TYPE_NONE;
    }

    bool properties::data::is_boolean() const
    {
      return (__desc & MASK_TYPE) == TYPE_BOOLEAN;
    }

    bool properties::data::is_integer() const
    {
      return (__desc & MASK_TYPE) == TYPE_INTEGER;
    }

    bool properties::data::is_real() const
    {
      return (__desc & MASK_TYPE) == TYPE_REAL;
    }

    bool properties::data::is_string() const
    {
      return (__desc & MASK_TYPE) == TYPE_STRING;
    }

    /******/

    bool properties::data::is_scalar() const
    {
      return (__desc & MASK_VECTOR) == 0;
    }

    bool properties::data::is_vector() const
    {
      return !is_scalar();
    }

    /******/

    bool properties::data::is_unlocked() const
    {
      return (__desc & MASK_LOCK) == 0;
    }

    bool properties::data::is_locked() const
    {
      return !is_unlocked();
    }

    /******/

    /*
      bool properties::data::is_visible() const
      {
      return (__desc & MASK_HIDDEN) == 0;
      }

      bool properties::data::is_hidden() const
      {
      return !is_visible();
      }
    */

    /******/

    int properties::data::lock()
    {
      __desc |= MASK_LOCK;
      return ERROR_SUCCESS;
    }

    int properties::data::unlock()
    {
      __desc &= ~MASK_LOCK;
      return ERROR_SUCCESS;
    }

    /******/

    size_t properties::data::get_size() const
    {
      if ( ! has_type() ) return 0;
      if ( is_scalar() ) {
	return 1;
      }
      else {
	if ( is_boolean() ) return __boolean_values.size();
	if ( is_integer() ) return __integer_values.size();
	if ( is_real()    ) return __real_values.size();     
	if ( is_string() ) return __string_values.size(); 
	return 0;
      }
    }

    size_t properties::data::size() const
    {
      return get_size();
    }

    /******/

    properties::data::data( char type_ , size_t size_ )
    {
      __desc  = 0;
      __init_values(type_,size_);
    }

    properties::data::data( bool value_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::ctor(...bool...): entering..." << std::endl;
      }   
      __desc  = 0;
      size_t size = (size_<2)?1:size_;
      __init_values(TYPE_BOOLEAN_SYMBOL,size);
      for ( int i=0; i<size; i++ ) {
	set_value(value_,i);
      }
    }

    properties::data::data( int value_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::ctor(...int...): entering..." << std::endl;
      }   
      __desc  = 0;
      size_t size = (size_<2)?1:size_;
      __init_values(TYPE_INTEGER_SYMBOL,size);
      for ( int i=0; i<size; i++ ) {
	set_value(value_,i);
      }
    }

    properties::data::data( double value_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::ctor(...double...): entering..." << std::endl;
      }   
      __desc  = 0;
      size_t size = (size_<2)?1:size_;
      __init_values(TYPE_REAL_SYMBOL,size);
      for ( int i=0; i<size; i++ ) {
	set_value(value_,i);
      }
    }

    properties::data::data( const std::string & value_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::ctor(...std::string...): entering..." << std::endl;
      }   
      __desc  = 0;
      size_t size = (size_<2)?1:size_;
      __init_values(TYPE_STRING_SYMBOL,size);
      for ( int i=0; i<size; i++ ) {
	set_value(value_,i);
      }
    }

    properties::data::data( const char * value_ , size_t size_ )
    {
      if ( properties::g_debug ) {
	std::cerr << "DEBUG: properties::data::ctor(...char *...): entering..." << std::endl;
      }   
      __desc  = 0;
      size_t size = (size_<2)?1:size_;
      __init_values(TYPE_STRING_SYMBOL,size);
      for ( int i=0; i<size; i++ ) {
	set_value(std::string(value_),i);
      }
    }

    properties::data::~data()
    {
    }

    /******************************************************/

    bool properties::data::index_is_valid( int index_ ) const
    {
      return (index_ >= 0) && (index_ < get_size());
    }

    /******************************************************/
 
    int properties::data::set_value( bool value_ , int index_ ) 
    {
      if ( !is_boolean() ) {
	return ERROR_BADTYPE;
      }
      if ( is_locked() ) {
	return ERROR_LOCK;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      __boolean_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value( int  value_ , int index_ ) 
    {
      if ( !is_integer() ) {
	return ERROR_BADTYPE;
      }
      if ( is_locked() ) {
	return ERROR_LOCK;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      __integer_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value( double value_ , int index_ ) 
    {
      if ( !is_real() ) {
	return ERROR_BADTYPE;
      }
      if ( is_locked() ) {
	return ERROR_LOCK;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      __real_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value( const std::string & value_ , int index_ ) 
    {
      if ( !is_string() ) {
	return ERROR_BADTYPE;
      }
      if ( is_locked() ) {
	return ERROR_LOCK;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      __string_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value( const char * value_ , int index_ ) 
    {
      return set_value(std::string(value_),index_);
    }

    /******************************************************/

    int properties::data::get_value( bool & value_ , int index_ ) const
    {
      if ( !is_boolean() ) {
	return ERROR_BADTYPE;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      value_ = __boolean_values[index_];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value( int & value_ , int index_ ) const
    {
      if ( !is_integer() ) {
	return ERROR_BADTYPE;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      value_ = __integer_values[index_];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value( double & value_ , int index_ ) const
    {
      if ( !is_real() ) {
	return ERROR_BADTYPE;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      value_ = __real_values[index_];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value( std::string & value_ , int index_ ) const
    {
      if ( !is_string() ) {
	return ERROR_BADTYPE;
      }
      if ( !index_is_valid(index_) ) {
	return ERROR_RANGE;
      } 
      value_ = __string_values[index_];
      return ERROR_SUCCESS;
    }

    /******************************************************/

    bool properties::data::get_boolean_value( int index_ ) const
    {
      bool value;
      if ( get_value(value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error("properties::data::get_boolean_value: type is not boolean!");    }
      return value;
    }

    int properties::data::get_integer_value( int index_ ) const
    {
      int value;
      if ( get_value(value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error("properties::data::get_integer_value: type is not integer!");    }
      return value;
    }
  
    double properties::data::get_real_value( int index_ ) const
    {
      double value;
      if ( get_value(value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error("properties::data::get_real_value: type is not double!");
      }
      return value;
    }

    std::string properties::data::get_string_value( int index_ ) const
    {
      std::string value;
      if ( get_value(value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error("properties::data::get_string_value: type is not double!");
      }
      return value;
    }

    std::string properties::data::get_type_label() const
    {
      if ( is_boolean() ) return "boolean";
      if ( is_integer() ) return "integer";
      if ( is_real()    ) return "real";     
      if ( is_string()    ) return "string";
      return "none";
    }

    std::string properties::data::get_vector_label() const
    {
      if ( is_scalar() ) return "scalar";
      return "vector";
    }
    
    void properties::data::dump( std::ostream & out_ ) const
    {
      tree_dump(std::cout,"datatools::utils::properties::data");
    }
    
    void properties::data::tree_dump( std::ostream & out_         , 
				      const std::string & title_  ,
				      const std::string & indent_ ,
				      bool inherit_                ) const
    {
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      if ( !title_.empty() ) {
	out_ << indent << title_ << std::endl;
      }
      out_ << indent << i_tree_dumpable::tag 
	   << "Type  : " <<  get_type_label();
      if ( is_vector() ) {
	out_ << '[' << size() << ']' << " (vector)";
      }
      else {
	out_ << " (scalar)";
      }
      out_ << std::endl;

      /*
      out_ << indent << i_tree_dumpable::tag;
      if ( is_vector() ) out_ << "Vector (size=" << size() << ")" << std::endl;
      else out_ << "Scalar" << std::endl;
      */

      out_ << indent << i_tree_dumpable::tag 
	   << "Lock  : " <<  (is_locked()?"locked":"unlocked") << std::endl;

      for ( int i=0; i< get_size() ; i++ ) 
	{
	  out_ << indent;
	  if ( i==get_size()-1 ) out_ << i_tree_dumpable::inherit_tag(inherit_);
	  else out_ << i_tree_dumpable::tag;
	  out_ << "Value";
	  if ( is_vector() ) {
	    out_ << "[" << i << "]";
	  }
	  out_ << " : ";
	  if ( is_boolean() ) out_ << std::dec << get_boolean_value(i) << std::endl;
	  if ( is_integer() ) out_ << std::dec << get_integer_value(i) << std::endl;
	  out_.precision(16);
	  if ( is_real() )    out_ << get_real_value(i) << std::endl;
	  if ( is_string() )  out_ << '"' <<  get_string_value(i) << '"' << std::endl;
	}
    }
    
    /**********************************************/
    const std::string properties::default_key_validator::ALLOWED_CHARS = 
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";

    properties::default_key_validator::default_key_validator()
    { 
       
    }

    properties::default_key_validator:: ~default_key_validator()
    {
    }

    bool properties::default_key_validator::operator()( const std::string & key_arg_ ) const
    {
      if ( key_arg_.empty() ) return false;
      if ( key_arg_.find_first_not_of(ALLOWED_CHARS) != key_arg_.npos ) return false;
      if ( key_arg_.find_first_of("0123456789.") == 0 ) return false;
      if ( key_arg_[key_arg_.size()-1] == '.' ) return false;
      return true;
    }
    
    void properties::__validate_key( const std::string & key_arg_ ) const
    {
      if ( __key_validator != 0 ) {
	//if ( ! (*__key_validator)(key_arg_) ) {
	if ( ! __key_validator->operator()(key_arg_) ) {
	  std::ostringstream message;
	  message << "properties::__validate_key: After key validator, the '" 
		  << key_arg_ << "' key is not valid!";
	  throw std::runtime_error(message.str());
	}
      }
    }

    bool properties::is_debug() const
    {
      return __debug;
    }
  
    void properties::set_debug( bool new_value_ )
    {
      __debug=new_value_;
    }

    size_t properties::size() const
    {
      return __props.size();
    }

    void properties::set_key_validator( const basic_key_validator & key_validator_ )
    {
      __clear_key_validator();
      __key_validator=&key_validator_;
      __key_validator_deletion = false;
    }
    
    void properties::set_key_validator( const basic_key_validator * key_validator_ , 
					bool  deletion_on_destroy_ )
    {
      if ( __key_validator != 0 && __key_validator== key_validator_ ) {
	return;
      }
      __clear_key_validator();
      if ( key_validator_ != 0 ) {
	__key_validator=key_validator_;
	__key_validator_deletion = deletion_on_destroy_;
      }
    }

    void properties::set_default_key_validator()
    {
      __clear_key_validator();
      __key_validator=&g_default_key_validator;// new default_key_validator;
      __key_validator_deletion = false; //true;
    }

    void properties::unset_key_validator()
    {
      __clear_key_validator();
    }
  
    // ctor/dtor:
    properties::properties() 
      : __debug(false) , __key_validator(0)
    {
      set_default_key_validator();
    }

    properties::properties( const basic_key_validator & key_validator_  ) 
      : __debug(false) , __key_validator(0)
    {
      set_key_validator(key_validator_);
    }
  
    properties::properties( const basic_key_validator * key_validator_ , 
			    bool deletion_on_destroy_ ) 
      : __debug(false) , __key_validator(0)
    {
      set_key_validator(key_validator_,deletion_on_destroy_);
    }

    void properties::__clear_key_validator()
    {
      if ( __key_validator != 0 ) {
	if ( __key_validator_deletion  ) {
	  delete __key_validator;
	}
	__key_validator=0;
      }
    }
  
    properties::~properties()
    {
      properties::clear();
    }

    void properties::erase( const std::string & key_ )
    {
      __props.erase(__props.find(key_));
    }

    void properties::clear()
    {
      __props.clear();
      __clear_key_validator();
      __debug=false;
    }

    bool properties::has_key( const std::string & key_ ) const
    {
      /*
	const data * data_ptr;
	return __find_key(key_,data_ptr);
      */
      return __props.find(key_) != __props.end();
    }
  
    void properties::__check_nokey( const std::string & key_ ) const
    {
      if ( has_key(key_) ) {
	std::ostringstream message;
	message << "properties::__check_nokey: key '" << key_ << "' already used!";
	throw std::runtime_error(message.str());
      }
    }
  
    void properties::__check_key( const std::string & key_ , 
				  data ** data_ )
    {
      pmap::iterator iter = __props.find(key_);
      if ( __props.find(key_) == __props.end() ) {
	std::ostringstream message;
	message << "properties::__check_key: key '" << key_ << "' does not exist!";
	throw std::runtime_error(message.str());
      }
      *data_=&(iter->second);
    }
  
    void properties::__check_key( const std::string & key_ , 
				  const data ** data_ ) const
    {
      pmap::const_iterator iter = __props.find(key_);
      if ( __props.find(key_) == __props.end() ) {
	std::ostringstream message;
	message << "properties::__check_key: key '" << key_ << "' does not exist!";
	throw std::runtime_error(message.str());
      }
      *data_=&(iter->second);
    }
  
    void properties::store( const std::string & key_ , bool   value_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      data a_data(value_,1);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock();
    }

    void properties::store( const std::string & key_ , int    value_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      data a_data(value_,1);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock();
    }

    void properties::store( const std::string & key_ , double value_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      data a_data(value_,1);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock();
    }

    void properties::store( const std::string & key_ , const std::string & value_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      data a_data(value_,1);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock();
    }

    void properties::store( const std::string & key_ , const char * value_ , bool lock_  )
    {
      properties::store(key_,std::string(value_),lock_);
    }

    void properties::store( const std::string & key_ , 
			    const std::vector<bool> & values_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      size_t size = values_.size();
      if ( size < 1 ) {
	throw std::runtime_error("properties::store: Invalid vector of booleans size!");
      }
      data a_data(data::TYPE_BOOLEAN_SYMBOL,size);
      __props[key_] = a_data;
      for ( int i=0; i<size; i++ ) {
	__props[key_].set_value(values_[i],i);
      }
      if ( lock_ ) __props[key_].lock();
    }
  
    void properties::store( const std::string & key_ , 
			    const std::vector<int> & values_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      size_t size = values_.size();
      if ( size < 1 ) {
	throw std::runtime_error("properties::store: Invalid vector of integers size!");
      }
      data a_data(data::TYPE_INTEGER_SYMBOL,size);
      __props[key_] = a_data;
      for ( int i=0; i<size; i++ ) {
	__props[key_].set_value(values_[i],i);
      }
      if ( lock_ ) __props[key_].lock();
    }
  
    void properties::store( const std::string & key_ , 
			    const std::vector<double> & values_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      size_t size = values_.size();
      if ( size < 1 ) {
	throw std::runtime_error("properties::store: Invalid vector of reals size!");
      }
      data a_data(data::TYPE_REAL_SYMBOL,size);
      __props[key_] = a_data;
      for ( int i=0; i<size; i++ ) {
	__props[key_].set_value(values_[i],i);
      }
      if ( lock_ ) __props[key_].lock();
    }
  
    void properties::store( const std::string & key_ , 
			    const std::vector<std::string> & values_ , bool lock_  )
    {
      __check_nokey(key_);
      __validate_key(key_);
      size_t size = values_.size();
      if ( size < 1 ) {
	throw std::runtime_error("properties::store: Invalid vector of string size!");
      }
      data a_data(data::TYPE_STRING_SYMBOL,size);
      __props[key_] = a_data;
      for ( int i=0; i<size; i++ ) {
	__props[key_].set_value(values_[i],i);
      }
      if ( lock_ ) __props[key_].lock();
    }
  
    /**********************************/

    void properties::change( const std::string & key_ , 
			     bool  value_ , int index_  )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->set_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::change: cannot change value for property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }
    }

    void properties::change( const std::string & key_ , 
			     int   value_ , int index_   )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->set_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::change: cannot change value for property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }
    }

    void properties::change( const std::string & key_ , 
			     double value_ , int index_   )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->set_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::change: cannot change value for property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }
    }

    void properties::change( const std::string & key_ , 
			     const std::string & value_ , int index_  )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->set_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::change: cannot change value for property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }
    }
  
    void properties::change( const std::string & key_ , 
			     const std::vector<bool> & values_ )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_boolean() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::change: property '" 
		<< key_ << "' is not a vector of booleans!";
	throw std::runtime_error(message.str());
      }
      if ( values_.size() != data_ptr->get_size() ) {
	int error = data_ptr->boolean(values_.size());
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
      for ( int i=0; i<values_.size(); i++ ) {
	int error = data_ptr->set_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
    }
  
    void properties::change( const std::string & key_ , 
			     const std::vector<int> & values_ )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_integer() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::change: property '" 
		<< key_ << "' is not a vector of integers!";
	throw std::runtime_error(message.str());
      }
      if ( values_.size() != data_ptr->get_size() ) {
	int error = data_ptr->integer(values_.size());
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
      for ( int i=0; i<values_.size(); i++ ) {
	int error = data_ptr->set_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
    }
  
    void properties::change( const std::string & key_ , 
			     const std::vector<double> & values_ )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_real() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::change: property '" 
		<< key_ << "' is not a vector of reals!";
	throw std::runtime_error(message.str());
      }
      if ( values_.size() != data_ptr->get_size() ) {
	int error = data_ptr->real(values_.size());
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
      for ( int i=0; i<values_.size(); i++ ) {
	int error = data_ptr->set_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
    }
  
    void properties::change( const std::string & key_ , 
			     const std::vector<std::string> & values_ )
    {
      data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_string() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::change: property '" 
		<< key_ << "' is not a vector of strings!";
	throw std::runtime_error(message.str());
      }
      if ( values_.size() != data_ptr->get_size() ) {
	int error = data_ptr->string(values_.size());
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
      for ( int i=0; i<values_.size(); i++ ) {
	int error = data_ptr->set_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::change: cannot change values for vector property '" << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
      }
    }

    /********************/
  
    void properties::fetch( const std::string & key_ , 
			    bool  & value_ , int index_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->get_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::fetch: cannot fetch boolean value from property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }    
    }

    void properties::fetch( const std::string & key_ , 
			    int    & value_ , int index_  ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->get_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::fetch: cannot fetch integer value from property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }    
    }

    void properties::fetch( const std::string & key_ , 
			    double & value_ , int index_  ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->get_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::fetch: cannot fetch real value from property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }    
    }

    void properties::fetch( const std::string & key_ , 
			    std::string & value_ , int index_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      int error = data_ptr->get_value(value_,index_);
      if ( error != data::ERROR_SUCCESS ) {
	std::ostringstream message;
	message << "properties::fetch: cannot fetch string value from property '" << key_ << "': "
		<< data::get_error_message(error) << "!";
	throw std::runtime_error(message.str());
      }    
    }
  
    bool properties::fetch_boolean( const std::string & key_ , int index_) const
    {
      bool value;
      fetch(key_,value,index_);
      return value;
    }

    int properties::fetch_integer( const std::string & key_ , int index_) const
    {
      int value;
      fetch(key_,value,index_);
      return value;
    }

    double properties::fetch_real( const std::string & key_ , int index_) const
    {
      double value;
      fetch(key_,value,index_);
      return value;
    }

    std::string properties::fetch_string( const std::string & key_ , int index_) const
    {
      std::string value;
      fetch(key_,value,index_);
      return value;
    }

    void properties::fetch( const std::string & key_ , 
			    std::vector<bool> & values_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_boolean() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::fetch: property '" 
		<< key_ << "' is not a vector of booleans!";
	throw std::runtime_error(message.str());
      }
      values_.resize(data_ptr->size());
      values_.assign(data_ptr->size(),false);
      for ( int i=0; i<values_.size(); i++ ) {
	bool val;
	int error = data_ptr->get_value(val,i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch boolean values for vector property '" 
		  << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
	values_[i]=val;
      }
    }
  
    void properties::fetch( const std::string & key_ , 
			    std::vector<int> & values_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_integer() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::fetch: property '" 
		<< key_ << "' is not a vector of integers!";
	throw std::runtime_error(message.str());
      }
      values_.resize(data_ptr->size());
      values_.assign(data_ptr->size(),0);
      for ( int i=0; i<values_.size(); i++ ) {
	//int val;
	//int error = data_ptr->get_value(val,i);
	int error = data_ptr->get_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch integer values for vector property '" 
		  << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
	//values_[i]=val;
      }
    }
  
    void properties::fetch( const std::string & key_ ,
			    std::vector<double> & values_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_real() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::fetch: property '" 
		<< key_ << "' is not a vector of reals!";
	throw std::runtime_error(message.str());
      }
      values_.resize(data_ptr->size());
      values_.assign(data_ptr->size(),0.0);
      for ( int i=0; i<values_.size(); i++ ) {
	//double val;
	//int error = data_ptr->get_value(val,i);
	int error = data_ptr->get_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch real values for vector property '" 
		  << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
	//values_[i]=val;
      }
    }
  
    void properties::fetch( const std::string & key_ , 
			    std::vector<std::string> & values_ ) const
    {
      const data * data_ptr=0;
      __check_key(key_,&data_ptr);
      if ( !data_ptr->is_string() || !data_ptr->is_vector() ) {
	std::ostringstream message;
	message << "properties::fetch: property '" 
		<< key_ << "' is not a vector of strings!";
	throw std::runtime_error(message.str());
      }
      values_.resize(data_ptr->size());
      values_.assign(data_ptr->size(),"");
      for ( int i=0; i<values_.size(); i++ ) {
	//std::string val;
	//int error = data_ptr->get_value(val,i);
	int error = data_ptr->get_value(values_[i],i);
	if ( error != data::ERROR_SUCCESS ) {
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch string values for vector property '" 
		  << key_ << "': "
		  << data::get_error_message(error) << "!";
	  throw std::runtime_error(message.str());
	}    
	//values_[i]=val;
      }
    }


    /****************************/

    void properties::tree_dump( std::ostream & out_         , 
				const std::string & title_  ,
				const std::string & indent_ ,
				bool inherit_                ) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      if ( !title_.empty() ) {
	out_ << indent << title_ << std::endl;
      }
      out_ << indent << du::i_tree_dumpable::tag 
	   << "Debug : " <<  __debug << std::endl;
      if ( __props.size() == 0 ) {
	out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
	     << "<no property>" << std::endl;
      }
      else {
	for ( pmap::const_iterator i = __props.begin() ;
	      i != __props.end() ;
	      i++ ) {
	  const std::string & key = i->first;
	  const du::properties::data & a_data = i->second;
	  out_ << indent;
	  std::ostringstream indent_oss;
	  indent_oss << indent;
	  pmap::const_iterator j=i; j++;
	  if ( j == __props.end() ) {
	    out_ << du::i_tree_dumpable::inherit_tag(inherit_);
	    indent_oss << du::i_tree_dumpable::inherit_skip_tag(inherit_);
	  }
	  else {
	    out_ << du::i_tree_dumpable::tag;
	    indent_oss << du::i_tree_dumpable::skip_tag;
	  }
	  out_ << "Name : " << '"' << key << '"' << std::endl;
	  a_data.tree_dump(out_,"",indent_oss.str());
	}
      }
    }

    void properties::dump( std::ostream & out_ ) const
    {
      tree_dump(std::cout,"datatools::utils::properties");
    }
  
  } // end of namespace utils 

} // end of namespace datatools 

// end of properties.cc
