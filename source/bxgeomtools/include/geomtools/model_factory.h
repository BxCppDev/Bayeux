// -*- mode: c++ ; -*- 
/* model_factory.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Factory for geometry models
 * 
 * History: 
 * 
 */

#ifndef __geomtools__model_factory_h
#define __geomtools__model_factory_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/detail/model_tools.h>
#include <geomtools/i_model.h>
#include <geomtools/logical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  class model_factory :
    public datatools::utils::i_tree_dumpable
  {
  public:

    static const string DEFAULT_WORLD_LABEL;

  private: 
    bool __locked;
    bool __debug;
    datatools::utils::multi_properties __mp;
    models_col_t           __models;
    logical_volume::dict_t __logicals;

  public: 

    bool is_locked () const;

    bool is_debug () const;

    void set_debug (bool);

    const models_col_t & get_models () const;

    const logical_volume::dict_t & get_logicals () const;
     
  public: 
    // ctor:
    model_factory (bool debug_ = false);

    // dtor:
    virtual ~model_factory ();
  
    void load (const string & mprop_file_);
  
    void lock ();
  
    void unlock ();

    void reset ();

  private:

    void __lock ();

    void __unlock ();

    void __construct ();

  public:
    
    virtual void tree_dump (std::ostream & out_         = clog, 
			    const std::string & title_  = "",
			    const std::string & indent_ = "",
			    bool inherit_               = false) const;
    
  public:
    
    /*** Object 3D getter ***/

    /*
    class getter : public i_object_3d::i_getter
    {
    private:
      model_factory * __factory;

    public:
      getter (const model_factory & factory_);

      virtual const i_object_3d * get (const datatools::utils::properties & params_);
      
    };
    */

  };

} // end of namespace geomtools

#endif // __geomtools__model_factory_h

// end of model_factory.h
