// -*- mode: c++ ; -*-
/* factory.cc
 */
#include <iostream>
#include <stdexcept>
#include <sstream>

#include <datatools/clhep_units.h>
#include <datatools/units.h>
#include <datatools/properties.h>
#include <datatools/logger.h>
#include <datatools/exception.h>

#include <materials/detail/tools.h>
#include <materials/factory.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>

namespace materials {

  using namespace std;

  bool factory::is_debug () const
  {
    return _debug_;
  }

  void factory::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  // ctor:
  factory::factory ()
  {
    _debug_ = false;
    return;
  }

  // dtor:
  factory::~factory ()
  {
    return;
  }

  isotope * factory::create_isotope (const string & name_,
                                     const datatools::properties & config_) const
  {
    int z = 0;
    int a = 0;
    int i = isotope::GROUND_STATE;

    DT_THROW_IF(!config_.has_key ("z"),
                std::logic_error, "Missing 'z' property for isotope '" << name_ << "' !");
    z =  config_.fetch_integer ("z");

    DT_THROW_IF(!config_.has_key ("a"),
                std::logic_error, "Missing 'a' property for isotope '" << name_ << "' !");
    a =  config_.fetch_integer ("a");

    if (config_.has_key ("I"))
      {
        string i_str = config_.fetch_string ("I");
        if (i_str == "GS")
          {
            i = isotope::GROUND_STATE;
          }
        else if (i_str == "M")
          {
            i = isotope::ISOMERIC_M;
          }
        else if (i_str == "N")
          {
            i = isotope::ISOMERIC_N;
          }
        else if (i_str == "O")
          {
            i = isotope::ISOMERIC_O;
          }
        else
          {
            DT_THROW_IF(true,
                        std::logic_error, "Invalid 'I' property label: '" << i_str << "' for isotope '" << name_ << "' !");
          }
      }
    isotope * iso = new isotope (name_, z, a, i);
    iso->build ();
    return iso;
  }


  element * factory::create_element (const string & name_,
                                     const datatools::properties & config_,
                                     const isotope_dict_type & isotopes_) const
  {
    int z = 0;
    double a = -1.0;
    vector<string> isotopes;
    vector<double> weights;

    DT_THROW_IF (!config_.has_key ("z"),
                 std::logic_error ,
                 "Missing 'z' property for element '" << name_ << "' !");
    z = config_.fetch_integer ("z");

    if (config_.has_key ("a")) {
      a = config_.fetch_real ("a");
      DT_THROW_IF (config_.has_explicit_unit("a"),
                   std::logic_error,
                   "Atomic mass property 'a' does not support any explicit unit !");
    } else {
      DT_THROW_IF (! config_.has_key ("isotope.names"),
                   std::logic_error ,
                   "Missing 'isotope.names' property for element '" << name_ << "' !");
      config_.fetch ("isotope.names", isotopes);

      DT_THROW_IF (! config_.has_key ("isotope.weights"),
                   std::logic_error , "Missing 'isotope.weights' property for element '" << name_ << "' !");
      config_.fetch ("isotope.weights", weights);

      DT_THROW_IF (isotopes.size () != weights.size (),
                   std::logic_error ,"Unmatching isotopes/weights vector size for element '" << name_ << "' !");
    }

    element * elmt = new element (name_, z);

    if (a > 0.0)
      {
        elmt->set_molar_mass (a);
      }
    else
      {
        // add isotopes in element:
        for (size_t i = 0; i < isotopes.size (); i++)
          {
            isotope_dict_type::const_iterator found = isotopes_.find (isotopes[i]);
            DT_THROW_IF (found == isotopes_.end (),
                         std::logic_error,
                         "Isotope '" << isotopes[i] << "' not found in map of isotopes for element '" << name_ << "' !");
            const isotope & iso = found->second.get_ref ();
            elmt->add_isotope (iso, weights[i]);
          }
      }
    elmt->build ();
    return elmt;
  }

