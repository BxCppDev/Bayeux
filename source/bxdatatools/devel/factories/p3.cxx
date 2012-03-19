// 

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace datatools {

  namespace factory {

    class base_factory_register
    {
    public:

      enum flag_type
        {
          verbose = 0x1,
        };

      base_factory_register ();

      virtual ~base_factory_register ();

    };

    base_factory_register::base_factory_register ()
    {
      return;
    }

    base_factory_register::~base_factory_register ()
    {
      return;
    }

    /*********************************************/

    template <class BaseType>
    class factory_register : public base_factory_register
    {
    public:

      typedef BaseType                            base_type;
      typedef boost::function<base_type*() >     factory_type;
      typedef std::map<std::string, factory_type> factory_map_type;

      bool is_verbose () const
      {
        return _verbose_;
      }

      void set_verbose (bool verbose_)
      {
        _verbose_ = verbose_;
        return;
      }

      const std::string & label () const
      {
        return _label_;
      }

      void list_of_factories (std::vector<std::string> & ids_) const
      {
        for (typename factory_map_type::iterator i = _registered_.begin ();
             i !=  _registered_.end ();
             i++)
          {
            ids_.push_back (i->first);
          }
        return;
      }

      bool has (const std::string & id_) const
      {
        return _registered_.find (id_) != _registered_.end ();
      }

      factory_type & get (const std::string & id_) //const
      {
        typename factory_map_type::iterator found = _registered_.find (id_);
        if (found == _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register::get: "
                    << "Class ID '" << id_ << "' is not registered !";
            throw std::logic_error (message.str ());
          }
        return found->second;
      }
 
      // factory_type & get (const std::string & id_)
      // {
      //   factory_register<base_type> * const_this = 
      //     const_cast<factory_register<base_type> *> (this);
      //   return (const_cast<factory_type *> (const_this->get (id_)));
      // }

      void registration (const std::string & id_, const factory_type & factory_)
      {
        if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">::registration: " 
                      << "registration of class with ID '" << id_ << "'" << std::endl;
          }
        typename factory_map_type::const_iterator found = _registered_.find (id_);
        if (found != _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register<" << _label_ << ">::registration: "
                    << "Class ID '" << id_ << "' is already registered !";
            throw std::logic_error (message.str ());
          }
        _registered_[id_] = factory_;
        return;
      }

      void unregistration (const std::string & id_)
      {
        if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">::unregistration:  " 
                      << "registration of class with ID '" << id_ << "'" << std::endl;
          }
        typename factory_map_type::const_iterator found = _registered_.find (id_);
        if (found == _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register<" << _label_ << ">::unregistration: "
                    << "Class ID '" << id_ << "' is not registered !";
            throw std::logic_error (message.str ());
          }
        return;
      };
 
      factory_register (const std::string & label_, unsigned int flags = 0x0)
      {
        _verbose_ = false;
        if (flags & verbose)
          {
            _verbose_ = true;
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << label_ << ">: Construction..." << std::endl;
          }
        _label_ = label_;
         return;
      }

      virtual ~factory_register ()
      {
        if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">: Destruction." << std::endl;
          }
        for (typename factory_map_type::iterator i = _registered_.begin ();
             i !=  _registered_.end ();
             i++)
          {
            if (i->second != 0)
              {
                if (is_verbose ())
                  {
                    std::clog << "VERBOSE: " 
                              << "datatools::factory: Destroying registered allocator/functor '"
                              << i->first << "'" << std::endl;
                  }
              }
          }
        _registered_.clear ();
        if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">: Destruction done." << std::endl;
          }
        return;
      }

      void import (const factory_register & factory_register_)
      {
       if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">::import: Importing registered factories from register '"
                      << factory_register_.label () << "'..." << std::endl;
          }
       for (typename factory_map_type::const_iterator i = factory_register_._registered_.begin ();
            i !=  factory_register_._registered_.end ();
            i++)
         {
           const factory_type & the_out_factory = i->second;
           registration (i->first, the_out_factory);
         }
       if (is_verbose ())
          {
            std::clog << "VERBOSE: " << "datatools::factory::factory_register<" 
                      << _label_ << ">::import: Done." << std::endl;
          }
        return;
      }

      void print (std::ostream & out_, const std::string & indent_) const
      {
        out_ << indent_ << "List of registered allocators/functors for label \"" 
             << _label_ << "\" : " << std::endl;
        for (typename factory_map_type::const_iterator i = _registered_.begin ();
             i != _registered_.end ();
             i++)
          {
            out_ << indent_;
            typename factory_map_type::const_iterator j = i;
            j++;
            if (j == _registered_.end ())
              {
                out_ << "`-- ";
              }
            else
              {
                out_ << "|-- ";
              }
            out_ << "ID: \"" <<  i->first << "\"" << std::endl;
          }
        return;
      }  

    private:

      bool             _verbose_;
      std::string      _label_;
      factory_map_type _registered_;

    };

  } // end of namespace factory

} // end of namespace datatools

