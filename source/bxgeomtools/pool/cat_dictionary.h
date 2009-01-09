// -*- mode: c++; -*- 
/* cat_dictionary.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-24
 * Last modified: 2006-11-24
 * 
 * License: 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * Description: 
 *   Dictionary of categories
 * 
 * History: 
 * 
 */

#ifndef __SNgeom__cat_dictionary_h
#define __SNgeom__cat_dictionary_h 1

// standard includes:
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
// <here you may add some other standard '#include' directives>

// miscellaneous includes:
#include <SNutils/string_tools.h>
// <here you may add some '#include' directives>

namespace geomutils {

  class cat_entry
  {
  
  // typedefs declarations:
  public:
    static const size_t       LABEL_MAXLEN;
    static const std::string  LABEL_ALLOWED_CHARS;
    static const std::string  LABEL_FIRST_FORBIDDEN_CHARS;
    static const std::string  LABEL_LAST_FORBIDDEN_CHARS;
  // static constants declarations:

  // static declarations:
  
  // non static fields declarations:
  protected: 
    std::string              _label;
    int                      _category;
    std::string              _description;
    std::vector<std::string> _address_desc;
  
  // getters/setters declarations:
  public: 
    const std::string & get_label() const;
    void set_label( const std::string & );
    const std::string & get_description() const;
    void set_description( const std::string & );
    int get_category() const;
    void set_category( int );
    const std::vector<std::string> & get_address_desc() const;
    int get_address_position( const std::string & ) const;
  // ctor/dtor:
  public: 
    cat_entry();
    virtual ~cat_entry();
  
  // methods:
  public: 
    size_t get_address_size() const;
    size_t get_address_depth() const;
    void add_address_component( const std::string & );
    const std::string & get_address_component( int ) const;
    static bool validate_label( const std::string & );
    // <here you may define more class methods>

  };

  // Dictionary of category descriptors:
  class cat_dictionary 
  {
  // static constants declarations:
  public: 
    //typedef std::map<int,cat_entry> dict_t;
    typedef std::vector<cat_entry> dict_t;
  public: 
    static const char DICT_STORE_SEP;
    static const char DICT_STORE_ADDRESS_SEP;
    static const char DICT_STORE_COMMENT;
    static const char CAT_INVALID;
    static const char CAT_SEP;
    static const char ADD_INVALID;
    static const char ADD_SEP;
    static const char ID_OPEN;
    static const char ID_SEP;
    static const char ID_CLOSE;
    static const int  INVALID;
  private: 
    bool   _initialized;
    dict_t _dict; 
  public:
    cat_dictionary();
    virtual ~cat_dictionary();
  protected:
    void _assert_lock( const std::string & what_ );
  private:
    int __get_next_category() const;
  public:
    bool is_initialized() const;
    void init();
    void reset();
    bool category_is_valid( int ) const;
    bool label_is_valid( const std::string & ) const;
    int get_category_from_label( const std::string & ) const;
    void insert_entry( const cat_entry & );
    const cat_entry & get_entry( int ) const;
    const cat_entry & get_entry( const std::string & ) const;
    static bool read_entry( const std::string & , cat_entry & );
    void load( std::istream & );
    void store( std::ostream & ) const;
    void dump( std::ostream & ,
	       const std::string & title_  = "geomutils::cat_dictionary", 
	       const std::string & indent_ = "" ,
	       bool inherit_               =  false ) const;
      
  };

  // Predicates:

  class has_label : public std::unary_function<cat_entry,bool>
  {
    std::string label;
  public:
    has_label( const std::string & label_ );
    bool operator()( const cat_entry & pair_entry_ ) const;
  };


} // end of namespace geomutils

#endif // __GeomUtils__cat_dictionary_h

// end of cat_dictionary.h
