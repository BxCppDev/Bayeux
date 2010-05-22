// -*- mode: c++ ; -*- 
/* geom_id.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   Geometry ID
 * 
 * A geometry ID is composed of two parts:
 * - the TYPE is an integer representing the category of geometry object 
 *   each type object in a geometry setup has an unique category, thus a unique TYPE integer
 * - the ADDRESS is a list of integers representing the path to the object located in
 *   the hierarchy tree of the setup.
 *
 * History: 
 * 
 */

#ifndef __geomtools__geom_id_h
#define __geomtools__geom_id_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

#include <boost/cstdint.hpp>

#include <datatools/serialization/i_serializable.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace geomtools {

  using namespace std;

  class geom_id : 
    public datatools::serialization::i_serializable
  {
  public:
    static const string SERIAL_TAG;
    /*
    static const string GEOM_ID_PREFIX;
    static const string GEOM_ID_CATEGORY_PROPERTY;
    static const int    GEOM_ID_WORLD_TYPE;
    static const string GEOM_ID_WORLD_CATEGORY;
    */

    static const uint32_t INVALID_TYPE;
    static const uint32_t INVALID_ADDRESS;
    static const size_t   DEFAULT_ADDRESS_DEPTH;

    static const char IO_ID_OPEN;
    static const char IO_ID_SEPARATOR;
    static const char IO_ADDRESS_SEPARATOR;
    static const char IO_TYPE_INVALID;
    static const char IO_ADDRESS_INVALID;
    static const char IO_ID_CLOSE;

  private:  
    uint32_t         __type;
    vector<uint32_t> __address;

  public: 

    bool is_type (uint32_t) const;

    const uint32_t & get_type () const;
 
    void set_type (const uint32_t &);

    void set_depth (size_t depth_);

    size_t get_depth () const;

    uint32_t get (int) const;

    void set (int, uint32_t val_);
    
    void set_address (uint32_t si0_, 
		      uint32_t si1_ = INVALID_ADDRESS, 
		      uint32_t si2_ = INVALID_ADDRESS, 
		      uint32_t si3_ = INVALID_ADDRESS, 
		      uint32_t si4_ = INVALID_ADDRESS, 
		      uint32_t si5_ = INVALID_ADDRESS, 
		      uint32_t si6_ = INVALID_ADDRESS, 
		      uint32_t si7_ = INVALID_ADDRESS, 
		      uint32_t si8_ = INVALID_ADDRESS, 
		      uint32_t si9_ = INVALID_ADDRESS);

    // ctor:
    geom_id (); 

    // ctor:
    geom_id (uint32_t type_, 
	     uint32_t si0_, 
	     uint32_t si1_ = INVALID_ADDRESS, 
	     uint32_t si2_ = INVALID_ADDRESS, 
	     uint32_t si3_ = INVALID_ADDRESS, 
	     uint32_t si4_ = INVALID_ADDRESS, 
	     uint32_t si5_ = INVALID_ADDRESS, 
	     uint32_t si6_ = INVALID_ADDRESS, 
	     uint32_t si7_ = INVALID_ADDRESS, 
	     uint32_t si8_ = INVALID_ADDRESS, 
	     uint32_t si9_ = INVALID_ADDRESS);

    // dtor:
    virtual ~geom_id ();

    void reset ();

    void invalidate ();
    
    bool is_valid () const;

    void extract_to (geom_id & target_) const;

    void extract_from (const geom_id & source_);

    void inherits_from (const geom_id & source_);
     
    static int compare (const geom_id & id1_, const geom_id & id2_);

    bool operator== (const geom_id & id_) const;

    bool operator< (const geom_id & id_) const;

    bool operator<= (const geom_id & id_) const;

    bool operator>= (const geom_id & id_) const;

    bool operator> (const geom_id & id_) const;

    friend ostream & operator<< (ostream & out_, const geom_id & id_);

    friend istream & operator>> (istream & in_, geom_id & id_);

    virtual const string & get_serial_tag () const;

    void make (int type_, int depth_);
    
    static void make (geom_id & id_, int type_, int depth_);

  private:
    
    friend class boost::serialization::access; 
    template<class Archive>
    void serialize (Archive            & ar_ , 
		    const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("type", __type);
      ar_ & boost::serialization::make_nvp ("address", __address);
    }

  public:
    
    static bool sub_id_comp (uint32_t si1_, uint32_t si2_);

  };

} // end of namespace geomtools

#endif // __geomtools__geom_id_h

// end of geom_id.h
