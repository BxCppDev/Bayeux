/// \file datatools/handle.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-04-01
 * Last modified : 2013-04-22
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  An serializable handle object referencing an object through
 *  a hiden shared pointer.
 *
 */
#ifndef DATATOOLS_HANDLE_H
#define DATATOOLS_HANDLE_H

// Standard Library:
#include <iostream>
#include <stdexcept>
#include <type_traits>

// Third Party:
// - Boost:
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/shared_ptr.hpp>

// This Project:
#include <datatools/i_predicate.h>
#include <datatools/exception.h>

namespace datatools {

  //! Forward declaration :
  template <typename T> struct handle_predicate;

  /*! \brief Templatized handle class that wraps a Boost shared pointer
   *         and behaves like a reference.
   *
   *  A handle object is given the responsability to
   *  handle a class instance through its pointer using the shared pointer
   *  mechanism from Boost. The inner hidden pointer can be null.
   *  It must be initialized with the 'new' construction operator.
   *  The handle class is copyable and can be used within STL containers.
   *
   *  \b Example:
   *
   *  \code
   *  #include <datatools/handle.h>
   *
   *  {
   *    using namespace datatools;
   *    handle<int> h0;  // a default int is allocated
   *    h0.grab () = 999; // access the int and modify its value
   *    cout << "h0 = " << h0.get () << endl; // print it
   *    h0.reset (new int (0)); // discard the former int
   *                            // and allocate a new one
   *    handle<int> h1 (new int (1)); // create a new handle for another int
   *    {
   *      handle<int> h2 (h1); // another handle that shares the same int
   *                           // than the previous handle
   *      h2.grab () = 666;     //change the int value through this handle
   *    }
   *    cout << "h1 = " << h1.get () << endl; // print it through the
   *                                          // first handle
   *  };
   *  \endcode
   *
   * \see make_handle()
   */
  template <typename T>
  class handle
  {
  public:
    typedef T value_type;
    typedef value_type & reference_type;
    typedef handle_predicate<T> predicate_type;


    //! A constructor from a pointer to some on-the-fly allocated instance.
    /*!
     * This constructor is given a pointer to some dynamically
     * allocated instance and pass it to the internal shared pointer.
     *
     * Example:
     * \code
     * datatools::handle<int> h(new int(3));
     * \endcode
     * or
     * \code
     * int * pi = new int(3);
     * datatools::handle<int> h(pi);
     * \endcode
     *
     * \warning Never use such kind of mechanism to initialize a handle:
     * \code
     * int i = 3;
     * datatools::handle<int> h(&i);
     * \endcode
     *
     */
    handle(T * held_ = nullptr)
      : _sp_(held_)
    {
      return;
    }

    //! A constructor from a pointer to some on-the-fly allocated instance.
    /*!
     * This constructor is given a pointer to some dynamically
     * allocated instance and pass it to the internal shared const pointer.
     * This means that the handle does not allow to modify the
     * element it handles.
     *
     * Example:
     * \code
     * datatools::handle<const int> h(new int(3));
     * \endcode
     */
    template<typename Q,
             typename = std::enable_if<
               std::is_same<const Q, T>::value &&
               std::is_const<T>::value &&
               !std::is_const<Q>::value>>
      handle(Q* held = nullptr)
      : _sp_(const_cast<Q*>(held))
    {
      return;
    }

    //! Constructor on a boost shared_ptr
    handle(const boost::shared_ptr<T> & sp_)
      : _sp_(sp_)
    {
      return;
    }

    //! Destructor
    virtual ~handle()
    {
      _sp_.reset();
      return;
    }

    //! Check if the current handle holds an uniquely referenced object
    bool unique() const
    {
      return _sp_.unique();
    }

    //! Return true if the internal shared pointer holds something.
    // 2013-05-12 FM : Preserved until all client code use the corresponding 'operator bool'.
    bool has_data() const
    {
      return _sp_.get() != nullptr;
    }

    //! Return true if the internal shared pointer holds something.
    operator bool() const
    {
      return _sp_.get() != nullptr;
    }

    void swap(handle<T> & other_)
    {
      _sp_.swap(other_._sp_);
      return;
    }

    //! Return a const reference to the hosted instance.
    const T & get() const
    {
      DT_THROW_IF(_sp_.get() == nullptr, std::logic_error, "Handle holds no data!");
      return *_sp_;
    }

    // 2016-12-01, FM: original version is inhibited when T is const:
    // //! Return a non-const reference to the hosted instance.
    // T& grab() {
    //   if (_sp_) return *_sp_;
    //   DT_THROW_IF(true, std::logic_error, "Handle holds no data!");
    // }

    //! Return a non-const reference to the hosted instance.
    template<typename Q = T>
    typename std::enable_if<
      std::is_same<Q, T>::value &&
    !std::is_const<Q>::value &&
    !std::is_const<T>::value,
      Q&>::type grab()
    {
      DT_THROW_IF(_sp_.get() == nullptr, std::logic_error, "Handle holds no data!");
      return *_sp_;
    }

    //! Reset the internal shared pointer with a new instance.
    void reset(T * elem_ = nullptr)
    {
      _sp_.reset(elem_);
      return;
    }

    //! Return a handle instance that hosts the const instance
    template<typename Q = T>
    typename std::enable_if<
      std::is_same<Q, T>::value &&
    !std::is_const<Q>::value &&
    !std::is_const<T>::value,
      handle<const Q> >::type to_const() const
    {
      boost::shared_ptr<const T> csp = boost::const_pointer_cast<const T, T>(_sp_);
      return handle<const T>(csp);
    }

