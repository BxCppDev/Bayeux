// -*- mode: c++ ; -*- 
/* i_locator.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-09
 * Last modified: 2010-02-09
 * 
 * License: 
 * 
 * Description: 
 *   Geometry locator abstract interface
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_locator_h
#define __geomtools__i_locator_h 1

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;

  class i_locator 
  {
  private:
    bool __debug;
    bool __initialized;
    properties __parameters;

  public:

    bool is_debug () const;

    bool is_initialized () const;

    const properties & parameters () const;
    
    properties & parameters ();

    i_locator ();

    virtual ~i_locator ();

    void initialize (const properties & params_);
 
    void reset ();

  protected:

    virtual void _initialize (const properties & params_);
 
    virtual void _reset ();

  public:

    // the following pure abstract methods should be implemented!

    // check if 'id_' is valid:
    virtual bool validate_id (const geom_id & id_) const = 0;

    // find the 'id_' of the object from its category where position 'position_' lies in;
    // return invalid 'id_' if not found:
    virtual void find_id (geom_id & id_, const vector_3d & position_) const = 0;

    geom_id find_id (const vector_3d & position_) const;

    // get the geometry data related to object with id 'id_':
    // virtual void locate_id (const geom_id & id_, geom_data & gdata_) const = 0;

  };

} // end of namespace geomtools

#endif // __geomtools__i_locator_h

// end of i_locator.h
