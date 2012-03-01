// -*- mode: c++; -*- 
/* geom_locator.cc
 */

#include <GeomUtils/geom_locator.h>

namespace geomutils {

  // static constants definitions:
  
  // static definitions:
  
  // getters/setters definitions:
  bool geom_locator::is_debug() const
  {
    return _debug;
  }
  
  void geom_locator::set_debug( bool new_value_ )
  {
    _debug=new_value_;
  }
  
  bool geom_locator::is_initialized() const
  {
    return _initialized;
  }
  
  void geom_locator::_assert_lock( const std::string & what_ )
  {
    if ( is_initialized() ) {
      std::ostringstream message;
      message << what_ << ": locator is locked; operation prohibited!";
      throw std::logic_error(message.str());
    }
  }
      
  // ctor/dtor:
  geom_locator::geom_locator()
  {
    _debug=false;
    _initialized=false;
  }
  
  geom_locator::~geom_locator()
  {
    if ( is_initialized() ) reset();
  }

  void geom_locator::_at_init_do()
  {
    if ( is_debug() ) std::cerr << "geom_locator::_at_init_do: DEBUG: Nothing special..." << std::endl;
    _categories_dict.init();
  }

  void geom_locator::_at_reset_do()
  {
    if ( is_debug() ) std::cerr << "geom_locator::_at_reset_do: DEBUG: Nothing special..." << std::endl;
    _categories_dict.reset();
  }
  
  void geom_locator::init() 
  { 
    if ( is_debug() ) std::cerr << "geom_locator::init: DEBUG: Entering..." << std::endl;
    if ( is_initialized() ) { 
      std::cerr << "geom_locator::init: warning: Already initialized!" << std::endl;
      return; 
    }
    _at_init_do();
    _initialized=true;
    if ( is_debug() ) std::cerr << "geom_locator::init: DEBUG: Exiting." << std::endl;
  } 
 
  void geom_locator::reset() 
  {
    if ( is_debug() ) std::cerr << "geom_locator::reset: DEBUG: Entering..." << std::endl;
    if ( !is_initialized() ) {
      std::cerr << "geom_locator::reset: warning: Not initialized!" << std::endl;
      return;
    }
    _at_reset_do();
    _initialized=false;
    if ( is_debug() ) std::cerr << "geom_locator::reset: DEBUG: Exiting." << std::endl;
  }

  bool geom_locator::category_is_valid( int cat_ ) const
  {
    return _categories_dict.category_is_valid(cat_);
  }

  bool geom_locator::category_is_valid( const std::string & cat_label_ ) const
  {
    return _categories_dict.label_is_valid(cat_label_ );
  }

  /*
  geom_id geom_locator::extract_id( const geom_id & from_id_ , int cat_ ) const
  {
    if ( !category_is_valid(cat_) ) {
      std::ostringstream message;
      message << "geom_locator::extract_id: Invalid category '"
	      << cat_ << "'!";
      throw std::runtime_error(message.str());
    }
    geom_id id;
    id.init(cat_,_categories_dict.get_entry(cat_).get_address_size());
    geom_id::extract(from_id_,id);
    return id;
  }
  */

  void geom_locator::extract_id( const geom_id & from_id_  , 
				 geom_id & to_id_ ) const
  {
    //NOOOOOO: to_id_.reset();
    if ( ! category_is_valid(to_id_.get_category()) ) {
      std::ostringstream message;
      message << "geom_locator::extract_id: Target id has no valid category !";
      throw std::runtime_error(message.str());      
    }
    //create_id(to_id_,to_id_.get_category());
    size_t add_sz=to_id_.get_address_depth();
    for ( int i=0; i<add_sz; i++ ) {
      to_id_.set_address_value(i,from_id_.get_address_value(i));
    }
  }

  bool geom_locator::is_id( const geom_id & id_ , int cat_ ) const
  {
    if ( !category_is_valid(cat_) ) {
      std::ostringstream message;
      message << "geom_locator::is_id: Category #" 
	      << cat_ << " does not exist!" << std::endl;
      throw std::runtime_error(message.str());
    }
    return (id_.get_category() == cat_ );
  }

  bool geom_locator::is_id( const geom_id & id_ , const std::string & cat_label_ ) const
  {
    if ( !category_is_valid(cat_label_) ) {
      std::ostringstream message;
      message << "geom_locator::is_id: Category '" 
	      << cat_label_ << "' does not exist!" << std::endl;
      throw std::runtime_error(message.str());
    }
    const cat_entry & ce = _categories_dict.get_entry(cat_label_);
    return is_id(id_,ce.get_category());
  }