  material * factory::create_material (const string & name_,
                                       const datatools::properties & config_,
                                       const element_dict_type & elements_,
                                       const material_dict_type & materials_) const
  {
    bool devel = false;
    //devel = true;
    double density = 1.0;
    double density_unit = material::g_per_cm3();
    //cerr << "DEVEL: factory::create_material: 1 g=" << CLHEP::g << endl;
    //cerr << "DEVEL: factory::create_material: 1 cm3=" << CLHEP::cm3 << endl;
    string formula;
    double temperature = -1.0;
    double temperature_unit = CLHEP::kelvin;
    double pressure = -1.0;
    double pressure_unit = CLHEP::bar;
    double molar_volume_unit = CLHEP::cm3/CLHEP::mole;
    double molar_concentration_unit = CLHEP::mole/CLHEP::cm3;
    string state = "";

    vector<string> composition_names;
    vector<int>    composition_nb_of_atoms;
    vector<double> composition_fraction_mass;
    string         composition_mode_label;
    int            composition_mode = -1;
    double         mean_z = -1.0;
    double         mean_a = -1.0;

    static const int BM_COMPOSITION = 0;
    static const int BM_DOPING      = 1;

    int build_mode = BM_COMPOSITION;

    if (config_.has_key ("build_mode")) {
      std::string bm_label = config_.fetch_string ("build_mode");
      if (bm_label == "composition") {
        build_mode = BM_COMPOSITION;
      } else if (bm_label == "doping") {
        build_mode = BM_DOPING;
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Invalid build mode '" << bm_label << "' for material '" << name_ <<"' !");
      }
    }

    if (config_.has_key ("density.unit")) {
      std::string density_unit_str = config_.fetch_string ("density.unit");
      density_unit = datatools::units::get_density_unit_from (density_unit_str);
    }
    if (devel) std::cerr << "DEVEL: material '" << name_ << "' : density_unit = "
                         << density_unit / (CLHEP::g/CLHEP::cm3) << " [g/cm3]" << std::endl;

    if (config_.has_key ("molar_volume.unit")) {
      std::string molar_volume_unit_str = config_.fetch_string ("molar_volume.unit");
      if (molar_volume_unit_str == "cm3/mol") {
        molar_volume_unit = CLHEP::cm3/CLHEP::mole;
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid molar volume unit '"
                    << molar_volume_unit_str << "' !");
      }
    }
    if (devel) std::cerr << "DEVEL: material '" << name_ << "' : molar_volume_unit = "
                         << molar_volume_unit / (CLHEP::cm3/CLHEP::mole) << " [cm3/mol]"
                         << std::endl;

    if (config_.has_key ("molar_concentration.unit")) {
      std::string molar_concentration_unit_str = config_.fetch_string ("molar_concentration.unit");
      if (molar_concentration_unit_str == "mol/cm3") {
        molar_concentration_unit = CLHEP::mole/CLHEP::cm3;
      } else if (molar_concentration_unit_str == "mol/dm3"
                 || molar_concentration_unit_str == "mol/L") {
        molar_concentration_unit = CLHEP::mole/(1000 * CLHEP::cm3);
      } else if (molar_concentration_unit_str == "mol/m3") {
        molar_concentration_unit = CLHEP::mole/CLHEP::m3;
      } else if (molar_concentration_unit_str == "/cm3"
                 || molar_concentration_unit_str == "atom/cm3"
                 || molar_concentration_unit_str == "atoms/cm3") {
        molar_concentration_unit = (CLHEP::mole/CLHEP::Avogadro)/CLHEP::cm3;
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid molar concentration unit '" << molar_concentration_unit_str << "' !");
      }
    }
    if (devel) std::cerr << "DEVEL: material '" << name_ << "' : molar_concentration_unit = "
                         << molar_concentration_unit / ((CLHEP::mole/CLHEP::Avogadro)/CLHEP::cm3)
                         << " [atoms/cm3]"
                         << std::endl;

    if (config_.has_key ("temperature.unit")) {
      std::string temperature_unit_str =  config_.fetch_string ("temperature.unit");
      temperature_unit = datatools::units::get_temperature_unit_from (temperature_unit_str);
    }
    if (devel) std::cerr << "DEVEL: material '" << name_ << "' : temperature_unit = "
                         << temperature_unit / (CLHEP::kelvin) << " [K]"
                         << std::endl;

