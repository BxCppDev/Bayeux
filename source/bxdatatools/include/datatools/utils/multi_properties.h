// -*- mode: c++; -*-
/* multi_properties.h */

#ifndef __datatools__utils__multi_properties_h
#define __datatools__utils__multi_properties_h 1

#include <string>
#include <fstream>
#include <stdexcept>
#include <map>
#include <string>

#include <datatools/utils/properties.h>

#include <boost/cstdint.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>

#include <datatools/serialization/i_serializable.h>

using namespace std;

namespace datatools {

  namespace utils {

    class multi_properties :    
	  public datatools::serialization::i_serializable  
      {
      public:
	static const char OPEN;
	static const char CLOSE;
	static const char COMMENT;
	static const string DEFAULT_KEY_LABEL;
	static const string DEFAULT_META_LABEL;
	static const bool with_header_footer;
	static const bool without_header_footer;
	static const bool write_public_only;
	static const bool write_private_also;
	static const bool read_public_only;
	static const bool read_private_also;
	static const string SERIAL_TAG;

	static bool g_debug;

      public:
	class entry :    
	  public datatools::serialization::i_serializable  
	{
	public:
	  static const string SERIAL_TAG;

	private:
	  string     __key;
	  string     __meta;
	  properties __properties;

	public:

	  const properties & get_properties () const;

	  properties & get_properties ();

	  const string & get_key () const;

	  void set_key (const string &);

	  const string & get_meta () const;

	  void set_meta (const string &);

	  bool has_meta () const;

	  entry (const string & key_ = "", 
		 const string & meta_ = "");

	  virtual ~entry ();

	private:

	  virtual const string & get_serial_tag () const;
	  
	  friend class boost::serialization::access; 
	  template<class Archive>
	  void
	  serialize (Archive            & ar_ , 
		     const unsigned int   version_)
	  {
	    ar_ & boost::serialization::make_nvp ("key", __key);
	    ar_ & boost::serialization::make_nvp ("meta", __meta);
	    ar_ & boost::serialization::make_nvp ("properties", __properties);
	  }

	};

      public:

	typedef map<string, entry> entries_col_t;

      private:
	
	bool          __debug;
	string        __description;
	string        __key_label;
	string        __meta_label;
	entries_col_t __entries;

      public:

	bool is_debug () const;

	void set_debug (bool = true);

	void set_description (const string & description_);

	const string & get_description () const;

	void set_key_label (const string & key_label_);

	const string & get_key_label () const;

	void set_meta_label (const string & meta_label_);

	const string & get_meta_label () const;
	
	void reset ();

	multi_properties (const string & key_label_ = "", 
			  const string & meta_label_ = "", 
			  const string & description_ = "", 
			  bool debug_ = false);

	virtual ~multi_properties ();

	void dump (ostream & out_) const;

      private:

	void __remove (const string & key_);

	void __add (const string & key_, 
		    const string & meta_ = "");

      public:
	virtual const string & get_serial_tag () const;

      private:
	
	friend class boost::serialization::access; 
	template<class Archive>
	void
	serialize (Archive          & ar_ , 
		   const unsigned int version_)
      {
	ar_ & boost::serialization::make_nvp ("description", __description);
	ar_ & boost::serialization::make_nvp ("key_label",  __key_label);
	ar_ & boost::serialization::make_nvp ("meta_label",  __meta_label);
	ar_ & boost::serialization::make_nvp ("entries",  __entries);
      }

      public:

	const entry & get (const string & key_) const;

	entry & get (const string & key_);

	bool has_key (const string & key_) const;

	void add (const string & key_, 
		  const string & meta_,
		  const properties & props_);

	void add (const string & key_, 
		  const properties & props_);

	void add (const string & key_, 
		  const string & meta_ = "");

	void remove (const string & key_);

	void write (const string & filename_,
		    bool header_footer_ = true,
		    bool write_private_ = false) const;
	
	void read (const string & filename_,
		   bool skip_private_);

      private:
	
	void __read (istream & in_, bool skip_private_);
	
      };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__multi_properties_h

/* end of multi_properties.h */
