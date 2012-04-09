/* cut_tools.cc 
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */

#include <stdexcept>
#include <sstream>

#include <cuts/cut_tools.h>
#include <cuts/i_cut.h>

namespace cuts {

  using namespace std;

  /*
  // 2012-04-07, FM :  
  cut_tools::scoped_cut_creator_db_type 
  cut_tools::g_cut_creator_db_ (0);
  */

  /*
  // 2012-04-07, FM :  
  cut_tools::g_cut_creator_db_ (new cut_creator_db (true));
  */

  bool cut_tools::g_devel = false;

  // ctor :
  cut_creator_db::cut_creator_db (bool verbose_)
  {
    bool debug = cut_tools::g_devel;
    _verbose_ = verbose_;
    if (_verbose_)
      {
        clog << "VERBOSE: cuts::cut_creator_db::cut_creator_db: "
             << "Entering..." << endl;
      }
    return;    
  }
      
  // dtor :
  cut_creator_db::~cut_creator_db ()
  {
    if (_verbose_)
      {
        clog << "VERBOSE: cuts::cut_creator_db::~cut_creator_db: Entering... " 
             << endl;
      }
    return;
  }
        
  const cut_creator_dict_type & cut_creator_db::get_dict () const
  {
    return _dict_;
  }
      
  cut_creator_dict_type & cut_creator_db::get_dict ()
  {
    return _dict_;
  }
      
  bool cut_creator_db::has_cut_creator (const string & a_cut_id) const
  {
    return _dict_.find (a_cut_id) != _dict_.end ();
  }

  cut_creator_type & 
  cut_creator_db::get_cut_creator (const string & a_cut_id)
  {
    cut_creator_dict_type::iterator found = _dict_.find (a_cut_id);
    if (found == _dict_.end ())
      {
        ostringstream message;
        message << "cuts::cut_creator_db::get_cut_creator: "
                << "No cut creator with ID='" << a_cut_id << "'!";
        throw logic_error (message.str ());
      }
    return (found->second);
  }
       
  void cut_creator_db::register_cut_creator (cut_creator_type a_cut_creator, 
                                             const string & a_cut_id)
  {
    bool devel = cut_tools::g_devel;
    //devel = true;
    using namespace std;
    if (_verbose_)
      {
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator: "
             << "a_cut_id='" << a_cut_id << "'"
             << endl;
      }
   
    if (a_cut_id.empty ())
      {
        ostringstream message;
        message << "cuts::cut_creator_db::register_cut_creator: " 
                << "Empty cut creator ID !";
        throw logic_error (message.str ());
      }

    if (has_cut_creator (a_cut_id))
      {
        ostringstream message;
        message << "cuts::cut_creator_db::register_cut_creator: " 
                << "Cut creator ID '" << a_cut_id << "' is already used "
                << "within the cut factory dictionnary !";
        throw logic_error (message.str ());
      }
    if (_verbose_)
      {
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator: "
             << "new '" << a_cut_id << "' cut creator  ID !"
             << endl;
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator:  "
             << "insert cut creator ID='" << a_cut_id << "'!"
             << endl;
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator: "
             << "with creator address='" << hex 
             << (void *) a_cut_creator << dec << "'"
             << endl;
      }
    pair<string, cut_creator_type> a_pair;
    a_pair.first = a_cut_id;
    a_pair.second = a_cut_creator;
    _dict_.insert (a_pair);
    size_t sz = get_dict ().size ();
    if (_verbose_)
      {
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator: size="
             << sz << " element" << (sz > 1? "s": "") << endl;
        clog << "VERBOSE: cuts::cut_creator_db::register_cut_creator: "
             << "done."
             << endl;
      }
    return;
  }

  void cut_creator_db::dump_cut_creators (ostream & a_out)
  {
    a_out << "List of event record processing  cut creators from the library database: ";
    size_t sz = get_dict ().size ();
    a_out << sz << " element(s)" << endl;
    size_t count = 0; 
    for (cut_creator_dict_type::const_iterator it = get_dict ().begin ();
         it != get_dict ().end ();
         it++) 
      {
        count++;
        if (count == sz) a_out << "`-- "; 
        else a_out << "|-- ";
        a_out << it->first << ": " 
              << hex << (void *) it->second 
              << dec << endl;
      }
    a_out << "end." << endl;
    return;
  }
     
     
  // 2012-04-09 FM : 
  // g++ 4.5.2-8ubuntu4 -> g++ 4.6.169 :
  cut_creator_db & cut_tools::get_cut_creator_db ()
  {
    static scoped_cut_creator_db_type g_cut_creator_db (0);
    if (g_cut_creator_db.get () == 0) 
      {
        clog << "NOTICE: cuts::cut_creator_db::get_cut_creator_db: "
             << "First use of the 'system' cut creator DB." << endl;
        g_cut_creator_db.reset (new cut_creator_db (true));
      }
    return *(g_cut_creator_db.get ());
  }

  // 2012-04-09 FM : 
  /*
  cut_creator_db & cut_tools::get_cut_creator_db ()
  {
    if (g_cut_creator_db_ == 0) 
      {
        // g++ 4.5.2-8ubuntu4 -> g++ 4.6.169 :
        //throw runtime_error ("get_cut_creator_db: Library has a critical bug !");

        cut_tools::g_cut_creator_db_.reset (new cut_creator_db (true));
      }
    return *(g_cut_creator_db_.get ());
  }
  */

}  // end of namespace cuts

// end of cut_tools.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
