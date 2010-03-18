// -*- mode: c++ ; -*- 
/* geom_id.cc
 */

#include <geomtools/geom_id.h>

namespace geomtools {
 
  const std::string geom_id::SERIAL_TAG = "__GEOM_ID__";

   const uint32_t geom_id::INVALID_TYPE = 0xFFFFFFFF;
   const uint32_t geom_id::INVALID_ADDRESS = 0xFFFFFFFF;
   const size_t   geom_id::DEFAULT_ADDRESS_DEPTH = 10;
  
   const char geom_id::IO_ID_OPEN           = '[';
   const char geom_id::IO_ID_SEPARATOR      = ':';
   const char geom_id::IO_ADDRESS_SEPARATOR = '.';
   const char geom_id::IO_TYPE_INVALID      = '?';
   const char geom_id::IO_ADDRESS_INVALID   = '?';
   const char geom_id::IO_ID_CLOSE          = ']';

  const std::string & geom_id::get_serial_tag () const
  {
    return geom_id::SERIAL_TAG;
  }

  bool geom_id::is_type (uint32_t type_) const
  {
    return __type == type_;
  }

  void geom_id::set_depth (size_t depth_)
  {
    size_t sz = __address.size ();
    if (depth_ > sz)
      {
	for (int i = sz; i < depth_; i++)
	  {
	    __address.push_back (INVALID_ADDRESS);
	  }
      }
    else 
      {
	__address.resize (depth_);
      }
  }

  size_t geom_id::get_depth () const
  {
    return __address.size ();
  }

  uint32_t geom_id::get (int i_) const
  {
    return __address.at (i_);
  }
  
  void geom_id::set (int i_, uint32_t val_)
  {
    if (i_ < 0) 
      {
	throw runtime_error ("geom_id::set: Invalid sub-address index !");
      }
    else 
      {
	if (i_ < __address.size ()) 
	  {
	    __address[i_] = val_;
	  }
	else
	  {
	    for (int j = __address.size (); j <= i_; j++)
	      {  
		uint32_t v = geom_id::INVALID_ADDRESS;
		__address.push_back (v);
	      }
	    __address[i_] = val_;
	  }
      }
  }
  
  const uint32_t & geom_id::get_type () const
  {
    return __type;
  }
  
  void geom_id::set_type (const uint32_t & new_value_)
  {
    __type = new_value_;
  }

  void geom_id::set_address (uint32_t si0_, 
			     uint32_t si1_, 
			     uint32_t si2_, 
			     uint32_t si3_, 
			     uint32_t si4_, 
			     uint32_t si5_, 
			     uint32_t si6_, 
			     uint32_t si7_, 
			     uint32_t si8_, 
			     uint32_t si9_)
  {
    //if (si0_ == INVALID_ADDRESS) return;
    set (0, si0_);
    if (si1_ == INVALID_ADDRESS) return;
    set (1, si1_);
    if (si2_ == INVALID_ADDRESS) return;
    set (2, si2_);
    if (si3_ == INVALID_ADDRESS) return;
    set (3, si3_);
    if (si4_ == INVALID_ADDRESS) return;
    set (4, si4_);
    if (si5_ == INVALID_ADDRESS) return;
    set (5, si5_);
    if (si6_ == INVALID_ADDRESS) return;
    set (6, si6_);
    if (si7_ == INVALID_ADDRESS) return;
    set (7, si7_);
    if (si8_ == INVALID_ADDRESS) return;
    set (8, si8_);
    if (si9_ == INVALID_ADDRESS) return;
    set (9, si9_);
  }
  
  // ctor:
  geom_id::geom_id ()
  {
    //clog << "geom_id::geom_id[1]: Entering..." << endl;
    __type = geom_id::INVALID_TYPE;
    __address.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
  }

  geom_id::geom_id (uint32_t type_, 
		    uint32_t si0_, 
		    uint32_t si1_, 
		    uint32_t si2_, 
		    uint32_t si3_, 
		    uint32_t si4_, 
		    uint32_t si5_, 
		    uint32_t si6_, 
		    uint32_t si7_, 
		    uint32_t si8_, 
		    uint32_t si9_)
  {  
    clog << "geom_id::geom_id[2]: Entering..." << endl;
    __type = geom_id::INVALID_TYPE;
    set_type (type_);
    __address.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    set_address (si0_, 
		 si1_, 
		 si2_, 
		 si3_, 
		 si4_, 
		 si5_, 
		 si6_, 
		 si7_, 
		 si8_, 
		 si9_);
    /* 
    if (type_ == INVALID_TYPE) return;
    __type = type_;
    if (si0_ == INVALID_ADDRESS) return;
    __address.push_back (si0_);
    if (si1_ == INVALID_ADDRESS) return;
    __address.push_back (si1_);
    if (si2_ == INVALID_ADDRESS) return;
    __address.push_back (si2_);
    if (si3_ == INVALID_ADDRESS) return;
    __address.push_back (si3_);
    if (si4_ == INVALID_ADDRESS) return;
    __address.push_back (si4_);
    if (si5_ == INVALID_ADDRESS) return;
    __address.push_back (si5_);
    if (si6_ == INVALID_ADDRESS) return;
    __address.push_back (si6_);
    if (si7_ == INVALID_ADDRESS) return;
    __address.push_back (si7_);
    if (si8_ == INVALID_ADDRESS) return;
    __address.push_back (si8_);
    if (si9_ == INVALID_ADDRESS) return;
    __address.push_back (si9_);
    */
  }
  
  // dtor:
  geom_id::~geom_id ()
  {
    __address.clear ();
  }
  
  void geom_id::reset ()
  {
    __type = INVALID_TYPE;
    __address.clear ();
    __address.reserve (DEFAULT_ADDRESS_DEPTH);   
  }

