/// \file emfield/multi_zone_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-04-19
 * Last modified: 2015-04-19
 *
 * License:
 *
 * Description:
 *
 *   Multi zone field
 *
 * History:
 *
 */

#ifndef EMFIELD_MULTI_ZONE_FIELD_H
#define EMFIELD_MULTI_ZONE_FIELD_H 1

// Standard library:
#include <string>
#include <vector>
#include <map>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/placement.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/shape_factory.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** \brief A class that computes the value of the EM field that depends on
   *         the position with respect to the coordinate system. Several
   *         regions are associated to several fields.
   */
  class multi_zone_field : public base_electromagnetic_field
  {
  public:

    enum zone_priority_type {
      ZONE_PRIORITY_INVALID   = -1,
      ZONE_PRIORITY_VERY_LOW  =  0,
      ZONE_PRIORITY_LOW       =  1,
      ZONE_PRIORITY_HIGH      =  2,
      ZONE_PRIORITY_VERY_HIGH =  3,
      ZONE_PRIORITY_MIN       =  ZONE_PRIORITY_VERY_LOW,
      ZONE_PRIORITY_MAX       =  ZONE_PRIORITY_VERY_HIGH,
      ZONE_PRIORITY_DEFAULT   =  ZONE_PRIORITY_MIN
    };

    /// Return the label associated to a given zone priority
    static std::string zone_priority_to_label(zone_priority_type);

    /// Return the zone priority associated to a given label
    static zone_priority_type label_to_zone_priority(const std::string &);

    /// \brief The information associated to a given zone
    class zone_field_entry {
    public:

      /// Default constructor
      zone_field_entry();

      /// Reset
      void reset();

    protected:

      /// Set default values to attributes
      void _set_defaults();

      /// Check if a zone field entry has higher priority than another one
      static bool _higher_zone_priority(const zone_field_entry * zfe1_,
                                        const zone_field_entry * zfe2_);

    private:

      std::string _label_; //!< Unique label of the association
      geomtools::placement _zone_positioning_; //!< Placement of the zone
      double _zone_tolerance_; //!< Tolerance of the zone
      const geomtools::i_shape_3d * _zone_shape_; //!< Shape of the zone
      const base_electromagnetic_field * _zone_field_; //!< Field associated to the zone
      bool _absolute_positioning_; //!< Flag for absolute positioning
      zone_priority_type _priority_; //!< Zone priority

      friend class multi_zone_field;
    };

    /// \brief Type alias for a dictionary of zone fields
    typedef std::map<std::string, zone_field_entry> zone_field_dict_type;

    /// Default constructor
    multi_zone_field(uint32_t = 0);

    /// Destructor
    ~multi_zone_field() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                            ::datatools::service_manager &,
                            ::emfield::base_electromagnetic_field::field_dict_type &) override;

    /// Reset
    void reset() override;

    /// Compute electric field
    int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const override;

    /// Compute magnetic field
    int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const override;

    /// Add a zone field
    void add_zone_field(const std::string & zone_label_,
                        const geomtools::placement & zone_positioning_,
                        const geomtools::i_shape_3d & zone_shape_,
                        double zone_tolerance_,
                        const base_electromagnetic_field & zone_field_,
                        bool absolute_positioning_ = true,
                        zone_priority_type priority_ = ZONE_PRIORITY_DEFAULT);

    /// Return the zone field entry associated to a given label
    const zone_field_entry & get_zone_field(const std::string & zone_label_) const;

    /// Find all zones where a position lies
    void fine_zones(const geomtools::vector_3d & position_,
                    std::vector<const zone_field_entry *> & zones_) const;

    /// Check the ownership flag for the shape factory
    bool owns_shape_factory() const;

    /// Check if the shape factory is set
    bool has_shape_factory() const;

    /// Reset the shape factory
    void reset_shape_factory();

    /// Set an external shape factory
    void set_shape_factory(const geomtools::shape_factory & shfact_);

    /// Create an embedded shape factory
    void create_shape_factory(const ::datatools::properties & setup_);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    bool _own_shape_factory_; //!< Ownership flag for the shape factory
    geomtools::shape_factory * _shape_factory_; //!< Handle to a shape factory
    zone_field_dict_type _zone_fields_; //!< Dictionary of zone fields

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(multi_zone_field)

  };

} // end of namespace emfield

#endif // EMFIELD_MULTI_ZONE_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
