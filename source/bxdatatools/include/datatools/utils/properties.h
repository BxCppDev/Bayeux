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
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>

#include <boost/cstdint.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

#include <datatools/serialization/i_serializable.h>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

namespace datatools {

  namespace utils {

    class properties : public datatools::utils::i_tree_dumpable,
		       public datatools::utils::i_clear,
		       public datatools::serialization::i_serializable     

    {

    public:
      static bool g_debug;
      static const std::string SERIAL_TAG;

      class data :    
	public datatools::serialization::i_serializable     

      {

      public:

	static const std::string SERIAL_TAG;
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

	static const char STRING_FORBIDDEN_CHAR = '"';

	static const int  SCALAR_DEF    = -1;
	static const int  SCALAR_SIZE   =  1;

	static const bool        DEFAULT_VALUE_BOOLEAN;
	static const int         DEFAULT_VALUE_INTEGER;
	static const double      DEFAULT_VALUE_REAL;
	static const std::string DEFAULT_VALUE_STRING;
      
      public:

	typedef std::vector<bool>        vbool;
	typedef std::vector<int32_t>     vint;
	typedef std::vector<double>      vdouble;
	typedef std::vector<std::string> vstring;

      private:
	/*
	 * format: VL000TTT
	 *  V   == vector bit
	 *  L   == lock bit
	 *  TTT == type bits 
	 */
	uint8_t     __flags; 
	std::string __description;
	vbool       __boolean_values;
	vint        __integer_values;
	vdouble     __real_values;
	vstring     __string_values;

      private:
	void
	__clear_values ();

	int
	__init_values (char type_ = TYPE_INTEGER_SYMBOL,
		      int size_  = SCALAR_DEF);

      public:

	void
	set_description (const std::string &);

	const std::string &
	get_description () const;

	bool
	has_type () const;

	bool
	is_boolean () const;

	bool
	is_integer () const;

	bool
	is_real () const;

	bool
	is_string () const;


	bool
	is_scalar () const;

	bool
	is_vector () const;


	bool
	is_locked () const;

	bool
	is_unlocked () const;


	int
	boolean (int size_ = SCALAR_DEF);

	int
	integer (int size_ = SCALAR_DEF);

	int
	real (int size_ = SCALAR_DEF);

	int
	string (int size_ = SCALAR_DEF);

	int 
	lock ();

	int
	unlock ();

	//size_t
	int32_t
	get_size () const;

	//size_t
	int32_t
	size () const;

	bool
	index_is_valid (int) const;

	bool
	get_boolean_value (int = 0) const;

	int
	get_integer_value (int = 0) const;

	double
	get_real_value (int = 0) const;

	std::string
	get_string_value (int = 0) const;

	int
	set_value (bool, int = 0);

	int
	set_value (int, int = 0);

	int
	set_value (double, int = 0);

	int
	set_value (const std::string &, int = 0);

	int
	set_value (const char *, int = 0);


	int
	get_value (bool &, int = 0) const;

	int
	get_value (int &, int = 0) const;

	int
	get_value (double &, int = 0) const;

	int
	get_value (std::string &, int = 0) const;

	std::string
	get_type_label () const;

	std::string
	get_vector_label () const;

	static bool
	has_forbidden_char (const std::string &);

	//ctor
	data (char type_ = TYPE_INTEGER_SYMBOL ,
	     int size_ = SCALAR_DEF);

	data (bool value_,
	     int size_ = SCALAR_DEF);

	data (int value_,
	     int size_ = SCALAR_DEF);

	data (double value_,
	     int size_ = SCALAR_DEF);

	data (const std::string & value_,
	     int size_ = SCALAR_DEF);

	data (const char * value_ ,
	     int size_ = SCALAR_DEF);

	// dtor
	virtual ~data ();

	void
	dump (std::ostream &) const;

	void 
	to_string (std::ostream & out_) const;

	static std::string
	get_error_message (int);

	virtual void
	tree_dump (std::ostream & out_         = std::cerr, 
		  const std::string & title_  = "",
		  const std::string & indent_ = "",
		  bool inherit_               = false) const;

	virtual const std::string &
	get_serial_tag () const;

      private:

	friend class boost::serialization::access; 
	template<class Archive>
	void
	serialize (Archive            & ar_ , 
		  const unsigned int   version_)
	{
	  ar_ & boost::serialization::make_nvp ("description", __description);
	  ar_ & boost::serialization::make_nvp ("flags", __flags);
	  if (is_boolean ())
	    {
	      ar_ & boost::serialization::make_nvp ("boolean_values",
						    __boolean_values);
	    }
	  if (is_integer ())
	    {
	      ar_ & boost::serialization::make_nvp ("integer_values",
						    __integer_values);
	    }
	  if (is_real ())
	    {
	      ar_ & boost::serialization::make_nvp ("real_values",
						    __real_values);
	    }
	  if (is_string ())
	    {
	      ar_ & boost::serialization::make_nvp ("string_values",
						    __string_values);
	    }
	}

	//BOOST_CLASS_EXPORT_GUID (data, "datatools::utils::properties::data")

      };

      // typedefs declarations:

    public: 

      typedef std::list<std::string>      vkeys;
      typedef std::map<std::string, data> pmap;
  
      struct basic_key_validator 
	: public std::unary_function<std::string,bool>
      {
	virtual bool
	operator () (const std::string & key_arg_) const = 0;
      };

      class default_key_validator 
	: public basic_key_validator
      {
	
      public:

	static const std::string ALLOWED_CHARS;

	// ctor
	default_key_validator ();

	// dtor	
	virtual ~default_key_validator ();

	virtual bool
	operator () (const std::string & key_arg_) const;

      };

    protected:

      static default_key_validator g_default_key_validator;

      // non static fields declarations:

    private: 

      bool                        __debug;
      std::string                 __description;
      pmap                        __props;
      const basic_key_validator * __key_validator;
      bool                        __key_validator_deletion;

      // getters/setters declarations:

    private: 

      void
      __validate_key (const std::string & key_arg_) const;

      void
      __clear_key_validator ();

    public: 
      static const std::string PRIVATE_PROPERTY_PREFIX;

      bool
      is_debug () const;

      void
      set_debug (bool);

      //size_t
      int32_t
      size () const;

      void
      set_description (const std::string &);

      const std::string &
      get_description () const;
  
    public: 

      void
      unset_key_validator ();

      void
      set_default_key_validator ();

      void
      set_key_validator (const basic_key_validator &);

      void
      set_key_validator (const basic_key_validator *, 
			bool deletion_on_destroy_ = true); 

      // ctor
      properties (); // with embedded default key validator

      properties (const std::string & desc_); // with embedded default key validator

      properties (const std::string & desc_, 
		  const basic_key_validator &);

      properties (const basic_key_validator &); 

      /* with external key validator (deletion_on_destroy_==false)
       * with internal key validator (deletion_on_destroy_==true) 
       *      validator is deleted in the destructor. 
       */

      properties (const std::string & desc_, 
		  const basic_key_validator *, 
		  bool deletion_on_destroy_ = true); 
      
      properties (const basic_key_validator *, 
		  bool deletion_on_destroy_ = true); 

      // dtor
      virtual ~properties ();
  
      // methods:

    private: 

      void
      __check_nokey (const std::string & key_) const;

      void
      __check_key (const std::string & key_,
		  data ** data_);

      void
      __check_key (const std::string & key_,
		  const data ** data_) const;


    public: 

      vkeys
      keys () const;

      void
      keys (vkeys &) const;

      void
      lock (const std::string & key_);

      void
      unlock (const std::string & key_);

      bool
      is_locked (const std::string & key_) const;

      static std::string
      make_private_key (const std::string & key_);

      static bool
      key_is_private (const std::string & key_);

      static bool
      key_is_public (const std::string & key_);

      bool
      is_private (const std::string & key_) const;

      bool
      is_public (const std::string & key_) const;

      bool
      is_boolean (const std::string & key_) const;

      bool
      is_integer (const std::string & key_) const;

      bool
      is_real (const std::string & key_) const;

      bool
      is_string (const std::string & key_) const;
      
      bool
      is_scalar (const std::string & key_) const;

      bool
      is_vector (const std::string & key_) const;

      int32_t
      size (const std::string & key_) const;

      bool
      has_key (const std::string & key_) const;

      void
      erase (const std::string & key_);

      void
      clean (const std::string & key_);

      virtual void
      clear ();
 
      void
      store_flag (const std::string & key_, 
		  const std::string & desc_ = "",
		  bool lock_ = false);

      void
      store (const std::string & key_, 
	     bool value_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);

      void
      store_boolean (const std::string & key_, 
		     bool value_, 
		     const std::string & desc_ = "",
		     bool lock_ = false)
      {
	store (key_, value_, desc_, lock_); 
      }

      void
      store (const std::string & key_, 
	     int value_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);
      
      void
      store_integer (const std::string & key_, 
		     int value_, 
		     const std::string & desc_ = "",
		     bool lock_ = false)
      {
	store (key_, value_, desc_, lock_); 
      }

      void
      store (const std::string & key_, 
	    double value_, 
	    const std::string & desc_ = "",
	    bool lock_ = false);

      void
      store_real (const std::string & key_, 
		  double value_, 
		  const std::string & desc_ = "",
		  bool lock_ = false)
      {
	store (key_, value_, desc_, lock_); 
      }

      void
      store (const std::string & key_, 
	     const std::string & value_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);

      void
      store_string (const std::string & key_, 
		    const std::string & value_, 
		    const std::string & desc_ = "",
		    bool lock_ = false)
      {
	store (key_, value_, desc_, lock_); 
      }

      void
      store (const std::string & key_, 
	     const char * value_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);
      
      void
      store (const std::string & key_, 
	     const data::vbool & values_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);
      
      void
      store (const std::string & key_, 
	     const data::vint & values_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);
      
      void
      store (const std::string & key_, 
	     const data::vdouble & values_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);

      void
      store (const std::string & key_, 
	     const data::vstring & values_, 
	     const std::string & desc_ = "",
	     bool lock_ = false);
      
      void
      change (const std::string & key_, 
	      bool value_, 
	      int index_ = 0);

      void
      change_boolean (const std::string & key_ , 
		      bool value_ , 
		      int index_ = 0)
      {
	change (key_, value_, index_);
      }
      
      void
      change (const std::string & key_ , 
	      int value_ , 
	      int index_ = 0);
      
      void
      change_integer (const std::string & key_ , 
		      int value_ , 
		      int index_ = 0)
      {
	change (key_, value_, index_);
      }
      
      void
      change (const std::string & key_, 
	      double value_, 
	      int index_ = 0);
      
      void
      change_real (const std::string & key_ , 
		   double value_ , 
		   int index_ = 0)
      {
	change (key_, value_, index_);
      }
      
      void
      change (const std::string & key_, 
	      const std::string & value_, 
	      int index_ = 0);
      
      void
      change_string (const std::string & key_ , 
		     const std::string & value_ , 
		     int index_ = 0)
      {
	change (key_, value_, index_);
      }
      
      void
      change (const std::string & key_, 
	      const char * value_, 
	      int index_ = 0);

      void
      change (const std::string & key_, 
	      const data::vbool & values_);

      void
      change (const std::string & key_, 
	     const data::vint & values_);

      void
      change (const std::string & key_, 
	     const data::vdouble & values_);

      void
      change (const std::string & key_, 
	     const data::vstring & values_);

      /*****/

      void
      update_flag (const std::string & key_);

      void
      update (const std::string & key_, 
	     bool value_);

      void
      update_boolean (const std::string & key_, 
		      bool value_)
      {
	update (key_, value_);
      }

      void
      update (const std::string & key_ , 
	      int value_);

      void
      update_integer (const std::string & key_ , 
		      int value_)
      {
	update (key_, value_);
      }

      void
      update (const std::string & key_, 
	      double value_);

      void
      update_real (const std::string & key_, 
		   double value_)
      {
	update (key_, value_);
      }

      void
      update (const std::string & key_, 
	      const std::string & value_);

      void
      update_string (const std::string & key_, 
		     const std::string & value_)
      {
	update (key_, value_);
      }


      void
      update (const std::string & key_, 
	      const data::vbool & values_);

      void
      update (const std::string & key_, 
	      const data::vint & values_);

      void
      update (const std::string & key_, 
	      const data::vdouble & values_);

      void
      update (const std::string & key_, 
	      const data::vstring & values_);

      /*****/ 

      bool
      has_flag (const std::string & key_) const;

      void
      fetch (const std::string & key_, 
	     bool   & value_, 
	     int index_ = 0) const;
      
      void
      fetch (const std::string & key_, 
	     int    & value_, 
	     int index_ = 0) const;
      
      void
      fetch (const std::string & key_, 
	     double & value_, 
	     int index_ = 0) const;
      
      void
      fetch (const std::string & key_, 
	     std::string & value_, 
	     int index_ = 0) const;
      
      void
      fetch (const std::string & key_ , 
	     data::vbool & values_) const;
      
      void
      fetch (const std::string & key_, 
	     data::vint & values_) const;
      
      void
      fetch (const std::string & key_,
	     data::vdouble & values_) const;
      
      void
      fetch (const std::string & key_, 
	     data::vstring & values_) const;
      
      bool
      fetch_boolean (const std::string &,
		     int index_ = 0) const;
      
      bool
      fetch_boolean_scalar (const std::string & name_) const
      {
	return fetch_boolean (name_, 0);
      }

      bool
      fetch_boolean_vector (const std::string & name_ , int i_) const
      {
	return fetch_boolean (name_, i_);
      }

      int
      fetch_integer (const std::string &,
		     int index_ = 0) const;

      int
      fetch_integer_scalar (const std::string & name_) const
      {
	return fetch_integer (name_, 0);
      }

      int
      fetch_integer_vector (const std::string & name_ , int i_) const
      {
	return fetch_integer (name_, i_);
      }

      double
      fetch_real (const std::string &,
		 int index_ = 0) const;

      double
      fetch_real_scalar (const std::string & name_) const
      {
	return fetch_real (name_, 0);
      }

      double
      fetch_real_vector (const std::string & name_ , int i_) const
      {
	return fetch_real (name_, i_);
      }

      std::string
      fetch_string (const std::string &,
		    int index_ = 0) const;

      std::string
      fetch_string_scalar (const std::string & name_) const
      {
	return fetch_string (name_, 0);
      }

      std::string
      fetch_string_vector (const std::string & name_ , int i_) const
      {
	return fetch_string (name_, i_);
      }

      void
      dump (std::ostream &) const;

      void
      dump_def () const
      {
	dump (std::cout);
      }

      void dump_stderr () const
      {
	dump (std::cerr);
      }
      
      void dump_stdout () const
      {
	dump (std::cout);
      }

      virtual void
      tree_dump (std::ostream & out_         = std::cerr, 
		const std::string & title_  = "",
		const std::string & indent_ = "",
		bool inherit_               = false) const;

      std::string 
      key_to_string (const std::string & key_) const;

      std::string 
      key_to_property_string (const std::string & key_) const;


      virtual const std::string & 
      get_serial_tag () const;

    private:

      friend class boost::serialization::access; 
      template<class Archive>
      void
      serialize (Archive          & ar_ , 
		const unsigned int version_)
      {
	ar_ & boost::serialization::make_nvp ("description",__description);
	ar_ & boost::serialization::make_nvp ("properties",__props);
      }

    public:

      friend class config;

      class config
      {

      public:

	static const char DEFAULT_CONTINUATION_CHAR    = '\\';
	static const char DEFAULT_COMMENT_CHAR = '#';
	static const char DEFAULT_ASSIGN_CHAR  = '=';
	static const char DEFAULT_DESC_CHAR    = ':';
	static const char OPEN_VECTOR          = '[';
	static const char CLOSE_VECTOR         = ']';

	static const int MODE_BARE          = 0;
	static const int MODE_HEADER_FOOTER = 1;
	static const int MODE_DEFAULT       = MODE_HEADER_FOOTER;
	static const int mode_header_footer = MODE_HEADER_FOOTER;
	
	static const bool write_private_also;
	static const bool write_public_only;

      private:

	bool   __debug;
	int    __mode;
	bool   __use_smart_modulo;
	bool   __write_public_only;
	size_t __read_line_count;
	char   __continuation_char;
	char   __comment_char;
	char   __assign_char;
	char   __desc_char;

      private:

	void
	__read (std::istream & in_ , properties & p_);
	
      public:
	
	bool
	is_debug () const;
	
	void
	set_debug (bool debug_);

	// ctor
	config (bool use_smart_modulo_ = true,
	       int mode_ = MODE_DEFAULT,
	       bool write_public_only_ = write_public_only);

	// dtor
	virtual ~config ();

	void
	read (std::istream & in_, properties & p_);
	
	void
	write (std::ostream & out_, const properties & p_);
       
	static bool
	read_quoted_string (std::istream &,
			   std::string &);


      };

      static void
      write_config (const std::string & filename_, 
		    const properties & p_,
		    bool use_smart_modulo_ = true,
		    bool write_public_only_ = true);
      
      static void
      read_config (const std::string & filename_, 
		   properties & p_);

      static std::string 
      build_property_key (const std::string & prefix_ ,
			  const std::string & subkey_);
      
      //BOOST_CLASS_EXPORT_GUID (properties, "datatools::utils::properties")

    };

  } // end of namespace utils 

    //BOOST_CLASS_EXPORT_GUID (datatools::utils::properties, "datatools::utils::properties")
    //BOOST_CLASS_EXPORT_GUID (datatools::utils::properties::data, "datatools::utils::properties::data")

} // end of namespace datatools 

#endif // __datatools__utils__properties_h

// end of properties.h
