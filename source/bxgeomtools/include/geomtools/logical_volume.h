// -*- mode: c++ ; -*- 
/* logical_volume.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   Logical volume.
 * 
 * History: 
 * 
 */

#ifndef GEOMTOOLS_LOGICAL_VOLUME_H_
#define GEOMTOOLS_LOGICAL_VOLUME_H_ 1

#include <string>
#include <map>


#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

#include <geomtools/i_shape_3d.h>
#include <geomtools/material.h>

namespace geomtools {

  class physical_volume;

  class logical_volume
    : public datatools::i_tree_dumpable
  {
  public:
    typedef std::map<std::string, const physical_volume *> physicals_col_type;

    static const std::string HAS_REPLICA_FLAG;

    bool is_locked () const;

    void lock ();

    void unlock ();

    const std::string & get_name () const;

    void set_name (const std::string &);

    const datatools::properties & parameters () const;

    datatools::properties & parameters ();

    bool has_shape () const;

    void set_shape (const i_shape_3d &);

    void set_shape (const i_shape_3d *);

    const i_shape_3d & get_shape () const;

    const physicals_col_type & get_physicals () const;

    const physicals_col_type & get_real_physicals () const;

    logical_volume ();

    logical_volume (const std::string &);

    logical_volume (const std::string &, const i_shape_3d &);

    logical_volume (const std::string &, const i_shape_3d *);

    virtual ~logical_volume ();

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;

    bool has_physical (const std::string & name_) const;

    bool has_material_ref () const;

    std::string get_material_ref () const;

    void set_material_ref (const std::string & = "");

    const physical_volume & get_physical (const std::string & name_) const;
      
    void add_physical (const physical_volume & phys_, const std::string & name_ = "");

    bool is_replica () const;

    bool is_abstract () const;

    void set_abstract (bool a_);

  private:
    
    void _clear_shape_ ();
   
    void _init_defaults_ ();
   
    void _at_lock_ ();

    void _compute_real_physicals_ ();

  private:

    std::string           _name_;
    bool                  _locked_;
    datatools::properties _parameters_;
    bool                  _own_shape_;
    const i_shape_3d *    _shape_;
    physicals_col_type    _physicals_;
    bool                  _abstract_;
    physicals_col_type    _real_physicals_;

  public:

    typedef std::map<std::string, const logical_volume *> dict_type;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_LOGICAL_VOLUME_H_

// end of logical_volume.h
