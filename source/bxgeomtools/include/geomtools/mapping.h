// -*- mode: c++ ; -*- 
/* mapping.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-21
 * Last modified: 2010-23-20
 * 
 * License: 
 * 
 * Description: 
 *   Default geometry mapping
 * 
 * History: 
 * 
 */

#ifndef __geomtools__mapping_h
#define __geomtools__mapping_h 1

#include <string>
#include <stdexcept>

#include <datatools/utils/ioutils.h>

#include <geomtools/utils.h>
#include <geomtools/geom_map.h>
#include <geomtools/physical_volume.h>

namespace geomtools {
  
  using namespace std;

  class mapping : public geom_map
  {
  public:
    static bool g_devel;

  private:
    const model_factory  * __factory;
    const logical_volume * __top_logical;

    size_t               __depth;
    datatools::utils::io::indenter __indenter;

  public:

    mapping ();

    virtual ~mapping ();
    
    virtual void build_from (const model_factory & factory_,
			     const string & mother_ = "world");

  private:

    void __build ();

    void __build_logical_children (const logical_volume & log_, 
				   const placement & mother_world_placement_,
				   const geom_id & mother_id_);

  public:

    void dump_dictionnary (ostream & out_) const;

    void test () 
    {
      namespace du = datatools::utils;
      ostringstream oss;
      __indenter++;
      oss << "test: " <<  du::io::devel << __indenter << endl;
      __indenter--;
    }

  };

} // end of namespace geomtools

#endif // __geomtools__mapping_h

// end of mapping.h
