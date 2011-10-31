// -*- mode: c++ ; -*- 
/* i_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2010-02-12
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __genvtx__i_vertex_generator_h
#define __genvtx__i_vertex_generator_h 1

#include <boost/scoped_ptr.hpp>

#include <geomtools/utils.h>
#include <geomtools/detail/model_tools.h>

#include <mygsl/rng.h>

#include <genvtx/detail/vg_tools.h>

namespace genvtx {

  using namespace std;

  class i_vertex_generator
  {
  
    // ctor/dtor:
  public: 

    static bool g_debug;

    i_vertex_generator ();

    virtual ~i_vertex_generator ();

    void shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_);

    geomtools::vector_3d shoot_vertex (mygsl::rng & random_);

  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_) = 0;

  public:

    // pure virtual methods for vertex generator factory stuff:
    virtual string vg_id () const = 0;

    virtual vg_creator_type vg_creator () const = 0;

    /**************************************************/
    class vertex_generator_db
    {
      vertex_generator_creator_dict_type  _dict_;

    public:

      vertex_generator_db (bool = false);

      virtual ~vertex_generator_db ();

      const vertex_generator_creator_dict_type & get_dict () const;
      
      vertex_generator_creator_dict_type & get_dict ();
      
      bool has_vertex_generator (const string & vertex_generator_id_) const;
 
      vg_creator_type & get_vertex_generator (const string & vertex_generator_id_);

      void register_vertex_generator (vg_creator_type, const string & vertex_generator_id_);

      void dump_vertex_generators (ostream & out_ = clog);

    };
 
    typedef boost::scoped_ptr<vertex_generator_db> scoped_vertex_generator_db_type;

  private:

    static scoped_vertex_generator_db_type g_vertex_generator_db_;

  public:
      
    static vertex_generator_db & get_vertex_generator_db ();

    /**************************************************/

  protected:

    /* utility to enable auto-registering of some vertex generator 
     * in the global dictionary:
     *
     * The templatized class 'vg_t' must inherits from 'i_vertex_generator'
     * and implements the following methods:
     *  - string       vg_id () const
     *  - vg_creator_t vg_creator () const
     *
     */
    template <class vg_t>
    class creator_registration
    {
      vg_t _vg_;

    public:

      creator_registration ()
      {
        bool devel = g_debug;
        //devel = true;
        using namespace std;
        if (devel) clog << "DEVEL: i_vertex_generator::creator_registration::ctor: "
                        << "Entering..."
                        << endl;
        string vg_id = _vg_.vg_id ();
        if (devel) clog << "DEVEL: i_vertex_generator::creator_registration::ctor: "
                        << "vg_id='" << vg_id << "'"
                        << endl;
        
        vg_creator_type vg_creator = _vg_.vg_creator ();
        if (devel) clog << "DEVEL: i_vertex_generator::creator_registration::ctor: "
                        << "vg_creator='" << hex << (void *) vg_creator << dec << "'"
                        << endl;
    
        try
          { 
            bool test = false;
            //test = true;
            if (! test)
              {
                if (devel) 
                  {
                    clog << "DEVEL: i_vertex_generator::creator_registration::ctor: "
                         << "register_vg='" << vg_id << " @ " 
                         << hex << (void *) vg_creator << dec << "'"
                         << endl;
                  }
                i_vertex_generator::get_vertex_generator_db ().register_vertex_generator (vg_creator,
                                                                                          vg_id);
              }
          }
        catch (exception & x)
          {
            cerr << "i_vertex_generator::creator_registration::ctor: ERROR: " 
                 << x.what () << endl;
          }
        catch (...)
          {
            cerr << "i_vertex_generator::creator_registration::ctor: ERROR: " 
                 << "unexpected!" << endl;
          }
        if (devel) clog << "i_vertex_generator::creator_registration::ctor: "
                        << "Exiting."
                        << endl;
      }

    };

  };

} // end of namespace genvtx

#endif // __genvtx__i_vertex_generator_h

// end of i_vertex_generator.h
