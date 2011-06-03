// -*- mode: c++; -*- 
/* multiple_placement.cc 
 */

#include <geomtools/multiple_placement.h>

#include <stdexcept>
#include <sstream>

namespace geomtools {

  using namespace std;

  bool multiple_placement::is_replica () const
  {
    return false;
  }
 
  bool multiple_placement::is_valid () const
  {
    return __placements.size () > 0;
  }

  void multiple_placement::invalidate ()
  {
    reset ();
    return;
  }

  void multiple_placement::add (const placement & p_)
  {
    __placements.push_back (p_);
    return;
  }
  
  const placement & multiple_placement::get_placement (int index_) const
  {
    return __placements.at (index_);
  }
  
  placement & multiple_placement::get_placement (int index_)
  {
    return __placements.at (index_);
  }
  
  size_t multiple_placement::get_number_of_items () const
  {
    return __placements.size ();
  }

  size_t multiple_placement::compute_index_map (vector<uint32_t> & map_, 
						int item_) const
  {
    if ((item_ < 0) || (item_ >= get_number_of_items ()))
      {
 	ostringstream message;
	message << "multiple_placement::compute_index_map: " 
		<< "Invalid item index '" << item_ << "' !" << endl;
	throw runtime_error (message.str ());
      }
    map_.clear ();
    map_.push_back (item_);
    return map_.size ();
  }
  
  void multiple_placement::get_placement (int item_, placement & p_) const
  {
    p_ = __placements[item_];
    return;
  }
  
  // ctor:  
  multiple_placement::multiple_placement ()
  {
    return;
  }
  
  // dtor:
  multiple_placement::~multiple_placement ()
  {
    reset ();
    return;
  }
  
  void multiple_placement::reset ()
  {
    __placements.clear ();
    return;
  }
    
  void multiple_placement::tree_dump (ostream & out_, 
				      const string & title_, 
				      const string & indent_, 
				      bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);


    out_ << indent << i_tree_dumpable::tag  
	 << "Size    :" << get_number_of_items () << endl;

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	 << "Replica :" << is_replica () << endl;

    return;
  }
 

} // end of namespace geomtools

// end of multiple_placement.cc

