/// \file geomtools/logical_volume.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2013-06-26
 *
 * License:
 *
 * Description:
 *   Logical volume.
 *
 */

#ifndef GEOMTOOLS_LOGICAL_VOLUME_H
#define GEOMTOOLS_LOGICAL_VOLUME_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/material.h>

namespace geomtools {

  // Forward declarations:
  class physical_volume;
  class i_model;
  class placement;

  /// \brief A logical geometry volume (ala GDML)
  class logical_volume
    : public datatools::i_tree_dumpable
  {
  public:

    /// Dictionary of (daughter) physical volumes
    typedef std::map<std::string, const physical_volume *> physicals_col_type;

    /// \brief Data structure resulting of the logical_volume::locate method
    class locate_result {
    public:
      /// Constructor
      locate_result();
      /// Reset
      void reset();
      /// Basic print
      void dump(std::ostream & out_ = std::clog, const std::string & indent_ = "") const;
      /// Set the shape domain flags
      void set_shape_domain_flags(uint32_t);
      /// Set the daughter name
      void set_daughter_name(const std::string &);
      /// Set the daughter physical volume
      void set_daughter(const physical_volume &);
      /// Set the daughter physical volume's placement index
      void set_daughter_placement_index(int);
      /// Set the position in the reference frame of the daughter volume
      void set_position_in_daughter(const vector_3d &);
      /// Return the shape domain flags
      uint32_t get_shape_domain_flags() const;
      /// Check if a daughter is available
      bool has_daughter() const;
      /// Set the daughter name
      const std::string & get_daughter_name() const;
      /// Return a const reference to the daughter
      const physical_volume & get_daughter() const;
      /// Check if the daughter placement index is set
      bool has_daughter_placement_index() const;
      /// Return the daughter physical volume's placement index
      int get_daughter_placement_index() const;
      /// Check if the position in the reference frame of the daughter volume is available
      bool has_position_in_daughter() const;
      /// Return the position in the reference frame of the daughter volume
      const vector_3d & get_position_in_daughter() const;
    private:
      uint32_t                _shape_domain_flags_;       //!< Shape domain flags
      std::string             _daughter_name_;            //!< Name/label of the daughter volume
      const physical_volume * _daughter_physical_;        //!< Handle to the daughter physical volume
      int                     _daughter_placement_index_; //!< Index ot the placement of the matching daughter volume
      vector_3d               _position_in_daughter_;     //!< The tested position in the reference frame of the daughter volume
    };

    /// Return the name of the flag that indicated a replica
    static const std::string & has_replica_flag();

    /// Check the lock flag
    bool is_locked () const;

    /// Lock the volume
    void lock ();

    /// Unlock the volume
    void unlock ();

    /// Check the name of the volume
    bool has_name() const;

    /// Return the name of the volume
    const std::string & get_name () const;

    /// Set the name of the volume
    void set_name (const std::string &);

    const datatools::properties & get_parameters () const;

    datatools::properties & grab_parameters ();

    bool has_shape () const;

    // Set external shape
    void set_shape (const i_shape_3d &);

    // Set owned shape
    void set_shape (const i_shape_3d *);

    const i_shape_3d & get_shape () const;

    const physicals_col_type & get_physicals () const;

    const physicals_col_type & get_real_physicals () const;

    /// Default constructor
    logical_volume ();

    /// Constructor
    logical_volume (const std::string &);

    /// Constructor
    logical_volume (const std::string &, const i_shape_3d &);

    /// Constructor
    logical_volume (const std::string &, const i_shape_3d *);

    /// Desctructor
    ~logical_volume () override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    bool has_physical (const std::string & name_) const;

    bool has_material_ref () const;

    std::string get_material_ref () const;

    void set_material_ref (const std::string & = "");

    const physical_volume & get_physical (const std::string & name_) const;

    void add_physical (const physical_volume & phys_, const std::string & name_ = "");

    bool is_replica () const;

    bool is_abstract () const;

    void set_abstract (bool a_);

    bool has_effective_shape() const;

    void set_effective_shape (const i_shape_3d &);

    const i_shape_3d & get_effective_shape () const;

    bool has_geometry_model() const;

    void set_geometry_model (const i_model &);

    const i_model & get_geometry_model () const;

    bool has_effective_material_ref () const;

    std::string get_effective_material_ref () const;

    void set_effective_material_ref (const std::string & = "");

    bool has_effective_relative_placement() const;

    void set_effective_relative_placement (const placement &);

    const placement & get_effective_relative_placement () const;

    uint32_t locate(const vector_3d & local_position_,
                    bool ignore_daugthers_ = false,
                    double tolerance_ = 0.0, double daughter_tolerance_ = 0.0,
                    locate_result * = 0) const;

    static bool same(const logical_volume & log1_, const logical_volume & log2_, uint32_t mode_ = 0);

  private:

    void _clear_shape_ ();

    void _init_defaults_ ();

    void _at_lock_ ();

    void _compute_real_physicals_ ();

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const;

  private:

    std::string           _name_;         //!< The name of the volume
    datatools::logger::priority _logging_priority_; //!< The logging priority threshold
    std::string           _material_ref_; //!< The name of the material
    bool                  _locked_;       //!< The lock flag
    datatools::properties _parameters_;   //!< The collection of auxiliary parameters
    bool                  _own_shape_;    //!< Shape owner flag
    const i_shape_3d *    _shape_ = nullptr;   //!< Shape handle
    physicals_col_type    _physicals_;    //!< Dictionary of daughter volumes
    bool                  _abstract_ = false;     //!< Abstract flag

    // Experimental:
    const i_model    *    _geo_model_ = nullptr;       //!< The geometry model
    const i_shape_3d *    _effective_shape_ = nullptr; //!< The effective shape to be taken into account for shape inspection
    const placement *     _effective_relative_placement_ = nullptr; //!< The effective relative placement to be taken into account for volume inspection
    std::string           _effective_material_ref_; //!< The name of the effective material to be taken into account for material inspection

    physicals_col_type    _real_physicals_; //!< Unused

  public:

    typedef std::map<std::string, const logical_volume *> dict_type;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_LOGICAL_VOLUME_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
