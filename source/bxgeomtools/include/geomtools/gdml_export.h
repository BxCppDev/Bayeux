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

#ifndef __geomtools__gdml_export_h
#define __geomtools__gdml_export_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>

#include <geomtools/detail/model_tools.h>
#include <geomtools/model_factory.h>
#include <geomtools/gdml_writer.h>
#include <geomtools/units.h>
#include <geomtools/material.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/regular_linear_placement.h>

namespace geomtools {

  using namespace std;

  class gdml_export
  {
  public:
    static bool g_devel;
    static const string DEFAULT_LENGTH_UNIT;
    static const string DEFAULT_ANGLE_UNIT;

  private:
    bool __debug;
    datatools::utils::properties __parameters;
    gdml_writer           __writer;
    const model_factory * __factory;
    string       __length_unit;
    string       __angle_unit;
    list<string> __solid_refs;
    list<string> __volumes_refs;
    const ostringstream * __external_materials_stream;

    bool __support_replica;

  public:

    bool is_debug () const;

    void set_debug (bool d_);
  
    bool is_replica_supported () const;

    void add_replica_support (bool s_);

    const datatools::utils::properties & parameters () const;

    datatools::utils::properties & parameters ();

  public:
    // ctor:
    gdml_export ();

    // dtor:
    virtual ~gdml_export ();

    virtual void export_gdml (const string & filename_, 
			      const model_factory & factory_,
			      const string & model_name_ = model_factory::DEFAULT_WORLD_LABEL);

  protected:

    // export solid
    virtual void _export_gdml_solid (const i_shape_3d & shape_,
				     const string & log_name_);

    virtual void _export_gdml_logical (const logical_volume & factory_);

    virtual void _export_gdml_model (const i_model & model_);

    virtual void _export_gdml (ostream & out_, 
			       const model_factory & factory_,
			       const string & model_name_);

  public:
    void attach_external_materials (const ostringstream & oss_);

  public:


  };


} // end of namespace geomtools

#endif // __geomtools__gdml_export_h

// end of gdml_export.h
