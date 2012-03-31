// -*- mode: c++ ; -*- 
/* i_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
  Description: 
 *   Interface of a geometry model
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_model_h
#define __geomtools__i_model_h 1

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

#include <geomtools/logical_volume.h>
#include <geomtools/detail/model_tools.h>

namespace geomtools {
  
  using namespace std;  
  
  class i_model : public datatools::utils::i_tree_dumpable
  {
  public:
    static bool g_devel;
    static const std::string SOLID_SUFFIX;
    static const std::string LOGICAL_SUFFIX;
    static const std::string PHYSICAL_SUFFIX;
    static const std::string PHANTOM_SOLID_FLAG;

    //static const std::string DEFAULT_WORLD_NAME;
    
    typedef geomtools::models_col_t models_col_t;
     
  public: 

    void assert_constructed (const std::string & where_, 
                             const std::string & what_ = "") const;

    void assert_unconstructed (const std::string & where_, 
                               const std::string & what_ = "") const;

    bool is_constructed () const;
    
    bool is_debug () const;

    bool is_phantom_solid () const;
    
    void set_name (const std::string & name_); 
    
    void set_debug (bool); 
    
    const std::string & get_name () const;
    
    const datatools::utils::properties & parameters () const;
    
    datatools::utils::properties & parameters ();

  protected :
 
    void _set_phantom_solid (bool);
   
  public: 

    // ctor:
    i_model (const std::string & name_ = "");
    
    // dtor:
    virtual ~i_model ();
    
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
    
    const geomtools::logical_volume & get_logical () const;

    geomtools::logical_volume & get_logical ();
    
    virtual void construct (const std::string & name_,
                            const datatools::utils::properties & setup_,
                            models_col_t * models_ = 0);
 
    /**************************************************/
     
  protected:

    virtual void _pre_construct (datatools::utils::properties & setup_);

    virtual void _post_construct (datatools::utils::properties & setup_);
 
    virtual void _at_construct (const std::string & name_,
                                const datatools::utils::properties & setup_,
                                models_col_t * models_ = 0);
    
   
  private: 
    bool _debug_;
    bool _constructed_;
    datatools::utils::properties _parameters_;
    std::string _name_;
    
  protected:
    bool                      _phantom_solid;
    geomtools::logical_volume _logical;

    /**************************************************/
      
  public:
    
    virtual std::string get_model_id () const = 0;
   
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
      
      bool has_model (const std::string & model_id_) const;
      
      model_creator_t & get_model (const std::string & model_id_);
      
      void register_model (model_creator_t, const std::string & model_id_);
      
      void dump_models (std::ostream & out_ = std::clog);
      
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
     *  - std::string          get_model_id () const
     *  - model_creator_t get_model_creator () const
     *
     */
    template <class model_type>
    class creator_registration
    {
    private:

      model_type __model;
      
    public:

      static i_model * creator (const std::string & name_,
                                const datatools::utils::properties & configuration_,
                                models_col_t * models_ = 0)
      {
        i_model * model = new model_type;
        model->construct (name_, configuration_, models_);
        return model;
      }

      creator_registration ()
      {
        bool devel = g_devel;
        using namespace std;
        if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
                        << "Entering..."
                        << endl;
        std::string model_id = __model.get_model_id ();
        if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
                        << "model_id='" << model_id << "'"
                        << endl;

        model_creator_t model_creator 
          = i_model::creator_registration<model_type>::creator;
        if (devel) clog << "DEVEL: i_model::creator_registration::ctor: "
                        << "model creator='" << hex << (void *) model_creator << dec << "'"
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
                         << "register model='" << model_id << " @ " 
                         << hex << (void *) model_creator << dec << "'"
                         << endl;
                  }
                i_model::get_model_db ().register_model (model_creator, model_id);
              }
          }
        catch (std::exception & x)
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
    
  public:

    static std::string make_solid_name (const std::string & basename_);

    static std::string make_logical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name (const std::string & basename_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_, 
                                                           int i_);

    static std::string make_physical_volume_name_per_item (const std::string & basename_, 
                                                           int i_, int j_);

    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int nitems_);

    static std::string make_physical_volume_name (const std::string & basename_,
                                                  int ncols_,
                                                  int nrows_);

    static std::string extract_label_from_physical_volume_name (const std::string & physical_volume_name_);

  }; // class i_model
  
} // end of namespace snemo

#include <geomtools/model_macros.h>

#endif // __geomtools__i_model_h

// end of i_model.h
