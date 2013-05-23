// -*- mode: c++ ; -*-
/* mapping.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-21
 * Last modified: 2010-23-20
 *
 * License:
 *
 * Description:
 *   Default geometry mapping: given a model factory (class model_factory)
 *   and a ID manager (class id_mgr), this geometry mapping dictionnary
 *   is auto generated.
 *
 * History:
 *
 */

#ifndef __geomtools__mapping_h
#define __geomtools__mapping_h 1

#include <string>
#include <list>

#include <datatools/ioutils.h>
#include <datatools/bit_mask.h>

#include <geomtools/utils.h>
#include <geomtools/geom_map.h>

namespace geomtools {

  class model_factory;
  class placement;
  class logical_volume;
  class geom_id;

  /// \brief The geometry ID mapping
  class mapping : public geom_map
  {
  public:

    static const size_t NO_MAX_DEPTH = 0;

    struct constants {
      std::string MAPPING_PREFIX;
      std::string MAPPING_DAUGHTER_ID_PREFIX;
      constants ();
      static const constants & instance ();
    };

    enum build_mode_type
      {
        BUILD_MODE_STRICT_MOTHERSHIP = 0,
        BUILD_MODE_LAZY_MOTHERSHIP   = 1,
        BUILD_MODE_DEFAULT           = BUILD_MODE_STRICT_MOTHERSHIP
      };

    enum mode_type
      {
        MODE_NONE     = 0,
        MODE_ONLY     = 1,
        MODE_EXCLUDED = 2
      };

  public:

    // tools to manipulate 'mapping' properties:
    static std::string make_key (const std::string & flag_);

    static void extract (const datatools::properties & source_,
                         datatools::properties & target_);

    static bool has_flag (const datatools::properties & config_,
                          const std::string & flag_);

    static bool has_key (const datatools::properties & config_,
                         const std::string & key_);

    bool is_initialized () const;
    bool is_debug () const;
    void set_debug(bool);
    bool is_mode_none () const;
    bool is_mode_only () const;
    bool is_mode_excluded () const;
    void add_only (const std::string &);
    void add_excluded (const std::string &);

    int get_build_mode () const;
    void set_build_mode (int bm_);
    bool is_build_mode_strict_mothership () const;
    bool is_build_mode_lazy_mothership () const;

    void set_max_depth (size_t max_depth_);

    size_t get_max_depth () const;

    mapping ();

    virtual ~mapping ();

    void initialize (const datatools::properties & config_);

    virtual void build_from (const model_factory & factory_,
                             const std::string & mother_ = "world");

    void dump_dictionnary (std::ostream & out_ = std::clog) const;

    enum smart_print_flags_type
      {
        PRINT_TITLE = datatools::bit_mask::bit00,
        PRINT_PAGER = datatools::bit_mask::bit01,
      };

    void smart_print (std::ostream & out_ = std::clog,
                      const std::string & indent_ = "",
                      uint32_t flags_ = 0) const;

  private:

    void _build_ ();

    void _build_logical_children_ (const logical_volume & log_,
                                   const placement & mother_world_placement_,
                                   const geom_id & mother_id_);

    void _build_logical_children_2_ (const logical_volume & log_,
                                     const placement & mother_world_placement_,
                                     const std::vector<geom_id> & mother_ids_);

  private:

    bool                           _initialized_;
    bool                           _debug_;
    const model_factory  *         _factory_;
    const logical_volume *         _top_logical_;
    bool                           _world_mapping_;
    size_t                         _depth_; //!< Running depth at build

    size_t                         _max_depth_;
    int                            _build_mode_;
    int                            _mode_;
    std::list<std::string>         _only_excluded_list_;

    // debug display utility:
    datatools::io::indenter _indenter_;

  };

} // end of namespace geomtools

#endif // __geomtools__mapping_h

// end of mapping.h
