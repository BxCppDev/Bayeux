// test_multi_properties.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Ourselves:
#include <datatools/multi_properties.h>
#include <datatools/multi_properties_config.h>

// This project:
#include <datatools/io_factory.h>

int main (int argc_ , char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test of the 'multi_properties' class..." << endl;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      // if (arg == "-d" || arg == "--debug") debug = true;
      iarg++;
    }

    {
      clog << endl << "Write test:" << endl;
      datatools::multi_properties mp ("name", "type", "A sample list of setups");

      mp.add ("object_1", "type_1");
      datatools::multi_properties::entry & e1 = mp.grab ("object_1");
      e1.grab_properties ().store ("debug", true);
      e1.grab_properties ().store ("colour", "blue");
      e1.grab_properties ().store ("depth", 1.234);

      mp.add ("object_2", "type_2");
      datatools::multi_properties::entry & e2 = mp.grab ("object_2");
      e2.grab_properties ().store ("debug", false);
      e2.grab_properties ().store ("temperature", 300.0);

      mp.add ("object_3", "type_1");
      datatools::multi_properties::entry & e3 = mp.grab ("object_3");
      e3.grab_properties ().store ("test", true);
      e3.grab_properties ().store ("mass", 15.0);
      e3.grab_properties ().store ("length", 0.12);
      e3.grab_properties ().store ("width", 0.04);
      e3.grab_properties ().store ("depth", 0.06);

      mp.add (datatools::properties::make_private_key ("object_4"), "type_3");
      datatools::multi_properties::entry & e4
        = mp.grab (datatools::properties::make_private_key ("object_4"));
      e4.grab_properties ().store ("secret", true);
      e4.grab_properties ().store ("password", "********");
      e4.grab_properties ().store ("code", 1234567);

      mp.add ("object_3a", "type_1");
      datatools::multi_properties::entry & e3a = mp.grab ("object_3a");
      e3a.grab_properties ().store ("to_be_removed", true);
      mp.add ("object_3b", "type_1");
      datatools::multi_properties::entry & e3b = mp.grab ("object_3b");
      e3b.grab_properties ().store ("to_be_removed", true);

      clog << endl << "mp :" << endl;
      mp.print_tree(clog);

      clog << endl << "Removing 2 entries..." << endl;
      mp.remove ("object_3a");
      mp.remove ("object_3b");

      clog << endl << "mp :" << endl;
      mp.print_tree(clog);

      clog << endl << "Saving in ASCII file keeping private entries..." << endl;
      datatools::multi_properties::write_config("test_multi_properties.conf", mp,
                datatools::multi_properties_config::HEADER_FOOTER);

      {
        clog << endl << "Serializing in XML archive file..." << endl;
        string filename = "test_multi_properties.xml";
        clog << "Writing to '" << filename << "' ..." << endl;
        datatools::data_writer writer (filename);
        writer.store (mp);
        clog << "Serialization is done." << endl;
      }

    }

    {
      clog << endl << "Read test:" << endl;
      datatools::multi_properties mp ("name", "type");
      clog << "Reading from ASCII file skipping private entries..." << endl;
      datatools::multi_properties::read_config("test_multi_properties.conf", mp,
               datatools::multi_properties_config::SKIP_PRIVATE_SECTIONS);
      clog << "mp :" << endl;
      mp.print_tree(clog);
      datatools::multi_properties::write_config("test_multi_properties_2.conf", mp,
                datatools::multi_properties_config::HEADER_FOOTER
                | datatools::multi_properties_config::SKIP_PRIVATE_SECTIONS);

      {
        clog << endl << "Deserializing from XML archive file..." << endl;
        string filename = "test_multi_properties.xml";
        clog << "Loading from '" << filename << "' ..." << endl;
        datatools::data_reader reader (filename);
        reader.load (mp);
        clog << "Deserialization is done." << endl;
        clog << "mp :" << endl;
        mp.print_tree(clog);
      }
    }

  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
