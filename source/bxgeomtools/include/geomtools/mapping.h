/// \file geomtools/mapping.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_MAPPING_H
#define GEOMTOOLS_MAPPING_H 1

// Standard library:
#include <string>
#include <list>

// Third party library:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/bit_mask.h>
#include <datatools/multi_properties.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/geom_map.h>

namespace geomtools {

  // Forward declarations:
  class model_factory;
  class placement;
  class logical_volume;
  class geom_id;

  /// \brief The geometry ID mapping
  class mapping : public geom_map
  {
  public:

    //! Constant representing unlimited mapping depth
    static const size_t NO_MAX_DEPTH = 0;

    // \brief Constants
    struct constants {
      std::string MAPPING_PREFIX;
      std::string MAPPING_DAUGHTER_ID_PREFIX;
      constants ();
      static const constants & instance ();
    };

    // \brief Mothership modes
    enum build_mode_type {
      BUILD_MODE_STRICT_MOTHERSHIP = 0,
      BUILD_MODE_LAZY_MOTHERSHIP   = 1,
      BUILD_MODE_DEFAULT           = BUILD_MODE_STRICT_MOTHERSHIP
    };

    // \brief Mode for including/excluding some geoemtry categories
    enum mode_type {
      MODE_NONE     = 0,
      MODE_ONLY     = 1,
      MODE_EXCLUDED = 2
    };

    // Tools to manipulate 'mapping' properties:
    static std::string make_key (const std::string & flag_);

    static void extract (const datatools::properties & source_,
                         datatools::properties & target_);

    static bool has_flag (const datatools::properties & config_,
                          const std::string & flag_);

    static bool has_key (const datatools::properties & config_,
                         const std::string & key_);

    //! Check initialization flag
    bool is_initialized () const;

    //! Check that no inclusion/exclusion of geometry categories mode is set
    bool is_mode_none () const;

    //! Check that the inclusion mode is set
    bool is_mode_only () const;

    //! Check that the exclusion mode is set
    bool is_mode_excluded () const;

    //! Add a geometry category in the list of processed geometry categories
    void add_only (const std::string &);

    //! Add a geometry category in the list of excluded geometry categories
    void add_excluded (const std::string &);

    //! Return the build mode
    int get_build_mode () const;

    //! Set the build mode
    void set_build_mode (int bm_);

    //! Check the 'strict mothership' build mode
    bool is_build_mode_strict_mothership () const;

    //! Check the 'lazy mothership' build mode
     bool is_build_mode_lazy_mothership () const;

    //! Set the max mapping depth
    void set_max_depth (size_t max_depth_);

    //! Return the max mapping depth
    size_t get_max_depth () const;

    //! Default constructor
    mapping ();

    //! Destructor
    ~mapping () override;

    //! Configure the mapping
    void initialize (const datatools::properties & config_);

    //! Build the mapping information
    void build_from (const model_factory & factory_,
                             const std::string & mother_ = "world") override;

    //! Basic print of the embedded mapping dictionary
    void dump_dictionnary (std::ostream & out_ = std::clog) const;

    //! \brief Print flags
    enum smart_print_flags_type {
      PRINT_TITLE = datatools::bit_mask::bit00,
      PRINT_PAGER = datatools::bit_mask::bit01
    };

    //! Smart print
    void smart_print (std::ostream & out_ = std::clog,
                      const std::string & indent_ = "",
                      uint32_t flags_ = 0) const;

  private:

    //! Build the mappig disctionary
    void _build_ ();

    void _build_logical_children_ (const logical_volume & log_,
                                   const placement & mother_world_placement_,
                                   const geom_id & mother_id_);

    void _build_logical_children_2_ (const logical_volume & log_,
                                     const placement & mother_world_placement_,
                                     const std::vector<geom_id> & mother_ids_);

  private:

    // Status:
    bool                           _initialized_; //!< Initialization flag

    // Configuration:
    bool                           _world_mapping_; //!< World mapping flag
    size_t                         _max_depth_; //!< Max mapping depth
    int                            _build_mode_; //!< Build mode
    int                            _mode_; //!< Inclusion/exclusion mode
    std::list<std::string>         _only_excluded_list_; //!< List of the geometry categories to be included (only) or excluded

    // Working data:
    const model_factory  *         _factory_; //!< Handle to the geometry factory
    const logical_volume *         _top_logical_; //!< Handle to the top logical volume (world)
    size_t                         _depth_; //!< Running depth at build

    // Debug display utility:
    datatools::io::indenter        _indenter_; //!< Indenter for printing

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::mapping)

#endif // GEOMTOOLS_MAPPING_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
