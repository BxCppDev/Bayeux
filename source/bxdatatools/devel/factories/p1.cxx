#include <string>
#include <map>
#include <iostream>

#include <boost/function.hpp>
//#include <boost/functional.hpp>
#include <boost/functional/factory.hpp>
#include <boost/shared_ptr.hpp>

struct base_type
{
  base_type ()
  {
    std::cout << "base_type: construction..." << std::endl;
  }
  virtual ~base_type ()
  {
    std::cout << "base_type: destruction." << std::endl;
  }
  virtual void print () const = 0;
};

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
  virtual void print () const 
  {
    std::clog << "derived_1_type" << std::endl;
  }
};

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
  virtual void print () const 
  {
    std::clog << "derived_2_type" << std::endl;
  }
};

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
  virtual void print () const 
  {
    std::clog << "derived_21_type" << std::endl;
  }
};

typedef boost::function< base_type*() > factory_type;
typedef std::map<std::string, factory_type> factory_map_type;

int main (void)
{
  factory_map_type factories;
  factories["derived_1_type"] =  boost::factory<derived_1_type*>();
  factories["derived_2_type"] =  boost::factory<derived_2_type*>();
  factories["derived_21_type"] =  boost::factory<derived_21_type*>();

  typedef std::map<std::string, std::string> directives_list_type;
  directives_list_type factory_directives;
  factory_directives["obj1"] = "derived_1_type";
  factory_directives["obj2"] = "derived_1_type";
  factory_directives["obj3"] = "derived_2_type";
  factory_directives["obj4"] = "derived_21_type";
  factory_directives["obj5"] = "derived_1_type";
  factory_directives["obj6"] = "derived_2_type";

  typedef boost::shared_ptr<base_type> object_smart_ptr_type;
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
      object_smart_ptr_type tmp_ptr (factories[obj_type_id] ());
      objects[obj_name] = tmp_ptr;
    }

  std::clog << "Done." << std::endl;

  for (object_map_type::const_iterator i 
         = objects.begin ();
       i != objects.end ();
       i++)
    {
      const std::string & obj_name = i->first;
      const base_type *   obj_ptr = i->second.get (); 
      obj_ptr->print ();
    }

  std::clog << "End." << std::endl;
  
  return 0;
}
