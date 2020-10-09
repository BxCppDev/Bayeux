/// \file genvtx/i_from_model_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-13
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *   Abstract base vertex generator from a geometry model addressed through some
 *   collection of geometry IDs extracted from a mapping object.
 *
 * History:
 *
 */

#ifndef GENVTX_I_FROM_MODEL_VG_H
#define GENVTX_I_FROM_MODEL_VG_H

// This project:
#include <genvtx/utils.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace geomtools {

  class id_selector;

}

namespace genvtx {

  /// \brief An abstract vertex generator based on a geometry model
  class i_from_model_vg : public i_vertex_generator
  {
  public:

    /// Constructor
    i_from_model_vg();

    /// Destructor
    ~i_from_model_vg() override;

    /// Check if origin definition is defined
    bool has_origin_def() const;

    /// Set the origin definition
    void set_origin_def(const origin_definition &);

    /// Return the origin definition
    const origin_definition & get_origin_def() const;

    /// Check if a mapping plugin name is defined
    bool has_mapping_plugin_name() const;

    /// Return the mapping plugin name
    const std::string & get_mapping_plugin_name() const;

    /// Set the mapping plugin name
    void set_mapping_plugin_name(const std::string & mpn_);

    /// Check if a materials plugin name is defined
    bool has_materials_plugin_name() const;

    /// Return the materials plugin name is defined
    const std::string & get_materials_plugin_name() const;

    /// Set the materials plugin name is defined
    void set_materials_plugin_name(const std::string & mpn_);

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// OCD support
    static void ocd_support(datatools::object_configuration_description &, const std::string & label_ = "");

  protected:

    void _set_defaults();

    void _initialize(const ::datatools::properties & ,
                     ::datatools::service_manager &,
                     ::genvtx::vg_dict_type &);

    void _reset();

    const geomtools::id_selector & _get_source_selector() const;

    void _initialize_source_selector();

  private:

    std::string       _mapping_plugin_name_;   //!< The name of the geometry 'mapping' plugin
    std::string       _materials_plugin_name_; //!< The name of the geometry 'materials' plugin
    origin_definition _origin_def_;     //!< Definition to select the physical volumes from where to generate vertexes

    // Pimplized:
    struct _work_type;
    boost::scoped_ptr<_work_type> _work_;

  };

} // end of namespace genvtx

#endif // GENVTX_I_FROM_MODEL_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
