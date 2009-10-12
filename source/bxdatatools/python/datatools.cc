/* datatools.cc */

#include <boost/python.hpp>
#include <boost/python/def.hpp>

#include <datatools/utils/command.h>
#include <datatools/utils/properties.h>
#include <datatools/event/event_id.h>
#include <datatools/event/basic_event.h>
#include <datatools/serialization/io_factory.h>

using namespace boost::python;

/*************/

void export_utils_command ()
{
  using namespace boost::python;

  // Create a member function pointers as above 
  // for both 'datatools::utils::command::init' overloads:

  void (datatools::utils::command::*du_command_init_s) (const std::string &) 
    = &datatools::utils::command::init;

  void (datatools::utils::command::*du_command_init_sb) (const std::string &,
						       bool) 
    = &datatools::utils::command::init;

  class_<datatools::utils::command> ("command", init<> ())

    .def(init<const std::string &>())

    .def(init<const std::string &, bool>())

    .def("init", du_command_init_s)

    .def("init", du_command_init_sb)

    .def("is_pass", 
	 &datatools::utils::command::is_pass)

    .def("is_error", 
	 &datatools::utils::command::is_error)

    .def("is_failure", 
	 &datatools::utils::command::is_failure)

    .def("is_success", 
	 &datatools::utils::command::is_success)

    .def("is_with_options", 
	 &datatools::utils::command::is_with_options)

    .def("has_option", 
	 &datatools::utils::command::has_option)

    .def("get_error_code", 
	 &datatools::utils::command::get_error_code)

    .def("set_error_code", 
	 &datatools::utils::command::set_error_code)

    .def("get_error_message", 
	 &datatools::utils::command::get_error_message)

    .def("set_error", 
	 &datatools::utils::command::set_error)

    .def("set_error_message", 
	 &datatools::utils::command::set_error_message)

    .def("set_returns", 
	 &datatools::utils::command::set_returns)

    .def("get_returns", 
	 &datatools::utils::command::get_returns)

    .def("reset", 
	 &datatools::utils::command::reset)

    .def("dump", 
	 &datatools::utils::command::dump_def)

    .def("dump_stdout", 
	 &datatools::utils::command::dump_stdout)

    .def("dump_stderr", 
	 &datatools::utils::command::dump_stderr)

    .def("get_name", 
	 &datatools::utils::command::get_name)

    .def("set_name", 
	 &datatools::utils::command::set_name)

    .def("get_number_of_arguments", 
	 &datatools::utils::command::get_number_of_arguments)

    .def("get_number_of_options", 
	 &datatools::utils::command::get_number_of_options)

    .def("add_argument", 
	 &datatools::utils::command::add_argument)

    .def("add_option", 
	 &datatools::utils::command::add_option)

    .def("pop_argument", 
	 &datatools::utils::command::pop_argument)

    .def("pop_option", 
	 &datatools::utils::command::pop_option)

    .def("get_argument", 
	 &datatools::utils::command::get_argument)

    .def("get_option", 
	 &datatools::utils::command::get_option)

    .def("shift_one", 
	 &datatools::utils::command::shift_one)

    .def("force_with_options", 
	 &datatools::utils::command::force_with_options)

    ;
}


/*************/
  
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_fetch_boolean_overloads, fetch_boolean, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_fetch_integer_overloads, fetch_integer, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_fetch_real_overloads,    fetch_real, 1, 2)
    
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_fetch_string_overloads,  fetch_string, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_store_boolean_overloads, store_boolean, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_store_integer_overloads, store_integer, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_store_real_overloads, store_real, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_store_string_overloads, store_string, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dup_store_flag, store_flag, 1, 3)

