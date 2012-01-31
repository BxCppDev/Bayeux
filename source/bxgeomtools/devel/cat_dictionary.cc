// -*- mode: c++; -*- 
/* cat_dictionary.cc
 */

#include <GeomUtils/cat_dictionary.h>

namespace geomutils {

  /* Class 'cat_entry' */

  // static constants definitions:
  const size_t      cat_entry::LABEL_MAXLEN = 32;
  const std::string cat_entry::LABEL_ALLOWED_CHARS ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-+";
  const std::string cat_entry::LABEL_FIRST_FORBIDDEN_CHARS ="0123456789.-+";
  const std::string cat_entry::LABEL_LAST_FORBIDDEN_CHARS ="._-+";
  
  // static definitions:

  // static method:
  bool cat_entry::validate_label( const std::string & label_ )
  {
    if ( label_.size() > LABEL_MAXLEN ) return false;
    size_t forbidden = label_.find_first_not_of(LABEL_ALLOWED_CHARS);
    if ( forbidden != label_.npos ) {
      return false;
    }
    forbidden=label_.npos;
    forbidden = label_.find_first_of(LABEL_FIRST_FORBIDDEN_CHARS);
    if ( forbidden == 0 ) {
      return false;
    }
    forbidden=label_.npos;
    forbidden = label_.find_last_of(LABEL_LAST_FORBIDDEN_CHARS);
    if ( forbidden == label_.size()-1 ) {
      return false;
    }
    return true;
  }
  
  // getters/setters definitions:
  const std::string & cat_entry::get_label() const
  {
    return _label;
  }
  
  void cat_entry::set_label( const std::string & new_value_ )
  {
    if ( ! validate_label(new_value_) ) {
      std::ostringstream message;
      message << "cat_entry::set_label: Label '" << new_value_ << "' is not valid!";
      throw std::runtime_error(message.str());
    }
    _label=new_value_;
  }
  
  const std::string & cat_entry::get_description() const
  {
    return _description;
  }
  
  void cat_entry::set_description( const std::string & new_value_ )
  {
    _description=new_value_;
  }
  
  int cat_entry::get_category() const
  {
    return _category;
  }
  
  void cat_entry::set_category( int new_value_ )
  {
    _category=new_value_;
  }
  
  const std::vector<std::string> & cat_entry::get_address_desc() const
  {
    return _address_desc;
  }
  
  // ctor/dtor:
  cat_entry::cat_entry()
  {
    _label="";
    _description="";
    _category=-1;
  }
  
  cat_entry::~cat_entry()
  {
    _address_desc.clear();
  }

  size_t cat_entry::get_address_size() const
  {
    return _address_desc.size();
  }

  size_t cat_entry::get_address_depth() const
  {
    return get_address_size();
  }

  void cat_entry::add_address_component( const std::string & label_ )
  {
    _address_desc.push_back(label_);
  }

  const std::string & cat_entry::get_address_component( int i_ ) const
  {
    return _address_desc.at(i_);
  }

  int cat_entry::get_address_position( const std::string & comp_label_ ) const
  {
    int found=-1;
    for ( std::vector<std::string>::const_iterator i = _address_desc.begin();
	  i != _address_desc.end();
	  i++ ) {
      found++;
      if ( *i == comp_label_) {
	return found;
      }
    }
    return -1;
  }

  /* Predicates */
  has_label::has_label( const std::string & label_ ) : label(label_) 
  {
  }

  bool has_label::operator()( const cat_entry & entry_  ) const
  {
    return entry_.get_label() == label;
  }

  /****************************************************************/

  /* Class 'cat_dictionary' */

  const char cat_dictionary::DICT_STORE_SEP           = ':' ;
  const char cat_dictionary::DICT_STORE_ADDRESS_SEP   = ',' ;
  const char cat_dictionary::DICT_STORE_COMMENT       = '#';

  bool cat_dictionary::is_initialized() const
  {
    return _initialized;
  }
  
  void cat_dictionary::_assert_lock( const std::string & what_ )
  {
    if ( is_initialized() ) {
      std::ostringstream message;
      message << what_ << ": dictionary is locked; operation prohibited!";
      throw std::logic_error(message.str());
    }
  }

  cat_dictionary::cat_dictionary()
  {
    _initialized=false;
  }

  cat_dictionary::~cat_dictionary()
  {
    if ( _initialized ) reset();
  }

  void cat_dictionary::init()
  {
    if ( _initialized ) return;
    _initialized=true;
  }

  void cat_dictionary::reset()
  {
    if ( !_initialized ) return;
    _dict.clear();
    _initialized=false;    
  }

  bool cat_dictionary::category_is_valid( int cat_ ) const
  {
    if ( cat_ < 0 || cat_ >= _dict.size() ) return false;
    return true;
  }

  bool cat_dictionary::label_is_valid( const std::string & label_ ) const
  {
    dict_t::const_iterator found=
      std::find_if(_dict.begin(),_dict.end(),has_label(label_));
    if ( found == _dict.end() ) return false;
    return true;
  }

  int cat_dictionary::get_category_from_label( const std::string & cat_label_ ) const
  {
    return get_entry(cat_label_).get_category();
  }

  int cat_dictionary::__get_next_category() const
  {
    return _dict.size();
  }

  void cat_dictionary::insert_entry( const cat_entry & entry_ )
  {
    // assign automatically the category number as the index in the '_dict' array
    // the vector<cat_entry> is faster than a map<int,cat_entry>:
    cat_entry e = entry_;
    int next_cat = __get_next_category();
    e.set_category(next_cat);
    _dict.push_back(e);
  }

