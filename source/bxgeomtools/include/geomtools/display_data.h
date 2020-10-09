/// \file geomtools/display_data.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_DISPLAY_DATA_H
#define GEOMTOOLS_DISPLAY_DATA_H 1

// Standard library:
#include <string>
#include <vector>
#include <map>
#include <list>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/properties.h>
// - Bayeux/geomtools :
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  class display_data :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable,
    public datatools::i_clear
  {
  public:

    enum display_type {
      DISPLAY_STATIC = 0,
      DISPLAY_FRAMED = 1
    };

    enum display_style_type {
      DISPLAY_STYLE_UNDEFINED = -1,
      DISPLAY_STYLE_WIRES  = 0
    };

    struct label_item {
      geomtools::placement posrot; //!< Placement of the text
      std::string          text;   //!< Text of the label
    };

    struct display_item {
      std::string                       frame_info;
      std::string                       style;
      std::string                       color;
      std::list<geomtools::polyline_3d> paths; //!< Obsolete
      wires_type                        wires; //!< Collection of polylines (in 'wires' style)
      //std::list<geomtools::facet_3d>  facets;  //!< Future?
      //std::list<geomtools::vector_3d> markers; //!< Future?
      //std::map<int,label_item>        labels;  //!< Future?
    public:
      display_item();
      virtual ~display_item();
      void reset();
      wires_type & grab_wires();
      DATATOOLS_SERIALIZATION_DECLARATION_NOINHERIT()
    };

    typedef std::map<int32_t, display_item> items_dict_type;

    struct display_entry
    {
      int32_t                         entry_type;
      std::string                     group;
      std::map<int32_t, display_item> items;
      datatools::properties           auxiliaries; //!< Auxiliary properties
    public:
      bool is_static() const;
      bool is_framed() const;
      display_entry();
      virtual ~display_entry();
      void reset();
      const std::map<int32_t, display_item> & get_items() const;
      std::map<int32_t, display_item> & grab_items();
      const display_item & get_static_item() const;
      const display_item & get_framed_item(int frame_index_) const;
      bool has_framed_item(int frame_index_) const;

      DATATOOLS_SERIALIZATION_DECLARATION_NOINHERIT()
    };

    typedef std::map<std::string, display_entry> entries_dict_type;

    void reset();

    /// Constructor
    display_data();

    /// Destructor
    ~display_data() override;

    void clear() override;

    const std::vector<std::string> & get_colors() const;

    std::vector<std::string> & grab_colors();

    const std::vector<std::string> & get_groups() const;

    std::vector<std::string> & grab_groups();

    const std::map<std::string, display_entry> & get_entries() const;

    std::map<std::string, display_entry> & grab_entries();

    const std::map<int32_t,std::string> & get_frames() const;

    std::map<int32_t,std::string> & grab_frames();

    void add_frame_info(int frame_index_, const std::string & frame_info_ = 0);

    const datatools::properties & get_auxiliaries() const;

    datatools::properties & grab_auxiliaries();


    void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const override;

    display_item & add_static_item(const std::string & name_,
                                   const std::string & group_ = "",
                                   const std::string & color_ = "");

    display_item & add_framed_item(const std::string & name_,
                                   int frame_,
                                   const std::string & group_ = "",
                                   const std::string & color_ = "",
                                   const std::string & frame_info_ = "");

    void process();

  protected:

    display_item & _add_item(const std::string & name_,
                             int entry_type_,
                             int frame_,
                             const std::string & group_,
                             const std::string & color_);

  private:

    std::vector<std::string>             _colors_;  //!< Collection of used colors
    std::vector<std::string>             _groups_;  //!< Collection of 'display' groups
    std::map<int32_t,std::string>        _frames_;  //!< Collection of frames and associated titles
    std::map<std::string, display_entry> _entries_; //!< Collection of display entities
    datatools::properties                _auxiliaries_; //!< Auxiliary properties

    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace geomtools

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/serialization/export.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
BOOST_CLASS_EXPORT_KEY2(geomtools::display_data, "geomtools::display_data")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::display_data::display_item, 2)
BOOST_CLASS_VERSION(geomtools::display_data, 1)

#endif // GEOMTOOLS_DISPLAY_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