void export_utils_properties ()
{
  using namespace boost::python;
    
  bool (datatools::utils::properties::*dup_fetch_boolean)(const std::string &, 
							  int) const
    = &datatools::utils::properties::fetch_boolean;
  
  int (datatools::utils::properties::*dup_fetch_integer)(const std::string &, 
							 int) const
    = &datatools::utils::properties::fetch_integer;
  
  double (datatools::utils::properties::*dup_fetch_real)(const std::string &, 
							 int) const
    = &datatools::utils::properties::fetch_real;
  
  std::string (datatools::utils::properties::*dup_fetch_string)(const std::string &, 
								int) const
    = &datatools::utils::properties::fetch_string;
  

  void (datatools::utils::properties::*dup_store_boolean)(const std::string &, 
							  bool,
							  const std::string &,
							  bool) 
    = &datatools::utils::properties::store_boolean;
  
  void (datatools::utils::properties::*dup_store_integer)(const std::string &, 
							  int,
							  const std::string &,
							  bool) 
    = &datatools::utils::properties::store_integer;
  
  void (datatools::utils::properties::*dup_store_real)(const std::string &, 
						       double,
						       const std::string &,
						       bool) 
    = &datatools::utils::properties::store_real;

  void (datatools::utils::properties::*dup_store_string)(const std::string &, 
							 const std::string &,
							 const std::string &,
							 bool) 
    = &datatools::utils::properties::store_string;
  
  
  void (datatools::utils::properties::*dup_change_boolean)(const std::string &, 
							   bool,
							   int) 
    = &datatools::utils::properties::change_boolean;
  
  void (datatools::utils::properties::*dup_change_integer)(const std::string &, 
							   int,
							   int) 
    = &datatools::utils::properties::change_integer;
  
  void (datatools::utils::properties::*dup_change_real)(const std::string &, 
							double,
							int) 
    = &datatools::utils::properties::change_real;
  
  void (datatools::utils::properties::*dup_change_string)(const std::string &, 
							  const std::string &,
							  int) 
    = &datatools::utils::properties::change_string;
  
  /******/
  
  void (datatools::utils::properties::*dup_update_boolean)(const std::string &, 
							   bool) 
    = &datatools::utils::properties::update_boolean;
  
  void (datatools::utils::properties::*dup_update_integer)(const std::string &, 
							   int) 
    = &datatools::utils::properties::update_integer;
  
  void (datatools::utils::properties::*dup_update_real)(const std::string &, 
							double) 
    = &datatools::utils::properties::update_real;
  
  void (datatools::utils::properties::*dup_update_string)(const std::string &, 
							  const std::string &) 
    = &datatools::utils::properties::update_string;

  class_<datatools::utils::properties> ("properties", init<> ())

    .def(init<const std::string &>())

    .def("is_debug", 
	 &datatools::utils::properties::is_debug)

    .def("set_debug", 
	 &datatools::utils::properties::set_debug)

    .def("get_description", 
	 &datatools::utils::properties::get_description,
	 return_internal_reference<>())

    .def("set_description", 
	 &datatools::utils::properties::set_description)

    .def("is_private", 
	 &datatools::utils::properties::is_private)

    .def("is_public", 
	 &datatools::utils::properties::is_public)

    .def("is_boolean", 
	 &datatools::utils::properties::is_boolean)

    .def("is_integer", 
	 &datatools::utils::properties::is_integer)

    .def("is_real", 
	 &datatools::utils::properties::is_real)

    .def("is_string", 
	 &datatools::utils::properties::is_string)

    .def("is_scalar", 
	 &datatools::utils::properties::is_scalar)

    .def("is_vector", 
	 &datatools::utils::properties::is_vector)

    .def("has_key", 
	 &datatools::utils::properties::has_key)

    .def("erase", 
	 &datatools::utils::properties::erase)

    .def("clean", 
	 &datatools::utils::properties::clean)

    .def("clear", 
	 &datatools::utils::properties::clear)

    .def("has_flag", 
	 &datatools::utils::properties::has_flag)

    .def("fetch_boolean_scalar", 
	 &datatools::utils::properties::fetch_boolean_scalar)

    .def("fetch_boolean_vector", 
	 &datatools::utils::properties::fetch_boolean_vector)

    .def("fetch_integer_scalar", 
	 &datatools::utils::properties::fetch_integer_scalar)

    .def("fetch_integer_vector", 
	 &datatools::utils::properties::fetch_integer_vector)

    .def("fetch_real_scalar", 
	 &datatools::utils::properties::fetch_real_scalar)

    .def("fetch_real_vector", 
	 &datatools::utils::properties::fetch_real_vector)

    .def("fetch_string_scalar", 
	 &datatools::utils::properties::fetch_string_scalar)

    .def("fetch_string_vector", 
	 &datatools::utils::properties::fetch_string_vector)

    .def("update_flag", 
	 &datatools::utils::properties::update_flag)

    .def("store_flag", 
	 &datatools::utils::properties::store_flag, dup_store_flag ())

    .def("set_flag", 
	 &datatools::utils::properties::set_flag)

    .def("unset_flag", 
	 &datatools::utils::properties::unset_flag)

    .def("fetch_boolean", dup_fetch_boolean, dup_fetch_boolean_overloads ())

    .def("fetch_integer", dup_fetch_integer, dup_fetch_integer_overloads ())

    .def("fetch_real",    dup_fetch_real,    dup_fetch_real_overloads ())

    .def("fetch_string",  dup_fetch_string,  dup_fetch_string_overloads ())

    .def("store_boolean", dup_store_boolean, dup_store_boolean_overloads ())

    .def("store_integer", dup_store_integer, dup_store_integer_overloads ())

    .def("store_real",    dup_store_real,    dup_store_real_overloads ())

    .def("store_string",  dup_store_string,  dup_store_string_overloads ())

    .def("change_boolean", 
	 dup_change_boolean)

    .def("change_integer", 
	 dup_change_integer)

    .def("change_real", 
	 dup_change_real)

    .def("change_string", 
	 dup_change_string)

    .def("update_boolean", 
	 dup_update_boolean)

    .def("update_integer", 
	 dup_update_integer)

    .def("update_real", 
	 dup_update_real)

    .def("update_string", 
	 dup_update_string)

    .def("get_serial_tag", 
	 &datatools::utils::properties::get_serial_tag,
	 return_value_policy<copy_const_reference> ())

    .def("dump", 
	 &datatools::utils::properties::dump_def)

    .def("dump_stdout", 
	 &datatools::utils::properties::dump_stdout)

    .def("dump_stderr", 
	 &datatools::utils::properties::dump_stderr)

    ;
   
}