  void geom_id::invalidate ()
  {
    __type = INVALID_TYPE;
    for (int i = 0; i < __address.size(); i++)
      {
	__address[i] = INVALID_ADDRESS;
      }
  }
    
  bool geom_id::is_valid () const
  {
    if (__type == INVALID_TYPE)
      {
	return false;
      }
    if (__address.size () == 0)
      {
	return false;
      }
    for (int i = 0; i < __address.size (); i++)
      {
	if (__address[i] == INVALID_ADDRESS)
	  {
	    return false;
	  }
      }
    return true;
  }

  void geom_id::extract_to (geom_id & target_) const
  {
    if (this->__address.size() < target_.__address.size ())
      {
	throw runtime_error ("geom_id::extract: Incompatible address depth !");
      }
    for (int i = 0; i < target_.__address.size (); i++) 
      {
	target_.set (i, this->__address[i]);
      }
  }

  void geom_id::extract_from (const geom_id & source_)
  {
    source_.extract_to (*this);
  }

  ostream & operator<< (ostream & out_, const geom_id & id_)
  {
    out_ << geom_id::IO_ID_OPEN;

    if (id_.__type != geom_id::INVALID_TYPE) 
      {
	out_ << id_.__type;
      }
    else 
      {
	out_ << geom_id::IO_TYPE_INVALID;
      }

    out_ << geom_id::IO_ID_SEPARATOR;
    for (int i = 0; i < id_.__address.size (); i++) 
      {
	if (id_.__address[i] != geom_id::INVALID_ADDRESS) 
	  {
	    out_ << id_.__address[i];
	  }
	else 
	  {
	    out_ << geom_id::IO_ADDRESS_INVALID;
	  }
	if (i < id_.__address.size () - 1)
	  {
	    out_ << geom_id::IO_ADDRESS_SEPARATOR;
	  }
      }
    out_ << geom_id::IO_ID_CLOSE;
    return out_;
  }

  istream & operator>> (istream & in_, geom_id & id_)
  {
    char open = in_.peek ();
    if (in_.eof ())
      {
	return in_;
      }
    if (open != geom_id::IO_ID_OPEN)
      {
	in_.setstate (ios::failbit);
	return in_;
      }
    else
      {
	in_.get ();
      }

    char check = in_.peek ();
    if (in_.eof ())
      {
	in_.setstate (ios::failbit);
	return in_;
      } 
    if (check != geom_id::IO_TYPE_INVALID)
      {
	in_ >> id_.__type;
	if (! in_)
	  {
	    return in_;
	  }
      }
    else
      {
	id_.__type = geom_id::INVALID_TYPE;
	in_.get ();
      }

    char separator = in_.peek ();
    if (in_.eof ())
      {
	in_.setstate (ios::failbit);
	return in_;
      }
    if (separator != geom_id::IO_ID_SEPARATOR)
      {
	in_.setstate (ios::failbit);
	return in_;
      }
    else
      {
	in_.get ();
      }
    int count = 0;
    id_.__address.clear ();
    id_.__address.reserve (geom_id::DEFAULT_ADDRESS_DEPTH);
    while (true)
      {
	char check = in_.peek ();
	if (in_.eof ())
	  {
	    return in_;
	  } 
	uint32_t val;
	if (check == geom_id::IO_ADDRESS_INVALID)
	  {
	    id_.set (count, geom_id::INVALID_ADDRESS);
	    in_.get ();
	  }
	else
	  {
	    in_ >> val;
	    if (! in_)
	      {
		return in_;
	      } 
	    id_.set (count, val);
	  }

	char sep = in_.peek ();
	if (in_.eof ())
	  {
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	if (sep == geom_id::IO_ID_CLOSE)
	  {
	    in_.get ();
	    break;
	  }
	else if (sep != geom_id::IO_ADDRESS_SEPARATOR)
	  {
	    in_.setstate (ios::failbit);
	    return in_;
	  }
	else
	  {
	    in_.get ();
	  }
	count++;
	if (count == 100)
	  {
	    in_.setstate (ios::failbit);
	    return in_;
	  }
      }

    return in_;
  }

  bool geom_id::sub_id_comp (uint32_t si1_, uint32_t si2_)
  { 
    return si1_ < si2_; 
  }

  int geom_id::compare (const geom_id & id1_, const geom_id & id2_)
  {
    if (id1_.__type < id2_.__type)
      {
	return -1;
      }
    else if (id1_.__type > id2_.__type)
      {
	return +1;
      }
    else
      {
	if (lexicographical_compare(id1_.__address.begin (),
				    id1_.__address.end (),
				    id2_.__address.begin (),
				    id2_.__address.end ()))
	  {
	    return -1;
	  }
	else
	  {
	    if (lexicographical_compare(id2_.__address.begin (),
					id2_.__address.end (),
					id1_.__address.begin (),
					id1_.__address.end ()))
	      {
		return +1;
	      }
	    else
	      {
		return 0;
	      }
	  }
      }
  }

  bool geom_id::operator== (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) == 0;
  }

  bool geom_id::operator< (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) < 0;
  }

  bool geom_id::operator<= (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) <= 0;
  }

  bool geom_id::operator>= (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) >= 0;
  }

  bool geom_id::operator> (const geom_id & id_) const
  {
    return geom_id::compare (*this, id_) > 0;
  }

  void geom_id::make (int type_, int depth_)
  {
    reset ();
    set_type (type_);
    set_depth (depth_);
    set_address (INVALID_ADDRESS);     
  }

  void geom_id::make (geom_id & id_, int type_, int depth_)
  {
    id_.make (type_, depth_);
  }
   
} // end of namespace geomtools

// end of geom_id.cc
