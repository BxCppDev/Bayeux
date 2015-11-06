// test_serialization.cxx

// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

// This project:
#include <datatools/io_factory.h>
#include <datatools/safe_serial.h>
// A serializable test class :
#include <datatools_test_my_data.cc>
#include <datatools_test_my_data.ipp>

// Special Boost serialization export/registration code :
BOOST_CLASS_EXPORT_KEY2(datatools::test::data_t, "datatools::test::data_t")
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t)
BOOST_CLASS_EXPORT_KEY2(datatools::test::more_data_t, "datatools::test::more_data_t")
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t)

// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(datatools::test::data_t, 33)
// BOOST_CLASS_VERSION(datatools::test::more_data_t, 66)

using namespace std;

void test_shared_ptr();

int main(int argc_, char ** argv_)
{
  try {
    bool debug = false;
    long seed  = 12345;
    string filename = "";
    size_t nrecords = 3;

    enum format_type {
      FORMAT_TXT = 0,
      FORMAT_XML = 1,
      FORMAT_BIN = 2
    };
    int fmt   = FORMAT_XML;
    bool test = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      if (arg[0] == '-') {
        if (arg == "-d") debug = true;
        if (arg == "-10") nrecords = 10;
        if (arg == "-100") nrecords = 100;
        if (arg == "-1000") nrecords = 1000;
        if (arg == "-xml") fmt = FORMAT_XML;
        if (arg == "-txt") fmt = FORMAT_TXT;
        if (arg == "-bin") fmt = FORMAT_BIN;
        if (arg == "-test") test = true;
      } else {
        if (filename.empty()) {
          filename = arg;
        } else {
          istringstream iss(arg);
          iss >> seed;
          if (! iss) {
            ostringstream message;
            message << "format error for 'seed'!";
            throw runtime_error(message.str());
          }
        }
      }
      iarg++;
    }

    if (filename.empty()) {
      if (fmt == FORMAT_XML) filename = "test_serialization.xml";
      if (fmt == FORMAT_TXT) filename = "test_serialization.txt";
      if (fmt == FORMAT_BIN) filename = "test_serialization.data";
    }

    srand48(seed);
    clog << "NOTICE: using filename '" << filename << "'" << endl;

    int mode_guess;
    if (datatools::io_factory::guess_mode_from_filename(filename, mode_guess)
        != datatools::io_factory::SUCCESS) {
      ostringstream message;
      message << "Cannot guess mode for file '" << filename << "'!";
      throw runtime_error(message.str());
    }
    if (debug) clog << "DEBUG: mode = " << hex
                    << mode_guess
                    << dec << endl;

    if (boost::filesystem::exists(filename)) {
      ostringstream message;
      message << "File '" << filename << "' already exists!";
      clog << "WARNING: " << message.str() << " Remove it !" << endl;
      unlink(filename.c_str());
    }

    {
      clog << "NOTICE: writing..." << endl;
      datatools::data_writer writer(filename);

      // safe_serial
      datatools::safe_serial<datatools::test::data_t>      ss_data;
      datatools::safe_serial<datatools::test::more_data_t> ss_more_data;

      for (int i = 0; i < (int) nrecords; i++) {
        if (debug) clog << "DEBUG: Counts = " << i << endl;
        double p = 0.25 + 0.5 * (i % 2);
        if (p < 0.5) {
          ss_data.make();
          randomize_data(ss_data.grab());
          ss_data.get().tree_dump(clog, "data_t", "<< ");
          writer.store(ss_data.get());
        } else {
          ss_more_data.make();
          randomize_more_data(ss_more_data.grab());
          ss_more_data.get().tree_dump(clog, "more_data_t", "<< ");
          writer.store(ss_more_data.get());
        }
      }
      if (test) {
        string test_str = "test-failure";
        writer.store("__TEST__", test_str);
      }
      clog << "NOTICE: writing done." << endl << endl;
    }

    {
      clog << endl << endl << "NOTICE: reading..." << endl;

      datatools::safe_serial<datatools::test::data_t>      ss_data;
      datatools::safe_serial<datatools::test::more_data_t> ss_more_data;

      size_t counts = 0;
      datatools::data_reader reader(filename);
      reader.dump(clog);
      while (reader.has_record_tag()) {
        if (debug) clog << "DEBUG: read next record..." << endl;
        if (reader.record_tag_is(datatools::test::data_t::SERIAL_TAG)) {
          if (debug) clog << "DEBUG: reading..."
                          << datatools::test::data_t::SERIAL_TAG << endl;
          if (debug) clog << "DEBUG: making a new safe record..."
                          << endl;
          ss_data.make();
          if (debug) clog << "DEBUG: loading the new safe record..."
                          << endl;
          reader.load(ss_data.grab());
          if (debug) clog << "DEBUG: loading done."
                          << endl;
          ss_data.get().tree_dump(clog, "data_t", ">> ");
        } else if (reader.record_tag_is(datatools::test::more_data_t::SERIAL_TAG)) {
          if (debug)clog << "DEBUG: reading..."
                         << datatools::test::more_data_t::SERIAL_TAG
                         << endl;
          if (debug) clog << "DEBUG: making a new safe record..."
                          << endl;
          ss_more_data.make();
          if (debug) clog << "DEBUG: loading the new safe record..."
                          << endl;
          reader.load(ss_more_data.grab());
          if (debug) clog << "DEBUG: loading done."
                          << endl;
          ss_more_data.get().tree_dump(clog, "more_data_t", ">> ");
        } else {
          string bad_tag = reader.get_record_tag ();
          clog << "ERROR: unknown data tag '"
               << bad_tag << "'!" << endl;
          break;
        }
        counts++;
        if(debug) clog << "DEBUG: Counts = " << counts << endl;
      }
      clog << "NOTICE: reading done." << endl << endl;
    }

    test_shared_ptr();

  }
  catch (exception & x) {
    cerr << "test_serialization: ERROR: " << x.what() << endl;
    exit(EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

class bar
{
public:
  bar(datatools::test::data_t * data_) : _sh_data_(data_) {}
  const datatools::test::data_t * get() const { return _sh_data_.get(); }
  datatools::test::data_t * grab() { return _sh_data_.get(); }
  void reset() { _sh_data_.reset(); }
private:
  boost::shared_ptr<datatools::test::data_t> _sh_data_;

  friend class boost::serialization::access;
  template<class Archive>
  void serialize (Archive & ar_,
                  const unsigned int /* version_ */)
  {
    ar_ & boost::serialization::make_nvp ("sh_data", _sh_data_);
    return;
  }
};

void test_shared_ptr()
{
  boost::shared_ptr<datatools::test::data_t> shFoo(new datatools::test::data_t('c', 17, 1.234e5, 7));
  bar foo(new datatools::test::data_t('z', 42, 3.14, 12));
  bar foo2(new datatools::test::more_data_t('a', 666, 2.718, 3, "dummy"));
  boost::shared_ptr<datatools::test::data_t> shFoo2 = shFoo;

  shFoo->tree_dump(std::clog, "shFoo: ");
  foo.get()->tree_dump(std::clog, "foo: ");
  foo2.get()->tree_dump(std::clog, "foo2: ");
  shFoo2->tree_dump(std::clog, "shFoo2: ");

  std::string filename = "test_serialization_shared_ptr.xml";
  {
    std::clog << "NOTICE: writing..." << std::endl;
    datatools::data_writer writer(filename);
    writer.store("shptr_bar", shFoo);
    writer.store("bar", foo);
    writer.store("bar", foo2);
    writer.store("shptr_bar", shFoo2);
    std::clog << "NOTICE: done." << std::endl;
  }

  std::clog << "NOTICE: reset objects..." << std::endl;
  shFoo.reset();
  foo.reset();
  foo2.reset();
  shFoo2.reset();
  std::clog << "NOTICE: done." << std::endl;

  {
    std::clog << "NOTICE: reading..." << std::endl;
    datatools::data_reader reader(filename);
    reader.load("shptr_bar", shFoo);
    reader.load("bar", foo);
    reader.load("bar", foo2);
    reader.load("shptr_bar", shFoo2);
    std::clog << "NOTICE: done." << std::endl;
  }

  shFoo->tree_dump(std::clog, "shFoo: ");
  foo.get()->tree_dump(std::clog, "foo: ");
  foo2.get()->tree_dump(std::clog, "foo2: ");
  shFoo2->tree_dump(std::clog, "shFoo2: ");

  return;
}
