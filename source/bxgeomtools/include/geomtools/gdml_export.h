
// -*- mode: c++ ; -*-
/* gdml_export.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_GDML_EXPORT_H_
#define GEOMTOOLS_GDML_EXPORT_H_ 1

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <list>

#include <datatools/utils.h>
#include <datatools/properties.h>

#include <geomtools/model_factory.h>
#include <geomtools/gdml_writer.h>

namespace geomtools {

  class gdml_export
  {
  public:
    static const std::string & default_length_unit();
    static const std::string & default_angle_unit();
    static const std::string & default_density_unit();

  public:

    datatools::logger::priority get_logging_priority () const;

    void set_logging_priority (datatools::logger::priority);

    bool is_auxiliary_supported () const;

    void add_auxiliary_support (bool s_ = true);

    bool is_replica_supported () const;

    void add_replica_support (bool s_ = true);

    bool has_fake_materials () const;

    void add_fake_materials(bool);

    const datatools::properties & parameters () const;

    datatools::properties & parameters ();

    // ctor:
    gdml_export ();

    // dtor:
    virtual ~gdml_export ();

    virtual void export_gdml (const std::string & filename_,
                              const model_factory & factory_,
                              const std::string & model_name_ = "");

    void attach_external_materials (const std::ostringstream & oss_);

  protected:

    // export solid
    virtual void _export_gdml_solid (const i_shape_3d & shape_,
                                     const std::string & log_name_);

    virtual void _export_gdml_logical (const logical_volume & factory_);

    virtual void _export_gdml_model (const i_model & model_);

    virtual void _export_gdml (std::ostream & out_,
                               const model_factory & factory_,
                               const std::string & model_name_);

  private:

    datatools::logger::priority _logging_priority_;

    datatools::properties _parameters_;
    gdml_writer           _writer_;
    const model_factory * _factory_;
    bool                  _fake_materials_;
    std::string           _length_unit_;
    std::string           _angle_unit_;
    std::string           _density_unit_;
    std::list<std::string> _solid_refs_;
    std::list<std::string> _volumes_refs_;
    const std::ostringstream * _external_materials_stream_;

    bool _support_replica_;
    bool _support_auxiliary_;

  };


} // end of namespace geomtools

#endif // GEOMTOOLS_GDML_EXPORT_H_

// end of gdml_export.h