  std::string geom_locator::get_category_label( int cat_ ) const
  {
    if ( !category_is_valid(cat_) ) return "";
    return _categories_dict.get_entry(cat_).get_label();
  }
  
  void geom_locator::create_id( geom_id & id_ , int cat_ ) const
  {
    if ( !is_initialized() ) {
      throw std::logic_error("geom_locator::create_id: Not initialized!");
    }
    if ( !category_is_valid(cat_) ) {
      std::ostringstream message;
      message << "geom_locator::create_id: Category #" 
	      << cat_ << "' does not exist!" << std::endl;
      throw std::runtime_error(message.str());
    }
    id_.reset();
    id_.init(cat_,_categories_dict.get_entry(cat_).get_address_size());
  }

  void geom_locator::create_id( geom_id & id_ , const std::string & cat_label_ ) const
  {
    if ( !is_initialized() ) {
      throw std::logic_error("geom_locator::create_id: Not initialized!");
    }
    const cat_entry & ce = _categories_dict.get_entry(cat_label_);
    id_.reset();
    id_.init(ce.get_category(),ce.get_address_size());
  }

  int geom_locator::get_address_id( const geom_id & id_ , int add_depth_ ) const
  {
    const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
    if ( add_depth_ < 0 || add_depth_ >= ce.get_address_depth() ) {
      std::ostringstream message;
      message << "geom_locator::get_address_id: Invalid depth '" << add_depth_ << "' for category '"
	      << ce.get_label() << "'!" << std::endl;
      throw std::runtime_error(message.str());
    }
    return id_.get_address_value(add_depth_);
  }

  int geom_locator::get_address_id( const geom_id & id_ , const std::string & add_label_ ) const
  {
    const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
    int add_depth = ce.get_address_position(add_label_);
    if ( add_depth == -1 ) {
      std::ostringstream message;
      message << "geom_locator::get_address_id: Invalid component label '" << add_label_ << "' for category '"
	      << ce.get_label() << "'!" << std::endl;
      throw std::runtime_error(message.str());
    }
    return get_address_id(id_,add_depth);
    
  }

  void geom_locator::set_address_id( geom_id & id_ , 
				     int add_depth_ ,
				     int val_ ) const
  {
    const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
    if ( add_depth_ < 0 || add_depth_ >= ce.get_address_depth() ) {
      std::ostringstream message;
      message << "geom_locator::get_address_id: Invalid depth '" << add_depth_ << "' for category '"
	      << ce.get_label() << "'!" << std::endl;
      throw std::runtime_error(message.str());
    }
    id_.set_address_value(add_depth_,val_);
  }

  void geom_locator::set_address_id( geom_id & id_ , 
				     const std::string & add_label_ ,
				     int val_ ) const
  {
    const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
    int add_depth = ce.get_address_position(add_label_);
    if ( add_depth == -1 ) {
      std::ostringstream message;
      message << "geom_locator::get_address_id: Invalid component label '" << add_label_ << "' for category '"
	      << ce.get_label() << "'!" << std::endl;
      throw std::runtime_error(message.str());
    }
    set_address_id(id_,add_depth,val_);
  }

  void geom_locator::set_full_address_id( geom_id & id_ , 
		     int val0_ , 
		     int val1_ , 
		     int val2_ , 
		     int val3_ , 
		     int val4_ ,
		     int val5_ , 
		     int val6_ , 
		     int val7_ , 
		     int val8_ ,
		     int val9_ ) const
  {
    //const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
    bool a_debug=is_debug();
    //a_debug=true;
    if ( a_debug ) {
      std::cerr << "geom_locator::set_full_address_id: DEBUG: Invoke 'set_address_values'" << std::endl;
    }
    id_.set_address_values(val0_,val1_,val2_,val3_,val4_,
			   val5_,val6_,val7_,val8_,val9_);
    if ( a_debug ) {
      std::cerr << "geom_locator::set_full_address_id: DEBUG: ok!" << std::endl;
    }
    // Better to validate on demand, so skip the following lines:
    /*
    if ( !validate_id(id_) ) {
      std::ostringstream message;
      message << "geom_locator::set_full_address_id: Invalid address!";
      throw std::runtime_error(message.str());
    }
    if ( is_debug() ) {
      std::cerr << "geom_locator::set_full_address_id: validated!" << std::endl;
    }
    */
  }

