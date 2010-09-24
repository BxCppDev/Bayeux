// -*- mode: c++; -*- 
/* properties.cc 
 */

#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    bool properties::g_debug                       = false;
    const std::string properties::SERIAL_TAG       = "__PROPERTIES__";
    const std::string properties::data::SERIAL_TAG = "__DATA_PROPERTIES__";

    /* 2010-06-01 FM: to be updated 
     */
    /*
    const std::string properties::SERIAL_TAG       = "datatools::utils::properties";
    const std::string properties::data::SERIAL_TAG = "__datatools::utils::properties::data";
    */

    const std::string & 
    properties::get_serial_tag () const
    {
      return properties::SERIAL_TAG;
    }

    const std::string & 
    properties::data::get_serial_tag () const
    {
      return properties::data::SERIAL_TAG;
    }

    properties::default_key_validator properties::g_default_key_validator;

    const bool        properties::data::DEFAULT_VALUE_BOOLEAN = false;
    const int         properties::data::DEFAULT_VALUE_INTEGER = 0;
    const double      properties::data::DEFAULT_VALUE_REAL    = 0.0;
    const std::string properties::data::DEFAULT_VALUE_STRING  = "";

    bool 
    properties::data::has_forbidden_char (const std::string & str_)
    {
      return str_.find_first_of (STRING_FORBIDDEN_CHAR) != str_.npos;
    }

    std::string 
    properties::data::get_error_message (int error_code_)
    {
      if (error_code_ == ERROR_SUCCESS) return "success";
      if (error_code_ == ERROR_FAILURE) return "generic error";
      if (error_code_ == ERROR_BADTYPE) return "invalid type";
      if (error_code_ == ERROR_RANGE  ) return "invalid index";
      if (error_code_ == ERROR_LOCK   ) return "lock error";
      return "generic error";
    }

    void 
    properties::data::__clear_values ()
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::__clear_values: entering..." 
		    << std::endl;
	}
      __boolean_values.clear ();
      __integer_values.clear ();
      __real_values.clear ();
      __string_values.clear ();
    }

    int 
    properties::data::__init_values (char type_ ,int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::__init_values: entering..." 
		    << std::endl;
	}
      int memsize = size_;
      if (size_ < 0) 
	{
	  memsize = 1;
	  __flags &= ~MASK_VECTOR; // force scalar
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: SCALAR... size=" 
			<< memsize << std::endl;
	    }
	}
      else 
	{
	  __flags |= MASK_VECTOR; // force vector
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: VECTOR... size=" 
			<< memsize << std::endl;
	    }
	}

      __flags &= ~MASK_TYPE;
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::__init_values: desc=" 
		    << std::hex << (int) __flags 
		    << std::dec << std::endl;
	}
    
      if (type_ == TYPE_BOOLEAN_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: BOOLEAN..." 
			<< std::endl;
	    }
	  __flags |= TYPE_BOOLEAN;
	  if (memsize > 0) __boolean_values.assign (memsize, DEFAULT_VALUE_BOOLEAN);
	}

      if (type_ == TYPE_INTEGER_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: INTEGER..." 
			<< " TYPE_INTEGER=" << (int) TYPE_INTEGER << std::endl;
	    }
	  __flags |= TYPE_INTEGER;
	  if ( memsize > 0 ) __integer_values.assign (memsize, DEFAULT_VALUE_INTEGER);
	}

      if (type_ == TYPE_REAL_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: REAL..." 
			<< std::endl;
	    }
	  __flags |= TYPE_REAL;
	  if (memsize > 0) __real_values.assign (memsize, DEFAULT_VALUE_REAL);
	}

      if (type_ == TYPE_STRING_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::__init_values: STRING..." 
			<< std::endl;
	    }
	  __flags |= TYPE_STRING;
	  if (memsize > 0) __string_values.assign (memsize, DEFAULT_VALUE_STRING);
	}

      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::__init_values: exiting." 
		    << std::endl;
	}
      return ERROR_SUCCESS;
    }

    int 
    properties::data::boolean (int size_)
    {
      __clear_values ();
      return __init_values (TYPE_BOOLEAN_SYMBOL, size_);
    }

    void 
    properties::data::set_description (const std::string & desc_)
    {
      __description = desc_;
    }

    const std::string & 
    properties::data::get_description () const
    {
      return __description;
    }

    int 
    properties::data::integer (int size_)
    {
      __clear_values ();
      return __init_values (TYPE_INTEGER_SYMBOL, size_);
    }

    int 
    properties::data::real (int size_)
    {
      __clear_values ();
      return __init_values (TYPE_REAL_SYMBOL, size_);
    }

    int 
    properties::data::string ( int size_ )
    {
      __clear_values ();
      return __init_values (TYPE_STRING_SYMBOL,size_);
    }

    /******/

    /*
      int properties::data::scalar ()
      {
      __flags^=MASK_VECTOR;
      return ERROR_SUCCESS;
      }

      int properties::data::vector ()
      {
      __flags|=MASK_VECTOR;
      return ERROR_SUCCESS;
      }
    */

    /******/

    bool 
    properties::data::has_type () const
    {
      return (__flags & MASK_TYPE) != TYPE_NONE;
    }

    bool 
    properties::data::is_boolean () const
    {
      return (__flags & MASK_TYPE) == TYPE_BOOLEAN;
    }

    bool 
    properties::data::is_integer () const
    {
      return (__flags & MASK_TYPE) == TYPE_INTEGER;
    }

    bool 
    properties::data::is_real () const
    {
      return (__flags & MASK_TYPE) == TYPE_REAL;
    }

    bool 
    properties::data::is_string () const
    {
      return (__flags & MASK_TYPE) == TYPE_STRING;
    }

    /******/

    bool 
    properties::data::is_scalar () const
    {
      return (__flags & MASK_VECTOR) == 0;
    }

    bool 
    properties::data::is_vector () const
    {
      return ! is_scalar ();
    }

    /******/

    bool 
    properties::data::is_unlocked () const
    {
      return (__flags & MASK_LOCK) == 0;
    }

    bool 
    properties::data::is_locked () const
    {
      return ! is_unlocked ();
    }

    /******/

    /*
      bool properties::data::is_visible () const
      {
      return (__flags & MASK_HIDDEN) == 0;
      }

      bool properties::data::is_hidden () const
      {
      return !is_visible ();
      }
    */

    /******/

    int 
    properties::data::lock ()
    {
      __flags |= MASK_LOCK;
      return ERROR_SUCCESS;
    }

    int 
    properties::data::unlock ()
    {
      __flags &= ~MASK_LOCK;
      return ERROR_SUCCESS;
    }

    /******/

    //size_t 
    int32_t
    properties::data::get_size () const
    {
      if (! has_type ()) 
	{
	  throw std::runtime_error ("properties::data::get_size: No type!");
	}

      if (is_vector ()) 
	{
	  if (is_boolean ()) return __boolean_values.size ();
	  if (is_integer ()) return __integer_values.size ();
	  if (is_real ()) return __real_values.size ();     
	  if (is_string ()) return __string_values.size (); 
	}
      else 
	{
	  return SCALAR_SIZE;
	}
      return 0;
    }

    //size_t 
    int32_t
    properties::data::size () const
    {
      return get_size ();
    }

    /******/

    properties::data::data (char type_, int size_)
    {
      __flags  = 0;
      __init_values (type_,size_);
    }

    properties::data::data (bool value_, int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...bool...): entering..."
		    << std::endl;
	}   
      __flags  = 0;
      //int size = (size_<0)?SCALAR_DEF:size_;
      __init_values (TYPE_BOOLEAN_SYMBOL, size_);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (value_, i);
	}
    }

    properties::data::data (int value_, int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...int...): entering..."
		    << std::endl;
	}   
      __flags  = 0;
      //int size = (size_<0)?SCALAR_DEF:size_;
      __init_values (TYPE_INTEGER_SYMBOL, size_);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (value_, i);
	}
    }

    properties::data::data (double value_, int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...double...): entering..." 
		    << std::endl;
	}   
      __flags  = 0;
      //int size = (size_<0)?SCALAR_DEF:size_;
      __init_values (TYPE_REAL_SYMBOL, size_);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (value_, i);
	}
    }

    properties::data::data (const std::string & value_, int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... " 
		    << value_  << std::endl;
	}   
      __flags  = 0;
      int code=0;
      //int size = (size_<0)?SCALAR_DEF:size_;
      __init_values (TYPE_STRING_SYMBOL, size_);
      if (has_forbidden_char (value_)) 
	{
	  std::ostringstream message;
	  message << "properties::data::data (std::string): Forbidden char in string '" << value_ << "'!";
	  throw std::runtime_error (message.str ());    
	}      
      for (int i = 0; i < (int) size (); i++) 
	{
	  code = set_value (value_, i);
	  if (code != ERROR_SUCCESS) 
	    {
	      throw std::runtime_error ("properties::data::data (std::string): failure!");   
	    }
	}
      if (properties::g_debug) 
	{
	  tree_dump (std::cerr,"properties::data::data","DEBUG: ");
	  std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... " 
		    << "code=" << code  << std::endl;
	
	}
    }

    properties::data::data (const char * value_, int size_)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...char *...): entering..." << std::endl;
	}   
      __flags = 0;
      //int size = (size_<0)?SCALAR_DEF:size_;
      __init_values (TYPE_STRING_SYMBOL, size_);
      std::string tmp;
      if (value_ != 0) 
	{
	  tmp = value_;
	  if (has_forbidden_char (tmp)) 
	    {
	      std::ostringstream message;
	      message << "properties::data::data (std::string): Forbidden char in string '" << tmp << "'!";
	      throw std::runtime_error (message.str ());    
	    }
	}      
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (tmp, i);
	}
    }

    properties::data::~data ()
    {
    }

    /******************************************************/

    bool 
    properties::data::index_is_valid (int index_) const
    {
      return (index_ >= 0) && (index_ < (int) get_size ());
    }

    /******************************************************/
 
    int 
    properties::data::set_value (bool value_, int index_) 
    {
      if (! is_boolean ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      __boolean_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int 
    properties::data::set_value (int value_, int index_) 
    {
      if (! is_integer ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      __integer_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int 
    properties::data::set_value (double value_, int index_) 
    {
      if (! is_real ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      __real_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int 
    properties::data::set_value (const std::string & value_, int index_) 
    {
      if (! is_string ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      /* special trick to forbid character '\"' in string as
       * it is used as separator for parsing:
       */
      if (has_forbidden_char (value_)) 
	{
	  return ERROR_FAILURE;
	}
      __string_values[index_] = value_;
      return ERROR_SUCCESS;
    }

    int 
    properties::data::set_value (const char * value_, int index_) 
    {
      return set_value (std::string (value_), index_);
    }

    /******************************************************/

    int 
    properties::data::get_value (bool & value_, int index_) const
    {
      if (! is_boolean ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      value_ = __boolean_values[index_];
      return ERROR_SUCCESS;
    }

    int 
    properties::data::get_value (int & value_, int index_) const
    {
      if (! is_integer ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      value_ = __integer_values[index_];
      return ERROR_SUCCESS;
    }

    int 
    properties::data::get_value (double & value_,int index_) const
    {
      if (! is_real ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      value_ = __real_values[index_];
      return ERROR_SUCCESS;
    }

    int 
    properties::data::get_value (std::string & value_, int index_) const
    {
      if (! is_string ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (index_)) 
	{
	  return ERROR_RANGE;
	} 
      value_ = __string_values[index_];
      return ERROR_SUCCESS;
    }

    /******************************************************/

    bool 
    properties::data::get_boolean_value (int index_) const
    {
      bool value;
      if ( get_value (value,index_) != ERROR_SUCCESS ) 
	{
	  throw std::runtime_error ("properties::data::get_boolean_value: type is not boolean!");    }
      return value;
    }

    int 
    properties::data::get_integer_value ( int index_ ) const
    {
      int value;
      if ( get_value (value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error ("properties::data::get_integer_value: type is not integer!");    }
      return value;
    }
  
    double 
    properties::data::get_real_value ( int index_ ) const
    {
      double value;
      if ( get_value (value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error ("properties::data::get_real_value: type is not double!");
      }
      return value;
    }

    std::string 
    properties::data::get_string_value ( int index_ ) const
    {
      std::string value;
      if ( get_value (value,index_) != ERROR_SUCCESS ) {
	throw std::runtime_error ("properties::data::get_string_value: type is not string!");
      }
      return value;
    }

    std::string 
    properties::data::get_type_label () const
    {
      if ( is_boolean () ) return "boolean";
      if ( is_integer () ) return "integer";
      if ( is_real ()    ) return "real";     
      if ( is_string ()    ) return "string";
      return "none";
    }

    std::string 
    properties::data::get_vector_label () const
    {
      if ( is_scalar () ) return "scalar";
      return "vector";
    }
    
    void 
    properties::data::dump (std::ostream & out_) const
    {
      tree_dump (std::cout,"datatools::utils::properties::data");
    }

    void 
    properties::data::to_string (std::ostream & out_) const
    {
      out_ << '[';
      out_ << get_type_label () << ':'
	   << get_vector_label () << ":"
	   << get_size () << ':';
      for (int i = 0; i < (int) get_size (); i++)
	{
	  if (i != 0) out_ << ' ';
	  if (is_boolean ()) out_ << get_boolean_value (i);
	  if (is_integer ()) out_ << get_integer_value (i);
	  if (is_real ())    out_ << get_real_value (i);
	  if (is_string ())  out_ << get_string_value (i);
	}
      out_ << ':'
	   << get_description (); 
      out_ << ']';
	
    }
    
    void 
    properties::data::tree_dump (std::ostream & out_, 
				 const std::string & title_,
				 const std::string & indent_,
				 bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty () ) indent = indent_;
      if (! title_.empty ()) 
	{
	  out_ << indent << title_ << std::endl;
	}

      if (! __description.empty ()) 
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Description  : " <<  get_description () << std::endl;
	}

      out_ << indent << du::i_tree_dumpable::tag 
	   << "Type  : " ;
      if (is_locked ()) out_ << "const ";
      out_ <<  get_type_label ();
      if (is_vector ()) 
	{
	  out_ << '[' << size () << ']' << " (vector)";
	}
      else 
	{
	  out_ << " (scalar)";
	}
      out_ << std::endl;

      /*
	out_ << indent << du::i_tree_dumpable::tag 
	<< "Lock  : " <<  (is_locked ()?"locked":"unlocked") << std::endl;
      */
      if (get_size () > 0)
	{
	  for (int i = 0; i < (int) get_size (); i++) 
	    {
	      out_ << indent;
	      if (i == (int) (get_size () - 1)) out_ << du::i_tree_dumpable::inherit_tag (inherit_);
	      else out_ << du::i_tree_dumpable::tag;
	      out_ << "Value";
	      if (is_vector ()) 
		{
		  out_ << "[" << i << "]";
		}
	      out_ << " : ";
	      if (is_boolean ()) out_ << std::dec << get_boolean_value (i) << std::endl;
	      if (is_integer ()) out_ << std::dec << get_integer_value (i) << std::endl;
	      out_.precision (16);
	      if (is_real ())   out_ << get_real_value (i) << std::endl;
	      if (is_string ())  out_ << '"' <<  get_string_value (i) << '"' << std::endl;
	    }
	}
      else
	{
	  out_ << indent;
	  out_ << du::i_tree_dumpable::inherit_tag (inherit_);
	  out_ << "<no value>" << std::endl;
	}
    }
    
    /**********************************************/
    const std::string properties::default_key_validator::ALLOWED_CHARS = 
                  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";

    properties::default_key_validator::default_key_validator ()
    {      
    }

    properties::default_key_validator:: ~default_key_validator ()
    {
    }

    bool 
    properties::default_key_validator::operator () (const std::string & key_arg_) const
    {
      if (key_arg_.empty ()) return false;
      if (key_arg_.find_first_not_of (ALLOWED_CHARS) != key_arg_.npos) return false;
      if (key_arg_.find_first_of ("0123456789.") == 0) return false;
      if (key_arg_[key_arg_.size ()-1] == '.') return false;
      return true;
    }

    /**********************************************/
    const std::string properties::PRIVATE_PROPERTY_PREFIX = "__";
    
    void properties::__validate_key (const std::string & key_arg_) const
    {
      if (__key_validator != 0) 
	{
	  //if ( ! (*__key_validator) (key_arg_) ) {
	  if (! __key_validator->operator () (key_arg_)) 
	    {
	      std::ostringstream message;
	      message << "properties::__validate_key: After key validator, the '" 
		      << key_arg_ << "' key is not valid!";
	      throw std::runtime_error (message.str ());
	    }
	}
    }

    void properties::set_description (const std::string & desc_)
    {
      __description = desc_;
    }

    const std::string & properties::get_description () const
    {
      return __description;
    }

    bool 
    properties::is_debug () const
    {
      return __debug;
    }
  
    void 
    properties::set_debug (bool new_value_)
    {
      __debug = new_value_;
    }

    //size_t
    int32_t
    properties::size () const
    {
      return __props.size ();
    }

    void 
    properties::set_key_validator (const basic_key_validator & key_validator_)
    {
      __clear_key_validator ();
      __key_validator = &key_validator_;
      __key_validator_deletion = false;
    }
    
    void 
    properties::set_key_validator (const basic_key_validator * key_validator_, 
				   bool  deletion_on_destroy_)
    {
      if (__key_validator != 0 && __key_validator== key_validator_) 
	{
	  return;
	}
      __clear_key_validator ();
      if (key_validator_ != 0) 
	{
	  __key_validator=key_validator_;
	  __key_validator_deletion = deletion_on_destroy_;
	}
    }

    void 
    properties::set_default_key_validator ()
    {
      __clear_key_validator ();
      __key_validator = &g_default_key_validator;// new default_key_validator;
      __key_validator_deletion = false; //true;
    }

    void 
    properties::unset_key_validator ()
    {
      __clear_key_validator ();
    }
  
    // ctor/dtor:
    properties::properties () 
      : __debug (false) , __key_validator (0)
    {
      set_description ("");
      set_default_key_validator ();
    }

    properties::properties (const std::string & desc_) 
      : __debug (false) , __key_validator (0)
    {
      set_description (desc_);
      set_default_key_validator ();
    }

    properties::properties (const std::string & desc_, 
			    const basic_key_validator & key_validator_) 
      : __debug (false) , __key_validator (0)
    {
      set_description (desc_);
      set_key_validator (key_validator_);
    }

    properties::properties (const basic_key_validator & key_validator_) 
      : __debug (false) , __key_validator (0)
    {
      set_description ("");
      set_key_validator (key_validator_);
    }
  
    properties::properties (const std::string & desc_, 
			    const basic_key_validator * key_validator_, 
			    bool deletion_on_destroy_) 
      : __debug (false) , __key_validator (0)
    {
      set_description (desc_);
      set_key_validator (key_validator_,deletion_on_destroy_);
    }

    properties::properties (const basic_key_validator * key_validator_, 
			    bool deletion_on_destroy_) 
      : __debug (false) , __key_validator (0)
    {
      set_description ("");
      set_key_validator (key_validator_, deletion_on_destroy_);
    }

    void 
    properties::__clear_key_validator ()
    {
      if (__key_validator != 0) 
	{
	  if (__key_validator_deletion) 
	    {
	      delete __key_validator;
	    }
	  __key_validator = 0;
	}
    }
  
    properties::~properties ()
    {
      properties::clear ();
    }

    void properties::erase (const std::string & key_)
    {
      pmap::iterator found = __props.find (key_);
      if (found == __props.end ())
	{
	  std::ostringstream message;
	  message << "properties::erase: key '" << key_ << "' not known!";
	  throw std::runtime_error (message.str ());	  
	}
      __props.erase (found);
    }

    void properties::erase_all_starting_with (const std::string & key_prefix_)
    {
      keys_col_t keys;
      keys_starting_with (keys, key_prefix_);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::erase_all_not_starting_with (const std::string & key_prefix_)
    {
      keys_col_t keys;
      keys_not_starting_with (keys, key_prefix_);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void
    properties::export_starting_with (properties & p_, 
				      const std::string & key_prefix_) const
    {
      if (this == &p_)
	{
	  throw runtime_error ("properties::export_starting_with: Self export is not allowed !");
	}
      keys_col_t ks;
      this->keys_starting_with (ks, key_prefix_);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  /*
	    clog << "DEVEL: properties::export_starting_with: property '" 
            << *i << "'..." << endl;
	  */
	  p_.__props[*i] = ptmp.__props[*i];
	}
    }

    void
    properties::export_not_starting_with (properties & p_, 
					  const std::string & key_prefix_) const
    {
      if (this == &p_)
	{
	  throw runtime_error ("properties::export_starting_with: Self export is not allowed !");
	}
      keys_col_t ks;
      keys_not_starting_with (ks, key_prefix_);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  p_.__props[*i] = ptmp.__props[*i];
	}
    }

    void properties::erase_all_ending_with (const std::string & key_suffix_)
    {
      keys_col_t keys;
      keys_ending_with (keys, key_suffix_);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::erase_all_not_ending_with (const std::string & key_suffix_)
    {
      keys_col_t keys;
      keys_not_ending_with (keys, key_suffix_);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void
    properties::export_ending_with (properties & p_, 
                                    const std::string & key_suffix_) const
    {
      if (this == &p_)
	{
	  throw runtime_error ("properties::export_ending_with: Self export is not allowed !");
	}
      keys_col_t ks;
      this->keys_ending_with (ks, key_suffix_);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  /*
	    clog << "DEVEL: properties::export_ending_with: property '" 
            << *i << "'..." << endl;
	  */
	  p_.__props[*i] = ptmp.__props[*i];
	}
    }

    void
    properties::export_not_ending_with (properties & p_, 
                                        const std::string & key_suffix_) const
    {
      if (this == &p_)
	{
	  throw runtime_error ("properties::export_ending_with: Self export is not allowed !");
	}
      keys_col_t ks;
      keys_not_ending_with (ks, key_suffix_);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  p_.__props[*i] = ptmp.__props[*i];
	}
    }

    void
    properties::erase_all ()
    {
      __props.clear ();
    }

    void properties::clean (const std::string & key_)
    { 
      pmap::iterator found = __props.find (key_);
      if (found != __props.end ())
	{
	  __props.erase (found);
	}
    }

    void properties::clear ()
    {
      reset ();
    }

    void properties::reset ()
    {
      set_description ("");
      __props.clear ();
      __clear_key_validator ();
      __debug = false;
    }

    void
    properties::keys_not_starting_with (properties::vkeys & keys_, 
					const std::string & key_prefix_) const
    {
      if (key_prefix_.empty ())
 	{
	  throw runtime_error("properties::keys_not_starting_with: Empty key prefix argument !");
	}
      size_t n = key_prefix_.size ();
      for (pmap::const_iterator iter = __props.begin (); 
	   iter != __props.end (); 
	   iter++) 
	{
	  bool push = true;
	  if (iter->first.substr (0, n) == key_prefix_)
	    {
	      push = false;
	    }
	  if (push)
	    {
	      keys_.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys
    properties::keys_not_starting_with (const std::string & key_prefix_) const
    {
      properties::vkeys lkeys;
      keys_not_starting_with (lkeys, key_prefix_);
      return lkeys;
    }

    void
    properties::keys_starting_with (properties::vkeys & keys_, 
				    const std::string & key_prefix_) const
    {
      if (key_prefix_.empty ())
	{
	  throw runtime_error("properties::keys_starting_with: Empty key prefix argument !");
	}
      size_t n = key_prefix_.size ();
      for (pmap::const_iterator iter = __props.begin (); 
	   iter != __props.end (); 
	   iter++) 
	{
	  if (iter->first.size () < n) continue;
	  if (iter->first.substr (0, n) == key_prefix_)
	    {
	      keys_.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys
    properties::keys_starting_with (const std::string & key_prefix_) const
    {
      properties::vkeys lkeys;
      keys_starting_with (lkeys, key_prefix_);
      return lkeys;
    }

    void
    properties::keys_not_ending_with (properties::vkeys & keys_, 
                                      const std::string & key_suffix_) const
    {
      if (key_suffix_.empty ())
 	{
	  throw runtime_error("properties::keys_not_ending_with: Empty key suffix argument !");
	}
      size_t n = key_suffix_.size ();
      for (pmap::const_iterator iter = __props.begin (); 
	   iter != __props.end (); 
	   iter++) 
	{
	  bool push = true;
	  if (iter->first.substr
              (iter->first.size () - n, iter->first.size ())
              == key_suffix_)
	    {
	      push = false;
	    }
	  if (push)
	    {
	      keys_.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys
    properties::keys_not_ending_with (const std::string & key_suffix_) const
    {
      properties::vkeys lkeys;
      keys_not_ending_with (lkeys, key_suffix_);
      return lkeys;
    }

    void
    properties::keys_ending_with (properties::vkeys & keys_, 
                                  const std::string & key_suffix_) const
    {
      if (key_suffix_.empty ())
	{
	  throw runtime_error("properties::keys_ending_with: Empty key suffix argument !");
	}
      size_t n = key_suffix_.size ();
      for (pmap::const_iterator iter = __props.begin (); 
	   iter != __props.end (); 
	   iter++) 
	{
	  if (iter->first.size () < n) continue;
	  if (iter->first.substr
              (iter->first.size ()-n, iter->first.size ())
              == key_suffix_)
	    {
	      keys_.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys
    properties::keys_ending_with (const std::string & key_suffix_) const
    {
      properties::vkeys lkeys;
      keys_ending_with (lkeys, key_suffix_);
      return lkeys;
    }

    properties::vkeys properties::keys () const
    {
      properties::vkeys lkeys;
      keys (lkeys);
      return lkeys;
    }
    
    void properties::keys (properties::vkeys & keys_) const
    {
      for (pmap::const_iterator iter = __props.begin (); 
	   iter != __props.end (); 
	   iter++ ) 
	{
	  keys_.push_back (iter->first);
	}
      return;
    }
    
    void properties::lock (const std::string & key_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      data_ptr->lock ();
      return;
    }
    
    void properties::unlock (const std::string & key_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      data_ptr->unlock ();
    }
    
    bool properties::is_locked (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_locked ();
    }

    std::string
    properties::make_private_key (const std::string & key_)
    {
      if (key_is_private (key_)) return key_;
      else
	{
	  std::ostringstream oss;
	  oss << PRIVATE_PROPERTY_PREFIX << key_;
	  return oss.str ();
	}
    }
    
    bool properties::key_is_private (const std::string & key_)
    {
      if (key_.size () < 2) return false;
      return key_.substr (0, PRIVATE_PROPERTY_PREFIX.size ()) == PRIVATE_PROPERTY_PREFIX;
    }

    bool properties::key_is_public (const std::string & key_)
    {
      return ! key_is_private (key_);
    }

    bool properties::is_private (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return key_is_private (key_);
    }

    bool properties::is_public (const std::string & key_) const
    {
      return ! is_private (key_);
    }
    
    bool properties::is_boolean (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_boolean ();
    }
    
    bool properties::is_integer (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_integer ();
    }
    
    bool properties::is_real (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_,&data_ptr);
      return data_ptr->is_real ();
    }
    
    bool properties::is_string (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_string ();
    }
    
    bool properties::is_scalar (const std::string & key_) const
    { 
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_scalar ();
    }
    
    bool properties::is_vector (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->is_vector ();
    }
     
    //size_t 
    int32_t
    properties::size (const std::string & key_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      return data_ptr->size ();
    }

    bool properties::has_key (const std::string & key_) const
    {
      if (__props.size () == 0) return false;
      bool hk = __props.find (key_) != __props.end ();
      return hk;
    }
  
    void properties::__check_nokey (const std::string & key_) const
    {
      //std::clog << "DEVEL: properties::__check_nokey: key='" << key_ << "'" << std::endl;
      if (has_key (key_)) 
	{
	  std::ostringstream message;
	  message << "properties::__check_nokey: key '" << key_ << "' already used!";
	  throw std::runtime_error (message.str ());
	}
    }
  
    void properties::__check_key (const std::string & key_, 
				  data ** data_)
    {
      pmap::iterator iter = __props.find (key_);
      if (__props.find (key_) == __props.end ()) 
	{
	  std::ostringstream message;
	  message << "properties::__check_key: key '" << key_ << "' does not exist!";
	  throw std::runtime_error (message.str ());
	}
      *data_ = & (iter->second);
    }
  
    void properties::__check_key (const std::string & key_, 
				  const data ** data_) const
    {
      pmap::const_iterator iter = __props.find (key_);
      if (__props.find (key_) == __props.end ()) 
	{
	  std::ostringstream message;
	  message << "properties::__check_key: key '" << key_ << "' does not exist!";
	  throw std::runtime_error (message.str ());
	}
      *data_ = & (iter->second);
    }

    void properties::store_flag (const std::string & key_, 
                                 const std::string & desc_, 
                                 bool lock_)
    {
      store (key_, true, desc_, lock_);
    }
  
    void properties::store (const std::string & key_, 
			    bool value_, 
			    const std::string & desc_, 
			    bool lock_) 
    {
      __check_nokey (key_);
      __validate_key (key_);
      data a_data (value_,data::SCALAR_DEF);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      if (lock_) __props[key_].lock ();
    }

    void properties::store (const std::string & key_, 
			    int value_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      data a_data (value_);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock ();
    }

    void properties::store (const std::string & key_, 
			    double value_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      data a_data (value_);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock ();
    }

    void properties::store (const std::string & key_, 
			    const std::string & value_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      data a_data (value_);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      if ( lock_ ) __props[key_].lock ();
    }

    void properties::store (const std::string & key_, 
			    const char * value_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      properties::store (key_, std::string (value_), desc_, lock_);
    }

    void properties::store (const std::string & key_, 
			    const data::vbool & values_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      int size = values_.size ();
      //       if ( size < 0 ) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of booleans size!");
      //       }
      data a_data (data::TYPE_BOOLEAN_SYMBOL,size);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  __props[key_].set_value (values_[i], i);
	}
      if (lock_) __props[key_].lock ();
    }
  
    void properties::store (const std::string & key_, 
			    const data::vint & values_, 
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      int size = values_.size ();
      //       if ( size < 0 ) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of integers size!");
      //       }
      data a_data (data::TYPE_INTEGER_SYMBOL, size);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  __props[key_].set_value (values_[i], i);
	}
      if (lock_) __props[key_].lock ();
    }
  
    void properties::store (const std::string & key_, 
			    const data::vdouble & values_, 
			    const std::string & desc_,
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      int size = values_.size ();
      //       if ( size < 0 ) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of reals size!");
      //       }
      data a_data (data::TYPE_REAL_SYMBOL, size);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  __props[key_].set_value (values_[i], i);
	}
      if (lock_) __props[key_].lock ();
    }
  
    void properties::store (const std::string & key_, 
			    const data::vstring & values_,
			    const std::string & desc_, 
			    bool lock_)
    {
      __check_nokey (key_);
      __validate_key (key_);
      int size = values_.size ();
      //       if ( size < 0 ) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of string size!");
      //       }
      data a_data (data::TYPE_STRING_SYMBOL, size);
      a_data.set_description (desc_);
      __props[key_] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  __props[key_].set_value (values_[i], i);
	}
      if (lock_) __props[key_].lock ();
    }
  
    /**********************************/

    void properties::change (const std::string & key_, 
			     bool  value_, 
			     int index_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->set_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: cannot change value for property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
    }

    void properties::change (const std::string & key_, 
			     int value_, 
			     int index_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->set_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: cannot change value for property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
    }

    void properties::change (const std::string & key_, 
			     double value_, 
			     int index_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->set_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: cannot change value for property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
    }

    void properties::change (const std::string & key_, 
			     const std::string & value_, 
			     int index_)
    {
      data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->set_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: cannot change value for property '" 
		  << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
    }

    void properties::change (const std::string & key_, 
			     const char * value_, 
			     int index_)
    {
      std::string value = "";
      if (value_ != 0) value = value_;
      change (key_,value,index_);
    }
  
    void properties::change (const std::string & key_, 
			     const data::vbool & values_)
    {
      data * data_ptr = 0;
      __check_key (key_,&data_ptr);
      if (! data_ptr->is_boolean () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << key_ << "' is not a vector of booleans!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) values_.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->boolean (values_.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  int error = data_ptr->set_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
    }
  
    void properties::change (const std::string & key_, 
			     const data::vint & values_)
    {
      data * data_ptr=0;
      __check_key (key_,&data_ptr);
      if (! data_ptr->is_integer () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << key_ << "' is not a vector of integers!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) values_.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->integer (values_.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int)  values_.size (); i++) 
	{
	  int error = data_ptr->set_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
    }
  
    void properties::change (const std::string & key_, 
			     const data::vdouble & values_)
    {
      data * data_ptr=0;
      __check_key (key_,&data_ptr);
      if (! data_ptr->is_real () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << key_ << "' is not a vector of reals!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) values_.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->real (values_.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  int error = data_ptr->set_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
    }
  
    void properties::change (const std::string & key_, 
			     const data::vstring & values_)
    {
      data * data_ptr=0;
      __check_key (key_,&data_ptr);
      if (! data_ptr->is_string () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << key_ << "' is not a vector of strings!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) values_.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->string (values_.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  int error = data_ptr->set_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: cannot change values for vector property '" << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
    }

    /********************/

    void
    properties::update_flag (const std::string & key_)
    {
      update (key_, true);
    }

    void
    properties::update (const std::string & key_, 
			bool value_)
    {
      if (has_key (key_))
	{
	  change (key_, value_);
	}
      else
	{
	  store (key_, value_);
	}
    }

    void
    properties::update (const std::string & key_ , 
		        int value_)
    {
      if (has_key (key_))
	{
	  //	  std::cerr << "DEVEL: properties::update: change : " << key_ << " to " << value_ << std::endl;
	  change (key_, value_);
	}
      else
	{
	  //	  std::cerr << "DEVEL: properties::update: store : " << key_ << " at " << value_ << std::endl;
	  store (key_, value_);
	}
    }

    void
    properties::update (const std::string & key_, 
			double value_)
    {
      if (has_key (key_))
	{
	  change (key_, value_);
	}
      else
	{
	  store (key_, value_);
	}
    }

    void
    properties::update (const std::string & key_, 
			const std::string & value_)
    {
      if (has_key (key_))
	{
	  change (key_, value_);
	}
      else
	{
	  store (key_, value_);
	}
    }

    void
    properties::update (const std::string & key_, 
			const data::vbool & values_)
    {
      if (has_key (key_))
	{
	  change (key_, values_);
	}
      else
	{
	  store (key_, values_);
	}
    }

    void
    properties::update (const std::string & key_, 
			const data::vint & values_)
    {
      if (has_key (key_))
	{
	  change (key_, values_);
	}
      else
	{
	  store (key_, values_);
	}
    }

    void
    properties::update (const std::string & key_, 
			const data::vdouble & values_)
    {
      if (has_key (key_))
	{
	  change (key_, values_);
	}
      else
	{
	  store (key_, values_);
	}
    }

    void
    properties::update (const std::string & key_, 
			const data::vstring & values_)
    {
      if (has_key (key_))
	{
	  change (key_, values_);
	}
      else
	{
	  store (key_, values_);
	}
    }

    /********************/
 
    void properties::fetch (const std::string & key_, 
			    bool & value_, int index_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->get_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch boolean value from property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
    }

    void properties::fetch (const std::string & key_, 
			    int & value_, int index_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->get_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch integer value from property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
    }

    void properties::fetch (const std::string & key_, 
			    double & value_, int index_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->get_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch real value from property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
    }

    void properties::fetch (const std::string & key_, 
			    std::string & value_, int index_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      int error = data_ptr->get_value (value_, index_);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: cannot fetch string value from property '" << key_ << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
    }
    
    bool
    properties::has_flag (const std::string & key_) const
    {
      bool flag_is_on = false;
      try
	{
	  flag_is_on = fetch_boolean (key_, 0);
	}
      catch (std::runtime_error & e)
	{
	}
      return flag_is_on;
    }
   
    bool properties::fetch_boolean (const std::string & key_, 
				    int index_) const
    {
      bool value;
      fetch (key_, value, index_);
      return value;
    }

    int properties::fetch_integer (const std::string & key_, 
				   int index_) const
    {
      int value;
      fetch (key_, value, index_);
      return value;
    }

    double properties::fetch_real (const std::string & key_, 
				   int index_) const
    {
      double value;
      fetch (key_, value, index_);
      return value;
    }

    std::string properties::fetch_string (const std::string & key_, 
					  int index_) const
    {
      std::string value;
      fetch (key_, value, index_);
      return value;
    }

    void properties::fetch (const std::string & key_, 
			    data::vbool & values_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      if (! data_ptr->is_boolean () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << key_ << "' is not a vector of booleans!";
	  throw std::runtime_error (message.str ());
	}
      values_.resize (data_ptr->size ());
      values_.assign (data_ptr->size (), data::DEFAULT_VALUE_BOOLEAN);
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  bool val;
	  int error = data_ptr->get_value (val, i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: cannot fetch boolean values for vector property '" 
		      << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  values_[i] = val;
	}
    }
  
    void properties::fetch (const std::string & key_, 
			    data::vint & values_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      if (! data_ptr->is_integer () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << key_ << "' is not a vector of integers!";
	  throw std::runtime_error (message.str ());
	}
      values_.resize (data_ptr->size ());
      values_.assign (data_ptr->size (), data::DEFAULT_VALUE_INTEGER);
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  //int val;
	  //int error = data_ptr->get_value (val,i);
	  int error = data_ptr->get_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: cannot fetch integer values for vector property '" 
		      << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //values_[i]=val;
	}
    }
  
    void properties::fetch (const std::string & key_,
			    data::vdouble & values_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      if (! data_ptr->is_real () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << key_ << "' is not a vector of reals!";
	  throw std::runtime_error (message.str ());
	}
      values_.resize (data_ptr->size ());
      values_.assign (data_ptr->size (), data::DEFAULT_VALUE_REAL);
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  //double val;
	  //int error = data_ptr->get_value (val,i);
	  int error = data_ptr->get_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: cannot fetch real values for vector property '" 
		      << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //values_[i]=val;
	}
    }
  
    void properties::fetch (const std::string & key_, 
			    data::vstring & values_) const
    {
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      if (! data_ptr->is_string () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << key_ << "' is not a vector of strings!";
	  throw std::runtime_error (message.str ());
	}
      values_.resize (data_ptr->size ());
      values_.assign (data_ptr->size (),data::DEFAULT_VALUE_STRING);
      for (int i = 0; i < (int) values_.size (); i++) 
	{
	  //std::string val;
	  //int error = data_ptr->get_value (val,i);
	  int error = data_ptr->get_value (values_[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: cannot fetch string values for vector property '" 
		      << key_ << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //values_[i]=val;
	}
    }


    /****************************/

    std::string 
    properties::key_to_string (const std::string & key_) const
    {
      if (! has_key (key_)) return "";
      std::ostringstream oss;
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      data_ptr->to_string (oss);
      return oss.str ();
    }

    std::string 
    properties::key_to_property_string (const std::string & key_) const
    {
      if (! has_key (key_)) return "";
      std::ostringstream oss;
      const data * data_ptr = 0;
      __check_key (key_, &data_ptr);
      oss << key_ << '=';
      data_ptr->to_string (oss);
      return oss.str ();
    }

    void properties::tree_dump (std::ostream & out_, 
				const std::string & title_,
				const std::string & indent_,
				bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) 
	{
	  out_ << indent << title_ << std::endl;
	}
      /*
        out_ << indent << du::i_tree_dumpable::tag 
        << "Debug : " <<  __debug << std::endl;
      */
      if (! __description.empty ()) 
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Description  : " <<  get_description () << std::endl;
	}
      if (__props.size () == 0) 
	{
	  out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	       << "<no property>" << std::endl;
	}
      else 
	{
	  for (pmap::const_iterator i = __props.begin ();
	       i != __props.end () ;
	       i++) 
	    {
	      const std::string & key = i->first;
	      const du::properties::data & a_data = i->second;
	      out_ << indent;
	      std::ostringstream indent_oss;
	      indent_oss << indent;
	      pmap::const_iterator j = i; 
	      j++;
	      if (j == __props.end ()) 
		{
		  out_ << du::i_tree_dumpable::inherit_tag (inherit_);
		  indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
		}
	      else 
		{
		  out_ << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;
		}
	      out_ << "Name : " << '"' << key << '"' << std::endl;
	      a_data.tree_dump (out_, "", indent_oss.str ());
	    }
	}
    }

    void properties::dump (std::ostream & out_) const
    {
      tree_dump (out_, "datatools::utils::properties");
    }

    void properties::write_config (const std::string & filename_, 
				   const properties & p_,
				   bool use_smart_modulo_,
				   bool write_public_only_)
    {
      std::ostream * out = &std::cout;
      std::ofstream fout;
      if (! filename_.empty ()) 
	{
	  fout.open (filename_.c_str ());
	  if (! fout) 
	    {
	      std::ostringstream message;
	      message << "properties::writeread_config: "
		      << "cannot open file '"
		      << filename_ << "!"
		      << std::endl;
	      throw std::runtime_error (message.str ());
	    }
	  out = &fout;
	}
      config a_cfg (use_smart_modulo_,
		    config::mode_header_footer,
		    (write_public_only_?
		     config::write_public_only:
		     config::write_private_also));
      a_cfg.write (*out, p_);
    }

    void properties::read_config (const std::string & filename_, 
				  properties & p_)
    {
      std::ifstream fin (filename_.c_str ());
      if (! fin) 
	{
	  std::ostringstream message;
	  message << "properties::read_config: "
		  << "cannot open file '"
		  << filename_ << "!"
		  << std::endl;
	  throw std::runtime_error (message.str ());
	}
      config a_cfg;
      p_.clear ();
      a_cfg.read (fin, p_);
    }

    /**********************************************************************/

    const bool properties::config::write_public_only = true;
    const bool properties::config::write_private_also = false;

    void properties::config::write (std::ostream & out_, 
				    const properties & p_)
    {
      int mode = __mode;
      /*
        clog << "DEVEL: properties::config::write: mode=" 
        << mode << endl; 
      */
      if (mode == MODE_HEADER_FOOTER) 
	{
	  out_ << "# List of properties (datatools::utils::properties):" << std::endl;
	  /*
            out_ << "# This file was autogenerated by "
            << "'datatools::utils::properties'." << std::endl;
            out_ << "# Do not modify it unless you know what you are doing!" 
            << std::endl;
	  */
	  out_ << std::endl;
	}

      if (! p_.get_description ().empty ()) 
	{
	  out_ << "#@config " << p_.get_description () << std::endl;
	  out_ << std::endl;
	}
 
      for (pmap::const_iterator i = p_.__props.begin ();
	   i != p_.__props.end ();
	   i++) 
	{
	  const std::string        a_key  = i->first;
	  const properties::data & a_data = i->second;

	  if (__write_public_only)
	    {
	      if (key_is_private (a_key)) continue;
	    }

	  if (! a_data.get_description ().empty ()) 
	    {
	      out_ << "#@description " << a_data.get_description () << std::endl;
	    }

	  // name:
	  out_ << a_key << " : ";

	  if (a_data.is_locked ()) 
	    {
	      out_ << "const ";
	    }

	  // type:
	  int size = properties::data::SCALAR_SIZE;
	  out_ << a_data.get_type_label ();
	  if (a_data.is_vector ()) 
	    {
	      size=a_data.get_size ();
	      out_ << '[' << size << ']';
	    } 
	  out_ << ' ';

	  out_ << "=";

	  // values (s):
	  int modulo = 1;

	  if (a_data.is_boolean ()) 
	    { 
	      modulo = 10;
	    }    

	  if (a_data.is_integer ()) 
	    { 
	      modulo = 5;
	    }    

	  if (__use_smart_modulo) 
	    {
	      if ( (size>1) && (size>modulo)) 
		{
		  out_ << ' ' << __continuation_char << '\n';
		}
	    }

	  for (int i = 0; i < size; i++) 
	    { 

	      //if ( (i+1) != size ) 
	      out_ << ' ';  
	    
	      if (a_data.is_boolean ()) 
		{ 
		  out_ << a_data.get_boolean_value (i);
		}

	      if (a_data.is_integer ()) 
		{
		  out_ << a_data.get_integer_value (i);
		}

	      if (a_data.is_real ()) 
		{
		  int oldprec = out_.precision ();
		  out_.precision (16);
		  out_ << a_data.get_real_value (i);
		  out_.precision (oldprec);
		}

	      if (a_data.is_string ()) 
		{
		  out_ << '"' << a_data.get_string_value (i) << '"';
		}

	      if (__use_smart_modulo) 
		{
		  if ( (i < (size - 1)) && ( (i + 1) % modulo) == 0) 
		    {
		      out_ << ' ' << __continuation_char << '\n';
		    }
		}
	    }
	  out_ << std::endl;
	  out_ << std::endl;
	}

      if (mode == MODE_HEADER_FOOTER) 
	{
	  out_ << "# End of list od properties." << std::endl;
	}
      return;
    }
    
    bool properties::config::is_debug () const
    {
      return __debug;
    }

    void properties::config::set_debug (bool debug_)
    {
      __debug =debug_;
    }

    properties::config::config (bool use_smart_modulo_, 
				int mode_,
				bool write_public_only_)
    {
      __debug           = false;
      __use_smart_modulo= use_smart_modulo_ ;
      __mode            = MODE_DEFAULT;
      __mode            = mode_;
      __write_public_only = write_public_only_;
      __read_line_count = 0;
      __continuation_char = DEFAULT_CONTINUATION_CHAR;
      __comment_char = DEFAULT_COMMENT_CHAR;
      __assign_char  = DEFAULT_ASSIGN_CHAR;
      __desc_char    = DEFAULT_DESC_CHAR;
    }

    properties::config::~config ()
    {
    }

    void properties::config::read (std::istream & in_,
				   properties & p_)
    {
      __read (in_,p_);
    }

    void properties::config::__read (std::istream & in_,
				     properties & p_)
    {
      bool verbose_parsing  = __debug || properties::g_debug;

      std::string line_in;
      std::string prop_description;
      bool line_goon = false;
      while (in_) 
	{
	  if (verbose_parsing) 
	    {
	      std::cerr << "DEBUG: properties::config::read_next_property: "
			<< "loop on input stream..." 
			<< std::endl; 
	    }
	  std::string line_get;
	  std::getline (in_,line_get);
	  /*
	    if ( !in_ ) {
	    if ( verbose_parsing ) {
	    std::cerr << "DEBUG: properties::config::read_next_property: "
	    << "Cannot get a new line from input!"
	    << std::endl;
	    } 
	    return;  
	    }
	  */
	  __read_line_count++;
      
	  // check if line has a continuation mark at the end:
	  bool line_continue = false; 
	  int sz = line_get.size ();
	  if (sz > 0 && line_get[sz-1] == __continuation_char) 
	    {
	      line_continue = true;
	      line_get = line_get.substr (0, sz - 1);
	      if (verbose_parsing) 
		{
		  std::cerr << "DEBUG: properties::config::read_next_property: "
			    << "line_get='" 
			    << line_get
			    << "'" << std::endl; 
		}
	    } 
      
	  if (line_goon) 
	    {
	      // append to previous line:
	      line_in += line_get;	
	      if (verbose_parsing) 
		{
		  std::cerr << "DEBUG: properties::config::read_next_property: "
			    << "append --> line_in='" 
			    << line_in << "'" << std::endl; 
		}
	    }
	  else 
	    {
	      // a new line
	      line_in = line_get;
	      if (verbose_parsing) 
		{
		  std::cerr << "DEBUG: properties::config::read_next_property: "
			    << "new --> line_in='" 
			    << line_in << "'" << std::endl; 
		}
	      __read_line_count++;
	    }
	  line_goon = false;
      
	  // check if line should go on:
	  if (line_continue) line_goon = true;
      
	  if (! line_goon) 
	    {
	      bool skip_line = false;

	      std::string line = line_in;
	      if (verbose_parsing) 
		{
		  std::cerr << "DEBUG: properties::config::read_next_property: line " 
			    << __read_line_count << " size is " 
			    << line.size () << "" << std::endl;
		}
	    
	      // check if line is blank:
	      std::istringstream check_iss (line_in);
	      std::string check_word;
	      check_iss >> std::ws >> check_word;
	      if (check_word.empty ()) 
		{
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_next_property: line " 
				<< __read_line_count << " is blank" << std::endl;
		    }
		  skip_line=true;
		}
	  
	      // check if line is a comment:
	      if (! skip_line) 
		{
		  std::istringstream iss (line);
		  char c = 0;
		  iss >> c;
		  if (c == __comment_char) 
		    {
		      if (verbose_parsing) 
			{
			  std::cerr << "DEBUG: properties::config::read_next_property: line " 
				    << __read_line_count << " is a comment" << std::endl;
			}
		
		      iss >> std::ws;
	      
		      std::string token;
	      
		      iss >> token;
		      if (token == "@config") 
			{
			  iss >> std::ws;
			  std::string desc;
			  std::getline (iss,desc);
			  if (! desc.empty ()) 
			    {
			      p_.set_description (desc);
			    }
			}
	      
		      if (token == "@description") 
			{
			  iss >> std::ws;
			  std::string desc;
			  std::getline (iss,desc);
			  if (! desc.empty ()) 
			    {
			      prop_description = desc;
			    }
			}
	      
		      skip_line = true; 
		    }
		} // if ( ! skip_line )
	  
	      // parse line:
	      if (! skip_line) 
		{
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_next_property: "
				<< "line " 
				<< __read_line_count << " is '" << line << "'" << std::endl;
		    }
		  std::string line_parsing = line;
	    
	    
		  int flag_pos = line_parsing.find_first_of (__assign_char);
		  if (flag_pos >= (int) line_parsing.size ()) 
		    {
		      std::ostringstream message;
		      message << "properties::config::read_next_property: "
			      << "Cannot find assign symbol !" ;
		      throw std::runtime_error (message.str ());
		    }
	    
		  // parse property desc:
		  std::string property_desc_str = line_parsing.substr (0,flag_pos);
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_next_property: "
				<< "property_desc_str='" 
				<< property_desc_str << "'" << std::endl;
		    }
	    
		  // parse property value:
		  std::string property_value_str = line_parsing.substr (flag_pos+1);
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_next_property: "
				<< "property_value_str='" 
				<< property_value_str << "'" << std::endl;
		    }
	    
	    
		  bool scalar = true;
		  bool locked = false;
		  int  vsize  = -1;
		  char type   = 0;
		  std::string prop_key;
	    
		  int desc_pos = property_desc_str.find_first_of (__desc_char);
		  if (desc_pos == (int) property_desc_str.npos) 
		    {
		      std::istringstream key_ss (property_desc_str);
		      key_ss >> std::ws >> prop_key;
		      type = properties::data::TYPE_STRING_SYMBOL;
		    }
		  else 
		    {
		      std::string key_str = property_desc_str.substr (0, desc_pos);
		      std::istringstream key_ss (key_str);
		      key_ss >> std::ws >> prop_key;
	      
	      
		      std::string type_str = property_desc_str.substr (desc_pos+1);
	      
		      //std::string vector_str =
		      //property_desc_str.substr (0,desc_pos);
		      std::string type_str2;
	      
		      int vec_pos = type_str.find_first_of (OPEN_VECTOR);
		      if (vec_pos != (int) type_str.npos) 
			{
			  scalar = false;
			  type_str2 = type_str.substr (0, vec_pos);
			  std::string vec_str = type_str.substr (vec_pos + 1);
			  std::istringstream vec_ss (vec_str);
			  vec_ss >> vsize;
			  if (! vec_ss) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_next_property: "
				      << "Cannot find vector size !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  if (vsize < 0) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_next_property: "
				      << "Invalid vector size !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  char c = 0;
			  if (! vec_ss) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_next_property: "
				      << "Cannot find expected vector size closing symbol !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  vec_ss >> c;
			  if (c != CLOSE_VECTOR) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_next_property: "
				      << "Invalid vector size closing symbol !" ;
			      throw std::runtime_error (message.str ());
			    }
			}
		      else 
			{
			  scalar = true;
			  type_str2 =  type_str;
			}
	      
		      std::istringstream type_ss (type_str2);
		      std::string token;
	      
		      type_ss >> std::ws >> token;
		      if (token == "const") 
			{
			  locked = true;
			  type_ss >> std::ws >> token;
			}
	      
		      if  (token == "boolean") 
			{
			  type = properties::data::TYPE_BOOLEAN_SYMBOL;		  
			}
		      else if (token == "integer") 
			{
			  type = properties::data::TYPE_INTEGER_SYMBOL;		  
			}
		      else if (token == "real") 
			{
			  type = properties::data::TYPE_REAL_SYMBOL;		  
			}
		      else if (token == "string")
			{
			  type = properties::data::TYPE_STRING_SYMBOL;		  
			}
		      else 
			{
			  std::ostringstream message;
			  message << "properties::config::read_next_property: "
				  << "Missing type specifier!" ;
			  throw std::runtime_error (message.str ());
			}
	      
		      if (verbose_parsing) 
			{
			  std::cerr << "DEBUG: properties::config::read_next_property: "
				    << "type='" 
				    << type << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_next_property: "
				    << "locked='" 
				    << locked << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_next_property: "
				    << "vsize='" 
				    << vsize << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_next_property: "
				    << "prop_description='" 
				    << prop_description << "'" << std::endl;
			}
	      
		      bool        a_boolean = false;
		      int         a_integer = 0;
		      double      a_real    = 0.0;
		      std::string a_string  = "";
		      std::vector<bool>        v_booleans;
		      std::vector<int>         v_integers;
		      std::vector<double>      v_reals;
		      std::vector<std::string> v_strings;
	      
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL && ! scalar) 
			{
			  if (vsize > 0) v_booleans.assign (vsize,
							    properties::data::DEFAULT_VALUE_BOOLEAN);
			}
		      if (type == properties::data::TYPE_INTEGER_SYMBOL && ! scalar) 
			{
			  if (vsize > 0) v_integers.assign (vsize,
							    properties::data::DEFAULT_VALUE_INTEGER);
			}
		      if (type == properties::data::TYPE_REAL_SYMBOL && ! scalar) 
			{
			  if (vsize > 0) v_reals.assign (vsize,
							 properties::data::DEFAULT_VALUE_REAL);
			}
		      if (type == properties::data::TYPE_STRING_SYMBOL && ! scalar) 
			{
			  if (vsize > 0) v_strings.assign (vsize,
							   properties::data::DEFAULT_VALUE_STRING);
			}
	      
		      /*****/

		      std::istringstream iss (property_value_str);
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL) 
			{
			  if (scalar) 
			    {
			      iss >> a_boolean;
			      if (! iss) 
				{
				  std::ostringstream message;
				  message << "properties::config::read_next_property: "
					  << "Cannot read boolean value!" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  bool b;
				  iss >> b;
				  if (! iss ) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_next_property: "
					      << "Cannot read vector boolean value!" ;
				      throw std::runtime_error (message.str ());
				    }
				  v_booleans[i] = b;
				}
			    }
			}
		
		      if (type == properties::data::TYPE_INTEGER_SYMBOL) 
			{
			  //std::istringstream iss (property_value_str);
			  if (scalar) 
			    {
			      iss >> a_integer;
			      if (! iss) 
				{

				  std::ostringstream message;
				  message << "properties::config::read_next_property: "
					  << "Cannot read integer value!" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  int k;
				  iss >> k;
				  if (! iss) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_next_property: "
					      << "Cannot read vector integer value!" ;
				      throw std::runtime_error (message.str ());
				    }
				  v_integers[i] = k;
				}
			    }
			}
		
		      if (type == properties::data::TYPE_REAL_SYMBOL) 
			{
			  //std::istringstream iss (property_value_str);
			  if (scalar) 
			    {
			      iss >> a_real;
			      if (! iss) 
				{
				  std::ostringstream message;
				  message << "properties::config::read_next_property: "
					  << "Cannot read real value!" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  double x;
				  iss >> x;
				  if (! iss) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_next_property: "
					      << "Cannot read vector real value!" ;
				      throw std::runtime_error (message.str ());
				    }
				  v_reals[i] = x;
				}
			    }

			}
		
		      if (type == properties::data::TYPE_STRING_SYMBOL) 
			{
			  //std::istringstream iss (property_value_str);
			  if (scalar) 
			    {
			      if (! read_quoted_string (iss, a_string)) 
				{
				  std::ostringstream message;
				  message << "properties::config::read_next_property: "
					  << "Cannot read string value!" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  std::string str;
				  if (! read_quoted_string (iss,str)) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_next_property: "
					      << "Cannot read string value!" ;
				      throw std::runtime_error (message.str ());
				    }
				  v_strings[i] = str;
				}
			    }
			}

		      /*****/
	      
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL && scalar) 
			{
			  p_.store (prop_key, a_boolean, prop_description, locked);
			}
		      if (type == properties::data::TYPE_INTEGER_SYMBOL && scalar) 
			{
			  p_.store (prop_key, a_integer, prop_description, locked);
			}
		      if (type == properties::data::TYPE_REAL_SYMBOL && scalar) 
			{
			  p_.store (prop_key, a_real, prop_description, locked);
			}
		      if (type == properties::data::TYPE_STRING_SYMBOL && scalar) 
			{
			  p_.store (prop_key, a_string, prop_description, locked);
			}
	      
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL &&  !scalar) 
			{
			  p_.store (prop_key, v_booleans, prop_description, locked);
			}
		      if (type == properties::data::TYPE_INTEGER_SYMBOL && ! scalar) 
			{
			  p_.store (prop_key, v_integers, prop_description, locked);
			}
		      if (type == properties::data::TYPE_REAL_SYMBOL && ! scalar) 
			{
			  p_.store (prop_key, v_reals, prop_description, locked);
			}
		      if (type == properties::data::TYPE_STRING_SYMBOL && ! scalar) 
			{
			  p_.store (prop_key, v_strings, prop_description, locked);
			}
		      prop_description = "";
	      
		    }
		} // !skip_line
	    } // if ( ! line_goon )
	} // while ( *_in ) 
      
      return;
    }

    bool properties::config::read_quoted_string (std::istream & in_, 
						 std::string & str_)
    {
      std::string chain = "";
      in_ >> std::ws;
      if (! in_) 
	{
	  str_ = chain;
	  return true;
	}

      bool quoted = false;
      char lastc = 0;      
      do 
	{
	  char c = 0;
	  in_.get (c);
	  if (! in_) break;
	  if (c == '"') 
	    {
	      if (quoted) 
		{
		  quoted=false;
		  break;
		  /*  
		      if ( lastc == '\\' ) {
		      chain += c;
		      lastc=c;
		      }
		      else {
		      quoted=false;
		      break;
		      }
		  */
		}
	      else 
		{
		  if (lastc == 0) 
		    {
		      quoted = true;
		      continue;
		    }
		  else 
		    {
		      chain += c;
		      lastc  = c;
		    }
		}
	    }
	  else 
	    {
	      if (isspace (c)) 
		{
		  if (! quoted) 
		    {
		      in_.putback (c);
		      break;
		    }
		}
	      chain += c;
	      lastc = c;
	    }

	} while ( in_ );
      if (quoted) 
	{
	  return false;
	}

      str_ = chain;
      return true;
    }
 
    std::string 
    properties::build_property_key (const std::string & prefix_,
				    const std::string & subkey_)
    {
      std::string s (prefix_);
      s += '.';
      s += subkey_;
      return s;
    }

    void properties::export_to_string_based_dictionary (map<string, string> & dict_, bool quoted_strings_) const
    {
      for (pmap::const_iterator i = __props.begin ();
	   i != __props.end ();
	   i++)
	{
	  const string & key = i->first;
	  ostringstream valoss;
	  const data & data = i->second;
	  if ( data.is_vector ())
	    {
	      valoss << '(';
	    }
	  for (int i = 0; i < (int) data.get_size (); i++)
	    {
	      if (i != 0) valoss << ',';
	      if (data.is_boolean ()) valoss << data.get_boolean_value (i);
	      if (data.is_integer ()) valoss << data.get_integer_value (i);
	      if (data.is_real ())
		{
		  valoss.precision (15);
		  valoss << data.get_real_value (i);
		}
	      if (data.is_string ()) 
		{
		  if (quoted_strings_) valoss << '"';
		  valoss << data.get_string_value (i);
		  if (quoted_strings_) valoss << '"';
		}
	    }
	  if ( data.is_vector ())
	    {
	      valoss << ')';
	    }
	  dict_[key] = valoss.str ();
	}
    }
    
  } // end of namespace utils 

} // end of namespace datatools 

// end of properties.cc
