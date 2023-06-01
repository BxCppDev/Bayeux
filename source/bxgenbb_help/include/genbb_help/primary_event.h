/// \file genbb_help/primary_event.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2015-12-04
 *
 * License:
 * Copyright 2007-2015 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Primary event from the GENBB generator.
 *
 *
 * History:
 *
 * 2012-06-21 FM :
 *  - support 'weighted' MC event in case
 *    of user energy range for DBD events
 *  - serialization version 2 supports new 'weight' attributes
 *
 */

#ifndef GENBB_HELP_PRIMARY_EVENT_H
#define GENBB_HELP_PRIMARY_EVENT_H 1

// Standard library:
#include <string>
#include <list>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/primary_particle.h>

namespace genbb {

  /// \brief A primary event from a Monte-Carlo generator
  class primary_event
    : public datatools::i_serializable,
      public datatools::i_tree_dumpable
  {

  public:

    /// Collection of primary particles
    typedef std::list<primary_particle> particles_col_type;

  public:

    /// Check the validity of the primary event
    bool is_valid() const;

    /// Reset the primary event
    void reset();

    /// Check if time is defined
    bool has_time() const;

    /// Set the time
    void set_time(double);

    /// Return the time
    double get_time() const;

    /// Check if vertex is defined
    bool has_vertex() const;

    /// Set the vertex
    void set_vertex(const geomtools::vector_3d &);

    /// Return the time
    const geomtools::vector_3d & get_vertex() const;

    /// Set the weight
    void set_genbb_weight(double genbb_weight_);

    /// Return the weight
    double get_genbb_weight() const;

    /// Check if the event is weighted
    bool is_genbb_weighted() const;

    /// Return a const reference to the list of primary particles
    const particles_col_type & get_particles() const;

    /// Return a mutable reference to the list of primary particles
    particles_col_type & grab_particles();

    /// Add a primary particle
    void add_particle(const primary_particle &);

    /// Add a primary particle
    primary_particle & add_particle();

    /// Return the number of primary particles
    unsigned int get_number_of_particles() const;

    /// Return a const reference to primary particle at given index
    const primary_particle & get_particle(int) const;

    /// Return a mutable reference to primary particle at given index
    primary_particle & grab_particle(int);

    /// Return a const pointer to primary particle of a certain type with given occurence
    const primary_particle * get_particle_of_type(int type_, int occurence_ = 0) const;

    /// Remove particle
    void remove_particle(int type_, int occurence_ = 0);

    /// Check if label is set
    bool has_label() const;

    /// Return the label
    const std::string & get_label() const;

    /// Set the label
    void set_label(const std::string & l_);

    /// Reset the label
    void reset_label();

    /// Check if classification is set
    bool has_classification() const;

    /// Return the classification string
    const std::string & get_classification() const;

    /// Set the classification string
    void set_classification(const std::string & c_);

    /// Reset the classification string
    void reset_classification();

    /// Compute the classification string
    void compute_classification();

    /// Rotate the event
    void rotate(double a_phi, double a_theta, double a_delta);

    /// Return the total kinetic energy of the event
    double get_total_kinetic_energy() const;

    /// Return a const reference to the auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return a mutable reference to the auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Set auxiliary properties
    void set_auxiliaries(const datatools::properties &);

    /// Shift the time of the particles by a delay
    void shift_particles_time(double delta_time_, int from_ = 0);

    /// Default constructor
    primary_event();

    /// Destructor
    ~primary_event() override;

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    /// Smart print (deprecated)
    void tree_dump(std::ostream       & out_    = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const override;

    /// Print
    void dump(std::ostream & a_out = std::clog,
              const std::string & a_indent = "") const;

    /// Print
    void dump(std::ostream & a_out,
               const std::string & a_title,
               const std::string & a_indent) const;

    /// Assign generation Ids to all particle
    void assign_generation_ids();

    /// Remove generation Ids from all particle
    void remove_generation_ids();

    static const std::string & original_event_id_key();

  protected:

    /// Set default attibutes values
    void _set_defaults();

  private:

    double                _time_;           //!< The generation time (optional)
    geomtools::vector_3d  _vertex_;         //!< The event vertex (optional)
    particles_col_type    _particles_;      //!< The list of primary particles
    std::string           _label_;          //!< A label associated to the generated event
    std::string           _classification_; //!< A classification string (optional)
    double                _genbb_weight_;   //!< The weight of the generated event with respect to a reference sample
    datatools::properties _auxiliaries_;    //!< Auxiliary properties

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(primary_event)

    //! Support for backward compatibility serialization tag
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

    //! Reflection interface
    DR_CLASS_RTTI()

  };

} // end of namespace genbb

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(genbb::primary_event, "genbb::primary_event")

DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::genbb::primary_event)

// Activate reflection layer for the genbb::primary_event class :
DR_CLASS_INIT(::genbb::primary_event)

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_event, 5)

#endif // GENBB_HELP_PRIMARY_EVENT_H

// Local Variables: --
// mode: c++ --
// End: --