  void geom_locator::encode_id( const geom_id & id_ , std::string & str_ ) const
  {
    std::ostringstream oss;
    oss << geom_id::TOKEN_ID_OPEN;
    if ( _categories_dict.category_is_valid(id_.get_category()) ) { 
      const cat_entry & ce = _categories_dict.get_entry(id_.get_category());
      oss << ce.get_label() << geom_id::TOKEN_ID_SEP;
      std::string add_str;
      geom_id::encode_address(add_str,id_.get_address(),id_.get_address_depth());
      oss << add_str;
    }
    else {
      oss << geom_id::TOKEN_CAT_INVALID << geom_id::TOKEN_ID_SEP 
	  << geom_id::TOKEN_ADD_INVALID;
    }
    oss << geom_id::TOKEN_ID_CLOSE;
    str_=oss.str();
  }

  std::string geom_locator::to_string_id( const geom_id & id_ ) const
  {
    std::string id_str;
    geom_locator::encode_id(id_,id_str);
    return id_str;
  }

  bool geom_locator::decode_id( geom_id & id_ , 
				const std::string & str_ ) const
  {
    id_.reset();
    if ( str_[0] != geom_id::TOKEN_ID_OPEN ) return false;

    size_t pos=str_.find(geom_id::TOKEN_ID_SEP,1);
    if ( pos == str_.npos ) return false;
    std::string cat_label=str_.substr(1,pos-1);
    if ( ! _categories_dict.label_is_valid(cat_label) ) {
      return false;
    }  
    const cat_entry & ce = _categories_dict.get_entry(cat_label);
    int cat=ce.get_category();
    size_t add_depth=ce.get_address_depth();
    create_id(id_,cat);
    pos++;
    size_t pos2=str_.find(geom_id::TOKEN_ID_CLOSE,pos);
    if ( pos2 == str_.npos ) return false;
    if ( pos2 != str_.size()-1 ) return false;
    
    std::string add_str=str_.substr(pos,pos2-pos);
    int values[geom_id::MAX_ADDRESS_DEPTH];
    bool res = geom_id::decode_address(add_str,values,add_depth);
    if ( !res ) return false;
    for ( int i=0; i<add_depth; i++ ) {
      id_.set_address_value(i,values[i]);
    }

    /* remove the validation check: better to invoke 'validate_id' at the user level:
    // after parsing, we check if the address is valid with respect to the category:
    if ( !validate_id(id_) ) { 
      return false;
    }
    */
    return true;
  }

  void geom_locator::dump_cat_dictionary( std::ostream & out_ ) const
  {
    _categories_dict.dump(out_,"geom_locator: Embedded categories dictionary");    
  }

  void geom_locator::load_cat_dictionary( const std::string & dict_filename_ )
  {
    if ( is_debug() ) std::cerr << "geom_locator::load_cat_dictionary: Entering..." << std::endl;
    _assert_lock("geom_locator::load_cat_dict");    
    std::ifstream fdict(dict_filename_.c_str());
    if ( !fdict ) {
      std::ostringstream message;
      message << "geom_locator::load_cat_dictionary: Cannot open category dictionnary file '" 
	      << dict_filename_ << "'!"<< std::endl;
      throw std::runtime_error(message.str());
    }
    _categories_dict.load(fdict);
    if ( is_debug() ) std::cerr << "geom_locator::load_cat_dictionary: Exiting." << std::endl;  
  }

  void geom_locator::dump( std::ostream & out_ ,
		      const std::string & title_  , 
		      const std::string & indent_ ,
		      bool inherit_ ) const
  {
    std::string title=title_;
    std::string indent=indent_;
    if ( title.empty() ) title=title_;
    out_ << title << std::endl;
    const std::string tag("|-- ");
    const std::string tagc("|   ");
    const std::string last_tag("`-- ");
    const std::string last_tagc("    ");
    const std::string * atag=&tag;
    const std::string * atagc=&tagc;
    out_ << indent << *atag << "Debug : " << (is_debug()?"Yes":"No") << std::endl;
    out_ << indent << *atag << "Initialized : " << (is_initialized()?"Yes":"No") << std::endl;
    if ( !inherit_ ) {
      atag=&last_tag;
      atagc=&last_tagc;
    }
    _categories_dict.dump(out_,indent+*atag+"Category dictionary",indent+*atagc);
  }

} // end of namespace geomutils

// end of geom_locator.cc
