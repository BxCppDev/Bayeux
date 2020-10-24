// test_multi_properties_merging.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Ourselves:
#include <datatools/multi_properties.h>
#include <datatools/io_factory.h>

void mpm();

int main (int argc_ , char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test of the 'multi_properties::merge_with' method..." << endl;
    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      // if (arg == "-d" || arg == "--debug") debug = true;
      iarg++;
    }

    mpm();

  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void mpm()
{
  using namespace std;

  datatools::multi_properties baseConf("name", "type", "Base configuration");

  baseConf.add("object_1", "type_1");
  datatools::multi_properties::entry & e1 = baseConf.grab("object_1");
  e1.grab_properties().store("debug", true);
  e1.grab_properties().store("colour", "blue");
  e1.grab_properties().store("depth", 1.234);
  e1.grab_properties().store("foo", "default");
  e1.grab_properties().store("bar", 3.14);

  baseConf.add("object_2", "type_2");
  datatools::multi_properties::entry & e2 = baseConf.grab("object_2");
  e2.grab_properties().store("debug", false);
  e2.grab_properties().store("temperature", 300.0);

  baseConf.add("object_3", "type_1");
  datatools::multi_properties::entry & e3 = baseConf.grab("object_3");
  e3.grab_properties().store("test", true);
  e3.grab_properties().store("mass", 15.0);
  e3.grab_properties().store("length", 0.12);
  e3.grab_properties().store("width", 0.04);
  e3.grab_properties().store("depth", 0.06);

  clog << endl << "baseConf :" << endl;
  baseConf.print_tree(clog);

  // merging functionalities:
  bool label_error = false;

  std::string meta_label = "type";
  if (label_error) {
    meta_label = "kind";
  }
  datatools::multi_properties addonConf("name", meta_label, "Add-on configuration");
  addonConf.add("new_object", "new_type");
  datatools::multi_properties::entry & s0 = addonConf.grab("new_object");
  s0.grab_properties().store("baz", true);
  s0.grab_properties().store("dummy", "dummy");
  addonConf.add("object_1", "type_1");
  datatools::multi_properties::entry & s1 = addonConf.grab("object_1");
  s1.grab_properties().store("foo", "overriden");

  bool override_sections = true;
  bool override_props = true;
  baseConf.merge_with(addonConf, override_sections, override_props);
  clog << endl << "baseConf after merging :" << endl;
  baseConf.print_tree(clog);

  {
    clog << endl << "Saving in ASCII file..." << endl;
    datatools::write_config("test_merged.conf", baseConf);
  }

  {
    clog << endl << "Serializing in XML archive file..." << endl;
    string filename = "test_merged.xml";
    clog << "Writing to '" << filename << "' ..." << endl;
    datatools::data_writer writer(filename);
    writer.store(baseConf);
    clog << "Serialization is done." << endl;
  }

  {
    clog << endl << "Read test:" << endl;
    clog << "Reading from ASCII file..." << endl;
    datatools::multi_properties effectiveConf("name", "type");
    datatools::read_config("test_merged.conf", effectiveConf);
    clog << endl << "effectiveConf after merging :" << endl;
    effectiveConf.print_tree(clog);
  }

  {
    clog << endl << "Deserializing from XML archive file..." << endl;
    string filename = "test_merged.xml";
    datatools::multi_properties effectiveConf("name", "type");
    clog << "Loading from '" << filename << "' ..." << endl;
    datatools::data_reader reader(filename);
    reader.load(effectiveConf);
    clog << endl << "effectiveConf after deserialization :" << endl;
    effectiveConf.print_tree(clog);
  }

  return;
}
