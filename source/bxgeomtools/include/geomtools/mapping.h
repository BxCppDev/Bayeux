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

#include <datatools/utils/ioutils.h>
#include <datatools/utils/bit_mask.h>

#include <geomtools/utils.h>
#include <geomtools/geom_map.h>

namespace geomtools {
  
  //using namespace std;

  class model_factory;
  class placement;
  class logical_volume;
  class geom_id;

  class mapping : public geom_map
  {
  public:

    static bool g_devel;
    static const size_t      NO_MAX_DEPTH = 0;
    static const std::string MAPPING_PREFIX;

    enum mode_t
      {
        MODE_NONE     = 0,
        MODE_ONLY     = 1,
        MODE_EXCLUDED = 2
      };

  public:

    // tools to manipulate 'mapping' properties:
    static std::string make_key (const std::string & flag_);

    static void extract (const datatools::utils::properties & source_,
                         datatools::utils::properties & target_);

    static bool has_flag (const datatools::utils::properties & config_,
                          const std::string & flag_);

    static bool has_key (const datatools::utils::properties & config_,
                         const std::string & key_);
 
    bool is_initialized () const;
    bool is_mode_none () const;
    bool is_mode_only () const;
    bool is_mode_excluded () const;
    void add_only (const std::string &);
    void add_excluded (const std::string &);

    void set_max_depth (size_t max_depth_);

    size_t get_max_depth () const;

    mapping ();

    virtual ~mapping ();

    void initialize (const datatools::utils::properties & config_);
    
    virtual void build_from (const model_factory & factory_,
                             const std::string & mother_ = "world");

  private:

    void _build_ ();

    void _build_logical_children_ (const logical_volume & log_, 
                                   const placement & mother_world_placement_,
                                   const geom_id & mother_id_);

  public:

    void dump_dictionnary (std::ostream & out_ = std::clog) const;

    enum smart_print_flags_type
      {
        PRINT_TITLE = datatools::utils::bit_mask::bit00,
        PRINT_PAGER = datatools::utils::bit_mask::bit01,
      };

    void smart_print (std::ostream & out_ = std::clog, 
                      const std::string & indent_ = "", 
                      uint32_t flags_ = 0) const;

    void test ();

  private:

    bool                           _initialized_;
    const model_factory  *         _factory_;
    const logical_volume *         _top_logical_;
    size_t                         _depth_; //!< Running depth at build

    size_t                         _max_depth_;
    int                            _mode_;
    std::list<std::string>         _only_excluded_list_;
    
    // debug display utility:
    datatools::utils::io::indenter _indenter_;

  };

} // end of namespace geomtools

#endif // __geomtools__mapping_h

// end of mapping.h
