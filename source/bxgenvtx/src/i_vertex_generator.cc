// -*- mode: c++ ; -*- 
/* i_vertex_generator.cc
 */

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  using namespace std;
  
  bool i_vertex_generator::g_debug = false;

  i_vertex_generator::scoped_vertex_generator_db_t i_vertex_generator::g__vertex_generator_db (new vertex_generator_db (true));

  /***************************************************/

  i_vertex_generator::vertex_generator_db::vertex_generator_db (bool test_)
  {
    bool debug = g_debug;
    //debug = true;
    if (debug)
      {
	clog << "******* DEVEL: i_vertex_generator::vertex_generator_db::vertex_generator_db: entering: " 
	     << (test_? "TEST": "-") << endl;
      }
    return;
  }

  i_vertex_generator::vertex_generator_db::~vertex_generator_db ()
  {
    bool debug = g_debug;
    //debug = true;
    if (debug)
      {
	clog << "******* DEVEL: i_vertex_generator::vertex_generator_db::~vertex_generator_db: entering..." 
	     << endl;
      }
    return;
  }
 
  bool i_vertex_generator::vertex_generator_db::has_vertex_generator (const string & vertex_generator_id_) const
  {
    return __dict.find (vertex_generator_id_) != __dict.end ();
  }

  vg_creator_t & 
  i_vertex_generator::vertex_generator_db::get_vertex_generator (const string & vertex_generator_id_)
  {
    vertex_generator_creator_dict_t::iterator found = __dict.find (vertex_generator_id_);
    if (found == __dict.end ())
      {
	ostringstream message;
	message << "i_vertex_generator::vertex_generator_db::get: "
		<< "No vertex_generator creator with ID='" << vertex_generator_id_ << "'!";
	throw runtime_error (message.str ());
      }
    return (found->second);
  }

  void i_vertex_generator::vertex_generator_db::register_vertex_generator (vg_creator_t creator_, 
					const string & vertex_generator_id_)
  {
    bool devel = g_debug;
    //devel = true;
    using namespace std;
    string vertex_generator_id = vertex_generator_id_;
    if (devel)
      {
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: "
	     << "vertex_generator_id='" << vertex_generator_id << "'"
	     << endl;
      }
    if (has_vertex_generator (vertex_generator_id))
      {
	ostringstream message;
	message << "i_vertex_generator::vertex_generator_db::register_vertex_generator: " 
		<< "Vertex_Generator ID '" << vertex_generator_id_ << "' is already used "
		<< "within the vertex_generator factory dictionnary!";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: "
	     << "new '" << vertex_generator_id << "' vertex_generator ID!"
	     << endl;
      }
    
    if (vertex_generator_id.empty ())
      {
	ostringstream message;
	message << "i_vertex_generator::vertex_generator_db::register_vertex_generator: " 
		<< "Empty vertex_generator ID!";
	throw runtime_error (message.str ());
      }
    if (devel)
      {
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: "
	     << "insert vertex_generator ID='" << vertex_generator_id << "'!"
	     << endl;
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: "
	     << "with creator address='" << hex 
	     << (void *) creator_ << dec << "'"
	     << endl;
      }
    pair<string, vg_creator_t> a_pair;
    a_pair.first = vertex_generator_id;
    a_pair.second = creator_;
    __dict.insert (a_pair);
    //__dict[vertex_generator_id] = creator_;
    size_t sz = get_dict ().size ();
    if (devel)
      {
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: size="
	     << sz << " element" << (sz > 1? "s": "") << endl;
	clog << "DEVEL: i_vertex_generator::vertex_generator_db::register_vertex_generator: "
	     << "done."
	     << endl;
      }
  }

  void i_vertex_generator::vertex_generator_db::dump_vertex_generators (ostream & out_)
  {
    out_ << "List of vertex_generator creators in 'vertex_generator_db::__dict': ";
    size_t sz = get_dict ().size ();
    out_ << sz << " element(s)" << endl;
    size_t count = 0; 
    for (vertex_generator_creator_dict_t::const_iterator it = get_dict ().begin ();
	 it != get_dict ().end ();
	 it++) 
      {
	count++;
	if (count == sz) out_ << "`-- "; 
	else out_ << "|-- ";
	out_ << it->first << ": " 
	     << hex << (void *) it->second 
	     << dec << endl;
      }
    out_ << "end." << endl;
  }

  i_vertex_generator::i_vertex_generator ()
  {
  }

  i_vertex_generator::~i_vertex_generator ()
  {
  }

  void i_vertex_generator::shoot_vertex (mygsl::rng & random_, 
					 geomtools::vector_3d & vertex_)
  {
    _shoot_vertex (random_, vertex_);
  }

  geomtools::vector_3d i_vertex_generator::shoot_vertex (mygsl::rng & random_)
  {
    geomtools::vector_3d vertex;
    shoot_vertex (random_, vertex);
    return vertex;
  }
     
  i_vertex_generator::vertex_generator_db & 
  i_vertex_generator::get_vertex_generator_db ()
  {
    if (! g__vertex_generator_db) 
      {
	throw runtime_error ("i_vertex_generator::get_vertex_generator_db: Library build critical bug !");
      }
    return *(g__vertex_generator_db.get ());
  }

} // end of namespace genvtx

// end of i_vertex_generator.cc