    if (config_.has_key ("pressure.unit")) {
      string pressure_unit_str =  config_.fetch_string ("pressure.unit");
      pressure_unit = datatools::units::get_pressure_unit_from (pressure_unit_str);
    }
    if (devel) std::cerr << "DEVEL: material '" << name_ << "' : pressure_unit = "
                         << pressure_unit / (CLHEP::pascal) << " [Pa]"
                         << std::endl;

    if (build_mode == BM_COMPOSITION) {
      DT_THROW_IF (!config_.has_key ("density"),
                   std::logic_error,
                   "Missing 'density' property for material '" << name_ << "' !");
      density = config_.fetch_real ("density");
      if (! config_.has_explicit_unit("density")) density *= density_unit;
    }

    if (config_.has_key ("temperature")) {
      temperature = config_.fetch_real ("temperature");
      if (! config_.has_explicit_unit("temperature")) temperature *= temperature_unit;
    }

    if (config_.has_key ("pressure")) {
      pressure = config_.fetch_real ("pressure");
      if (! config_.has_explicit_unit("pressure")) pressure *= pressure_unit;
    }

    if (config_.has_key ("formula")) {
      formula = config_.fetch_string ("formula");
    }

    if (config_.has_key ("state")) {
      state = config_.fetch_string ("state");
    }

