// -*- mode: c++; -*- 
/* color.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-23
 * Last modified: 2010-03-24
 * 
 * License: 
 * 
 * Description: 
 *  Utilities for color manipulation:
 *
 *
 *
 * History: 
 * 
 */

#ifndef __geomtools__color_h
#define __geomtools__color_h 1

#include <iostream>
#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>

namespace geomtools {

  class color
  {
  public:
   
    struct constants
    {
      std::string white;
      std::string black;
      std::string red;
      std::string green;
      std::string blue;
      std::string magenta;
      std::string cyan;
      std::string yellow;
      std::string orange;
      std::string grey;
      std::string default_color;

      constants ();

      static const constants & instance ();
    };

  public:
    int    code;
    std::string name;
    int    red_amount, green_amount, blue_amount;

  public:
    // ctor:
    color ();

    // ctor:
    color (int, const std::string &, int, int, int);

    /*** general color map ***/
    typedef std::map<std::string, int> color_map_type;

    class color_db
    {
    public:
      
      color_db ();
      
      virtual ~color_db ();

      //bool has_color (const string & id_) const;
      
      int get_color (const std::string & a_color_id) const;
      
      //void register_color (color, const string & id_);
      
      //void dump_colors (ostream & out_ = clog);

    private:

      color_map_type map_of_colors_;
     
    };
    
    typedef boost::scoped_ptr<color_db> scoped_color_db_type;
    
  public:
    
    static const color_db & get_color_db ();

    static int get_color (const std::string & a_name);

  };

}  // end of namespace geomtools

#endif // __geomtools__color_h

// end of color.h
