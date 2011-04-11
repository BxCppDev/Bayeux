/* datatools/utils/handle.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-05-10
 * Last modified : 2011-05-10
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
 * Description: 
 *
 *  A pool of handles.
 *
 */

#ifndef __datatools__utils__handle_pool_h
#define __datatools__utils__handle_pool_h 1

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/handle.h>

using namespace std;

namespace datatools {

  namespace utils {

    template <class T>
      class handle_pool 
      {
      public:
	typedef T element_type;
	typedef datatools::utils::handle<element_type> handle_type;

      protected:
	void _resize (size_t a_size)
	{
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	  clog << "DEVEL: " << "handle_pool<T>::_resize: Entering...\n";
#endif
	  size_t current_size = buffer_.size ();
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	  clog << "DEVEL: " << "handle_pool<T>::_resize:   current_size = " << current_size << "\n";
#endif
	  if (a_size <= current_size)
	    {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	      clog << "DEVEL: " << "handle_pool<T>::_resize:   Capacity is enough for " << a_size << "\n";
#endif
	      return;
	    }
	  buffer_.reserve (a_size);
	  for (int i = current_size; i < a_size; i++)
	    {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	      clog << "DEVEL: " << "handle_pool<T>::_resize:   Add a handle with a new element...\n";
#endif
	      buffer_.push_back (handle_type (new element_type));
	    }
	  number_of_used_item_ = 0; 
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	  clog << "DEVEL: " << "handle_pool<T>::_resize: Exiting.\n";
#endif
	  return;
	}
      public:

	void clear ()
	{
	  buffer_.clear ();
	  return;
	}

	size_t get_capacity () const
	{
	  return buffer_.size ();
	}

	size_t get_number_of_used_item () const
	{
	  return number_of_used_item_;
	}

	void reset ()
	{
	  number_of_used_item_ = 0;
	  return;
	}
	void resize (size_t a_size)
	{
	  if (number_of_used_item_ > 0)
	    {
	      throw std::logic_error ("datatools::utils::handle_pool<T>::resize: Operation not allowed !");
	    }
	  _resize (a_size);
	  return;
	} 
	handle_pool ()
	  {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::ctor: Entering...\n";
#endif
	    number_of_used_item_ = 0;
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::ctor: Exiting.\n";
#endif
	    return;
	  }
	handle_pool (size_t a_size)
	  {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::ctor: Entering...\n";
#endif
	    number_of_used_item_ = 0;
	    _resize (a_size);
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::ctor: Exiting.\n";
#endif
	    return;
	  }
	virtual ~handle_pool ()
	  {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::dtor: Entering...\n";
	    clog << "DEVEL: " << "handle_pool<T>::dtor: "
		 << "  buffer size is " << buffer_.size () << "\n";
	    clog << "DEVEL: " << "handle_pool<T>::dtor: " 
		 << " number of used element " << number_of_used_item_ << "\n";
#endif
	    number_of_used_item_ = 0;
	    buffer_.clear ();
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	    clog << "DEVEL: " << "handle_pool<T>::dtor: Exiting.\n";
#endif
	    return;
	  }
	const handle_type & create ()
	{
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	  clog << "DEVEL: " << "handle_pool<T>::create: Entering...\n";
	  clog << "DEVEL: " << "handle_pool<T>::create:   buffer size is " << buffer_.size () << "\n";
	  clog << "DEVEL: " << "handle_pool<T>::create: " 
	       << "  number of used element " << number_of_used_item_ << "\n";
#endif
	  if (number_of_used_item_ < buffer_.size ())
	    {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	      clog << "DEVEL: " << "handle_pool<T>::create: " 
		   << "  Use a pre-allocated element @ position " << number_of_used_item_  << " \n";
	      clog << "DEVEL: " << "handle_pool<T>::create: Exiting.\n";
#endif
	      return buffer_[number_of_used_item_++];
	    }
	  element_type * ptr = new element_type;
	  buffer_.push_back (handle_type (ptr));
	  number_of_used_item_++;
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
	  clog << "DEVEL: " << "handle_pool<T>::create: " 
	       << "  Use a newly allocated element @ position " << (number_of_used_item_ - 1) << " \n";
	  clog << "DEVEL: " << "handle_pool<T>::create: Exiting.\n";
#endif
	  return buffer_.back ();
	}

	void dump (ostream & a_out, const string & a_title, bool short_ = true) const
	{
	  if (! a_title.empty ())
	    {
	      a_out << a_title << " : " << endl;
	    }
	  a_out << "|-- " << "Capacity     : " << get_capacity () << endl;
	  a_out << "|-- " << "# Used items : " << number_of_used_item_ << endl;
	  a_out << "`-- " << "Items @ " << endl;
	  for (int i = 0; i < buffer_.size (); i++)
	    {
	      if (short_)
		{
		  int max_short = 10;
		  if (i >= max_short) continue;
		}
	      a_out << "    ";
	      const handle_type & h = buffer_[i];
	      if (i < buffer_.size () - 1)
		{
		  a_out << "|-- ";
		}
	      else
		{
		  a_out << "`-- ";
		}
	      a_out << "index " << i << " : ";
	      if (! h.has_data ())
		{
		  a_out << "No element!" << endl;
		}
	      else
		{
		  a_out << "@ " << &h.get () << " [" << h.count () << "]"<< endl;
		}	
	    }
	  return;
	}
      protected:
	std::vector<handle_type> buffer_;
	size_t number_of_used_item_;
      };

  } // namespace utils 

} // namespace datatools 

#endif // __datatools__utils__handle_pool_h 1
