/// \file geomtools/model_with_internal_mesh_tools.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 *                Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2015-07-06
 * Last modified: 2015-07-06
 *
 * License:
 *
 * Description:
 *
 *   Some tools to make a model owning internal items
 *   positioned on a regular 3D mesh within its mother shape
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MODEL_WITH_INTERNAL_MESH_TOOLS_H
#define GEOMTOOLS_MODEL_WITH_INTERNAL_MESH_TOOLS_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/regular_3d_mesh_placement.h>
#include <geomtools/detail/model_tools.h> // for models_col_type
#include <geomtools/physical_volume.h>

namespace geomtools {

  class i_model;

  //! \brief Handler to add daughter volumes on a regular 3D mesh in a mother logical volume
  class model_with_internal_mesh_data : public datatools::i_tree_dumpable
  {
  public:

    static const std::string INTERNAL_MESH_PREFIX;           // "internal_mesh."
    static const std::string INTERNAL_MESH_LABELS_KEY;       // "internal_mesh.labels"
    static const std::string INTERNAL_MESH_MODEL_PREFIX;     // "internal_mesh.model."
    static const std::string INTERNAL_MESH_RANK_PREFIX;      // "internal_mesh.rank."
    static const std::string INTERNAL_MESH_PLACEMENT_PREFIX; // "internal_mesh.placement."

  public:

    //! Mesh record
    struct mesh_type
    {
    public:

      /// Invalid mesh rank
      static const int INVALID_RANK = -1;

      /// Default constructor
      mesh_type();

      /// Destructor
      ~mesh_type();

      /// Set the mother logical volume
      void set_mother_logical(logical_volume &);

      /// Check if the mother logical is defined
      bool has_mother_logical() const;

      /// Return a const reference to the mother logical
      const logical_volume & get_mother_logical() const;

      /// Return a mutable reference to the mother logical
      logical_volume & grab_mother_logical();

      /// Set the internal daughter mesh label
      void set_label(const std::string &);

      /// Return the internal daughter mesh label
      const std::string & get_label() const;

      /// Set the internal daughter mesh rank
       void set_rank(int rank_);

      /// Return the internal daughter mesh rank
      int get_rank() const;

      /// Set the geometry model of the mesh node
      void set_model(const i_model &);

      /// Return the geometry model of the mesh node
      const i_model & get_model() const;

      void set_mesh_placement_config(const datatools::properties &);

      const datatools::properties & get_mesh_placement_config() const;

      void reset_mesh_placement();

      const regular_3d_mesh_placement & get_mesh_placement() const;

      const physical_volume & get_physical_volume() const;

      physical_volume & grab_physical_volume();

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization
      void initialize();

      /// Reset
      void reset();

    private:
      // Configuration parameters:
      std::string               _label_;
      logical_volume *          _mother_log_;
      const i_model *           _model_;
      int                       _rank_;
      datatools::properties     _mesh_placement_config_;

      // Internal data:
      regular_3d_mesh_placement * _mesh_placement_;
      physical_volume             _phys_;

    };

    typedef std::map<std::string, mesh_type> mesh_dict_type;

    /// Default constructor
    model_with_internal_mesh_data();

    /// Destructor
    ~model_with_internal_mesh_data() override;

    /// Set the logging priprity threshold
    void set_logging(datatools::logger::priority);

    /// Return the logging priprity threshold
    datatools::logger::priority get_logging() const;

    /// Return a const reference to the dictionary of internal meshes
    const mesh_dict_type & get_meshes() const;

    /// Return a mutable reference to the dictionary of internal meshes
    mesh_dict_type & grab_meshes();

    /// Return the number of meshes
    size_t get_number_of_meshes() const;

    /// Check if a mesh exists
    bool has_mesh(const std::string &) const;

    /// Return the mesh by name
    const mesh_type & get_mesh(const std::string &) const;

    /// Return the mesh by name
    mesh_type & grab_mesh(const std::string &);

    /// Add a mesh
    void add_mesh(const std::string & label_,
                  logical_volume & mother_log_,
                  const i_model & model_,
                  int rank_,
                  const datatools::properties & mesh_placement_config_
                  );

    /// Initialization
    void initialize();

    /// Reset
    void reset();

    /// Plug internal meshes in a mother logical volume
    void plug_internal_meshes(const datatools::properties & setup_,
                              logical_volume & log_,
                              models_col_type * models_);

    /*
    static void init_ocd(datatools::object_configuration_description & ocd_,
                         const std::string & prefix_ = "");
    */

    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

  private:

    datatools::logger::priority _logging_; ///< Logging priority
    mesh_dict_type _meshes_; //!< Collection of ranked meshes

  };

  // Type alias:
  // typedef model_with_internal_mesh_data MWIH;

} // end of namespace geomtools

#endif // GEOMTOOLS_MODEL_WITH_INTERNAL_MESH_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