  const cat_entry & cat_dictionary::get_entry( int cat_ ) const
  {
    if ( cat_ < 0 || cat_ >= _dict.size() ) {
      std::ostringstream message;
      message << "cat_dictionary::get_entry: No category with ID '" 
	      << cat_ << "'!";
      throw std::range_error(message.str());
    } 
    return _dict[cat_];
  }

  const cat_entry & cat_dictionary::get_entry( const std::string & label_ ) const
  {
    dict_t::const_iterator found=
      std::find_if(_dict.begin(),_dict.end(),has_label(label_));
    if ( found == _dict.end() ) {
      std::ostringstream message;
      message << "cat_dictionary::get_entry: No category with name '" << label_ << "'!";
      throw std::range_error(message.str());
    }
    else {
      return *found;
    }
  }

  bool cat_dictionary::read_entry( 
    const std::string & in_str_ , 
    cat_entry & entry_ )
  {
    std::list<std::string> tokens;
    std::string sseps;
    sseps.push_back(DICT_STORE_SEP);
    snemo::string_tools::split_string(in_str_,sseps,tokens);
    if ( tokens.size() < 3 ) {
      return false;
    } 
    int count=0;
    for ( std::list<std::string>::const_iterator i=tokens.begin(); 
	  i!=tokens.end();
	  i++ ) {
      //std::cerr << "cat_dictionary::read_entry: token='" << *i << "'" << std::endl; 
      if ( count==0 ) entry_.set_label(*i);
      /*
      // category (int) is not used anymore:
      if ( count==1 ) {
	std::istringstream iss(*i);
	int cat;
	iss >> cat;
	if ( !iss ) {
	  return false;
	}
	entry_.set_category(cat);
      }
      */
      if ( count==1 ) {
	std::string add_str = *i;
	std::list<std::string> adds_str;
	std::string seps;
	seps.push_back(DICT_STORE_ADDRESS_SEP);
	snemo::string_tools::split_string(add_str,seps,adds_str);
	if ( adds_str.size() == 0 ) return false;
	for ( std::list<std::string>::const_iterator i2=adds_str.begin();
	      i2!=adds_str.end(); 
	      i2++ ) {
	  entry_.add_address_component(*i2);
	}
      }
      if ( count==2 ) entry_.set_description(*i);
      
      count++;
    }
    return true;
  }

  void cat_dictionary::store( std::ostream & out_ ) const
  {
    for ( dict_t::const_iterator i=_dict.begin();
	  i!=_dict.end();
	  i++ ) {
      const cat_entry & entry=*i;
      out_ << entry.get_label()  
	   //<< DICT_STORE_SEP << entry.get_category()
	   << DICT_STORE_SEP
	   << DICT_STORE_SEP << entry.get_description() 
	   << std::endl;    
    }
  }

  void cat_dictionary::load( std::istream & in_ )
  {
    if ( !in_ ) {
      throw std::runtime_error("cat_dictionary::load: I/O error!");
    } 
    int line_counter=0;
    while ( !in_.eof() ) {
      std::string line;
      std::getline(in_,line);
      line_counter++;
      std::istringstream iss(line);
      std::string word;
      iss >> word;
      /*
      std::cerr << "cat_dictionary::load: debug: Line  #" 
		<< line_counter 
		<< " is '" << line << "'" 
		<< std::endl;
      */
      if ( !word.empty() && word[0] != DICT_STORE_COMMENT ) {
	cat_entry entry;
	bool res = read_entry(line,entry);
	if ( res ) {
	  if ( std::find_if(_dict.begin(),
			    _dict.end(),
			    has_label(entry.get_label()) ) != _dict.end() ) {
	    std::ostringstream message;
	    message << "cat_dictionary::load: " 
		    << "Label conflict! Category labelled '" << entry.get_label()
		    << "' is already registred in the dictionnary!";
	    throw std::logic_error(message.str());
	  }
	  else {
	    insert_entry(entry);
	  }
	}
	else {
	    std::ostringstream message;
	    message << "cat_dictionary::load: " 
		    << "Parser error! Format error at line '" 
		    << line_counter
		    << "'!";
	    throw std::logic_error(message.str());	  
	}
      }
    }
  }

  void cat_dictionary::dump( std::ostream & out_ ,
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
    out_ << indent << *atag << "Initialized :          " 
	 << (is_initialized()?"Yes":"No") << std::endl;
    out_ << indent << *atag << "Number of categories : " 
	 << _dict.size() << std::endl;
    if ( !inherit_ ) {
      atag=&last_tag;
      atagc=&last_tagc;
    }
    out_ << indent << *atag << "Categories : " << std::endl;
    int cat_counter=0;
    for ( dict_t::const_iterator i=_dict.begin(); 
	  i!=_dict.end(); 
	  i++ ) {
      const std::string * atag2=&tag;
      const std::string * atagc2=&tagc;
      if ( cat_counter == _dict.size()-1 ) {
	atag2=&last_tag;
	atagc2=&last_tagc;
      }
      const cat_entry & entry = *i;
      out_ << indent << *atagc << *atag2 << "Category [" << entry.get_label()
	   << "]: category='" 
	   << entry.get_category() 
	   << "' address-depth='" <<  entry.get_address_size()
	   << "' description='" << entry.get_description();
      out_ << "' address=[";
      for ( int j=0; j<entry.get_address_size(); j++ ) {
	out_ <<  entry.get_address_component(j);
	if ( j<entry.get_address_size()-1 ) out_ << ',';
      }
      out_ << "]"; 
      out_ << std::endl;
      cat_counter++; 
    } 
  } 

} // end of namespace geomutils

// end of cat_dictionary.cc