    if (build_mode == BM_COMPOSITION) {

      DT_THROW_IF (! config_.has_key ("composition.mode"),
                   std::logic_error,
                   "Missing 'composition.mode' property for material '" << name_ << "' !");
      composition_mode_label =  config_.fetch_string ("composition.mode");

      if (composition_mode_label == "number_of_atoms") {
        composition_mode = material::NUMBER_OF_ATOMS;
      } else if (composition_mode_label == "fraction_mass") {
        composition_mode = material::FRACTION_MASS;
      } else if (composition_mode_label == "mean_za") {
        composition_mode = material::MEAN_ZA;
      }  else {
        DT_THROW_IF (true,std::logic_error,"Invalid 'composition.mode' property ('" << composition_mode_label << "') for material '" << name_ << "' !");
      }

      if (composition_mode == material::MEAN_ZA) {
        DT_THROW_IF (! config_.has_key ("mean_z"),
                     std::logic_error,
                     "Missing 'mean_z' property !");
        mean_z = config_.fetch_real ("mean_z");

        DT_THROW_IF (! config_.has_key ("mean_a"),
                     std::logic_error,
                     "Missing 'mean_a' property !");
        mean_a = config_.fetch_real ("mean_a");
      } else {
        DT_THROW_IF (! config_.has_key ("composition.names"),
                     std::logic_error,
                     "Missing 'composition.names' property for material '" << name_ << "' !");
        config_.fetch ("composition.names", composition_names);
        DT_THROW_IF (composition_names.size () == 0,
                     std::logic_error,
                     "Empty list of compounds for material '" << name_ << "' !");

        if (composition_mode == material::NUMBER_OF_ATOMS) {
          DT_THROW_IF (! config_.has_key ("composition.number_of_atoms"),
                       std::logic_error,
                       "Missing 'composition.number_of_atoms' property for material '" << name_ << "' !");
          config_.fetch ("composition.number_of_atoms", composition_nb_of_atoms);
          DT_THROW_IF (composition_names.size () != composition_nb_of_atoms.size (),
                       std::logic_error,
                       "Unmatching sizes of list of compounds/number of atoms !");
        }

        if (composition_mode == material::FRACTION_MASS) {
          DT_THROW_IF (! config_.has_key ("composition.fraction_mass"),
                       std::logic_error,
                       "Missing 'composition.fraction_mass' property for material '" << name_ << "' !");
          config_.fetch ("composition.fraction_mass", composition_fraction_mass);
          DT_THROW_IF (composition_names.size () != composition_fraction_mass.size (),
                       std::logic_error,
                       "Unmatching sizes of list of compounds/fraction mass for material '" << name_ << "' !");
        }
      }
      // end of BM_COMPOSITION
    } else if (build_mode == BM_DOPING) {
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : dopping build mode..." << std::endl;
      std::string doped_material_name;
      std::vector<std::string> doping_elements;
      std::vector<double> doping_concentrations;
      std::vector<double> doping_molar_volumes;
      std::vector<double> doping_molar_masses;

      // Doped material:
      DT_THROW_IF(! config_.has_key("doping.doped_material"), std::logic_error,
                  "Missing 'doping.doped_material' property for material '" << name_ << "' !");
      doped_material_name = config_.fetch_string("doping.doped_material");
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : doped_material = '"
                           << doped_material_name << "'"  << std::endl;

      // List of doping elements:
      DT_THROW_IF(! config_.has_key("doping.doping_elements"), std::logic_error,
                  "Missing 'doping.doping_elements' property for material '" << name_ << "' !");
      config_.fetch("doping.doping_elements", doping_elements);
      DT_THROW_IF(doping_elements.size()<1, std::logic_error,
                  "Invalid 'doping.doping_elements' property value for material '" << name_ << "' !");
      if (devel) {
        std::cerr << "DEVEL: material '" << name_ << "' : doping elements = ";
        for (size_t i = 0; i < doping_elements.size(); i++) {
          std::cerr << "'" << doping_elements[i] << "' ";
        }
        std::cerr << std::endl;
      }
      // List of doping concentrations:
      DT_THROW_IF(! config_.has_key("doping.doping_concentrations"), std::logic_error,
                  "Missing 'doping.doping_concentrations' property for material '" << name_ << "' !");
      config_.fetch("doping.doping_concentrations", doping_concentrations);
      DT_THROW_IF(doping_elements.size() != doping_concentrations.size(), std::logic_error,
                  "'doping.doping_concentrations' and 'doping.doping_elements' size mismatch for material '" << name_ << "' !");
      DT_THROW_IF (config_.has_explicit_unit("doping.doping_concentrations"),
                   std::logic_error,
                   "Cannot use explicit unit for property '" << "doping.doping_concentrations"
                   << "' for material '" << name_ << "' !");
      for (size_t i = 0; i < doping_concentrations.size(); i++) {
        doping_concentrations[i] *= molar_concentration_unit;
      }
      double atoms_per_cm3 = 1./CLHEP::mole/CLHEP::Avogadro/CLHEP::cm3;
      if (devel) {
        std::cerr << "DEVEL: material '" << name_ << "' : doping concentration = ";
        for (size_t i = 0; i < doping_concentrations.size(); i++) {
          std::cerr << " " << doping_concentrations[i] / atoms_per_cm3 << " ";
        }
        std::cerr << " [atoms/cm3]" << std::endl;
      }
      // List of doping molar volumes:
      DT_THROW_IF(! config_.has_key("doping.doping_molar_volumes"), std::logic_error,
                  "Missing 'doping.doping_molar_volumes' property for material '" << name_ << "' !");
      config_.fetch("doping.doping_molar_volumes", doping_molar_volumes);
      DT_THROW_IF(doping_elements.size() != doping_molar_volumes.size(), std::logic_error,
                  "'doping.doping_molar_volumes' and 'doping.doping_elements' size mismatch for material '" << name_ << "' !");
      DT_THROW_IF (config_.has_explicit_unit("doping.doping_molar_volumes"),
                   std::logic_error,
                   "Cannot use explicit unit for property '" << "doping.doping_molar_volumes"
                   << "' for material '" << name_ << "' !");
      for (size_t i = 0; i < doping_molar_volumes.size(); i++) {
        doping_molar_volumes[i] *= molar_volume_unit;
      }
      if (devel) {
        std::cerr << "DEVEL: material '" << name_ << "' : doping molar volumes = ";
        for (size_t i = 0; i < doping_molar_volumes.size(); i++) {
          std::cerr << " " << doping_molar_volumes[i] / (CLHEP::cm3/CLHEP::mole) << " ";
        }
        std::cerr << " [cm3/mol]" << std::endl;
      }

      material_dict_type::const_iterator found_doped = materials_.find (doped_material_name);
      DT_THROW_IF (found_doped == materials_.end (),
                   std::logic_error,
                   "Unknown doped material '" << doped_material_name << "' for material '" << name_ << "' !");

      double doped_density = found_doped->second.get_ref().get_density();
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : doped_density  = "
                           << doped_density / (CLHEP::g/CLHEP::cm3) << " [g/cm3]" << std::endl;

      // Reference volume for the computing of fraction masses:
      const double volume_ref = 1. * CLHEP::cm3;

      // Volume effectively used by the doped material:
      double doped_volume_ref = volume_ref;
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : doped original ref mass = "
                           << volume_ref * doped_density / CLHEP::g << " [g]"
                           << std::endl;

      std::vector<double> doping_masses_ref;
      double total_mass_ref = 0.0;
      for (size_t i = 0; i < doping_elements.size(); i++) {
        const std::string & doping_element_name = doping_elements[i];
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' :  --> doping_element_name = '"
                             << doping_element_name << "': " << std::endl;
        element_dict_type::const_iterator found_doping = elements_.find (doping_element_name);
        DT_THROW_IF (found_doped == materials_.end (),
                     std::logic_error,
                     "Unknown doping element '" << doping_element_name
                     << "' for material '" << name_ << "' !");
        const element & doping_elmt = found_doping->second.get_ref ();
        double doping_molar_mass    = doping_elmt.get_molar_mass() * (CLHEP::g/CLHEP::mole);
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' :    --> doping_molar_mass  = "
                             << doping_molar_mass / (CLHEP::g/CLHEP::mole) << " [g/mol]" << std::endl;
        double doping_concentration = doping_concentrations[i];
        double doping_molar_volume  = doping_molar_volumes[i];
        double doping_amount_ref    = doping_concentration * volume_ref;
        double doping_mass_ref      = doping_amount_ref * doping_molar_mass;
        double doping_volume_ref    = doping_amount_ref * doping_molar_volume;
        if (devel) {
          std::cerr << "DEVEL: material '" << name_ << "' :    --> doping_volume_ref  = "
                    << doping_volume_ref / CLHEP::cm3 << " [cm3]" << std::endl;
          std::cerr << "DEVEL: material '" << name_ << "' :    --> doping_mass_ref  = "
                    << doping_mass_ref / CLHEP::g << " [g]" << std::endl;
        }
        doping_masses_ref.push_back(doping_mass_ref);
        total_mass_ref += doping_mass_ref;
        doped_volume_ref -= doping_volume_ref;
        DT_THROW_IF(doped_volume_ref <= 0.0, std::logic_error,
                    "Invalid remaining quantity of doped material for material '" << name_ << "' !");

      }
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : doped material remaining ref volume = "
                           << doped_volume_ref / CLHEP::cm3 << " [cm3]" << std::endl;
      // Compute the mass of doped material inside the reference volume:
      double doped_mass_ref = doped_volume_ref * doped_density;
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : doped material remaining ref mass = "
                           << doped_mass_ref / CLHEP::g << " [g]" << std::endl;

      // Compute the total mass of doped+doping materials inside the reference volume:
      total_mass_ref += doped_mass_ref;
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : total_mass_ref = "
                           << total_mass_ref / CLHEP::g << " [g]" << std::endl;

      // Build fraction mass composition:
      composition_mode = material::FRACTION_MASS;

      // Add the doped material first:
      composition_names.push_back(doped_material_name);
      composition_fraction_mass.push_back(doped_mass_ref / total_mass_ref);
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Add compound '" << doped_material_name
                           << "' with fraction mass = " << doped_mass_ref / total_mass_ref << std::endl;
      // Then add the doping elements:
      for (size_t i = 0; i < doping_elements.size(); i++) {
        const std::string & doping_element_name = doping_elements[i];
        composition_names.push_back(doping_element_name);
        composition_fraction_mass.push_back(doping_masses_ref[i] / total_mass_ref);
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Add compound '" << doping_element_name
                             << "' with fraction mass = " << doping_masses_ref[i] / total_mass_ref << std::endl;
      }

      density = total_mass_ref / volume_ref;
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : density = "
                           << density / (CLHEP::g / CLHEP::cm3) << " [g/cm3]" << std::endl;

      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Done." << std::endl;
      // end of BM_DOPING
    }

