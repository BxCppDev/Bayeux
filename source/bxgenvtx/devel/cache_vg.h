// -*- mode: c++ ; -*- 
/* cache_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-02-01
 * Last modified: 2012-02-01
 * 
 * License: 
 * 
 * Description: 
 *
 *   Vertex generator with cache file(s).
 * 
 * Idea:
 *   the first time it is run, the embedded vertex generator is used to shoot
 *   vertexes which are saved in parallel within cache files. The next time it is run,
 *   the embedded vertex generator is not used anymore but vertexes are loaded from
 *   the cache files.
 *
 * History: 
 * 
 */

#ifndef __genvtx__cache_vg_h
#define __genvtx__cache_vg_h 1

#include <genvtx/i_vertex_generator.h>
#include <datatools/utils/smart_filename.h>

namespace genvtx {

  class cache_vg : public i_vertex_generator
  {
  public: 

    bool has_vg () const;

    bool is_owned () const;

    const i_vertex_generator & get_vg () const;

    void set_vg (i_vertex_generator &);

    void set_vg (i_vertex_generator *);
  
  private:

    void _clear_vg_ ();

  public: 

    // ctor:
    cache_vg ();

    // dtor:
    cache_vg (const i_vertex_generator &, 
              const string & path_, 
              const string & basename_, 
              const string & extension_, 
              size_t capacity_);

    virtual ~cache_vg ();
  
  public:

    virtual string vg_id () const;

    virtual vg_creator_type vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_, 
					void * user_ = 0);
  
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
                                geomtools::vector_3d & vertex_);

  private: 

    datatools::utils::smart_filename _cache_;
    std::ifstream *      _fin_;
    std::ofstream *      _fout_;
    bool                 _owned_;
    i_vertex_generator * _vg_;
    size_t _counter_;

  private:

    static creator_registration<cache_vg> g_cr_;
  
  };

} // end of namespace genvtx

#endif // __genvtx__cache_vg_h

// end of cache_vg.h
