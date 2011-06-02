// -*- mode: c++; -*- 
/* properties.cc 
 */

#include <datatools/utils/properties.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace datatools {

  namespace utils {

    bool properties::g_debug                       = false;

    /* 2010-06-01 FM: to be updated */ 
    const std::string properties::SERIAL_TAG       = "datatools::utils::properties";
    const std::string properties::data::SERIAL_TAG = "__datatools::utils::properties::data";

    const std::string & properties::get_serial_tag () const
    {
      return properties::SERIAL_TAG;
    } 

    const std::string & properties::data::get_serial_tag () const
    {
      return properties::data::SERIAL_TAG;
    }

    properties::default_key_validator properties::g_default_key_validator;

    const bool        properties::data::DEFAULT_VALUE_BOOLEAN = false;
    const int         properties::data::DEFAULT_VALUE_INTEGER = 0;
    const double      properties::data::DEFAULT_VALUE_REAL    = 0.0;
    const std::string properties::data::DEFAULT_VALUE_STRING  = "";

    bool properties::data::has_forbidden_char (const std::string & a_str)
    {
      return a_str.find_first_of (STRING_FORBIDDEN_CHAR) != a_str.npos;
    }

    std::string properties::data::get_error_message (int a_error_code)
    {
      if (a_error_code == ERROR_SUCCESS) return "success";
      if (a_error_code == ERROR_FAILURE) return "generic error";
      if (a_error_code == ERROR_BADTYPE) return "invalid type";
      if (a_error_code == ERROR_RANGE ) return "invalid index";
      if (a_error_code == ERROR_LOCK  ) return "lock error";
      return "generic error";
    }

    void properties::data::clear_values_ ()
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::clear_values_: entering..." 
		    << std::endl;
	}
      boolean_values_.clear ();
      integer_values_.clear ();
      real_values_.clear ();
      string_values_.clear ();
      return;
    }

    int properties::data::init_values_ (char a_type, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::init_values_: entering..." 
		    << std::endl;
	}
      int memsize = a_size;
      if (a_size < 0) 
	{
	  memsize = 1;
	  flags_ &= ~MASK_VECTOR; // force scalar
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: SCALAR... size=" 
			<< memsize << std::endl;
	    }
	}
      else 
	{
	  flags_ |= MASK_VECTOR; // force vector
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: VECTOR... size=" 
			<< memsize << std::endl;
	    }
	}

      flags_ &= ~MASK_TYPE;
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::init_values_: desc=" 
		    << std::hex << (int) flags_ 
		    << std::dec << std::endl;
	}
    
      if (a_type == TYPE_BOOLEAN_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: BOOLEAN..." 
			<< std::endl;
	    }
	  flags_ |= TYPE_BOOLEAN;
	  if (memsize > 0) boolean_values_.assign (memsize, DEFAULT_VALUE_BOOLEAN);
	}

      if (a_type == TYPE_INTEGER_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: INTEGER..." 
			<< " TYPE_INTEGER=" << (int) TYPE_INTEGER << std::endl;
	    }
	  flags_ |= TYPE_INTEGER;
	  if (memsize > 0) integer_values_.assign (memsize, DEFAULT_VALUE_INTEGER);
	}

      if (a_type == TYPE_REAL_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: REAL..." 
			<< std::endl;
	    }
	  flags_ |= TYPE_REAL;
	  if (memsize > 0) real_values_.assign (memsize, DEFAULT_VALUE_REAL);
	}

      if (a_type == TYPE_STRING_SYMBOL) 
	{
	  if (properties::g_debug) 
	    {
	      std::cerr << "DEBUG: properties::data::init_values_: STRING..." 
			<< std::endl;
	    }
	  flags_ |= TYPE_STRING;
	  if (memsize > 0) string_values_.assign (memsize, DEFAULT_VALUE_STRING);
	}

      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::init_values_: exiting." 
		    << std::endl;
	}
      return ERROR_SUCCESS;
    }

    int properties::data::boolean (int a_size)
    {
      clear_values_ ();
      return init_values_ (TYPE_BOOLEAN_SYMBOL, a_size);
    }

    void properties::data::set_description (const std::string & a_description)
    {
      description_ = a_description;
      return;
    }

    const std::string & properties::data::get_description () const
    {
      return description_;
    }

    int properties::data::integer (int a_size)
    {
      clear_values_ ();
      return init_values_ (TYPE_INTEGER_SYMBOL, a_size);
    }

    int properties::data::real (int a_size)
    {
      clear_values_ ();
      return init_values_ (TYPE_REAL_SYMBOL, a_size);
    }

    int properties::data::string (int a_size)
    {
      clear_values_ ();
      return init_values_ (TYPE_STRING_SYMBOL, a_size);
    }

    /******/

    /*
      int properties::data::scalar ()
      {
      flags_^=MASK_VECTOR;
      return ERROR_SUCCESS;
      }

      int properties::data::vector ()
      {
      flags_|=MASK_VECTOR;
      return ERROR_SUCCESS;
      }
    */

    /******/

    bool properties::data::has_type () const
    {
      return (flags_ & MASK_TYPE) != TYPE_NONE;
    }

    bool properties::data::is_boolean () const
    {
      return (flags_ & MASK_TYPE) == TYPE_BOOLEAN;
    }

    bool properties::data::is_integer () const
    {
      return (flags_ & MASK_TYPE) == TYPE_INTEGER;
    }

    bool properties::data::is_real () const
    {
      return (flags_ & MASK_TYPE) == TYPE_REAL;
    }

    bool properties::data::is_string () const
    {
      return (flags_ & MASK_TYPE) == TYPE_STRING;
    }

    /******/

    bool properties::data::is_scalar () const
    {
      return (flags_ & MASK_VECTOR) == 0;
    }

    bool properties::data::is_vector () const
    {
      return ! is_scalar ();
    }

    /******/

    bool properties::data::is_unlocked () const
    {
      return (flags_ & MASK_LOCK) == 0;
    }

    bool properties::data::is_locked () const
    {
      return ! is_unlocked ();
    }

    /******/

    /*
      bool properties::data::is_visible () const
      {
      return (flags_ & MASK_HIDDEN) == 0;
      }

      bool properties::data::is_hidden () const
      {
      return !is_visible ();
      }
    */

    /******/

    int properties::data::lock ()
    {
      flags_ |= MASK_LOCK;
      return ERROR_SUCCESS;
    }

    int properties::data::unlock ()
    {
      flags_ &= ~MASK_LOCK;
      return ERROR_SUCCESS;
    }

    /******/

    int32_t properties::data::get_size () const
    {
      if (! has_type ()) 
	{
	  throw std::runtime_error ("properties::data::get_size: No type!");
	}

      if (is_vector ()) 
	{
	  if (is_boolean ()) return boolean_values_.size ();
	  if (is_integer ()) return integer_values_.size ();
	  if (is_real ())    return real_values_.size ();     
	  if (is_string ())  return string_values_.size (); 
	}
      else 
	{
	  return SCALAR_SIZE;
	}
      return 0;
    }

    int32_t properties::data::size () const
    {
      return get_size ();
    }

    /******/

    properties::data::data (char a_type, int a_size)
    {
      flags_  = 0;
      init_values_ (a_type, a_size);
      return;
    }

    properties::data::data (bool a_value, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...bool...): entering..."
		    << std::endl;
	}   
      flags_  = 0;
      //int size = (a_size<0)?SCALAR_DEF:a_size;
      init_values_ (TYPE_BOOLEAN_SYMBOL, a_size);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (a_value, i);
	}
      return;
    }

    properties::data::data (int a_value, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...int...): entering..."
		    << std::endl;
	}   
      flags_  = 0;
      //int size = (a_size<0)?SCALAR_DEF:a_size;
      init_values_ (TYPE_INTEGER_SYMBOL, a_size);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (a_value, i);
	}
      return;
    }

    properties::data::data (double a_value, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...double...): entering..." 
		    << std::endl;
	}   
      flags_  = 0;
      //int size = (a_size<0)?SCALAR_DEF:a_size;
      init_values_ (TYPE_REAL_SYMBOL, a_size);
      for (int i = 0; i < (int) size (); i++) 
	{
	  set_value (a_value, i);
	}
      return;
    }

    properties::data::data (const std::string & a_value, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... " 
		    << a_value  << std::endl;
	}   
      flags_  = 0;
      int code=0;
      //int size = (a_size<0)?SCALAR_DEF:a_size;
      init_values_ (TYPE_STRING_SYMBOL, a_size);
      if (has_forbidden_char (a_value)) 
	{
	  std::ostringstream message;
	  message << "properties::data::data (std::string): Forbidden char in string '" << a_value << "'!";
	  throw std::runtime_error (message.str ());    
	}      
      for (int i = 0; i < (int) size (); i++) 
	{
	  code = set_value (a_value, i);
	  if (code != ERROR_SUCCESS) 
	    {
	      throw std::runtime_error ("properties::data::data (std::string): failure!");   
	    }
	}
      if (properties::g_debug) 
	{
	  tree_dump (std::cerr, "properties::data::data", "DEBUG: ");
	  std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... " 
		    << "code=" << code  << std::endl;
	
	}
      return;
    }

    properties::data::data (const char * a_value, int a_size)
    {
      if (properties::g_debug) 
	{
	  std::cerr << "DEBUG: properties::data::ctor (...char *...): entering..." << std::endl;
	}   
      flags_ = 0;
      //int size = (a_size<0)?SCALAR_DEF:a_size;
      init_values_ (TYPE_STRING_SYMBOL, a_size);
      std::string tmp;
      if (a_value != 0) 
	{
	  tmp = a_value;
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
      return;
    }

    properties::data::~data ()
    {
      return;
    }

    /******************************************************/

    bool properties::data::index_is_valid (int a_index) const
    {
      return (a_index >= 0) && (a_index < (int) get_size ());
    }

    /******************************************************/
 
    int properties::data::set_value (bool a_value, int a_index) 
    {
      if (! is_boolean ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      boolean_values_[a_index] = a_value;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value (int a_value, int a_index) 
    {
      if (! is_integer ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      integer_values_[a_index] = a_value;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value (double a_value, int a_index) 
    {
      if (! is_real ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      real_values_[a_index] = a_value;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value (const std::string & a_value, int a_index) 
    {
      if (! is_string ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (is_locked ()) 
	{
	  return ERROR_LOCK;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      /* special trick to forbid character '\"' in string as
       * it is used as separator for parsing:
       */
      if (has_forbidden_char (a_value)) 
	{
	  return ERROR_FAILURE;
	}
      string_values_[a_index] = a_value;
      return ERROR_SUCCESS;
    }

    int properties::data::set_value (const char * a_value, int a_index) 
    {
      return set_value (std::string (a_value), a_index);
    }

    /******************************************************/

    int properties::data::get_value (bool & a_value, int a_index) const
    {
      if (! is_boolean ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      a_value = boolean_values_[a_index];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value (int & a_value, int a_index) const
    {
      if (! is_integer ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      a_value = integer_values_[a_index];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value (double & a_value, int a_index) const
    {
      if (! is_real ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      a_value = real_values_[a_index];
      return ERROR_SUCCESS;
    }

    int properties::data::get_value (std::string & a_value, int a_index) const
    {
      if (! is_string ()) 
	{
	  return ERROR_BADTYPE;
	}
      if (! index_is_valid (a_index)) 
	{
	  return ERROR_RANGE;
	} 
      a_value = string_values_[a_index];
      return ERROR_SUCCESS;
    }

    /******************************************************/

    bool properties::data::get_boolean_value (int a_index) const
    {
      bool value;
      if (get_value (value, a_index) != ERROR_SUCCESS) 
	{
	  throw std::runtime_error ("properties::data::get_boolean_value: type is not boolean!");   
	}
      return value;
    }

    int properties::data::get_integer_value (int a_index) const
    {
      int value;
      if (get_value (value, a_index) != ERROR_SUCCESS) {
	throw std::runtime_error ("properties::data::get_integer_value: type is not integer!");    }
      return value;
    }
  
    double properties::data::get_real_value (int a_index) const
    {
      double value;
      if (get_value (value, a_index) != ERROR_SUCCESS) {
	throw std::runtime_error ("properties::data::get_real_value: type is not double!");
      }
      return value;
    }

    std::string properties::data::get_string_value (int a_index) const
    {
      std::string value;
      if (get_value (value, a_index) != ERROR_SUCCESS) {
	throw std::runtime_error ("properties::data::get_string_value: type is not string!");
      }
      return value;
    }

    std::string properties::data::get_type_label () const
    {
      if (is_boolean ()) return "boolean";
      if (is_integer ()) return "integer";
      if (is_real ()   ) return "real";     
      if (is_string ()   ) return "string";
      return "none";
    }

    std::string properties::data::get_vector_label () const
    {
      if (is_scalar ()) return "scalar";
      return "vector";
    }
    
    void properties::data::dump (std::ostream & a_out) const
    {
      tree_dump (a_out, "datatools::utils::properties::data");
      return;
    }

    void properties::data::to_string (std::ostream & a_out) const
    {
      a_out << '[';
      a_out << get_type_label () << ':'
	    << get_vector_label () << ":"
	    << get_size () << ':';
      for (int i = 0; i < (int) get_size (); i++)
	{
	  if (i != 0) a_out << ' ';
	  if (is_boolean ()) a_out << get_boolean_value (i);
	  if (is_integer ()) a_out << get_integer_value (i);
	  if (is_real ())    a_out << get_real_value (i);
	  if (is_string ())  a_out << get_string_value (i);
	}
      a_out << ':'
	    << get_description (); 
      a_out << ']';
      return;	
    }
    
    void properties::data::tree_dump (std::ostream & a_out, 
				      const std::string & a_title, 
				      const std::string & a_indent, 
				      bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ()) 
	{
	  a_out << indent << a_title << std::endl;
	}

      if (! description_.empty ()) 
	{
	  a_out << indent << du::i_tree_dumpable::tag 
		<< "Description  : " <<  get_description () << std::endl;
	}

      a_out << indent << du::i_tree_dumpable::tag 
	    << "Type  : " ;
      if (is_locked ()) a_out << "const ";
      a_out <<  get_type_label ();
      if (is_vector ()) 
	{
	  a_out << '[' << size () << ']' << " (vector)";
	}
      else 
	{
	  a_out << " (scalar)";
	}
      a_out << std::endl;

      /*
	a_out << indent << du::i_tree_dumpable::tag 
	<< "Lock  : " <<  (is_locked ()?"locked":"unlocked") << std::endl;
      */
      if (get_size () > 0)
	{
	  for (int i = 0; i < (int) get_size (); i++) 
	    {
	      a_out << indent;
	      if (i == (int) (get_size () - 1)) a_out << du::i_tree_dumpable::inherit_tag (a_inherit);
	      else a_out << du::i_tree_dumpable::tag;
	      a_out << "Value";
	      if (is_vector ()) 
		{
		  a_out << "[" << i << "]";
		}
	      a_out << " : ";
	      if (is_boolean ()) a_out << std::dec << get_boolean_value (i) << std::endl;
	      if (is_integer ()) a_out << std::dec << get_integer_value (i) << std::endl;
	      a_out.precision (16);
	      if (is_real ())   a_out << get_real_value (i) << std::endl;
	      if (is_string ())  a_out << '"' <<  get_string_value (i) << '"' << std::endl;
	    }
	}
      else
	{
	  a_out << indent;
	  a_out << du::i_tree_dumpable::inherit_tag (a_inherit);
	  a_out << "<no value>" << std::endl;
	}
      return;	
    }
    
    /**********************************************/
    const std::string properties::default_key_validator::ALLOWED_CHARS = 
		   "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";

    properties::default_key_validator::default_key_validator ()
    {      
      return;	
    }

    properties::default_key_validator:: ~default_key_validator ()
    {
      return;	
    }

    bool properties::default_key_validator::operator () (const std::string & a_key_arg) const
    {
      if (a_key_arg.empty ()) return false;
      if (a_key_arg.find_first_not_of (ALLOWED_CHARS) != a_key_arg.npos) return false;
      if (a_key_arg.find_first_of ("0123456789.") == 0) return false;
      if (a_key_arg[a_key_arg.size ()-1] == '.') return false;
      return true;
    }

    /**********************************************/
    const std::string properties::PRIVATE_PROPERTY_PREFIX = "__";
    
    void properties::validate_key_ (const std::string & a_key_arg) const
    {
      if (key_validator_ != 0) 
	{
	  //if (! (*key_validator_) (a_key_arg)) {
	  if (! key_validator_->operator () (a_key_arg)) 
	    {
	      std::ostringstream message;
	      message << "properties::validate_key_: After key validator, the '" 
		      << a_key_arg << "' key is not valid!";
	      throw std::runtime_error (message.str ());
	    }
	}
      return;	
    }

    void properties::set_description (const std::string & a_description)
    {
      description_ = a_description;
      return;	
    }

    const std::string & properties::get_description () const
    {
      return description_;
    }

    bool properties::is_debug () const
    {
      return debug_;
    }
  
    void properties::set_debug (bool a_new_value)
    {
      debug_ = a_new_value;
      return;	
    }

    int32_t properties::size () const
    {
      return props_.size ();
    }

    void properties::set_key_validator (const basic_key_validator & a_key_validator)
    {
      clear_key_validator_ ();
      key_validator_ = &a_key_validator;
      key_validator_deletion_ = false;
      return;	
    }
    
    void properties::set_key_validator (const basic_key_validator * a_key_validator, 
					bool  a_deletion_on_destroy)
    {
      if (key_validator_ != 0 && key_validator_== a_key_validator) 
	{
	  return;
	}
      clear_key_validator_ ();
      if (a_key_validator != 0) 
	{
	  key_validator_=a_key_validator;
	  key_validator_deletion_ = a_deletion_on_destroy;
	}
      return;	
    }

    void properties::set_default_key_validator ()
    {
      clear_key_validator_ ();
      key_validator_ = &g_default_key_validator;// new default_key_validator;
      key_validator_deletion_ = false; //true;
      return;	
    }

    void properties::unset_key_validator ()
    {
      clear_key_validator_ ();
      return;	
    }
  
    // ctor:
    properties::properties () 
      : debug_ (false), key_validator_ (0)
    {
      set_description ("");
      set_default_key_validator ();
      return;	
    }

    // ctor:
    properties::properties (const std::string & a_description) 
      : debug_ (false), key_validator_ (0)
    {
      set_description (a_description);
      set_default_key_validator ();
      return;	
    }

    // ctor:
    properties::properties (const std::string & a_description, 
			    const basic_key_validator & a_key_validator) 
      : debug_ (false), key_validator_ (0)
    {
      set_description (a_description);
      set_key_validator (a_key_validator);
      return;	
    }

    // ctor:
    properties::properties (const basic_key_validator & a_key_validator) 
      : debug_ (false), key_validator_ (0)
    {
      set_description ("");
      set_key_validator (a_key_validator);
      return;	
    }
  
    // ctor:
    properties::properties (const std::string & a_description, 
			    const basic_key_validator * a_key_validator, 
			    bool a_deletion_on_destroy) 
      : debug_ (false), key_validator_ (0)
    {
      set_description (a_description);
      set_key_validator (a_key_validator, a_deletion_on_destroy);
      return;	
    }

    properties::properties (const basic_key_validator * a_key_validator, 
			    bool a_deletion_on_destroy) 
      : debug_ (false), key_validator_ (0)
    {
      set_description ("");
      set_key_validator (a_key_validator, a_deletion_on_destroy);
      return;	
    }

    DATATOOLS_CLONEABLE_IMPLEMENTATION(properties)
 
    void properties::clear_key_validator_ ()
    {
      if (key_validator_ != 0) 
	{
	  if (key_validator_deletion_) 
	    {
	      delete key_validator_;
	    }
	  key_validator_ = 0;
	}
      return;	
    }
  
    // dtor:
    properties::~properties ()
    {
      properties::clear ();
      return;	
    }

    void properties::erase (const std::string & a_key)
    {
      pmap::iterator found = props_.find (a_key);
      if (found == props_.end ())
	{
	  std::ostringstream message;
	  message << "properties::erase: key '" << a_key << "' not known!";
	  throw std::runtime_error (message.str ());	  
	}
      props_.erase (found);
      return;	
    }

    void properties::erase_all_starting_with (const std::string & a_key_prefix)
    {
      keys_col_t keys;
      keys_starting_with (keys, a_key_prefix);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::erase_all_not_starting_with (const std::string & a_key_prefix)
    {
      keys_col_t keys;
      keys_not_starting_with (keys, a_key_prefix);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::export_and_rename_starting_with (properties & a_props, 
						      const std::string & a_key_prefix, 
						      const std::string & a_new_prefix) const
    {
      if (this == &a_props)
	{
	  throw runtime_error ("properties::export_and_rename_starting_with: Self export is not allowed !");
	}
      keys_col_t ks;
      this->keys_starting_with (ks, a_key_prefix);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  /*
	    clog << "DEVEL: properties::export_starting_with: property '" 
            << *i << "'..." << endl;
	  */
	  string new_key = *i;
	  boost::replace_first (new_key, a_key_prefix, a_new_prefix); 
	  a_props.props_[new_key] = ptmp.props_[*i];
	}
      return;	
    }

    void properties::export_starting_with (properties & a_props, 
					   const std::string & a_key_prefix) const
    {
      if (this == &a_props)
	{
	  throw runtime_error ("properties::export_starting_with: Self export is not allowed !");
	}
      keys_col_t ks;
      this->keys_starting_with (ks, a_key_prefix);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  /*
	    clog << "DEVEL: properties::export_starting_with: property '" 
            << *i << "'..." << endl;
	  */
	  a_props.props_[*i] = ptmp.props_[*i];
	}
      return;	
    }

    void properties::export_not_starting_with (properties & a_props, 
					       const std::string & a_key_prefix) const
    {
      if (this == &a_props)
	{
	  throw runtime_error ("properties::export_starting_with: Self export is not allowed !");
	}
      keys_col_t ks;
      keys_not_starting_with (ks, a_key_prefix);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  a_props.props_[*i] = ptmp.props_[*i];
	}
      return;	
    }

    void properties::erase_all_ending_with (const std::string & a_key_suffix)
    {
      keys_col_t keys;
      keys_ending_with (keys, a_key_suffix);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::erase_all_not_ending_with (const std::string & a_key_suffix)
    {
      keys_col_t keys;
      keys_not_ending_with (keys, a_key_suffix);
      for (keys_col_t::const_iterator i = keys.begin ();
	   i != keys.end ();
	   i++)
	{
	  erase (*i);
	}
      return;	
    }

    void properties::export_ending_with (properties & a_props, 
					 const std::string & a_key_suffix) const
    {
      if (this == &a_props)
	{
	  throw runtime_error ("properties::export_ending_with: Self export is not allowed !");
	}
      keys_col_t ks;
      this->keys_ending_with (ks, a_key_suffix);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  /*
	    clog << "DEVEL: properties::export_ending_with: property '" 
            << *i << "'..." << endl;
	  */
	  a_props.props_[*i] = ptmp.props_[*i];
	}
      return;	
    }

    void properties::export_not_ending_with (properties & a_props, 
					     const std::string & a_key_suffix) const
    {
      if (this == &a_props)
	{
	  throw runtime_error ("properties::export_ending_with: Self export is not allowed !");
	}
      keys_col_t ks;
      keys_not_ending_with (ks, a_key_suffix);
      for (keys_col_t::const_iterator i = ks.begin ();
	   i !=  ks.end ();
	   i++)
	{
	  properties & ptmp = const_cast<properties &> (*this);
	  a_props.props_[*i] = ptmp.props_[*i];
	}
      return;	
    }

    void properties::erase_all ()
    {
      props_.clear ();
      return;	
    }

    void properties::clean (const std::string & a_key)
    { 
      pmap::iterator found = props_.find (a_key);
      if (found != props_.end ())
	{
	  props_.erase (found);
	}
      return;	
    }

    void properties::clear ()
    {
      reset ();
      return;	
    }

    void properties::reset ()
    {
      set_description ("");
      props_.clear ();
      clear_key_validator_ ();
      debug_ = false;
      return;	
    }

    void properties::keys_not_starting_with (properties::vkeys & a_keys, 
					     const std::string & a_key_prefix) const
    {
      if (a_key_prefix.empty ())
 	{
	  throw runtime_error("properties::keys_not_starting_with: Empty key prefix argument !");
	}
      size_t n = a_key_prefix.size ();
      for (pmap::const_iterator iter = props_.begin (); 
	   iter != props_.end (); 
	   iter++) 
	{
	  bool push = true;
	  if (iter->first.substr (0, n) == a_key_prefix)
	    {
	      push = false;
	    }
	  if (push)
	    {
	      a_keys.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys properties::keys_not_starting_with (const std::string & a_key_prefix) const
    {
      properties::vkeys lkeys;
      keys_not_starting_with (lkeys, a_key_prefix);
      return lkeys;
    }

    void properties::keys_starting_with (properties::vkeys & a_keys, 
					 const std::string & a_key_prefix) const
    {
      if (a_key_prefix.empty ())
	{
	  throw runtime_error("properties::keys_starting_with: Empty key prefix argument !");
	}
      size_t n = a_key_prefix.size ();
      for (pmap::const_iterator iter = props_.begin (); 
	   iter != props_.end (); 
	   iter++) 
	{
	  if (iter->first.size () < n) continue;
	  if (iter->first.substr (0, n) == a_key_prefix)
	    {
	      a_keys.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys properties::keys_starting_with (const std::string & a_key_prefix) const
    {
      properties::vkeys lkeys;
      keys_starting_with (lkeys, a_key_prefix);
      return lkeys;
    }

    void properties::keys_not_ending_with (properties::vkeys & a_keys, 
					   const std::string & a_key_suffix) const
    {
      if (a_key_suffix.empty ())
 	{
	  throw runtime_error("properties::keys_not_ending_with: Empty key suffix argument !");
	}
      size_t n = a_key_suffix.size ();
      for (pmap::const_iterator iter = props_.begin (); 
	   iter != props_.end (); 
	   iter++) 
	{
	  bool push = true;
	  if (iter->first.substr
              (iter->first.size () - n, iter->first.size ())
              == a_key_suffix)
	    {
	      push = false;
	    }
	  if (push)
	    {
	      a_keys.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys properties::keys_not_ending_with (const std::string & a_key_suffix) const
    {
      properties::vkeys lkeys;
      keys_not_ending_with (lkeys, a_key_suffix);
      return lkeys;
    }

    void properties::keys_ending_with (properties::vkeys & a_keys, 
				       const std::string & a_key_suffix) const
    {
      if (a_key_suffix.empty ())
	{
	  throw runtime_error("properties::keys_ending_with: Empty key suffix argument !");
	}
      size_t n = a_key_suffix.size ();
      for (pmap::const_iterator iter = props_.begin (); 
	   iter != props_.end (); 
	   iter++) 
	{
	  if (iter->first.size () < n) continue;
	  if (iter->first.substr
              (iter->first.size ()-n, iter->first.size ())
              == a_key_suffix)
	    {
	      a_keys.push_back (iter->first);
	    }
	}
      return;
    }

    properties::vkeys properties::keys_ending_with (const std::string & a_key_suffix) const
    {
      properties::vkeys lkeys;
      keys_ending_with (lkeys, a_key_suffix);
      return lkeys;
    }

    properties::vkeys properties::keys () const
    {
      properties::vkeys lkeys;
      keys (lkeys);
      return lkeys;
    }
    
    void properties::keys (properties::vkeys & a_keys) const
    {
      for (pmap::const_iterator iter = props_.begin (); 
	   iter != props_.end (); 
	   iter++) 
	{
	  a_keys.push_back (iter->first);
	}
      return;
    }
    
    void properties::lock (const std::string & a_key)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      data_ptr->lock ();
      return;
    }
    
    void properties::unlock (const std::string & a_key)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      data_ptr->unlock ();
      return;	
    }
    
    bool properties::is_locked (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_locked ();
    }

    std::string properties::make_private_key (const std::string & a_key)
    {
      if (key_is_private (a_key)) 
	{
	  return a_key;
	}
      std::ostringstream oss;
      oss << PRIVATE_PROPERTY_PREFIX << a_key;
      return oss.str ();
    }
    
    bool properties::key_is_private (const std::string & a_key)
    {
      if (a_key.size () < 2) return false;
      return a_key.substr (0, PRIVATE_PROPERTY_PREFIX.size ()) == PRIVATE_PROPERTY_PREFIX;
    }

    bool properties::key_is_public (const std::string & a_key)
    {
      return ! key_is_private (a_key);
    }

    bool properties::is_private (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return key_is_private (a_key);
    }

    bool properties::is_public (const std::string & a_key) const
    {
      return ! is_private (a_key);
    }
    
    bool properties::is_boolean (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_boolean ();
    }
    
    bool properties::is_integer (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_integer ();
    }
    
    bool properties::is_real (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_real ();
    }
    
    bool properties::is_string (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_string ();
    }
    
    bool properties::is_scalar (const std::string & a_key) const
    { 
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_scalar ();
    }
    
    bool properties::is_vector (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->is_vector ();
    }
     
    int32_t properties::size (const std::string & a_key) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      return data_ptr->size ();
    }

    bool properties::has_key (const std::string & a_key) const
    {
      if (props_.size () == 0) return false;
      bool hk = props_.find (a_key) != props_.end ();
      return hk;
    }
  
    void properties::check_nokey_ (const std::string & a_key) const
    {
      //std::clog << "DEVEL: properties::check_nokey_: key='" << a_key << "'" << std::endl;
      if (has_key (a_key)) 
	{
	  std::ostringstream message;
	  message << "properties::check_nokey_: key '" << a_key << "' already used";
	  if (! get_description ().empty ())
	    {
	      message << " (" << get_description () << ")";
	    }
	  message << " !";
	  throw std::runtime_error (message.str ());
	}
      return;
    }
  
    void properties::check_key_ (const std::string & a_key, 
				  data ** a_data)
    {
      pmap::iterator iter = props_.find (a_key);
      if (props_.find (a_key) == props_.end ()) 
	{
	  std::ostringstream message;
	  message << "properties::check_key_: key '" << a_key << "' does not exist!";
	  throw std::runtime_error (message.str ());
	}
      *a_data = & (iter->second);
      return;
    }
  
    void properties::check_key_ (const std::string & a_key, 
				  const data ** a_data) const
    {
      pmap::const_iterator iter = props_.find (a_key);
      if (props_.find (a_key) == props_.end ()) 
	{
	  std::ostringstream message;
	  message << "properties::check_key_: key '" << a_key << "' does not exist!";
	  throw std::runtime_error (message.str ());
	}
      *a_data = & (iter->second);
      return;
    }

    void properties::store_flag (const std::string & a_key, 
                                 const std::string & a_description, 
                                 bool a_lock)
    {
      store (a_key, true, a_description, a_lock);
      return;
    }
  
    void properties::store (const std::string & a_key, 
			    bool a_value, 
			    const std::string & a_description, 
			    bool a_lock) 
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      data a_data (a_value, data::SCALAR_DEF);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      if (a_lock) props_[a_key].lock ();
      return;
    }

    void properties::store (const std::string & a_key, 
			    int a_value, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      data a_data (a_value);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      if (a_lock) props_[a_key].lock ();
      return;
    }

    void properties::store (const std::string & a_key, 
			    double a_value, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      data a_data (a_value);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      if (a_lock) props_[a_key].lock ();
      return;
    }

    void properties::store (const std::string & a_key, 
			    const std::string & a_value, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      data a_data (a_value);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      if (a_lock) props_[a_key].lock ();
      return;
    }

    void properties::store (const std::string & a_key, 
			    const char * a_value, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      properties::store (a_key, std::string (a_value), a_description, a_lock);
      return;
    }

    void properties::store (const std::string & a_key, 
			    const data::vbool & a_values, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      int size = a_values.size ();
      //       if (size < 0) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of booleans size!");
      //       }
      data a_data (data::TYPE_BOOLEAN_SYMBOL, size);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  props_[a_key].set_value (a_values[i], i);
	}
      if (a_lock) props_[a_key].lock ();
      return;
    }
  
    void properties::store (const std::string & a_key, 
			    const data::vint & a_values, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      int size = a_values.size ();
      //       if (size < 0) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of integers size!");
      //       }
      data a_data (data::TYPE_INTEGER_SYMBOL, size);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  props_[a_key].set_value (a_values[i], i);
	}
      if (a_lock) props_[a_key].lock ();
      return;
    }
  
    void properties::store (const std::string & a_key, 
			    const data::vdouble & a_values, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      int size = a_values.size ();
      //       if (size < 0) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of reals size!");
      //       }
      data a_data (data::TYPE_REAL_SYMBOL, size);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  props_[a_key].set_value (a_values[i], i);
	}
      if (a_lock) props_[a_key].lock ();
      return;
    }
  
    void properties::store (const std::string & a_key, 
			    const data::vstring & a_values, 
			    const std::string & a_description, 
			    bool a_lock)
    {
      check_nokey_ (a_key);
      validate_key_ (a_key);
      int size = a_values.size ();
      //       if (size < 0) {
      // 	throw std::runtime_error ("properties::store: Invalid vector of string size!");
      //       }
      data a_data (data::TYPE_STRING_SYMBOL, size);
      a_data.set_description (a_description);
      props_[a_key] = a_data;
      for (int i = 0; i < size; i++) 
	{
	  props_[a_key].set_value (a_values[i], i);
	}
      if (a_lock) props_[a_key].lock ();
      return;
    }

      void properties::set_flag (const std::string & a_key)
      {
	store_flag (a_key, "", false);
	return;
      }

      void properties::unset_flag (const std::string & a_key)
      {
	clean (a_key);
	return;
      }

       void properties::store_boolean (const std::string & a_key, 
			  bool a_value, 
			  const std::string & a_desc,
			  bool a_lock)
      {
	store (a_key, a_value, a_desc, a_lock); 
	return;
      }
     
      void properties::store_integer (const std::string & a_key, 
			  int a_value, 
			  const std::string & a_desc,
			  bool a_lock)
      {
	store (a_key, a_value, a_desc, a_lock); 
	return;
      }
  
      void properties::store_real (const std::string & a_key, 
		       double a_value, 
		       const std::string & a_desc,
		       bool a_lock)
      {
	store (a_key, a_value, a_desc, a_lock); 
	return;
      }

      void properties::store_string (const std::string & a_key, 
			 const std::string & a_value, 
			 const std::string & a_desc,
			 bool a_lock)
      {
	store (a_key, a_value, a_desc, a_lock); 
	return;
      }

    /**********************************/

      void properties::change_boolean (const std::string & a_key , 
			   bool a_value , 
			   int a_index)
      {
	change (a_key, a_value, a_index);
	return;
      }

     void properties::change_integer (const std::string & a_key , 
			   int a_value , 
			   int a_index )
      {
	change (a_key, a_value, a_index);
	return;
      }
      
      void properties::change_real (const std::string & a_key , 
			double a_value , 
			int a_index )
      {
	change (a_key, a_value, a_index);
	return;
      }
 
    void properties::change_string (const std::string & a_key , 
			  const std::string & a_value , 
				    int a_index )
    {
      change (a_key, a_value, a_index);
    }
    
    void properties::change (const std::string & a_key, 
			     bool  a_value, 
			     int a_index)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->set_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: Cannot change value for property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
      return;
    }

    void properties::change (const std::string & a_key, 
			     int a_value, 
			     int a_index)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->set_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: Cannot change value for property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
      return;
    }

    void properties::change (const std::string & a_key, 
			     double a_value, 
			     int a_index)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->set_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: Cannot change value for property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
      return;
    }

    void properties::change (const std::string & a_key, 
			     const std::string & a_value, 
			     int a_index)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->set_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::change: Cannot change value for property '" 
		  << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}
      return;
    }

    void properties::change (const std::string & a_key, 
			     const char * a_value, 
			     int a_index)
    {
      std::string value = "";
      if (a_value != 0) value = a_value;
      change (a_key, value, a_index);
      return;
    }
  
    void properties::change (const std::string & a_key, 
			     const data::vbool & a_values)
    {
      data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_boolean () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << a_key << "' is not a vector of booleans!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) a_values.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->boolean (a_values.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  int error = data_ptr->set_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      return;
    }
  
    void properties::change (const std::string & a_key, 
			     const data::vint & a_values)
    {
      data * data_ptr=0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_integer () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << a_key << "' is not a vector of integers!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) a_values.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->integer (a_values.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int)  a_values.size (); i++) 
	{
	  int error = data_ptr->set_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      return;
    }
  
    void properties::change (const std::string & a_key, 
			     const data::vdouble & a_values)
    {
      data * data_ptr=0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_real () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << a_key << "' is not a vector of reals!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) a_values.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->real (a_values.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  int error = data_ptr->set_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      return;
    }
  
    void properties::change (const std::string & a_key, 
			     const data::vstring & a_values)
    {
      data * data_ptr=0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_string () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::change: property '" 
		  << a_key << "' is not a vector of strings!";
	  throw std::runtime_error (message.str ());
	}
      if ((int) a_values.size () != data_ptr->get_size ()) 
	{
	  int error = data_ptr->string (a_values.size ());
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  int error = data_ptr->set_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::change: Cannot change values for vector property '" << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	}
      return;
    }

    /********************/
    
    void properties::dump_def () const
    {
      dump (std::cout);
      return;
    }

    void properties::dump_stderr () const
    {
      dump (std::cerr);
	return;
    }
    
    void properties::dump_stdout () const
    {
      dump (std::cout);
      return;
    }

    void properties::update_string (const std::string & a_key, 
				    const std::string & a_value)
    {
      update (a_key, a_value);
      return;
    }

    void properties::update_boolean (const std::string & a_key, 
				     bool a_value)
    {
      update (a_key, a_value);
      return;
    }
 
    int properties::fetch_integer_scalar (const std::string & a_name) const
    {
      return fetch_integer (a_name, 0);
    }

    int properties::fetch_integer_vector (const std::string & a_name , int a_index) const
    {
      return fetch_integer (a_name, a_index);
    }
     
    bool properties::fetch_boolean_scalar (const std::string & a_name) const
    {
      return fetch_boolean (a_name, 0);
    }

    bool properties::fetch_boolean_vector (const std::string & a_name , int a_index) const
    {
      return fetch_boolean (a_name, a_index);
    }
    
    double properties::fetch_real_scalar (const std::string & a_name) const
    {
      return fetch_real (a_name, 0);
    }
    
    double properties::fetch_real_vector (const std::string & a_name , int a_index) const
    {
      return fetch_real (a_name, a_index);
    }

    void properties::update_flag (const std::string & a_key)
    {
      update (a_key, true);
      return;
    }

    std::string properties::fetch_string_scalar (const std::string & a_name) const
    {
      return fetch_string (a_name, 0);
    }
    
    std::string properties::fetch_string_vector (const std::string & a_name , int a_index) const
    {
      return fetch_string (a_name, a_index);
    }

    void properties::update (const std::string & a_key, 
			     bool a_value)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_value);
	}
      else
	{
	  store (a_key, a_value);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     int a_value)
    {
      if (has_key (a_key))
	{
	  //	  std::cerr << "DEVEL: properties::update: change : " << a_key << " to " << a_value << std::endl;
	  change (a_key, a_value);
	}
      else
	{
	  //	  std::cerr << "DEVEL: properties::update: store : " << a_key << " at " << a_value << std::endl;
	  store (a_key, a_value);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     double a_value)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_value);
	}
      else
	{
	  store (a_key, a_value);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     const std::string & a_value)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_value);
	}
      else
	{
	  store (a_key, a_value);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     const data::vbool & a_values)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_values);
	}
      else
	{
	  store (a_key, a_values);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     const data::vint & a_values)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_values);
	}
      else
	{
	  store (a_key, a_values);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     const data::vdouble & a_values)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_values);
	}
      else
	{
	  store (a_key, a_values);
	}
      return;
    }

    void properties::update (const std::string & a_key, 
			     const data::vstring & a_values)
    {
      if (has_key (a_key))
	{
	  change (a_key, a_values);
	}
      else
	{
	  store (a_key, a_values);
	}
      return;
    }

    void properties::update_integer (const std::string & a_key , 
				     int a_value)
    {
      update (a_key, a_value);
      return;
    }
    
    void properties::update_real (const std::string & a_key, 
				   double a_value)
    {
      update (a_key, a_value);
      return;
    }
    
    /********************/
 
    void properties::fetch (const std::string & a_key, 
			    bool & a_value, int a_index) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->get_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: Cannot fetch boolean value from property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
      return;
    }

    void properties::fetch (const std::string & a_key, 
			    int & a_value, int a_index) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->get_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: Cannot fetch integer value from property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
      return;
    }

    void properties::fetch (const std::string & a_key, 
			    double & a_value, int a_index) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->get_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: Cannot fetch real value from property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
      return;
    }

    void properties::fetch (const std::string & a_key, 
			    std::string & a_value, int a_index) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      int error = data_ptr->get_value (a_value, a_index);
      if (error != data::ERROR_SUCCESS) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: Cannot fetch string value from property '" << a_key << "': "
		  << data::get_error_message (error) << "!";
	  throw std::runtime_error (message.str ());
	}    
      return;
    }
    
    bool properties::has_flag (const std::string & a_key) const
    {
      bool flag_is_on = false;
      try
	{
	  flag_is_on = fetch_boolean (a_key, 0);
	}
      catch (std::runtime_error & e)
	{
	}
      return flag_is_on;
    }
   
    bool properties::fetch_boolean (const std::string & a_key, 
				    int a_index) const
    {
      bool value;
      fetch (a_key, value, a_index);
      return value;
    }

    int properties::fetch_integer (const std::string & a_key, 
				   int a_index) const
    {
      int value;
      fetch (a_key, value, a_index);
      return value;
    }

    double properties::fetch_real (const std::string & a_key, 
				   int a_index) const
    {
      double value;
      fetch (a_key, value, a_index);
      return value;
    }

    std::string properties::fetch_string (const std::string & a_key, 
					  int a_index) const
    {
      std::string value;
      fetch (a_key, value, a_index);
      return value;
    }

    void properties::fetch (const std::string & a_key, 
			    data::vbool & a_values) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_boolean () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << a_key << "' is not a vector of booleans!";
	  throw std::runtime_error (message.str ());
	}
      a_values.resize (data_ptr->size ());
      a_values.assign (data_ptr->size (), data::DEFAULT_VALUE_BOOLEAN);
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  bool val;
	  int error = data_ptr->get_value (val, i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: Cannot fetch boolean values for vector property '" 
		      << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  a_values[i] = val;
	}
      return;
    }
  
    void properties::fetch (const std::string & a_key, 
			    data::vint & a_values) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_integer () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << a_key << "' is not a vector of integers!";
	  throw std::runtime_error (message.str ());
	}
      a_values.resize (data_ptr->size ());
      a_values.assign (data_ptr->size (), data::DEFAULT_VALUE_INTEGER);
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  //int val;
	  //int error = data_ptr->get_value (val, i);
	  int error = data_ptr->get_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: Cannot fetch integer values for vector property '" 
		      << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //a_values[i]=val;
	}
      return;
    }
  
    void properties::fetch (const std::string & a_key, 
			    data::vdouble & a_values) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_real () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << a_key << "' is not a vector of reals!";
	  throw std::runtime_error (message.str ());
	}
      a_values.resize (data_ptr->size ());
      a_values.assign (data_ptr->size (), data::DEFAULT_VALUE_REAL);
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  //double val;
	  //int error = data_ptr->get_value (val, i);
	  int error = data_ptr->get_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: Cannot fetch real values for vector property '" 
		      << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //a_values[i]=val;
	}
      return;
    }
  
    void properties::fetch (const std::string & a_key, 
			    data::vstring & a_values) const
    {
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      if (! data_ptr->is_string () || ! data_ptr->is_vector ()) 
	{
	  std::ostringstream message;
	  message << "properties::fetch: property '" 
		  << a_key << "' is not a vector of strings!";
	  throw std::runtime_error (message.str ());
	}
      a_values.resize (data_ptr->size ());
      a_values.assign (data_ptr->size (), data::DEFAULT_VALUE_STRING);
      for (int i = 0; i < (int) a_values.size (); i++) 
	{
	  //std::string val;
	  //int error = data_ptr->get_value (val, i);
	  int error = data_ptr->get_value (a_values[i], i);
	  if (error != data::ERROR_SUCCESS) 
	    {
	      std::ostringstream message;
	      message << "properties::fetch: Cannot fetch string values for vector property '" 
		      << a_key << "': "
		      << data::get_error_message (error) << "!";
	      throw std::runtime_error (message.str ());
	    }    
	  //a_values[i]=val;
	}
      return;
    }


    /****************************/

    std::string properties::key_to_string (const std::string & a_key) const
    {
      if (! has_key (a_key)) return "";
      std::ostringstream oss;
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      data_ptr->to_string (oss);
      return oss.str ();
    }

    std::string properties::key_to_property_string (const std::string & a_key) const
    {
      if (! has_key (a_key)) return "";
      std::ostringstream oss;
      const data * data_ptr = 0;
      check_key_ (a_key, &data_ptr);
      oss << a_key << '=';
      data_ptr->to_string (oss);
      return oss.str ();
    }

    void properties::tree_dump (std::ostream & a_out, 
				const std::string & a_title, 
				const std::string & a_indent, 
				bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ()) 
	{
	  a_out << indent << a_title << std::endl;
	}
      /*
        a_out << indent << du::i_tree_dumpable::tag 
        << "Debug : " <<  debug_ << std::endl;
      */
      if (! description_.empty ()) 
	{
	  a_out << indent << du::i_tree_dumpable::tag 
		<< "Description  : " <<  get_description () << std::endl;
	}
      if (props_.size () == 0) 
	{
	  a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
		<< "<no property>" << std::endl;
	}
      else 
	{
	  for (pmap::const_iterator i = props_.begin ();
	       i != props_.end () ;
	       i++) 
	    {
	      const std::string & key = i->first;
	      const du::properties::data & a_data = i->second;
	      a_out << indent;
	      std::ostringstream indent_oss;
	      indent_oss << indent;
	      pmap::const_iterator j = i; 
	      j++;
	      if (j == props_.end ()) 
		{
		  a_out << du::i_tree_dumpable::inherit_tag (a_inherit);
		  indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
		}
	      else 
		{
		  a_out << du::i_tree_dumpable::tag;
		  indent_oss << du::i_tree_dumpable::skip_tag;
		}
	      a_out << "Name : " << '"' << key << '"' << std::endl;
	      a_data.tree_dump (a_out, "", indent_oss.str ());
	    }
	}
      return;
    }

    void properties::dump (std::ostream & a_out) const
    {
      tree_dump (a_out, "datatools::utils::properties");
      return;
    }

    void properties::write_config (const std::string & a_filename, 
				   const properties & a_props, 
				   bool a_use_smart_modulo, 
				   bool a_write_public_only)
    {
      std::ostream * out = &std::cout;
      std::ofstream fout;
      if (! a_filename.empty ()) 
	{
	  fout.open (a_filename.c_str ());
	  if (! fout) 
	    {
	      std::ostringstream message;
	      message << "properties::write_config: "
		      << "Cannot open file '"
		      << a_filename << "!"
		      << std::endl;
	      throw std::runtime_error (message.str ());
	    }
	  out = &fout;
	}
      config a_cfg (a_use_smart_modulo, 
		    config::mode_header_footer, 
		    (a_write_public_only?
		     config::write_public_only:
		     config::write_private_also));
      a_cfg.write (*out, a_props);
      return;
    }

    void properties::read_config (const std::string & a_filename, 
				  properties & a_props)
    {
      std::ifstream fin (a_filename.c_str ());
      if (! fin) 
	{
	  std::ostringstream message;
	  message << "properties::read_config: "
		  << "Cannot open file '"
		  << a_filename << "!"
		  << std::endl;
	  throw std::runtime_error (message.str ());
	}
      config a_cfg;
      a_props.clear ();
      a_cfg.read (fin, a_props);
      return;
    }

    /**********************************************************************/

    const bool properties::config::write_public_only = true;
    const bool properties::config::write_private_also = false;

    void properties::config::write (std::ostream & a_out, 
				    const properties & a_props)
    {
      int mode = mode_;
      /*
        clog << "DEVEL: properties::config::write: mode=" 
        << mode << endl; 
      */
      if (mode == MODE_HEADER_FOOTER) 
	{
	  a_out << "# List of properties (datatools::utils::properties):" << std::endl;
	  /*
            a_out << "# This file was autogenerated by "
            << "'datatools::utils::properties'." << std::endl;
            a_out << "# Do not modify it unless you know what you are doing!" 
            << std::endl;
	  */
	  a_out << std::endl;
	}

      if (! a_props.get_description ().empty ()) 
	{
	  a_out << "#@config " << a_props.get_description () << std::endl;
	  a_out << std::endl;
	}
 
      for (pmap::const_iterator i = a_props.props_.begin ();
	   i != a_props.props_.end ();
	   i++) 
	{
	  const std::string        a_key  = i->first;
	  const properties::data & a_data = i->second;

	  if (write_public_only_)
	    {
	      if (key_is_private (a_key)) continue;
	    }

	  if (! a_data.get_description ().empty ()) 
	    {
	      a_out << "#@description " << a_data.get_description () << std::endl;
	    }

	  // name:
	  a_out << a_key << " : ";

	  if (a_data.is_locked ()) 
	    {
	      a_out << "const ";
	    }

	  // type:
	  int size = properties::data::SCALAR_SIZE;
	  a_out << a_data.get_type_label ();
	  if (a_data.is_vector ()) 
	    {
	      size=a_data.get_size ();
	      a_out << '[' << size << ']';
	    } 
	  a_out << ' ';

	  a_out << "=";

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

	  if (use_smart_modulo_) 
	    {
	      if ((size>1) && (size>modulo)) 
		{
		  a_out << ' ' << continuation_char_ << '\n';
		}
	    }

	  for (int i = 0; i < size; i++) 
	    { 

	      //if ((i+1) != size) 
	      a_out << ' ';  
	    
	      if (a_data.is_boolean ()) 
		{ 
		  a_out << a_data.get_boolean_value (i);
		}

	      if (a_data.is_integer ()) 
		{
		  a_out << a_data.get_integer_value (i);
		}

	      if (a_data.is_real ()) 
		{
		  int oldprec = a_out.precision ();
		  a_out.precision (16);
		  a_out << a_data.get_real_value (i);
		  a_out.precision (oldprec);
		}

	      if (a_data.is_string ()) 
		{
		  a_out << '"' << a_data.get_string_value (i) << '"';
		}

	      if (use_smart_modulo_) 
		{
		  if ((i < (size - 1)) && ((i + 1) % modulo) == 0) 
		    {
		      a_out << ' ' << continuation_char_ << '\n';
		    }
		}
	    }
	  a_out << std::endl;
	  a_out << std::endl;
	}

      if (mode == MODE_HEADER_FOOTER) 
	{
	  a_out << "# End of list of properties." << std::endl;
	}
      return;
    }
    
    bool properties::config::is_debug () const
    {
      return debug_;
    }

    void properties::config::set_debug (bool a_debug)
    {
      debug_ = a_debug;
      return;
    }

    properties::config::config (bool a_use_smart_modulo, 
				int a_mode, 
				bool a_write_public_only)
    {
      debug_           = false;
      use_smart_modulo_ = a_use_smart_modulo ;
      mode_            = MODE_DEFAULT;
      mode_            = a_mode;
      write_public_only_ = a_write_public_only;
      read_line_count_ = 0;
      continuation_char_ = DEFAULT_CONTINUATION_CHAR;
      comment_char_ = DEFAULT_COMMENT_CHAR;
      assign_char_  = DEFAULT_ASSIGN_CHAR;
      desc_char_    = DEFAULT_DESC_CHAR;
      return;
    }

    properties::config::~config ()
    {
      return;
    }

    void properties::config::read (std::istream & a_in, 
				   properties & a_props)
    {
      read_ (a_in, a_props);
      return;
    }

    void properties::config::read_ (std::istream & a_in, 
				     properties & a_props)
    {
      bool verbose_parsing  = debug_ || properties::g_debug;

      std::string line_in;
      std::string prop_config;
      std::string prop_description;
      bool line_goon = false;
      while (a_in) 
	{
	  if (verbose_parsing) 
	    {
	      std::cerr << "DEBUG: properties::config::read_: "
			<< "loop on input stream..." 
			<< std::endl; 
	    }
	  std::string line_get;
	  std::getline (a_in, line_get);
	  /*
	    if (!a_in) {
	    if (verbose_parsing) {
	    std::cerr << "DEBUG: properties::config::read_: "
	    << "Cannot get a new line from input!"
	    << std::endl;
	    } 
	    return;  
	    }
	  */
	  read_line_count_++;
      
	  // check if line has a continuation mark at the end:
	  bool line_continue = false; 
	  int sz = line_get.size ();
	  if (sz > 0 && line_get[sz-1] == continuation_char_) 
	    {
	      line_continue = true;
	      line_get = line_get.substr (0, sz - 1);
	      if (verbose_parsing) 
		{
		  std::cerr << "DEBUG: properties::config::read_: "
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
		  std::cerr << "DEBUG: properties::config::read_: "
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
		  std::cerr << "DEBUG: properties::config::read_: "
			    << "new --> line_in='" 
			    << line_in << "'" << std::endl; 
		}
	      read_line_count_++;
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
		  std::cerr << "DEBUG: properties::config::read_: line " 
			    << read_line_count_ << " size is " 
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
		      std::cerr << "DEBUG: properties::config::read_: line " 
				<< read_line_count_ << " is blank" << std::endl;
		    }
		  skip_line=true;
		}
	  
	      // check if line is a comment:
	      if (! skip_line) 
		{
		  std::istringstream iss (line);
		  char c = 0;
		  iss >> c;
		  if (c == comment_char_) 
		    {
		      if (verbose_parsing) 
			{
			  std::cerr << "DEBUG: properties::config::read_: line " 
				    << read_line_count_ << " is a comment" << std::endl;
			}
		
		      iss >> std::ws;
	      
		      std::string token;
	      
		      iss >> token;
		      // Maybe we should ensure only one '@config' directive
		      // here only warn...
		      if (token == "@config") 
			{
			  iss >> std::ws;
			  std::string config_desc;
			  if (! prop_config.empty ())
			    {
			      cerr << "WARNING: " << "properties::config::read_: "
				   << "Duplicated '@config' directive !" << endl;
			    }
			  std::getline (iss, config_desc);
			  if (! config_desc.empty ()) 
			    {
			      prop_config = config_desc;

			      a_props.set_description (config_desc);
			    }
			}
	      
		      if (token == "@description") 
			{
			  iss >> std::ws;
			  std::string desc;
			  std::getline (iss, desc);
			  if (! desc.empty ()) 
			    {
			      prop_description = desc;
			    }
			}
	      
		      skip_line = true; 
		    }
		} // if (! skip_line)
	  
	      // parse line:
	      if (! skip_line) 
		{
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_: "
				<< "line " 
				<< read_line_count_ << " is '" << line << "'" << std::endl;
		    }
		  std::string line_parsing = line;
	    
	    
		  int flag_pos = line_parsing.find_first_of (assign_char_);
		  if (flag_pos >= (int) line_parsing.size ()) 
		    {
		      std::ostringstream message;
		      message << "properties::config::read_: "
			      << "Cannot find assign symbol !" ;
		      throw std::runtime_error (message.str ());
		    }
	    
		  // parse property desc:
		  std::string property_desc_str = line_parsing.substr (0, flag_pos);
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_: "
				<< "property_desc_str='" 
				<< property_desc_str << "'" << std::endl;
		    }
	    
		  // parse property value:
		  std::string property_value_str = line_parsing.substr (flag_pos+1);
		  if (verbose_parsing) 
		    {
		      std::cerr << "DEBUG: properties::config::read_: "
				<< "property_value_str='" 
				<< property_value_str << "'" << std::endl;
		    }
	    
	    
		  bool scalar = true;
		  bool locked = false;
		  int  vsize  = -1;
		  char type   = 0;
		  std::string prop_key;
	    
		  int desc_pos = property_desc_str.find_first_of (desc_char_);
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
		      //property_desc_str.substr (0, desc_pos);
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
			      message << "properties::config::read_: "
				      << "Cannot find vector size for key '" << prop_key << "' at line '" << line << "' !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  if (vsize < 0) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_: "
				      << "Invalid vector size !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  char c = 0;
			  if (! vec_ss) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_: "
				      << "Cannot find expected vector size closing symbol for key '" << prop_key << "' at line '" << line << "'  !" ;
			      throw std::runtime_error (message.str ());		    
			    }
			  vec_ss >> c;
			  if (c != CLOSE_VECTOR) 
			    {
			      std::ostringstream message;
			      message << "properties::config::read_: "
				      << "Invalid vector size closing symbol for key '" << prop_key << "' at line '" << line << "'  !" ;
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
			  message << "properties::config::read_: "
				  << "Invalid type specifier '" << token << "' " 
				  << "at key '" << prop_key << "' ";
			  if (! a_props.get_description ().empty ())
			    {
			      message << "for list described by '" << a_props.get_description () << "' ";
			    }
			  message << "!" ;
			  throw std::runtime_error (message.str ());
			}
	      
		      if (verbose_parsing) 
			{
			  std::cerr << "DEBUG: properties::config::read_: "
				    << "type='" 
				    << type << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_: "
				    << "locked='" 
				    << locked << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_: "
				    << "vsize='" 
				    << vsize << "'" << std::endl;
			  std::cerr << "DEBUG: properties::config::read_: "
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
				  message << "properties::config::read_: "
					  << "Cannot read boolean value for key '" << prop_key << "' at line '" << line << "' !" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  bool b;
				  iss >> b;
				  if (! iss) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_: "
					      << "Cannot read vector boolean value for key '" << prop_key << "' at line '" << line << "' !" ;
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
				  message << "properties::config::read_: "
					  << "Cannot read integer value for key '" << prop_key << "' at line '" << line << "' !" ;
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
				      message << "properties::config::read_: "
					      << "Cannot read vector integer value for key '" << prop_key << "' at line '" << line << "' !" ;
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
				  message << "properties::config::read_: "
					  << "Cannot read real value for key '" << prop_key << "' at line '" << line << "' !" ;
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
				      message << "properties::config::read_: "
					      << "Cannot read vector real value for key '" << prop_key << "' at line '" << line << "' !" ;
				      throw std::runtime_error (message.str ());
				    }
				  //cerr << "DEVEL: ***** x=" << x << " sz=" << vsize << endl;
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
				  message << "properties::config::read_: "
					  << "Cannot read string value for key '" << prop_key << "' at line '" << line << "' !" ;
				  throw std::runtime_error (message.str ());
				}
			    }
			  else 
			    {
			      for (int i = 0; i < vsize; i++) 
				{
				  std::string str;
				  if (! read_quoted_string (iss, str)) 
				    {
				      std::ostringstream message;
				      message << "properties::config::read_: "
					      << "Cannot read string value for key '" << prop_key << "' at line '" << line << "' !" ;
				      throw std::runtime_error (message.str ());
				    }
				  v_strings[i] = str;
				}
			    }
			}

		      /*****/
	      
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL && scalar) 
			{
			  a_props.store (prop_key, a_boolean, prop_description, locked);
			}
		      if (type == properties::data::TYPE_INTEGER_SYMBOL && scalar) 
			{
			  a_props.store (prop_key, a_integer, prop_description, locked);
			}
		      if (type == properties::data::TYPE_REAL_SYMBOL && scalar) 
			{
			  a_props.store (prop_key, a_real, prop_description, locked);
			}
		      if (type == properties::data::TYPE_STRING_SYMBOL && scalar) 
			{
			  a_props.store (prop_key, a_string, prop_description, locked);
			}
	      
		      if (type == properties::data::TYPE_BOOLEAN_SYMBOL &&  !scalar) 
			{
			  a_props.store (prop_key, v_booleans, prop_description, locked);
			}
		      if (type == properties::data::TYPE_INTEGER_SYMBOL && ! scalar) 
			{
			  a_props.store (prop_key, v_integers, prop_description, locked);
			}
		      if (type == properties::data::TYPE_REAL_SYMBOL && ! scalar) 
			{
			  a_props.store (prop_key, v_reals, prop_description, locked);
			}
		      if (type == properties::data::TYPE_STRING_SYMBOL && ! scalar) 
			{
			  a_props.store (prop_key, v_strings, prop_description, locked);
			}
		      prop_description = "";
	      
		    }
		} // !skip_line
	    } // if (! line_goon)
	} // while (*_in) 
      
      return;
    }

    bool properties::config::read_quoted_string (std::istream & a_in, 
						 std::string & a_str)
    {
      std::string chain = "";
      a_in >> std::ws;
      if (! a_in) 
	{
	  a_str = chain;
	  return true;
	}

      bool quoted = false;
      char lastc = 0;      
      do 
	{
	  char c = 0;
	  a_in.get (c);
	  if (! a_in) break;
	  if (c == '"') 
	    {
	      if (quoted) 
		{
		  quoted=false;
		  break;
		  /*  
		      if (lastc == '\\') {
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
		      a_in.putback (c);
		      break;
		    }
		}
	      chain += c;
	      lastc = c;
	    }

	} while (a_in);
      if (quoted) 
	{
	  return false;
	}

      a_str = chain;
      return true;
    }
 
    std::string properties::build_property_key (const std::string & a_prefix, 
						const std::string & a_subkey)
    {
      std::string s (a_prefix);
      s += '.';
      s += a_subkey;
      return s;
    }

    void properties::export_to_string_based_dictionary (map<string, string> & a_dict, bool a_quoted_strings) const
    {
      for (pmap::const_iterator i = props_.begin ();
	   i != props_.end ();
	   i++)
	{
	  const string & key = i->first;
	  ostringstream valoss;
	  const data & data = i->second;
	  if (data.is_vector ())
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
		  if (a_quoted_strings) valoss << '"';
		  valoss << data.get_string_value (i);
		  if (a_quoted_strings) valoss << '"';
		}
	    }
	  if (data.is_vector ())
	    {
	      valoss << ')';
	    }
	  a_dict[key] = valoss.str ();
	}
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

// end of properties.cc
