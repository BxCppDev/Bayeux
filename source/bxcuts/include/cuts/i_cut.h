// -*- mode: c++ ; -*- 
/* i_cut.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-20
 * 
 * License: 
 */

/** 
 * Description: 
 * A pure abstract class that interface a generic cut
 *   on some arbitrary data.
 * 
 */

#ifndef __cuts__i_cut_h
#define __cuts__i_cut_h 1

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include <cuts/detail/cut_tools.h>

namespace cuts {

  class i_cut
  {
  public:

    static const bool ACCEPT;
    static const bool REJECT;

    static bool g_debug;

  private:

    void * __user_data;

  protected:

    void * _get_user_data ();


  public: 

    // ctor:
    i_cut ();

    // dtor:
    virtual ~i_cut ();
  
    bool has_user_data () const;

    virtual void set_user_data (void *);

    virtual void unset_user_data ();

    virtual bool accept (); 
 
    bool operator() ();

  protected:

    virtual void _prepare_cut ();

    virtual void _finish_cut ();

    virtual bool _accept () = 0; 

    /**************************************************/
    
  public:
    // pure virtual methods for vertex generator factory stuff:
    virtual string cut_id () const = 0;

    virtual cut_creator_t cut_creator () const = 0;

    /**************************************************/
    class cut_creator_db
    {
      cut_creator_dict_t  __dict;

    public:

      cut_creator_db (bool = false);

      virtual ~cut_creator_db ();

      const cut_creator_dict_t & get_dict () const
      {
	return __dict;
      }
	  
      cut_creator_dict_t & get_dict ()
      {
	return __dict;
      }

      bool has_cut_creator (const string & cut_id_) const;
 
      cut_creator_t & get_cut_creator (const string & cut_id_);

      void register_cut_creator (cut_creator_t, const string & cut_id_);

      void dump_cut_creators (ostream & out_ = clog);

    }; // cut_creator_db

    /**************************************************/
 
    typedef boost::scoped_ptr<cut_creator_db> scoped_cut_creator_db_t;

  private:

    static scoped_cut_creator_db_t g__cut_creator_db;

  public:
      
    static cut_creator_db & get_cut_creator_db ();

  protected:

    /* utility to enable auto-registering of some cut 
     * in the global dictionary:
     *
     * The templatized class 'cut_t' must inherits from 'i_cut'
     * and implements the following methods:
     *  - string        cut_id () const
     *  - cut_creator_t cut_creator () const
     *
     */
    template <class cut_t>
    class creator_registration
    {
      cut_t __cut;

    public:

      creator_registration ()
      {
	bool devel = g_debug;
	//devel = true;
	using namespace std;
	if (devel) clog << "DEVEL: i_cut::creator_registration::ctor: "
			<< "Entering..."
			<< endl;
	string cut_id = __cut.cut_id ();
	if (devel) clog << "DEVEL: i_cut::creator_registration::ctor: "
			<< "cut_id='" << cut_id << "'"
			<< endl;
	
	cut_creator_t cut_creator = __cut.cut_creator ();
	if (devel) clog << "DEVEL: i_cut::creator_registration::ctor: "
			<< "cut_creator='" << hex << (void *) cut_creator << dec << "'"
			<< endl;
    
	try
	  { 
	    bool test = false;
	    //test = true;
	    if (! test)
	      {
		if (devel) 
		  {
		    clog << "DEVEL: i_cut::creator_registration::ctor: "
			 << "register_cut_creator='" << cut_id << " @ " 
			 << hex << (void *) cut_creator << dec << "'"
			 << endl;
		  }
		i_cut::get_cut_creator_db ().register_cut_creator (cut_creator,
								   cut_id);
	      }
	  }
	catch (exception & x)
	  {
	    cerr << "ERROR: i_cut::creator_registration::ctor: " 
		 << x.what () << endl;
	  }
	catch (...)
	  {
	    cerr << "ERROR: i_cut::creator_registration::ctor: " 
		 << "unexpected!" << endl;
	  }
	if (devel) clog << "DEVEL: i_cut::creator_registration::ctor: "
			<< "Exiting."
			<< endl;
      }

    };
    
  };

} // end of namespace cuts

#endif // __cuts__i_cut_h

// end of dummy_cuts.h
