// -*- mode: c++; -*- 
/* properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-19
 * Last modified: 2008-02-19
 * 
 * License: 
 * 
 * 
 * Description: 
 *   A simple properties dictionary
 * 
 * History: 
 * 
 */

#ifndef __datatools__utils__properties_h
#define __datatools__utils__properties_h 1

// standard includes:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <functional>

// miscellaneous includes:

#include <datatools/serialization/serialization.h>
/*
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
*/

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

namespace datatools {

  namespace utils {

    class properties : public i_tree_dumpable,
		       public i_clear
    {
    public:
      static bool g_debug;

      class data
      {
      public:
	static const int  ERROR_SUCCESS = 0;
	static const int  ERROR_FAILURE = 1;
	static const int  ERROR_BADTYPE = 2;
	static const int  ERROR_RANGE   = 3;
	static const int  ERROR_LOCK    = 4;

	static const char MASK_TYPE    = 0x7;
	static const char MASK_LOCK    = 0x40;
	static const char MASK_VECTOR  = 0x80;
      
	static const char TYPE_NONE    = 0x0;
	static const char TYPE_BOOLEAN = 0x1;
	static const char TYPE_INTEGER = 0x2;
	static const char TYPE_REAL    = 0x3;
	static const char TYPE_STRING  = 0x4;

	static const char TYPE_BOOLEAN_SYMBOL = 'B';
	static const char TYPE_INTEGER_SYMBOL = 'I';
	static const char TYPE_REAL_SYMBOL    = 'R';
	static const char TYPE_STRING_SYMBOL  = 'S';
      
      public:
	typedef std::vector<bool>         vbool;
	typedef std::vector<int>          vint;
	typedef std::vector<double>       vdouble;
	typedef std::vector<std::string>  vstring;

      private:
	/*
	 * format: VL000TTT
	 *  V   == vector bit
	 *  L   == lock bit
	 *  TTT == type bits 
	 */
	unsigned char __desc; 
	vbool    __boolean_values;
	vint     __integer_values;
	vdouble  __real_values;
	vstring  __string_values;

      private:
	void __clear_values();
	int  __init_values( char   type_ = TYPE_INTEGER_SYMBOL ,
			    size_t size_ = 1 );

      public:
	bool has_type() const;
	bool is_boolean() const;
	bool is_integer() const;
	bool is_real() const;
	bool is_string() const;

	bool is_scalar() const;
	bool is_vector() const;

	bool is_locked() const;
	bool is_unlocked() const;

	int boolean( size_t = 1 );
	int integer( size_t = 1 );
	int real( size_t = 1 );
	int string( size_t = 1 );
	int lock();
	int unlock();
	size_t      get_size() const;
	size_t      size() const;

	bool        index_is_valid( int ) const;

	bool        get_boolean_value( int = 0 ) const;
	int         get_integer_value( int = 0 ) const;
	double      get_real_value( int = 0 ) const;
	std::string get_string_value( int = 0 ) const;

	int set_value( bool    , int = 0 );
	int set_value( int     , int = 0 );
	int set_value( double  , int = 0 );
	int set_value( const std::string & , int = 0 );
	int set_value( const char * , int = 0 );

	int get_value( bool & , int = 0 ) const;
	int get_value( int & , int = 0 ) const;
	int get_value( double & , int = 0 ) const;
	int get_value( std::string & , int = 0 ) const;

	std::string get_type_label() const;
	std::string get_vector_label() const;

	data( char       type_ = TYPE_INTEGER_SYMBOL ,
	      size_t     size_ = 1);
	data( bool       value_ ,
	      size_t     size_ = 1);
	data( int        value_ ,
	      size_t     size_ = 1);
	data( double       value_ ,
	      size_t     size_ = 1);
	data( const std::string & value_ ,
	      size_t     size_ = 1);
	data( const char * value_ ,
	      size_t     size_ = 1);
	virtual ~data();

	void dump( std::ostream & ) const;
	//static int create_data( data & );

	static std::string get_error_message( int );

	virtual void tree_dump( std::ostream & out_         = std::cerr , 
				const std::string & title_  = "" ,
				const std::string & indent_ = "",
				bool inherit_               = false ) const;
      private:
	friend class boost::serialization::access; 
	template<class Archive>
	void serialize( Archive            & ar_ , 
			const unsigned int   version_ )
	{
	  ar_ & boost::serialization::make_nvp("desc",__desc);
	  if ( is_boolean() ) {
	    ar_ & boost::serialization::make_nvp("boolean_values",__boolean_values);
	  }
	  if ( is_integer() ) {
	    ar_ & boost::serialization::make_nvp("integer_values",__integer_values);
	  }
	  if ( is_real() ) {
	    ar_ & boost::serialization::make_nvp("real_values",__real_values);
	  }
	  if ( is_string() ) {
	    ar_ & boost::serialization::make_nvp("string_values",__string_values);
	  }
	}

      };

      // typedefs declarations:
    public: 
      typedef std::map<std::string,data> pmap;
  
      struct basic_key_validator : public std::unary_function<std::string,bool>
      {
	virtual bool operator()( const std::string & key_arg_ ) const = 0;
      };

      class default_key_validator : public basic_key_validator
      {
	
      public:
	static const std::string ALLOWED_CHARS;
	default_key_validator();
	virtual ~default_key_validator();
	virtual bool operator()( const std::string & key_arg_ ) const;
      };

    protected:
      static default_key_validator g_default_key_validator;

      // non static fields declarations:
    private: 
      bool __debug;
      pmap __props;
      const basic_key_validator * __key_validator;
      bool                        __key_validator_deletion;

      // getters/setters declarations:
    private: 
      void __validate_key( const std::string & key_arg_ ) const;

      void __clear_key_validator();

    public: 
      bool is_debug() const;
      void set_debug( bool );
      size_t size() const;
  
      // ctor/dtor:
    public: 

      void unset_key_validator();

      void set_default_key_validator();

      void set_key_validator( const basic_key_validator & );

      void set_key_validator( const basic_key_validator * , 
			      bool deletion_on_destroy_ = true ); 

      properties(); // with embedded default key validator

      properties( const basic_key_validator * , bool deletion_on_destroy_ = true ); 
      /* with external key validator (deletion_on_destroy_==false)
       * with internal key validator (deletion_on_destroy_==true) 
       *      validator is deleted in the destructor. 
       */

      properties( const basic_key_validator & ); // with external key validator
      virtual ~properties();
  
      // methods:
    private: 

      void __check_nokey( const std::string & key_ ) const;
      void __check_key( const std::string & key_ , data ** data_ );
      void __check_key( const std::string & key_ , const data ** data_ ) const;

    public: 
      bool has_key( const std::string & key_ ) const;

      void erase( const std::string & key_ );

      virtual void clear();
 
      void store( const std::string & key_ , 
		  bool value_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  int value_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  double value_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const std::string & value_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const char * value_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const std::vector<bool> & values_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const std::vector<int> & values_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const std::vector<double> & values_ , 
		  bool lock_ = false );

      void store( const std::string & key_ , 
		  const std::vector<std::string> & values_ , 
		  bool lock_ = false );

      void change( const std::string & key_ , 
		   bool   value_ , 
		   int index_ = 0 );

      void change( const std::string & key_ , 
		   int    value_ , 
		   int index_ = 0  );

      void change( const std::string & key_ , 
		   double value_ , 
		   int index_ = 0  );

      void change( const std::string & key_ , 
		   const std::string & value_ , 
		   int index_ = 0  );

      void change( const std::string & key_ , 
		   const char * value_ , 
		   int index_ = 0  );

      void change( const std::string & key_ , 
		   const std::vector<bool> & values_ );

      void change( const std::string & key_ , 
		   const std::vector<int> & values_ );

      void change( const std::string & key_ , 
		   const std::vector<double> & values_ );

      void change( const std::string & key_ , 
		   const std::vector<std::string> & values_ );

      void fetch( const std::string & key_ , 
		  bool   & value_ , 
		  int index_ = 0 ) const;

      void fetch( const std::string & key_ , 
		  int    & value_ , 
		  int index_ = 0  ) const;

      void fetch( const std::string & key_ , 
		  double & value_ , 
		  int index_ = 0  ) const;

      void fetch( const std::string & key_ , 
		  std::string & value_ , 
		  int index_ = 0  ) const;

      void fetch( const std::string & key_ , 
		  std::vector<bool> & values_ ) const;

      void fetch( const std::string & key_ , 
		  std::vector<int> & values_ ) const;

      void fetch( const std::string & key_ ,
		  std::vector<double> & values_ ) const;

      void fetch( const std::string & key_ , 
		  std::vector<std::string> & values_ ) const;

      bool   fetch_boolean( const std::string & , int index_ = 0 ) const;

      int    fetch_integer( const std::string &  , int index_ = 0) const;

      double fetch_real( const std::string &  , int index_ = 0) const;

      std::string fetch_string( const std::string & , int index_ = 0 ) const;

      void dump( std::ostream & ) const;

      virtual void tree_dump( std::ostream & out_         = std::cerr , 
			      const std::string & title_  = "" ,
			      const std::string & indent_ = "",
			      bool inherit_               = false ) const;

    private:
      friend class boost::serialization::access; 
      template<class Archive>
      void serialize( Archive            & ar_ , 
		      const unsigned int   version_ )
      {
	ar_ & boost::serialization::make_nvp("properties",__props);
      }

    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__properties_h

// end of properties.h
