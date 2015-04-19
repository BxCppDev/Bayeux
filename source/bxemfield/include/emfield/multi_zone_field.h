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

    /// \brief The information associated to a given zone
    class zone_field_entry {
    public:

      /// Default constructor
      zone_field_entry();

      /// Reset
      void reset();

    private:

      std::string _label_;
      geomtools::placement _zone_positioning_;
      double _zone_tolerance_;
      const geomtools::i_shape_3d * _zone_shape_;
      const base_electromagnetic_field * _zone_field_;
      bool _absolute_positioning_;

      friend class multi_zone_field;
    };

    /// \brief Type alias for a dictionary of zone fields
    typedef std::map<std::string, zone_field_entry> zone_field_dict_type;

    /// Default constructor
    multi_zone_field(uint32_t = 0);

    /// Destructor
    virtual ~multi_zone_field();

    /// Initialization
    virtual void initialize(const ::datatools::properties &,
                            ::datatools::service_manager &,
                            ::emfield::base_electromagnetic_field::field_dict_type &);

    /// Reset
    virtual void reset();

    /// Compute electric field
    virtual int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const;

    /// Compute magnetic field
    virtual int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const;

    /// Add a zone field
    void add_zone_field(const std::string & zone_label_,
                        const geomtools::placement & zone_positioning_,
                        const geomtools::i_shape_3d & zone_shape_,
                        double zone_tolerance_,
                        const base_electromagnetic_field & zone_field_,
                        bool absolute_positioning_ = true);

    /// Return the zone field entry associated to a given label
    const zone_field_entry & get_zone_field(const std::string & zone_label_) const;

    /// Find all zones where a position lies
    void find_zone(const geomtools::vector_3d & position_,
                   std::vector<const zone_field_entry *> & zones_) const;

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    zone_field_dict_type _zone_fields_; //!< Dictionary of zone fields

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(multi_zone_field);

  };

} // end of namespace emfield

#endif // EMFIELD_MULTI_ZONE_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
