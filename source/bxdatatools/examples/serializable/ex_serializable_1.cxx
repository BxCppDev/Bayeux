/* ex_serializable_1.cxx
 *
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-05-15
 * Last modified : 2013-05-15
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description :
 * This example illustrates the use of serializable
 * classes.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// Third party:
// - Boost:
// Code dedicated to the serialization of the ``std::vector`` template class :
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/clhep_units.h>
#include <datatools/things.h>
#include <datatools/properties.h>
#include <datatools/handle.h>
#include <datatools/io_factory.h>

// This project:
#include <raw_hit.h>
#include <raw_data.h>

/// Serialize a single ``raw_hit`` instance in a XML file
void ex_raw_hit_1(datatools::logger::priority logging)
{
  // Declare a ``raw_hit`` object :
  DT_LOG_DEBUG(logging, "Declare a raw hit");
  raw_hit RH;

  // Initialize the hit attributes :
  DT_LOG_DEBUG(logging, "Initialize the raw hit");
  RH.set_id(34);
  RH.randomize();

  // Print it :
  DT_LOG_NOTICE(logging, "Raw hit : ");
  RH.dump(std::clog);

  // Store the hit in a Boost/archive file (XML format):
  DT_LOG_DEBUG(logging, "Serialize the raw hit...");
  datatools::data_writer serializer("raw_hit.xml",
                                    datatools::using_multiple_archives);
  serializer.store(RH);
  DT_LOG_INFORMATION(logging, "The hit has been stored in the ``raw_hit.xml`` file.");
}


/// Deserialize a single ``raw_hit`` instance from a XML file
void ex_raw_hit_2(datatools::logger::priority logging)
{
  // Declare a ``raw_hit`` object :
  DT_LOG_DEBUG(logging, "Declare a raw hit");
  raw_hit RH;

  // Load the hit from a Boost/archive file (XML format):
  DT_LOG_DEBUG(logging, "Deserialize the raw hit...");
  datatools::data_reader deserializer("raw_hit.xml",
                                      datatools::using_multiple_archives);
  deserializer.load(RH);

  // Print it :
  DT_LOG_NOTICE(logging, "Deserialized raw hit : ");
  RH.dump(std::clog);
}


/// Serialize several ``raw_hit`` instances in a XML file
void ex_raw_hit_3(datatools::logger::priority logging)
{
  DT_LOG_DEBUG(logging, "Create many hits and serialize them");

  // We use here the multi-archive mode to store all the hits.
  // This means that each hit is stored and confined indenpendantly
  // in its own Boost XML archive. This protect the data against
  // archive corruption due to the reuse by each hit of the same
  // memory region (see the section about *memory tracking* in the
  // Boost/Serialization documentation) :
  datatools::data_writer serializer("raw_hits.xml",
                                    datatools::using_multiple_archives);
  raw_hit RH;
  int number_of_hits = 3;
  for (int hit_id = 0; hit_id < number_of_hits; hit_id++) {
    RH.set_id(hit_id);
    RH.randomize();

    DT_LOG_NOTICE(logging, "Serialized raw hit #" << hit_id);
    RH.dump(std::clog);

    serializer.store(RH);
    RH.reset();
  }
  DT_LOG_INFORMATION(logging,
                     "Number of serialized raw hits is "
                     << number_of_hits);
  DT_LOG_INFORMATION(logging,
                     "The hits have been stored in the ``raw_hits.xml`` file.");
}


/// Deserialize several ``raw_hit`` instances from a XML file
void ex_raw_hit_4(datatools::logger::priority logging)
{
  // We use the multi-archive mode to load the hits because this was
  // the mode used by the serializer :
  datatools::data_reader deserializer("raw_hits.xml",
                                      datatools::using_multiple_archives);
  raw_hit RH;
  int number_of_hits = 0;
  while (deserializer.has_record_tag()) {
    DT_THROW_IF(! deserializer.record_tag_is(raw_hit::SERIAL_TAG),
                std::runtime_error,
                "Cannot load a raw_hit from the archive !");
    deserializer.load(RH);

    DT_LOG_NOTICE(logging, "Deserialized raw hit #" << number_of_hits);
    RH.dump(std::clog);

    number_of_hits++;
  }
  DT_LOG_INFORMATION(logging,
                     "Number of deserialized raw hits is "
                     << number_of_hits);
}

/// Serialize several ``raw_hit`` handles in a XML file
void ex_raw_hit_5(datatools::logger::priority logging)
{
  // An useful typedef :
  typedef datatools::handle<raw_hit> handle_type;

  // Declare a collection of  hit handles :
  DT_LOG_DEBUG(logging, "Create a collection of hit handles");
  std::vector<handle_type> hits;
  int number_of_hits = 4;
  for (int hit_id = 0; hit_id < number_of_hits; hit_id++) {
    handle_type RHH(new raw_hit);
    RHH.grab().set_id(hit_id);
    RHH.grab().randomize();
    hits.push_back(RHH);

    DT_LOG_NOTICE(logging, "Serialized raw hit #" << hit_id);
    hits.back().get().dump(std::clog);
  }

  // Declare a single hit handle that references the 3rd hit (index==2)
  // in the collection :
  handle_type special_RHH(hits[2]);
  DT_LOG_NOTICE(logging, "Serialized special raw hit (reference to the 3rd hit in the collection)");
  special_RHH.get().dump(std::clog);

  // We use here a single archive to store all the hits hold
  // by the collection of handles and the special handle.
  // This allows the archive to preserved the layout of the
  // memory as managed by the handles, with cross-references
  // between them :
  datatools::data_writer serializer("raw_hits_h.xml",
                                    datatools::using_single_archive);

  // Store the vector of handles passing an arbitrary *serial tag* of our choice
  // because class ``std::vector<handle_type>`` does not inherit the
  // ``datatools::i_serializable`` interface. We need here to tag the
  // object with some arbitrary character string that will help us
  // to recognize the container at deserialization.
  serializer.store("hit_handle_collection", hits);

  // Store the special hit handle with an arbitrary *serial tag*,
  // this only save some information to reference the 3rd item
  // in the collection, and does not duplicate the storage of
  // the corresponding hit because handle objects use shared pointers:
  serializer.store("hit_handle", special_RHH);

  DT_LOG_INFORMATION(logging,
                     "Number of serialized raw hit handles is "
                     << number_of_hits);

  DT_LOG_INFORMATION(logging,
                     "The hit handles have been stored in the ``raw_hits_h.xml`` file.");
}


/// Serialize several ``raw_hit`` handles in a XML file
void ex_raw_hit_6(datatools::logger::priority logging)
{
  // An useful typedef :
  typedef datatools::handle<raw_hit> handle_type;

  // We use here a single archive to load all the hits
  // because this mode was used by the serializer :
  datatools::data_reader deserializer("raw_hits_h.xml",
                                      datatools::using_single_archive);

  // Deserialize a collection of hit handles with the same
  // *serial tag* than for the serialization :
  std::string expected_serial_tag = "hit_handle_collection";
  DT_THROW_IF(! deserializer.has_record_tag(),
              std::runtime_error,
              "Cannot load any data from the archive !");
  DT_THROW_IF(! deserializer.record_tag_is(expected_serial_tag),
              std::runtime_error,
              "Cannot load a collection of raw hit handle !");
  std::vector<handle_type> hits;
  deserializer.load(expected_serial_tag, hits);

  // Deserialize the lone hit handle with the same
  // *serial tag* than for the serialization :
  expected_serial_tag = "hit_handle";
  DT_THROW_IF(! deserializer.has_record_tag(),
              std::runtime_error,
              "Cannot load any more data from the archive !");
  DT_THROW_IF(! deserializer.record_tag_is(expected_serial_tag),
              std::runtime_error,
              "Cannot load a raw hit handle !");
  handle_type special_RHH;
  deserializer.load(expected_serial_tag, special_RHH);

  // Print the deserialized data :
  for (int i = 0; i < hits.size(); i++) {
    DT_LOG_NOTICE(logging, "Deserialized raw hit #" << i);
    hits[i].get().dump(std::clog);
  }
  DT_LOG_NOTICE(logging, "Deserialized special raw hit (reference to the 3rd hit in the collection)");
  special_RHH.get().dump(std::clog);
}


/// Store some ``raw_hit`` instances in a ``datatools::things`` container
/// and fail to serialize the container :
void ex_raw_hit_7(datatools::logger::priority logging)
{
  // Declare a ``datatools::things`` instance as the data record :
  datatools::things DR;

  // Instantiate and initialize a raw hit object from within a data bank :
  raw_hit & RH0 = DR.add<raw_hit>("RH0");
  RH0.set_id(666);
  RH0.randomize();

  // Instantiate and initialize another raw hit object from within a data bank :
  raw_hit & RH1 = DR.add<raw_hit>("RH1");
  RH1.set_id(667);
  RH1.randomize();

  // Print the raw hits stored in both banks :
  DT_LOG_NOTICE(logging, "Raw hit bank: " << "RH0");
  DR.get<raw_hit>("RH0").dump(std::clog);

  DT_LOG_NOTICE(logging, "Raw hit bank: " << "RH1");
  DR.get<raw_hit>("RH1").dump(std::clog);

  // Store the data record in a Boost/archive file (XML format):
  DT_LOG_DEBUG(logging, "Serialize the data record...");
  datatools::data_writer serializer("raw_hit_banks.xml",
                                    datatools::using_single_archive);
  try {
    serializer.store(DR);
    DT_LOG_INFORMATION(logging, "The data record has been stored in the ``data_record.xml`` file.");
  }
  catch(std::exception& x){
    DT_LOG_ERROR(logging,
                 "As expected, the serialization failed ! Boost report is: " << x.what() << " !\n"
                 << "\tThis is because  the ``raw_data`` class, while  being serializable, has not been  registered\n"
                 << "\tthrough the *Boost serialization export* mechanism ! That means that the ``datatools::things``\n"
                 << "\tcontainer turns to be not serializable if it contains **at least one** object in a data bank\n"
                 << "\tof which the class is not serializable nor registered with the *class export* mechanism of the\n"
                 << "\tBoost/Serialization library.\n"
                 );
  }
}


/// Store some ``raw_data`` instances in a ``datatools::things`` container
/// and serialize the container successfully because the ``raw_data`` class
/// is registered in the Boost/Serialization export system :
void ex_raw_hit_8(datatools::logger::priority logging)
{
  // Declare a ``datatools::things`` instance as the data record :
  datatools::things DR;

  // Instantiate and initialize a raw data object from within a data bank :
  raw_data & RD0 = DR.add<raw_data>("RD0");
  RD0.randomize_hits(3);
  RD0.grab_auxiliaries().store_flag("test");
  RD0.grab_auxiliaries().store("welcome", "Hello World !");

  // Instantiate and initialize another raw data object from within a data bank :
  raw_data & RD1 = DR.add<raw_data>("RD1");
  RD1.randomize_hits(5);
  RD1.lock("datatools");

  DT_LOG_NOTICE(logging, "The data record with 2 raw data banks: ");
  DR.tree_dump(std::clog);

  // Print the raw hits stored in both banks :
  DT_LOG_NOTICE(logging, "Raw data bank: " << "RD0");
  DR.get<raw_data>("RD0").dump(std::clog);

  DT_LOG_NOTICE(logging, "Raw data bank: " << "RD1");
  DR.get<raw_data>("RD1").dump(std::clog);

  // Store the data record in a Boost/archive file (XML format):
  DT_LOG_DEBUG(logging, "Serialize the data record...");
  datatools::data_writer serializer("raw_data_banks.xml",
                                    datatools::using_single_archive);
  try {
    serializer.store(DR);
    DT_LOG_INFORMATION(logging, "The data record has been stored in the ``data_record.xml`` file.");
  }
  catch(std::exception& x){
    DT_LOG_ERROR(logging,
                 "Serialization failed ! Boost report is: " << x.what() << " !\n"
                 );
  }
}


/// Deserialize a ``datatools::things`` container
/// and access the stored raw data banks.
void ex_raw_hit_9(datatools::logger::priority logging)
{
  // Declare a ``datatools::things`` instance as the data record :
  datatools::things DR;

  // Load the data record from a Boost/archive file (XML format):
  DT_LOG_DEBUG(logging, "Deserialize the data record...");
  datatools::data_reader deserializer("raw_data_banks.xml",
                                      datatools::using_single_archive);

  try {
    deserializer.load(DR);
    DT_LOG_INFORMATION(logging, "The data record has been loaded from the ``data_record.xml`` file.");

    DT_LOG_NOTICE(logging, "The data record with 2 raw data banks: ");
    DR.tree_dump(std::clog);

    // Print the raw hits stored in both banks :
    DT_LOG_NOTICE(logging, "Raw data bank: " << "RD0");
    DR.get<raw_data>("RD0").dump(std::clog);

    DT_LOG_NOTICE(logging, "Raw data bank: " << "RD1");
    DR.get<raw_data>("RD1").dump(std::clog);

    // Access the first raw data bank and reset the object :
    DT_THROW_IF(! DR.is_a<raw_data>("RD0"),
                std::logic_error,
                "Cannot find a raw data bank named ``RD0`` !");
    raw_data & RD0 = DR.grab<raw_data>("RD0");
    RD0.reset();

    // Access the second raw data bank, unlock it then add a property in a given hit :
    DT_THROW_IF(! DR.is_a<raw_data>("RD1"),
                std::logic_error,
                "Cannot find a raw data bank named ``RD1`` !");
    raw_data & RD1 = DR.grab<raw_data>("RD1");
    RD1.unlock("datatools");
    RD1.grab_hits().pop_back();
    RD1.grab_hits().erase(RD1.grab_hits().begin()+1);
    RD1.grab_hits()[1].grab_auxiliaries().store_flag("golden_hit");

    // Print the raw hits stored in both banks :
    DT_LOG_NOTICE(logging, "Raw data bank after reset: " << "RD0");
    DR.get<raw_data>("RD0").dump(std::clog);

    DT_LOG_NOTICE(logging, "Raw data bank after manipulation: " << "RD1");
    DR.get<raw_data>("RD1").dump(std::clog);

  }
  catch(std::exception& x){
    DT_LOG_ERROR(logging,
                 "Serialization failed ! Boost report is: " << x.what() << " !\n"
                 );
  }

}

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  int error_code = EXIT_SUCCESS;
  try {
    long prng_seed = 314159;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          logging = datatools::logger::PRIO_DEBUG;
        } else if ((option == "-v") || (option == "--verbose")) {
          logging = datatools::logger::PRIO_INFORMATION;
        } else  {
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
      }
      iarg++;
    }

    DT_LOG_INFORMATION(logging, "Welcome to the datatools example program : ex_serializable_1");

    DT_LOG_DEBUG(logging, "Initialize the PRNG");
    srand48(prng_seed);

    ex_raw_hit_1(logging);
    ex_raw_hit_2(logging);

    ex_raw_hit_3(logging);
    ex_raw_hit_4(logging);

    ex_raw_hit_5(logging);
    ex_raw_hit_6(logging);

    ex_raw_hit_7(logging);

    ex_raw_hit_8(logging);
    ex_raw_hit_9(logging);

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
