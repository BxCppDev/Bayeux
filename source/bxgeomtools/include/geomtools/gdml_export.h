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
    static bool g_devel;
    static const std::string DEFAULT_LENGTH_UNIT;
    static const std::string DEFAULT_ANGLE_UNIT;
    static const std::string DEFAULT_DENSITY_UNIT;

  public:

    bool is_debug () const;

    void set_debug (bool d_);

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
                              const std::string & model_name_ = model_factory::DEFAULT_WORLD_LABEL);

  protected:

    // export solid
    virtual void _export_gdml_solid (const i_shape_3d & shape_,
                                     const std::string & log_name_);

    virtual void _export_gdml_logical (const logical_volume & factory_);

    virtual void _export_gdml_model (const i_model & model_);

    virtual void _export_gdml (std::ostream & out_,
                               const model_factory & factory_,
                               const std::string & model_name_);

  public:

    void attach_external_materials (const std::ostringstream & oss_);

  private:

    bool _debug_;
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
