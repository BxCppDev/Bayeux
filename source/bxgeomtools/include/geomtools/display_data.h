// -*- mode: c++ ; -*-
/* display_data.h
 * Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-10-19
 * Last modified: 2012-10-20
 *
 * License:
 *
 * Description:
 *
 *   Generic display data for 3D wired rendering 
 *
 *
 * History:
 *
 */

#ifndef __geomtools__display_data_h
#define __geomtools__display_data_h 1

#include <string>
#include <vector>
#include <map>
#include <list>

// Boost  serialization hook :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>

// Interfaces :
#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/properties.h>

#include <geomtools/polyline_3d.h>

namespace geomtools {

  class display_data :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::utils::i_tree_dumpable,
    public datatools::utils::i_clear
  {
  public:

    enum display_type
      {
        DISPLAY_STATIC = 0,
        DISPLAY_FRAMED = 1,
      };

    struct display_item
    {
      std::string                       frame_info;
      std::string                       color;
      std::list<geomtools::polyline_3d> paths;
    public:
      display_item ();
      void reset ();
      DATATOOLS_SERIALIZATION_DECLARATION();
    };
 
    typedef std::map<int32_t, display_item> items_dict_type;

    struct display_entry
    {
      int32_t                         entry_type;
      std::string                     group;
      std::map<int32_t, display_item> items;
      datatools::utils::properties    auxiliaries;
    public:
      bool is_static () const;
      bool is_framed () const;
      display_entry ();
      void reset ();
      const std::map<int32_t, display_item> & get_items () const;
      std::map<int32_t, display_item> & grab_items ();
      const display_item & get_static_item () const;
      const display_item & get_framed_item (int frame_index_) const;
      bool has_framed_item (int frame_index_) const;

      DATATOOLS_SERIALIZATION_DECLARATION();
    };

    typedef std::map<std::string, display_entry> entries_dict_type;

    void reset ();

    // ctor:
    display_data ();

    // dtor:
    virtual ~display_data ();

    /*** interface i_clear ***/
    virtual void clear ();

    const std::vector<std::string> & get_colors () const;

    std::vector<std::string> & grab_colors ();

    const std::vector<std::string> & get_groups () const;

    std::vector<std::string> & grab_groups ();

    const std::map<std::string, display_entry> & get_entries () const;

    std::map<std::string, display_entry> & grab_entries ();

    const datatools::utils::properties & get_auxiliaries () const;

    datatools::utils::properties & grab_auxiliaries ();


    /*** interface i_tree_dumpable ***/
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    display_item & add_static_item (const std::string & name_, 
                                    const std::string & group_ = "",
                                    const std::string & color_ = "");

    display_item & add_framed_item (const std::string & name_, 
                                    int frame_,
                                    const std::string & group_ = "",
                                    const std::string & color_ = "");

  protected:
        
    display_item & _add_item (const std::string & name_,
                              int entry_type_,
                              int frame_,
                              const std::string & group_,
                              const std::string & color_);

  private:

    std::vector<std::string>             _colors_;
    std::vector<std::string>             _groups_;
    std::map<std::string, display_entry> _entries_;
    datatools::utils::properties         _auxiliaries_;

    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::display_data, "geomtools::display_data")

#endif //__geomtools__display_data_h_h

// end of display_data.h