    // Build the new material:
    material * matl = new material ();
    try {
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Allocating new material..." << std::endl;
      matl->set_name (name_);
      matl->set_density (density);
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : density = "
                           <<  matl->get_density() / (CLHEP::g / CLHEP::cm3) << " [g/cm3]" << std::endl;

      if (composition_mode == material::MEAN_ZA) {
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by MEAN_ZA..." << std::endl;
        matl->set_mean_z_a (mean_z, mean_a);
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by MEAN_ZA done." << std::endl;
      }
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : composition_mode = "
                           << composition_mode << std::endl;

      if (composition_mode == material::NUMBER_OF_ATOMS) {
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by NUMBER_OF_ATOMS..." << std::endl;
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : number of compounds = "
                             << composition_names.size () << std::endl;
        for (size_t i = 0; i < composition_names.size (); i++) {
          if (devel) std::cerr << "DEVEL: material '" << name_ << "' : compound name = '"
                               << composition_names[i] << "'" << std::endl;
          element_dict_type::const_iterator found = elements_.find (composition_names[i]);
          DT_THROW_IF (found == elements_.end (),
                       std::logic_error,
                       "Unknown element '" << composition_names[i] << "') !");
          const element & elmt = found->second.get_ref ();
          int nb_atoms = composition_nb_of_atoms[i];
          matl->add_element_by_nb_of_atoms (elmt , nb_atoms);
        }
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by NUMBER_OF_ATOMS done." << std::endl;
      }

      if (composition_mode == material::FRACTION_MASS) {
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by FRACTION_MASS..." << std::endl;
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : number of compounds = "
                             << composition_names.size () << std::endl;
        for (size_t i = 0; i < composition_names.size (); i++) {
          if (devel) std::cerr << "DEVEL: material '" << name_ << "' : compound name = '"
                               << composition_names[i] << "'" << std::endl;
          const element * a_elmt = 0;
          const material * a_matl = 0;
          if (composition_names[i] == name_) {
            ostringstream message;
            message << "Self-referenced material with name '"
                    << composition_names[i] << "' for material '" << name_ << "' !";
            delete matl;
            DT_THROW_IF(true, std::logic_error, message.str ());
          }
          element_dict_type::const_iterator found = elements_.find (composition_names[i]);
          if (found != elements_.end ()) {
            a_elmt = found->second.get_ptr ();
            if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Found compound element '"
                                 << composition_names[i] << "'" << std::endl;
          } else {
            if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Compound '"
                                 << composition_names[i] << "' is not a known element..." << std::endl;
            material_dict_type::const_iterator found2 = materials_.find (composition_names[i]);
            if (found2 != materials_.end ()) {
              DT_THROW_IF (found2->second.is_alias(),
                           std::logic_error,
                           "Cannot use material alias '" << composition_names[i]
                           << "' as a component for material '" << name_ << "' !");
              a_matl = found2->second.get_ptr ();
              if (devel) std::cerr << "DEVEL: material '" << name_ << "' : Found compound material '"
                                   << composition_names[i] << "'" << std::endl;
            } else {
              ostringstream message;
              message << "materials::factory::create_material: "
                      << "Unknown element or material '" << composition_names[i]
                      << "' for material '" << name_ << "' !";
              DT_THROW_IF(true, std::logic_error,
                          "Unknown element or material '" << composition_names[i]
                          << "' for material '" << name_ << "' !");
            }
          }
          double f_mass = composition_fraction_mass[i];
          if (a_elmt !=  0) {
            matl->add_element_by_mass (*a_elmt, f_mass);
          }  else {
            matl->add_material_by_mass (*a_matl, f_mass);
          }
        }
        if (devel) std::cerr << "DEVEL: material '" << name_ << "' : by FRACTION_MASS done." << std::endl;
      }

      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : auxiliary properties..." << std::endl;

      // Some auxiliary properties:
      if (! formula.empty ()) {
        matl->grab_properties ().store ("formula", formula);
      }

      if (! state.empty ()) {
        matl->grab_properties ().store ("state", state);
      }

      if (temperature > 0.0) {
        matl->grab_properties ().store ("temperature", temperature);
      }

      if (pressure > 0.0) {
        matl->grab_properties ().store ("pressure", pressure);
      }

      // 2012-11-03 FM : support for various "Material Properties Tables"
      // ala Geant4 :
      config_.export_starting_with (matl->grab_properties (), "mpt.");

      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : building..." << std::endl;
      matl->build ();
      if (devel) std::cerr << "DEVEL: material '" << name_ << "' : building done." << std::endl;
    }
    catch (exception & x) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what ());
      //cerr << "materials::factory::create_material: " << "Exception : " << x.what () << endl;
      delete matl;
      matl = 0;
      throw x;
    }
    return matl;
  }

} // end of namespace materials

// end of factory.cc
