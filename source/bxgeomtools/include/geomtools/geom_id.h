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


#include <iostream>
#include <vector>

#include <boost/cstdint.hpp>

#include <datatools/serialization/i_serializable.h>

namespace geomtools {

  // using namespace std;

  class geom_id : 
    DATATOOLS_SERIALIZABLE_CLASS
  {
  public:

    static const uint32_t INVALID_TYPE;
    static const uint32_t INVALID_ADDRESS;
    static const size_t   DEFAULT_ADDRESS_DEPTH;
    static const uint32_t UNIVERSE_TYPE;
    static const uint32_t WORLD_TYPE;

    static const char IO_ID_OPEN;
    static const char IO_ID_SEPARATOR;
    static const char IO_ADDRESS_SEPARATOR;
    static const char IO_TYPE_INVALID;
    static const char IO_ADDRESS_INVALID;
    static const char IO_ID_CLOSE;

  public: 

    bool is_type (uint32_t) const;

    uint32_t get_type () const;
 
    void set_type (uint32_t);

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

    void reset_address ();

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

    bool operator!= (const geom_id & id_) const;

    bool operator< (const geom_id & id_) const;

    bool operator<= (const geom_id & id_) const;

    bool operator>= (const geom_id & id_) const;

    bool operator> (const geom_id & id_) const;

    friend std::ostream & operator<< (std::ostream & out_, const geom_id & id_);

    friend std::istream & operator>> (std::istream & in_, geom_id & id_);

    void make (int type_, int depth_);
    
    static void make (geom_id & id_, int type_, int depth_);

  public:
    
    static bool sub_id_comp (uint32_t si1_, uint32_t si2_);

  private: 
 
    uint32_t              _type_;
    std::vector<uint32_t> _addresses_;

    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::geom_id, "geomtools::geom_id")

#endif // __geomtools__geom_id_h

// end of geom_id.h