/***************************************************************************/

#include <boost/type_traits/is_base_of.hpp>

namespace datatools {

  namespace factory {

    /* Utility to enable auto-registration of the Type class in the
     * the system factory register:
     */
    template <class BaseType, class DerivedType>
    class _system_factory_registrator
    {
    private:
      void _trigger_factory_registration_ (const std::string & type_id_)
      {
        if (! boost::is_base_of<BaseType, DerivedType>::value)
          {
            std::ostringstream message;
            message << "datatools::factory::_system_factory_registrator::_trigger_factory_registration_: "
                    << "Class ID '" << type_id_ << "' cannot be registered in register '" 
                    << BaseType::grab_system_factory_registration ().label () << "' !";
            throw std::logic_error (message.str ());
          }
        BaseType::grab_system_factory_registration ().registration (type_id_, boost::factory<DerivedType*>());
        return;
      }

    public:
      _system_factory_registrator (const std::string & type_id_)
      {
        _trigger_factory_registration_ (type_id_);
        return;
      }
    };

  } // end of namespace factory

} // end of namespace datatools

/***************************************************************************/

#define DATATOOLS_FACTORY_INTERFACE(BaseType) \
  public: \
    typedef datatools::factory::factory_register<BaseType> factory_register_type; \
  /**/

#define DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(BaseType) \
    DATATOOLS_FACTORY_INTERFACE (BaseType) \
  public: \
    typedef boost::scoped_ptr<factory_register_type> scoped_factory_register_type; \
    static factory_register_type & grab_system_factory_registration (); \
    static const factory_register_type & get_system_factory_registration (); \
   \
  private : \
    static scoped_factory_register_type _g_datatools_factory_system_factory_registration_; \
  /**/

// Instantiate the system allocator/functor register and accessors : 
#define DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(BaseType, RegisterLabel) \
 \
  BaseType::scoped_factory_register_type BaseType::_g_datatools_factory_system_factory_registration_ ( \
    new BaseType::factory_register_type (RegisterLabel)); \
 \
  BaseType::factory_register_type & BaseType::grab_system_factory_registration () \
  { \
    return *(BaseType::_g_datatools_factory_system_factory_registration_.get ()); \
  } \
 \
  const BaseType::factory_register_type & BaseType::get_system_factory_registration () \
  { \
    return *(BaseType::_g_datatools_factory_system_factory_registration_.get ()); \
  } \
  /**/

#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(BaseType, DerivedType) \
  private:                                                  \
  static datatools::factory::_system_factory_registrator<BaseType,DerivedType> _g_system_factory_registration_; \
  /**/

#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(BaseType, DerivedType, TypeId) \
  datatools::factory::_system_factory_registrator<BaseType,DerivedType> DerivedType::_g_system_factory_registration_ (TypeId); \
  /**/

/**************************************************************************************/

namespace my
{
  /// Base class
  class base_type
  {
  public:

    base_type ();

    virtual ~base_type ();

    virtual std::string to_string  () const = 0;
  
    // Factory stuff :
  
  public:

    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE (base_type);

  };

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (base_type, "my::base_type/__system__");

  base_type::base_type ()
  {
    std::cout << "base_type: construction..." << std::endl;
    return;
  }