/*************/

void export_event_id ()
{
  using namespace boost::python;

  class_<datatools::event::event_id> ("event_id", init<> ())

    .def(init<int, int>())

    .def("clear", &datatools::event::event_id::clear)

    .def("get_run_number", &datatools::event::event_id::get_run_number)

    .def("get_event_number", &datatools::event::event_id::get_event_number)

    .def("set_run_number", &datatools::event::event_id::set_run_number)

    .def("set_event_number", &datatools::event::event_id::set_event_number)

    .def("set", &datatools::event::event_id::set)

    .def("is_valid", &datatools::event::event_id::is_valid)

    .def("has", &datatools::event::event_id::has)

    .def("to_string", &datatools::event::event_id::to_string)

    .def("from_string", &datatools::event::event_id::from_string)

    .def("dump", &datatools::event::event_id::dump)

    .def("get_serial_tag", 
	 &datatools::event::event_id::get_serial_tag,
	 return_value_policy<copy_const_reference> ())

    ;
}

/*************/

void export_basic_event ()
{
  using namespace boost::python;

  class_<datatools::event::basic_event> ("basic_event", init<> ())

    .def (init<const datatools::event::event_id &>())

    .def ("clear", 
	 &datatools::event::basic_event::clear)

    .def ("set_id", 
	 &datatools::event::basic_event::set_id)

    .def ("get_id", 
	 &datatools::event::basic_event::get_id,
	 return_value_policy<copy_const_reference> ())

    .def ("get_properties", 
	 &datatools::event::basic_event::get_properties_ro,
	 return_value_policy<copy_const_reference> ())

    .def ("properties", 
	 &datatools::event::basic_event::get_properties_rw,
	 return_internal_reference<> ())

    .def ("get_serial_tag", 
	 &datatools::event::basic_event::get_serial_tag,
	 return_value_policy<copy_const_reference> ())

    .def ("dump", 
	 &datatools::event::basic_event::dump)

    ;
}

