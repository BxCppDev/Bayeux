// -*- mode: c++ ; -*- 
/* base_io.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-03
 * Last modified: 2010-11-03
 * 
 * License: 
 * 
 * Description: 
 *
 *   A brio reader/writer base class
 * 
 * History: 
 * 
 */


#ifndef __brio__base_io_h
#define __brio__base_io_h 1

#include <string>
#include <list>
#include <locale>

//#ifdef IOFACTORY_USE_FPU
#include <boost/math/nonfinite_num_facets.hpp>
//#endif

#include <boost/filesystem.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <brio/utils.h>

#include <TFile.h>

namespace brio {

  using namespace std;

  class base_io : public datatools::utils::i_tree_dumpable
  { 
  public: 

    static bool g_devel;

    static const string PBA_LABEL; // not implemented yet
    static const string TEXT_LABEL;
    static const string QPBA_LABEL; // temporary solution

    enum format_t
      {
	FORMAT_UNDEFINED = 0,
	FORMAT_PBA       = 1, // not implemented yet
	FORMAT_TEXT      = 2,
	FORMAT_QPBA      = 100 // temporary solution
      };

    enum rw_t
      {
	RW_UNDEFINED = 0,
	RW_READ      = 1,
	RW_WRITE     = 2
      };

  public: 

    static int get_format (const string & format_str_);

    bool is_debug () const;

    void set_debug (bool);

    bool is_verbose () const;

    void set_verbose (bool);

    bool is_reading () const;

    bool is_writing () const;

    bool is_format_pba () const; // not implemented yet

    bool is_format_qpba () const; // temporary solution

    bool is_format_text () const;

    int get_format () const;

    void set_format (int format_);

    void set_format (const string & format_str_);
 
  public:

    virtual bool is_opened () const;

    int64_t get_number_of_entries (const string & label_ = "") const;

    int64_t get_current_entry (const string & label_ = "") const;

    const string & get_serialization_tag (const string & label_ = "") const;

    bool has_store (const string & label_) const;

    void select_store (const string & label_);

    bool has_automatic_store () const;

    void select_automatic_store ();

    void unselect_store ();

    base_io (int rw_);

    base_io (int rw_, int format_, bool verbose_, bool debug_);

    base_io (int rw_, bool verbose_, bool debug_);

    virtual ~base_io ();

    void get_list_of_stores (list<string> & list_) const;

    virtual void tree_dump (std::ostream & out_ = std::clog, 
			    const std::string & title_ = "", 
			    const std::string & indent_ = "", 
			    bool inherit_ = false) const;
   
    // File open/close operations:
    virtual void open (const string & filename_);

    virtual void close ();
     
  protected:

    virtual void _at_open (const string & filename_) = 0;

    void _only_if_opened (const string & where_) const;

    void _only_if_not_opened (const string & where_) const;

    void _set_default ();
   
    void _reset ();

    store_info * _get_store_info (const string & label_ = "");

    const store_info * _get_store_info (const string & label_ = "") const;

  private:
   
    bool __debug;
    bool __verbose;
    int  __format;

  protected:
    
    string            _filename;
    TFile *           _file;
    store_info_dict_t _store_infos;
    store_info *      _current_store;
    int               _rw;
    std::locale    *  _default_locale;
    std::locale    *  _locale;

  };

} // end of namespace brio

#endif // __brio_base_io_h

// end of base_io.h