  base_type::~base_type ()
  {
    std::cout << "base_type: destruction." << std::endl;
    return;
  }

  /***************************************************************************/

  /// Derived class #1
  struct derived_1_type : public base_type
  {
    derived_1_type ()
    {
      std::cout << "derived_1_type: construction..." << std::endl;
    }
    virtual ~derived_1_type ()
    {
      std::cout << "derived_1_type: destruction." << std::endl;
    }
    virtual std::string to_string  () const 
    {
      return "derived_1_type";
    }
    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (base_type, derived_1_type);
  };

  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (base_type, derived_1_type, "my::derived_1_type");

  /***************************************************************************/

  /// Derived class #2
  struct derived_2_type : public base_type
  {
    derived_2_type ()
    {
      std::cout << "derived_2_type: construction..." << std::endl;
    }
    virtual ~derived_2_type ()
    {
      std::cout << "derived_2_type: destruction." << std::endl;
    }
    virtual std::string to_string  () const 
    {
      return "derived_2_type";
    }
    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (base_type, derived_2_type);
  };

  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (base_type, derived_2_type, "my::derived_2_type");

  /***************************************************************************/

  /// Derived class #2/#1
  struct derived_21_type : public base_type
  {
    derived_21_type ()
    {
      std::cout << "derived_21_type: construction..." << std::endl;
    }
    virtual ~derived_21_type ()
    {
      std::cout << "derived_21_type: destruction." << std::endl;
    }
    virtual std::string to_string  () const 
    {
      return "derived_21_type";
    }
  };

} // end of namespace my;

/***************************************************************************/

int main (void)
{
  
  /*
  my::base_type::grab_system_factory_registration ().registration ("my::derived_1_type", boost::factory<my::derived_1_type*>());
  my::base_type::grab_system_factory_registration ().registration ("my::derived_2_type", boost::factory<my::derived_2_type*>());
  */
  my::base_type::grab_system_factory_registration ().print (std::clog, "DEVEL: ");

  my::base_type::factory_register_type the_factory_register (
    "my::base_type::factory", 
    my::base_type::factory_register_type::verbose);
  the_factory_register.import (my::base_type::grab_system_factory_registration ());
  the_factory_register.registration ("my::derived_21_type", boost::factory<my::derived_21_type*>());
  the_factory_register.print (std::clog, "The : ");

  typedef std::map<std::string, std::string> directives_list_type;
  directives_list_type factory_directives;
  factory_directives["obj1"] = "my::derived_1_type";
  factory_directives["obj2"] = "my::derived_1_type";
  factory_directives["obj3"] = "my::derived_2_type";
  factory_directives["obj4"] = "my::derived_21_type";
  factory_directives["obj5"] = "my::derived_1_type";
  factory_directives["obj6"] = "my::derived_2_type";

  typedef boost::shared_ptr<my::base_type> object_smart_ptr_type;
  typedef std::map<std::string, object_smart_ptr_type> object_map_type;
  object_map_type objects;

  std::clog << "Creating objects..." << std::endl;
  for (directives_list_type::const_iterator i 
         = factory_directives.begin ();
       i != factory_directives.end ();
       i++)
    {
      const std::string & obj_name = i->first;
      const std::string & obj_type_id = i->second;
      if (! the_factory_register.has (obj_type_id))
        {
          std::cerr << "No registered class with ID '" 
                    << obj_type_id << "' !" << std::endl;
        }
      else
        {
          my::base_type::factory_register_type::factory_type & the_factory 
            = the_factory_register.get (obj_type_id);
          object_smart_ptr_type tmp_ptr (the_factory ());
          objects[obj_name] = tmp_ptr;
        }
    }
  
  std::clog << "Done." << std::endl;
  
  for (object_map_type::const_iterator i 
         = objects.begin ();
       i != objects.end ();
       i++)
    {
      const std::string & obj_name = i->first;
      const my::base_type * obj_ptr = i->second.get (); 
      std::clog << "'" << obj_name << "' is a '" << obj_ptr->to_string () << "'" << std::endl;
    }
  
  std::clog << "End." << std::endl;
  
  return 0;
}