  private:

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive & ar_, int /*version*/)
    {
      ar_ & boost::serialization::make_nvp("sp", _sp_);
      return;
    }

  private:

    boost::shared_ptr<T> _sp_; //!< The embedded shared pointer.

  };

  /*! \brief Constructs an object of type T and wraps it in a datatools::handle
   *
   * This function behaves in the same way as std::make_shared or
   * std::make_unique. It is functionally equivalent to:
   * \code
   * handle<T>(new T(std::forward<Args>(args)...))
   * \endcode
   *
   * \tparam T the type to construct and wrap in the handle
   * \param args list of arguments with which the instance of T will be
   * constructed
   *
   * \b Example:
   * \code
   * #include <datatools/handle.h>
   * #include <iostream>
   *
   * struct Vec3
   * {
   *   int x, y, z;
   *   Vec3() : x(0), y(0), z(0) { }
   *   Vec3(int x, int y, int z) : x(x), y(y), z(z) { }
   *   friend std::ostream& operator<<(std::ostream& os, Vec3& v) {
   *     return os << '{' << "x:" << v.x << " y:" << v.y << " z:" << v.z  << '}';
   *   }
   * };
   *
   * int main()
   * {
   *   // Uses default constructor
   *   auto v1 = datatools::make_handle<Vec3>();
   *   // Uses constructor matching the arguments
   *   auto v2 = datatools::make_handle<Vec3>(1, 2, 3);
   *
   *   std::cout << "v1: " << v1.grab() << '\n'
   *             << "v2: " << v2.grab() << '\n';
   * }
   * \endcode
   *
   * Output:
   * \code
   * v1: {x:0 y:0 z:0}
   * v2: {x:1 y:2 z:3}
   * \endcode
   */
  template <typename T, typename... Args>
  handle<T> make_handle(Args&& ... args)
  {
    // Use shared_ptr constructor for simplicity
    return handle<T>(boost::make_shared<T>(std::forward<Args>(args)...));
  }

  /*! \brief Templatized predicate class associated to handle instance.
   *
   * The handle_predicate class is used to wrap a predicate object to some
   * instance and pass it to some algorithms (typically the std::find_if function)t.
   *
   * \b Example:
   * \code
   * #include <iostream>
   * #include <vector>
   * #include <cmath>
   * #include <algorithm>
   *
   * #include <datatools/i_predicate.h>
   * #include <datatools/handle.h>
   *
   * struct is_pi : public i_predicate <double>
   * {
   *   bool operator () (const double & a_number) const
   *   {
   *     return a_number = M_PI;
   *   }
   * };
   *
   * int main (void)
   * {
   *   is_pi DP;
   *   typedef handle<double> handle_type;
   *   handle_predicate<double> HP (DP);
   *   std::vector<handle_type> values;
   *   values.push_back (handle_type (new double (1.0)));
   *   values.push_back (handle_type (new double (M_PI)));
   *   values.push_back (handle_type (new double (10.0)));
   *   std::vector<handle_type>::const_iterator it =
   *     std::find_if (values.begin (), values.end (), HP);
   *   if (it != values.end ()) std::cout << "Found Pi !" << std::endl;
   *   return 0;
   * }
   * \endcode
   *
   */
  template <typename T>
  struct handle_predicate
    : public i_predicate<handle<T> >
  {
  public:
    //! The default constructor.
    handle_predicate(const i_predicate<T> & predicate_,
                     bool no_data_means_false_ = true)
      : _predicate_(predicate_)
      , _no_data_means_false_(no_data_means_false_)
    {
      return;
    }


    //! Call operator taking handle as input
    bool operator()(const handle<T> & handle_) const
    {
      if (! handle_) {
        if (_no_data_means_false_) return false;
        DT_THROW(std::logic_error, "Handle has no data !");
      }
      return _predicate_(handle_.get());
    }

  private:
    const i_predicate<T> & _predicate_; /*!< The embedded predicate. */
    bool _no_data_means_false_; /*!< A flag to indicate the behaviour in case of NULL pointer : predicate returns false (default) or throws an exception). */
  };

} // end of namespace datatools

// From "boost/serialization/shared_ptr.hpp" :
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
namespace boost {
  namespace serialization{
    // don't track shared pointers
    //! \brief Struct used internally by the serialization mechanism of handles for memory tracking
    template<class T>
    struct tracking_level< ::datatools::handle<T> > {
      typedef mpl::integral_c_tag tag;
#if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
      typedef BOOST_DEDUCED_TYPENAME mpl::int_< ::boost::serialization::track_never> type;
#else
      typedef mpl::int_< ::boost::serialization::track_never> type;
#endif
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x570))
      BOOST_STATIC_CONSTANT(int, value = ::boost::serialization::track_never);
#else
      BOOST_STATIC_CONSTANT(int, value = type::value);
#endif
    };
  }}
#define BOOST_SERIALIZATION_DATATOOLS_HANDLE(T)
#else
// define macro to let users of these compilers do this
#define BOOST_SERIALIZATION_DATATOOLS_HANDLE(T)             \
  BOOST_SERIALIZATION_SHARED_PTR(T)                         \
  BOOST_CLASS_TRACKING(::datatools::handle< T >,            \
                       ::boost::serialization::track_never) \
  /**/
#endif

#endif // DATATOOLS_HANDLE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
