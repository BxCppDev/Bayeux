// -*- mode: c++ ; -*- 
/* i_binary_cut.cc
 */

#include <cuts/i_binary_cut.h>
#include <cuts/factory.h> // for 'factory::get_creators'

namespace cuts {

  using namespace std;

  void i_binary_cut::set_cut_1 (i_cut & cut_1_)
  {
    if (&cut_1_ == this)
      {
	ostringstream message;
	message << "i_binary_cut::set_cut_1: "
		<< "Binary cut with self-reference is not allowed !";
	throw runtime_error (message.str ( ));
      }
    _cut_1 = &cut_1_;
    return;
  }
  
  void i_binary_cut::set_cut_2 (i_cut & cut_2_)
  {
    if (&cut_2_ == this)
      {
	ostringstream message;
	message << "i_binary_cut::set_cut_2: "
		<< "Binary cut with self-reference is not allowed !";
	throw runtime_error (message.str ( ));
      }
    _cut_2 = &cut_2_;
    return;
  }
   
  void i_binary_cut::set_cuts (i_cut & cut_1_, i_cut & cut_2_)
  {
    set_cut_1 (cut_1_);
    set_cut_2 (cut_2_);
    return;
  }
    
  void i_binary_cut::set_user_data (void * user_data_)
  {
    if (_cut_1 == 0) 
      {
	throw std::runtime_error ("i_binary_cut::set_user_data: Null 'cut 1' !");
      }
    _cut_1->set_user_data (user_data_);
    if (_cut_2 == 0) 
      {
	throw std::runtime_error ("i_binary_cut::set_user_data: Null 'cut 2' !");
      }
    _cut_2->set_user_data (user_data_);
    return;
  }
  
  // ctor:
  i_binary_cut::i_binary_cut ()
  {
    _cut_1 = 0;
    _cut_2 = 0;
  }

  // ctor:
  i_binary_cut::i_binary_cut (i_cut & cut_1_, i_cut & cut_2_)
  {
    set_cuts (cut_1_, cut_2_);
  }
  
  // dtor:
  i_binary_cut::~i_binary_cut ()
  {
  }

  void i_binary_cut::install_cuts (const properties & configuration_, 
				   cut_dict_t * cut_dict_, 
				   i_binary_cut & binary_cut_)
  {
    i_cut * cut_1 = 0;
    i_cut * cut_2 = 0;
    string cut_1_name;
    string cut_2_name;
    if (configuration_.has_key ("cut_1"))
      {
	cut_1_name = configuration_.fetch_string ("cut_1");
      }
    else
      {
	throw runtime_error ("i_binary_cut::create: Missing 'cut_1' name property !");
      }

    if (configuration_.has_key ("cut_2"))
      {
	cut_2_name = configuration_.fetch_string ("cut_2");
      }
    else
      {
	throw runtime_error ("i_binary_cut::create: Missing 'cut_2' name property !");
      }

    cut_dict_t::iterator it_cut_1 = cut_dict_->find (cut_1_name);
    if (it_cut_1 == cut_dict_->end ())
      {
	ostringstream message;
	message << "i_binary_cut::create: "
		<< "No cut named '" << cut_1_name << "' !";
	throw runtime_error (message.str ());
      }
    cut_1 = it_cut_1->second;

    cut_dict_t::iterator it_cut_2 = cut_dict_->find (cut_2_name);
    if (it_cut_2 == cut_dict_->end ())
      {
	ostringstream message;
	message << "i_binary_cut::create: "
		<< "No cut named '" << cut_2_name << "' !";
	throw runtime_error (message.str());
      }
    cut_2 = it_cut_2->second;
    binary_cut_.set_cuts (*cut_1, *cut_2);
    
    return;
  }
  
} // end of namespace cuts

// end of i_binary_cut.cc
