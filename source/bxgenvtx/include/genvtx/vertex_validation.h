/// \file genvtx/vertex_validation.h
/* Author(s) : Xavier Garrido <garrido@lal.in2p3.fr>
 *             Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-07-06
 * Last modified: 2014-07-13
 *
 * License: GPL 3.0
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GENVTX_VERTEX_VALIDATION_H
#define GENVTX_VERTEX_VALIDATION_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>
#include <datatools/object_configuration_description.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace datatools {
  // Forward declaration:
  class properties;
}

namespace cuts {
  // Forward declaration:
  class i_cut;
}

namespace geomtools {
  // Forward declarations:
  class logical_volume;
  class geom_info;
  class geom_id;
}

namespace genvtx {

  /// \brief The vertex validation
  class vertex_validation : public datatools::i_tree_dumpable
  {
  public:

    /// Object which embeds the geometry context of a given vertex
    /** It contains:
     *    - a reference to the logical volume refering to the originated vertex
     *    - the local candidate vertex position to be validated (in the logical volume frame)
     *    - the global candidate vertex position to be validated (in the world volume frame)
     */
    class geometry_context {
    public:

      /// Check geometry info
      bool has_ginfo() const;

      /// Check geometry id
      bool has_gid() const;

      /// Check logical volume
      bool has_logical_volume() const;

      /// Check local candidate vertex
      bool has_local_candidate_vertex() const;

      /// Check global candidate vertex
      bool has_global_candidate_vertex() const;

      /// Check geometry info
      const geomtools::geom_info & get_ginfo() const;

      /// Check geometry Id
      const geomtools::geom_id & get_gid() const;

      /// Return a non mutable reference to the logical volume
      const geomtools::logical_volume & get_logical_volume() const;

      /// Set the referenced geometry info
      void set_ginfo(const geomtools::geom_info &);

      /// Set the referenced geometry Id
      void set_gid(const geomtools::geom_id &);

      /// Set the referenced logical volume
      void set_logical_volume(const geomtools::logical_volume &);

      /// Return the non mutable referecence to the local candidate vertex
      const geomtools::vector_3d & get_local_candidate_vertex() const;

      /// Set the local candidate vertex
      void set_local_candidate_vertex(const geomtools::vector_3d &);

      /// Return the non mutable referecence to the global candidate vertex
      const geomtools::vector_3d & get_global_candidate_vertex() const;

      /// Set the global candidate vertex
      void set_global_candidate_vertex(const geomtools::vector_3d &);

      /// Default constructor
      geometry_context();

      /// Reset
      void reset();

      /// Check validity
      bool is_valid() const;

      /// Return a handle to the validator factory register (system singleton)
      static datatools::factory_register<cuts::i_cut> & validator_factory_system_register();

    private:

      const geomtools::geom_info * _ginfo_;                   //!< Handle to a geometry information
      const geomtools::geom_id * _gid_;                       //!< Handle to a geometry Id
      const geomtools::logical_volume * _log_volume_;         //!< Handle to a logical volume
      const geomtools::vector_3d * _global_candidate_vertex_; //!< Handle to the candidate vertex to be validated in the global frame
      geomtools::vector_3d _local_candidate_vertex_;          //!< Candidate vertex to be validated in the logical volume's frame
    };

    /// The default maximum number of tries
    static const size_t DEFAULT_MAX_NUMBER_OF_TRIES = 1000;

    /// \brief Status of the validation
    enum validate_status_type {
      VS_UNDEFINED = -1, //!< Vertex validation status is not defined
      VS_ACCEPTED  =  0, //!< Vertex is accepted
      VS_REJECTED  =  1, //!< Vertex is rejected
      VS_MAXTRIES  =  2  //!< Vertex validation reaches max number of tries
    };

    /// Default constructor
    vertex_validation();

    /// Destructor
    ~vertex_validation() override;

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Set the  maximum number of tries
    void set_maximum_number_of_tries(const size_t tries_);

    /// Return the maximum number of tries
    size_t get_maximum_number_of_tries() const;

    /// Check the vertex validator cut ownership
    bool is_owned_validator() const;

    /// Set the vertex validator by reference
    void set_validator(cuts::i_cut & validator_);

    /// Set the vertex validator by pointer
    void set_validator(cuts::i_cut * validator_);

    /// Reset the vertex validator
    void reset_validator();

    /// Check if there is a vertex validator
    bool has_validator() const;

    /// Check initialization status
    bool is_initialized() const;

    /// Initialize the validation system
    void initialize(const datatools::properties & config_);

    /// Reset the internals
    void reset();

    /// Reset the current geometry context
    void reset_geometry_context();

    /// Return a reference to the mutable current geometry context
    geometry_context & grab_geometry_context();

    /// Return a reference to the current geometry context
    const geometry_context & get_geometry_context() const;

    /// Smart print
    void tree_dump(std::ostream & out = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

    /// Validation
    validate_status_type validate();

    /// Reset the current number of tries
    void reset_number_of_tries();

    /// Return the current number of tries
    size_t get_number_of_tries() const;

    /// Check if the maximum number of tries is reached
    bool is_maximum_number_of_tries_reached() const;

    /// OCD support
    static void ocd_support(datatools::object_configuration_description &, const std::string & label_ = "");

  private:

    bool                        _initialized_;             //<! Initialization flag
    datatools::logger::priority _logging_;                 //<! Logging priority
    bool                        _owned_validator_;         //<! Flag to own the validator cut (if any)
    cuts::i_cut *               _validator_;               //<! Validator cut
    boost::scoped_ptr<geometry_context> _geo_context_;     //<! Current geometry context
    size_t                      _maximum_number_of_tries_; //<! Maximum number of tries before escaping
    size_t                      _number_of_tries_;         //<! Current number of tries

  };

} // end of namespace genvtx

#endif // GENVTX_VERTEX_VALIDATION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