/*************/

void export_io_data_reader ()
{
  using namespace boost::python;

  class_<datatools::serialization::data_reader> ("data_reader", init<> ())

    .def (init<const std::string &, bool> ( ))

    .def (init<const std::string &, int> ( ))

    .def ("has_record_tag", 
	 &datatools::serialization::data_reader::has_record_tag)

    .def ("record_tag_is", 
	 &datatools::serialization::data_reader::record_tag_is)

    .def ("reset", 
	 &datatools::serialization::data_reader::reset)

    .def ("is_initialized", 
	 &datatools::serialization::data_reader::is_initialized)

    .def ("is_multi_archives", 
	 &datatools::serialization::data_reader::is_multi_archives)

    .def ("is_single_archive", 
	 &datatools::serialization::data_reader::is_single_archive)

    .def ("is_compressed", 
	 &datatools::serialization::data_reader::is_compressed)

    .def ("is_uncompressed", 
	 &datatools::serialization::data_reader::is_uncompressed)

    .def ("is_gzip", 
	 &datatools::serialization::data_reader::is_gzip)

    .def ("is_bzip2", 
	 &datatools::serialization::data_reader::is_bzip2)

    .def ("is_text", 
	 &datatools::serialization::data_reader::is_text)

    .def ("is_binary", 
	 &datatools::serialization::data_reader::is_binary)

    .def ("is_xml", 
	 &datatools::serialization::data_reader::is_xml)

    .def ("init_multi", 
	 &datatools::serialization::data_reader::init_multi)

    .def ("init_single", 
	 &datatools::serialization::data_reader::init_single)


    ;
}


void export_io_data_writer ()
{
  using namespace boost::python;

  class_<datatools::serialization::data_writer> ("data_writer", init<> ())

    .def (init<const std::string &, bool, bool> ( ))

    .def (init<const std::string &, int> ( ))

    .def ("reset", 
	 &datatools::serialization::data_writer::reset)

    .def ("is_initialized", 
	 &datatools::serialization::data_writer::is_initialized)

    .def ("is_multi_archives", 
	 &datatools::serialization::data_writer::is_multi_archives)

    .def ("is_single_archive", 
	 &datatools::serialization::data_writer::is_single_archive)

    .def ("is_compressed", 
	 &datatools::serialization::data_writer::is_compressed)

    .def ("is_uncompressed", 
	 &datatools::serialization::data_writer::is_uncompressed)

    .def ("is_gzip", 
	 &datatools::serialization::data_writer::is_gzip)

    .def ("is_bzip2", 
	 &datatools::serialization::data_writer::is_bzip2)

    .def ("is_text", 
	 &datatools::serialization::data_writer::is_text)

    .def ("is_binary", 
	 &datatools::serialization::data_writer::is_binary)

    .def ("is_xml", 
	 &datatools::serialization::data_writer::is_xml)


    ;
}

/*******************************************************************/
BOOST_PYTHON_MODULE (datatools)
{
  using namespace boost::python;

  export_utils_command ();

  export_utils_properties ();

  export_event_id ();

  export_basic_event ();

  export_io_data_reader ();

  export_io_data_writer ();

}

/* end of datatools.cc */

