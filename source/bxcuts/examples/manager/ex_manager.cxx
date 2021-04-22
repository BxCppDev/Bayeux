// -*- mode: c++ ; -*-
// ex_manager.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/datatools.h>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
// - Bayeux/cuts:
#include <cuts/cuts_config.h>
#include <cuts/cut_manager.h>

// This project:
// The data model to be selected :
#include <data.h>
// Registered cuts on the data class :
#include <range_cut.h>
#include <sphere_cut.h>
#include <color_cut.h>

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    long seed = 314159;
    std::string config_filename = "";
    std::string cut_name = "";

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          logging = datatools::logger::PRIO_DEBUG;
        } else if ((option == "-V") || (option == "--verbose")) {
          logging = datatools::logger::PRIO_INFORMATION;
        } else if ((option == "-w") || (option == "--warning")) {
          logging = datatools::logger::PRIO_WARNING;
        } else if ((option == "-T") || (option == "--trace")) {
          logging = datatools::logger::PRIO_TRACE;
        } else {
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        if (config_filename.empty ()) {
          config_filename = argument;
        } else if (cut_name.empty()) {
          cut_name = argument;
        } else {
          DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
        }
      }
      iarg++;
    }
    DT_LOG_INFORMATION(logging, "Welcome to the ``cuts`` example program : ``ex_manager`` !");

    srand48(seed);

    // Declare a set of configuration parameters :
    datatools::properties cut_manager_config;
    if (config_filename.empty()) {
      config_filename = "config/manager.conf";
    }
    datatools::fetch_path_with_env(config_filename);
    // Load configuration parameters from a file :
    datatools::read_config(config_filename,
                                       cut_manager_config);

    // Declare a cut manager :
    cuts::cut_manager my_cut_manager;
    my_cut_manager.set_logging_priority(logging);

    // Initialization from the set of parameters extracted from the configuration file :
    my_cut_manager.initialize(cut_manager_config);

    // Pickup a specific cut by name :
    cuts::i_cut * the_cut = 0;
    if (cut_name.empty()) {
      cut_name = "selector";
    }
    DT_LOG_INFORMATION(logging, "Cut name is '" << cut_name << "'");
    DT_THROW_IF(! my_cut_manager.has(cut_name),
                std::domain_error,
                "No cut with name '" << cut_name << "' !");
    the_cut = &my_cut_manager.grab(cut_name);

    // Create a dictionnary of streams (colored outputs for Gnuplot) :
    std::map<std::string,std::ostringstream *> ss0;

    // Build a collection of random data points:
    std::vector<data> points;
    int npoints = 1000000;
    points.reserve(npoints);
    for (int i = 0; i < npoints; i++) {
      {
        data dummy;
        points.push_back(dummy);
      }
      data & d = points.back();
      // Randomize the 3D-coordinates of the point in a box :
      d.x = -1.0 + 2.0 * drand48();
      d.y = -1.0 + 2.0 * drand48();
      d.z = -1.0 + 2.0 * drand48();
      d.color =(int)(3.5 * drand48());
      std::ostringstream * pss = 0;
      // Randomize the color of the point :
      if (d.color == data::COLOR_BLACK) {
        if (ss0.find("black") == ss0.end()) {
          ss0["black"] = new std::ostringstream();
        }
        pss = ss0["black"];
      } else if (d.color == data::COLOR_RED) {
        if (ss0.find("red") == ss0.end()) {
          ss0["red"] = new std::ostringstream();
        }
        pss = ss0["red"];
      } else if (d.color == data::COLOR_GREEN) {
        if (ss0.find("green") == ss0.end()) {
          ss0["green"] = new std::ostringstream();
        }
        pss = ss0["green"];
      } else if (d.color == data::COLOR_BLUE) {
        if (ss0.find("blue") == ss0.end()) {
          ss0["blue"] = new std::ostringstream();
        }
        pss = ss0["blue"];
      }
      if (pss != 0) {
        // Store 10% of the data in a 'colored' stream :
        if ((i % 10) == 0) {
          d.print(*pss);
        }
      }
    }

    {
      std::ofstream forig("ex_manager.orig.data");
      // Print original points in colored streams:
      for (std::map<std::string,std::ostringstream *>::const_iterator i = ss0.begin();
           i != ss0.end();
           i++) {
        forig << "# " << i->first << std::endl;
        forig << i->second->str();
        forig << std::endl;
        forig << std::endl;
        delete i->second;
      }
    }

    // Apply the selection with the active cut :
    std::map<std::string,std::ostringstream *> ss;
    ss["black"] = new std::ostringstream();
    ss["red"]   = new std::ostringstream();
    ss["green"] = new std::ostringstream();
    ss["blue"]  = new std::ostringstream();
    bool has_black = false;
    bool has_red = false;
    bool has_green = false;
    bool has_blue = false;
    DT_LOG_NOTICE(logging, "Selection loop... ");
    // For each data points :
    for (int i = 0; i < points.size(); i++) {
      const data & a_data = points[i];
      // Set the data to the active cut :
      the_cut->set_user_data(a_data);
      // Invoke the cut selection method :
      int status = the_cut->process();
      if (status == cuts::SELECTION_ACCEPTED) {
        std::ostringstream * pss = 0;
        if (points[i].color == data::COLOR_BLACK) {
          pss = ss["black"];
          has_black = true;
        } else if (points[i].color == data::COLOR_RED) {
          pss = ss["red"];
          has_red = true;
        } else if (points[i].color == data::COLOR_GREEN) {
          pss = ss["green"];
          has_green = true;
        } else if (points[i].color == data::COLOR_BLUE) {
          pss = ss["blue"];
          has_blue = true;
        }
        if (pss != 0) {
          points[i].print(*pss);
        }
      }
    }
    my_cut_manager.reset();

    {
      // Print selected points in colored streams:
      std::ofstream fselected("ex_manager.selected.data");
      for (std::map<std::string,std::ostringstream *>::const_iterator i = ss.begin();
           i != ss.end();
           i++) {
        fselected << "# " << i->first << std::endl;
        fselected << i->second->str();
        if (i->first == "black" && ! has_black) {
          fselected << "1000 1000 1000" << std::endl;
        }
        if (i->first == "red" && ! has_red) {
          fselected << "1000 1000 1000" << std::endl;
        }
        if (i->first == "blue" && ! has_blue) {
          fselected << "1000 1000 1000" << std::endl;
        }
        if (i->first == "green" && ! has_green) {
          fselected << "1000 1000 1000" << std::endl;
        }
        fselected << std::endl;
        fselected << std::endl;
        delete i->second;
      }
    }

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging,x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging,"Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
