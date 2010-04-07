// -*- mode: c++ ; -*- 
/* i_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *   Interface of a geometry model
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_model_h
#define __geomtools__i_model_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/logical_volume.h>
#include <geomtools/detail/model_tools.h>

namespace geomtools {
  
  using namespace std;  
  
  class i_model :
    public datatools::utils::i_tree_dumpable
  {
  public:
    static bool g_devel;
    static const string SOLID_SUFFIX;
    static const string LOGICAL_SUFFIX;
    static const string PHYSICAL_SUFFIX;
    
    typedef map <string, i_model *> models_col_t;
    
  private: 
    bool __debug;
    bool __constructed;
    datatools::utils::properties __parameters;
    string __name;
    
  protected:
    geomtools::logical_volume _logical;
    
  public: 

    void assert_constructed (const string & where_, 
		             const string & what_ = "") const;

    void assert_unconstructed (const string & where_, 
			       const string & what_ = "") const;

    bool is_constructed () const;
    
    bool is_debug () const;
    
    void set_name (const string & name_); 
    
    void set_debug (bool); 
    
    const string & get_name () const;
    
    const datatools::utils::properties & parameters () const;
    
    datatools::utils::properties & parameters ();
    
  public: 
    // ctor:
    i_model (const string & name_ = "");
    
    // dtor:
    virtual ~i_model ();
    
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
    
    const geomtools::logical_volume & get_logical () const;

    geomtools::logical_volume & get_logical ();
    
    virtual void construct (const string & name_,
			    const datatools::utils::properties & setup_,
			    models_col_t * models_ = 0);
 
    /**************************************************/
     
  protected:
    virtual void _pre_construct (datatools::utils::properties & setup_);
    virtual void _post_construct (datatools::utils::properties & setup_);
 
    virtual void _at_construct (const string & name_,
				const datatools::utils::properties & setup_,
				models_col_t * models_ = 0);
    

    /**************************************************/
      
  public:
    
    virtual string get_model_id () const = 0;
   
    class model_db
    {
      model_creator_dict_t  __dict;
      
    public:
      
      model_db (bool = false);
      
      virtual ~model_db ();
      
      const model_creator_dict_t & get_dict () const
      {
	return __dict;
      }
      
      model_creator_dict_t & get_dict ()
      {
	return __dict;
      }
      
      bool has_model (const string & model_id_) const;
      
      model_creator_t & get_model (const string & model_id_);
      
      void register_model (model_creator_t, const string & model_id_);
      
      void dump_models (ostream & out_ = clog);
      
    };
    
    typedef boost::scoped_ptr<model_db> scoped_model_db_t;
    
  private:
    
    static scoped_model_db_t g__model_db;
    
  public:
    
    static model_db & get_model_db ();
    
    /**************************************************/
    
  protected:
    
    /* utility to enable auto-registering of some geometry model 
     * in the global dictionary:
     *
     * The templatized class 'model_type' must inherits from 'i_model'
     * and thus implements the following methods:
     *  - string          get_model_id () const
     *  - model_creator_t get_model_creator () const
     *
     */
    template <class model_type>
    class creator_registration
    {
    private:

      model_type __model;
      
    public:

      static i_model * creator (const string & name_,
				const properties & configuration_,
				models_col_t * models_ = 0)
      {
	i_model * model = new model_type;
	model->construct (name_, configuration_, models_);
	return model;
      }

      creator_registration ()
      {
	bool devel = g_devel;
	//devel = true;
	using namespace std;
	if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
			<< "Entering..."
			<< endl;
	string model_id = __model.get_model_id ();
	if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
			<< "model_id='" << model_id << "'"
			<< endl;

	model_creator_t model_creator 
	  = i_model::creator_registration<model_type>::creator;
	if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
			<< "vg_creator='" << hex << (void *) model_creator << dec << "'"
			<< endl;
	
	try
	  { 
	    bool test = false;
	    //test = true;
	    if (! test)
	      {
		if (devel) 
		  {
		    clog << "DEVEL: i_model::creator_registration::ctor: "
			 << "register_vg='" << model_id << " @ " 
			 << hex << (void *) model_creator << dec << "'"
			 << endl;
		  }
		i_model::get_model_db ().register_model (model_creator, model_id);
	      }
	  }
	catch (exception & x)
	  {
	    cerr << "i_model::creator_registration::ctor: ERROR: " 
		 << x.what () << endl;
	  }
	catch (...)
	  {
	    cerr << "i_model::creator_registration::ctor: ERROR: " 
		 << "unexpected!" << endl;
	  }
	if (devel) clog << "i_model::creator_registration::ctor: "
			<< "Exiting."
			<< endl;
      }

    }; // template <class model_type> class creator_registration
    

    static string make_solid_name (const string & basename_);

    static string make_logical_volume_name (const string & basename_);

    static string make_physical_volume_name (const string & basename_);

    static string make_physical_volume_name_per_item (const string & basename_, 
						      int i_);

    static string make_physical_volume_name_per_item (const string & basename_, 
						      int i_, int j_);

    static string make_physical_volume_name (const string & basename_,
					     int nitems_);

    static string make_physical_volume_name (const string & basename_,
					     int ncols_,
					     int nrows_);

  }; // class i_model
  
} // end of namespace snemo

#endif // __geomtools__i_model_h

// end of i_model.h
