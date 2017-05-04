/// \file geomtools/gdml_export.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *   GDML export utility
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GDML_EXPORT_H
#define GEOMTOOLS_GDML_EXPORT_H 1

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
// #include <list>
#include <set>

// Third party:
// - Bayeux/datatools :
#include <datatools/utils.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/model_factory.h>
#include <geomtools/gdml_writer.h>

namespace geomtools {

  //! \brief GDML export class
  class gdml_export
  {
  public:
    static const std::string & default_length_unit();
    static const std::string & default_angle_unit();
    static const std::string & default_density_unit();

  public:

    //! Return the logging priority threshold
    datatools::logger::priority get_logging_priority () const;

    //! Set the logging priority threshold
    void set_logging_priority (datatools::logger::priority);

    //! Check the flag to support auxiliary
    bool is_auxiliary_supported () const;

    //! Set the flag to support auxiliary
    void add_auxiliary_support (bool s_ = true);

    //! Check the flag to support replica
    bool is_replica_supported () const;

    //! Set the flag to support replica
    void add_replica_support (bool s_ = true);

    //! Check the flag to use mock materials
    bool has_fake_materials () const;

    //! Set the flag to use mock materials
    void add_fake_materials(bool);

    //! Return a reference to the non mutable container of parameters
    const datatools::properties & parameters () const;

    //! Return a reference to the mutable container of parameters
    datatools::properties & parameters ();

    //! Default constructor
    gdml_export ();

    //! Destructor
    virtual ~gdml_export ();

    //! GDML export
    virtual void export_gdml (const std::string & filename_,
                              const model_factory & factory_,
                              const std::string & model_name_ = "");

    //! Attach an external GDML stream fro materials
    void attach_external_materials (const std::ostringstream & oss_);

  protected:

    //! GDML export of a solid
    virtual void _export_gdml_solid (const i_shape_3d & shape_,
                                     const std::string & log_name_);

    //! GDML export of a logical volume
    virtual void _export_gdml_logical (const logical_volume & log_vol__);


    //! GDML export of a geometry model
    virtual void _export_gdml_model (const i_model & model_);

    //! GDML export of the model factory, given the name of the top level model
    virtual void _export_gdml (std::ostream & out_,
                               const model_factory & factory_,
                               const std::string & model_name_);

  private:

    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    datatools::properties _parameters_; //!< Parameters
    gdml_writer           _writer_; //!< GDML writer
    const model_factory * _factory_; //!< Handle to the model factory
    bool                  _fake_materials_; //!< Flag for fake materials
    std::string           _length_unit_; //!< Current length unit
    std::string           _angle_unit_;  //!< Current angle unit
    std::string           _density_unit_; //!< Current density unit
    std::set<std::string> _solid_refs_; //!< Set of referenced solids
    std::set<std::string> _volume_refs_; //!< Set of references logical volumes
    const std::ostringstream * _external_materials_stream_; //!< Handle to an external materials GDML stream

    bool _support_replica_; //!< Flag to support native replica
    bool _support_auxiliary_; //!< Flag to support auxiliary properties

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_GDML_EXPORT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
