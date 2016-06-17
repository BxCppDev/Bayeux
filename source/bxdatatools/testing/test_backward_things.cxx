// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

// This project:
#include <datatools/things.h>
#include <datatools/things.ipp>
#include <datatools/io_factory.h>
#include <datatools/serialization_macros.h>

class backward_things : public datatools::things
{
public:

  int dummy;

  backward_things();

  virtual ~backward_things();

  virtual void tree_dump(std::ostream& a_out = std::clog,
                         const std::string& a_title  = "",
                         const std::string& a_indent = "",
                         bool a_inherit = false) const;

  //! Serialization interface
  DATATOOLS_SERIALIZATION_DECLARATION()
  DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

};

// Serialization declaration :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(backward_things)
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(backward_things,"datatools::utils::things")

void test1();
void test2();

int main (int /*argc_*/ , char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    test1();
    test2();
  }
  catch (std::exception & x) {
    std::clog << "error: " << x.what() << std::endl;
    error_code =  EXIT_FAILURE;
  }
  catch (...) {
    std::clog << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test1()
{
  std::clog << "\ntest1...\n";
  std::string filename = "test_backward_things.xml";
  std::string filename2 = "test_new_things.xml";

  {
    backward_things bt;
    bt.set_name("garrido_ne_comprend_rien");
    bt.set_description("mais_il_se_soigne");
    bt.tree_dump(std::clog, "Backward things:");
    datatools::data_writer writer(filename);
    writer.store(bt);
  }

  {
    backward_things bt;
    {
      // Read it:
      datatools::data_reader reader(filename);
      reader.load(bt);
    }
    // Cast it as a legacy things:
    const datatools::things & t = bt;
    t.tree_dump(std::clog, "Things (inherited):");
    {
      // Store it as a legacy things:
      t.datatools::things::tree_dump(std::clog, "Things:");
      datatools::data_writer writer(filename2);
      // Explicit use of the datatools::things serial tag
      // to bypass inheritance of the 'backward_things::get_serial_tag'
      writer.store<datatools::things>(datatools::things::serial_tag(), t);
    }
  }
  return;
}

void test2()
{
  std::clog << "\ntest2...\n";
  std::string filename = "test2_backward_things.xml";
  {
    datatools::things t;
    t.set_name("garrido_va_comprendre");
    t.set_description("un jour");
    t.tree_dump(std::clog, "Things:");
    datatools::things t2;
    t2.set_name("garrido_va_comprendre_le_retour");
    t2.set_description("peut_etre");
    t2.tree_dump(std::clog, "Things(2):");
    {
      datatools::data_writer writer(filename);
      writer.store("datatools::utils::things", t);
      writer.store(t2); // using modern serial tag
    }

  }

  {
    {
      datatools::data_reader reader(filename);
      while (reader.has_record_tag()) {
        datatools::things t;
        if (reader.record_tag_is("datatools::utils::things")) {
          reader.load("datatools::utils::things", t);
          std::cerr << "Old serial tag!\n";
        } else if (reader.record_tag_is("datatools::things")) {
          reader.load(t);
          std::cerr << "Modern serial tag!\n";
        } else {
          std::cerr << "Unrecognized serial tag!\n";
        }
        t.tree_dump(std::clog, "Things (from mixed serial tags) :");
      }
    }
  }
  return;
}

backward_things::backward_things()
{
  dummy = 666;
  return;
}

backward_things::~backward_things()
{
  return;
}

void backward_things::tree_dump(std::ostream& a_out, const std::string& a_title,
                                const std::string& a_indent,
                                bool a_inherit) const {
  this->datatools::things::tree_dump(a_out, a_title, a_indent, true);
  a_out << a_indent << i_tree_dumpable::inherit_tag(a_inherit)
        << "Dummy : " << dummy << std::endl;

  return;
}

DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(backward_things,archive,version)
{
  this->datatools::things::serialize(archive, version);
  archive & boost::serialization::make_nvp("dummy", dummy);
  return;
}

// Serialization implementation :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(backward_things, "datatools::utils::things")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(backward_things, "not_used")
// #include <datatools/archives_instantiation.h>
// DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(backward_things)
BOOST_CLASS_EXPORT_IMPLEMENT(backward_things)
