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

// using namespace   std;

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
        std::clog << "DEVEL: " << "handle_pool<T>::_resize: Entering...\n";
#endif
        size_t current_size = _buffer.size ();
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::_resize:   current_size = " << current_size << "\n";
#endif
        if (a_size <= current_size)
          {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
            std::clog << "DEVEL: " << "handle_pool<T>::_resize:   Capacity is enough for " << a_size << "\n";
#endif
            return;
          }
        _buffer.reserve (a_size);
        for (int i = current_size; i < (int) a_size; i++)
          {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
            std::clog << "DEVEL: " << "handle_pool<T>::_resize:   Add a handle with a new element...\n";
#endif
            _buffer.push_back (handle_type (new element_type));
          }
        _number_of_used_item = 0; 
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::_resize: Exiting.\n";
#endif
        return;
      }
    public:

      void clear ()
      {
        _buffer.clear ();
        return;
      }

      size_t get_capacity () const
      {
        return _buffer.size ();
      }

      size_t get_number_of_used_item () const
      {
        return _number_of_used_item;
      }

      void reset ()
      {
        _number_of_used_item = 0;
        return;
      }
      void resize (size_t a_size)
      {
        if (_number_of_used_item > 0)
          {
            throw std::logic_error ("datatools::utils::handle_pool<T>::resize: Operation not allowed !");
          }
        _resize (a_size);
        return;
      } 
      handle_pool ()
      {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::ctor: Entering...\n";
#endif
        _number_of_used_item = 0;
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::ctor: Exiting.\n";
#endif
        return;
      }
      handle_pool (size_t a_size)
      {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::ctor: Entering...\n";
#endif
        _number_of_used_item = 0;
        _resize (a_size);
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::ctor: Exiting.\n";
#endif
        return;
      }
      virtual ~handle_pool ()
      {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::dtor: Entering...\n";
        std::clog << "DEVEL: " << "handle_pool<T>::dtor: "
             << "  buffer size is " << _buffer.size () << "\n";
        std::clog << "DEVEL: " << "handle_pool<T>::dtor: " 
             << " number of used element " << _number_of_used_item << "\n";
#endif
        _number_of_used_item = 0;
        _buffer.clear ();
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::dtor: Exiting.\n";
#endif
        return;
      }
      const handle_type & create ()
      {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::create: Entering...\n";
        std::clog << "DEVEL: " << "handle_pool<T>::create:   buffer size is " << _buffer.size () << "\n";
        std::clog << "DEVEL: " << "handle_pool<T>::create: " 
             << "  number of used element " << _number_of_used_item << "\n";
#endif
        if (_number_of_used_item < _buffer.size ())
          {
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
            std::clog << "DEVEL: " << "handle_pool<T>::create: " 
                 << "  Use a pre-allocated element @ position " << _number_of_used_item  << " \n";
            std::clog << "DEVEL: " << "handle_pool<T>::create: Exiting.\n";
#endif
            return _buffer[_number_of_used_item++];
          }
        element_type * ptr = new element_type;
        _buffer.push_back (handle_type (ptr));
        _number_of_used_item++;
#ifdef DATATOOLS_UTILS_HANDLE_POOL_DEVEL
        std::clog << "DEVEL: " << "handle_pool<T>::create: " 
             << "  Use a newly allocated element @ position " << (_number_of_used_item - 1) << " \n";
        std::clog << "DEVEL: " << "handle_pool<T>::create: Exiting.\n";
#endif
        return _buffer.back ();
      }

      void dump (std::ostream & a_out, const std::string & a_title, bool short_ = true) const
      {
        if (! a_title.empty ())
          {
            a_out << a_title << " : " << std::endl;
          }
        a_out << "|-- " << "Capacity     : " << get_capacity () << std::endl;
        a_out << "|-- " << "# Used items : " << _number_of_used_item << std::endl;
        a_out << "`-- " << "Items @ " << std::endl;
        for (int i = 0; i < _buffer.size (); i++)
          {
            if (short_)
              {
                int max_short = 10;
                if (i >= max_short) continue;
              }
            a_out << "    ";
            const handle_type & h = _buffer[i];
            if (i < _buffer.size () - 1)
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
                a_out << "No element!" << std::endl;
              }
            else
              {
                a_out << "@ " << &h.get () << " [" << h.count () << "]"<< std::endl;
              } 
          }
        return;
      }

    protected:

      std::vector<handle_type> _buffer;
      size_t                   _number_of_used_item;

    };

  } // namespace utils 

} // namespace datatools 

#endif // __datatools__utils__handle_pool_h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

