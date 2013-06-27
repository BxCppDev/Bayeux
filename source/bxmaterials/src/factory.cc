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
        for (int i = 0; i < isotopes.size (); i++)
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
    double density = 1.0;
    double density_unit = material::g_per_cm3;
    //cerr << "DEVEL: factory::create_material: 1 g=" << CLHEP::g << endl;
    //cerr << "DEVEL: factory::create_material: 1 cm3=" << CLHEP::cm3 << endl;
    string formula;
    double temperature = -1.0;
    double temperature_unit = CLHEP::kelvin;
    double pressure = -1.0;
    double pressure_unit = CLHEP::bar;
    string state = "";

    if (config_.has_key ("density.unit"))
      {
        string density_unit_str = config_.fetch_string ("density.unit");
        density_unit = datatools::units::get_density_unit_from (density_unit_str);
      }

    if (config_.has_key ("temperature.unit"))
      {
        string temperature_unit_str =  config_.fetch_string ("temperature.unit");
        temperature_unit = datatools::units::get_temperature_unit_from (temperature_unit_str);
      }

    if (config_.has_key ("pressure.unit"))
      {
        string pressure_unit_str =  config_.fetch_string ("pressure.unit");
        pressure_unit = datatools::units::get_pressure_unit_from (pressure_unit_str);
      }

    DT_THROW_IF (!config_.has_key ("density"), std::logic_error, "Missing 'density' property for material '" << name_ << "' !");
    density = config_.fetch_real ("density");
    if (! config_.has_explicit_unit("density")) density *= density_unit;

    if (config_.has_key ("temperature"))
      {
        temperature = config_.fetch_real ("temperature");
        if (! config_.has_explicit_unit("temperature")) temperature *= temperature_unit;
      }

    if (config_.has_key ("pressure"))
      {
        pressure = config_.fetch_real ("pressure");
        if (! config_.has_explicit_unit("pressure")) pressure *= pressure_unit;
      }

    if (config_.has_key ("formula"))
      {
        formula = config_.fetch_string ("formula");
      }

    if (config_.has_key ("state"))
      {
        state = config_.fetch_string ("state");
      }

    vector<string> composition_names;
    vector<int>    composition_nb_of_atoms;
    vector<double> composition_fraction_mass;
    string         composition_mode_label;
    int            composition_mode = -1;
    double         mean_z = -1.0;
    double         mean_a = -1.0;

    DT_THROW_IF (! config_.has_key ("composition.mode"),
                 std::logic_error,
                 "Missing 'composition.mode' property for material '" << name_ << "' !");
    composition_mode_label =  config_.fetch_string ("composition.mode");

    if (composition_mode_label == "number_of_atoms")
      {
        composition_mode = material::NUMBER_OF_ATOMS;
      }
    else if (composition_mode_label == "fraction_mass")
      {
        composition_mode = material::FRACTION_MASS;
      }
    else if (composition_mode_label == "mean_za")
      {
        composition_mode = material::MEAN_ZA;
      }
    else
      {
        DT_THROW_IF (true,std::logic_error,"Invalid 'composition.mode' property ('" << composition_mode_label << "') for material '" << name_ << "' !");
      }

    if (composition_mode == material::MEAN_ZA)
      {
        DT_THROW_IF (! config_.has_key ("mean_z"),
                     std::logic_error,
                     "Missing 'mean_z' property !");
        mean_z = config_.fetch_real ("mean_z");

        DT_THROW_IF (! config_.has_key ("mean_a"),
                     std::logic_error,
                     "Missing 'mean_a' property !");
        mean_a = config_.fetch_real ("mean_a");
      }
    else
      {
        DT_THROW_IF (! config_.has_key ("composition.names"),
                     std::logic_error,
                     "Missing 'composition.names' property for material '" << name_ << "' !");
        config_.fetch ("composition.names", composition_names);
        DT_THROW_IF (composition_names.size () == 0,
                     std::logic_error,
                     "Empty list of compounds for material '" << name_ << "' !");

        if (composition_mode == material::NUMBER_OF_ATOMS)
          {
            DT_THROW_IF (! config_.has_key ("composition.number_of_atoms"),
                         std::logic_error,
                         "Missing 'composition.number_of_atoms' property for material '" << name_ << "' !");
            config_.fetch ("composition.number_of_atoms", composition_nb_of_atoms);
            DT_THROW_IF (composition_names.size () != composition_nb_of_atoms.size (),
                         std::logic_error,
                         "Unmatching sizes of list of compounds/number of atoms !");
          }

        if (composition_mode == material::FRACTION_MASS)
          {
            DT_THROW_IF (! config_.has_key ("composition.fraction_mass"),
                         std::logic_error,
                         "Missing 'composition.fraction_mass' property for material '" << name_ << "' !");
            config_.fetch ("composition.fraction_mass", composition_fraction_mass);
            DT_THROW_IF (composition_names.size () != composition_fraction_mass.size (),
                         std::logic_error,
                         "Unmatching sizes of list of compounds/fraction mass for material '" << name_ << "' !");
          }
      }

    material * matl = new material ();
    try
      {
        matl->set_name (name_);
        matl->set_density (density);

        if (composition_mode == material::MEAN_ZA)
          {
            matl->set_mean_z_a (mean_z, mean_a);
          }

        if (composition_mode == material::NUMBER_OF_ATOMS)
          {
            for (int i = 0; i < composition_names.size (); i++)
              {
                element_dict_type::const_iterator found = elements_.find (composition_names[i]);
                DT_THROW_IF (found ==  elements_.end (),
                             std::logic_error,
                             "Unknown element '" << composition_names[i] << "') !");
                const element & elmt = found->second.get_ref ();
                int nb_atoms = composition_nb_of_atoms[i];
                matl->add_element_by_nb_of_atoms (elmt , nb_atoms);
              }
          }

        if (composition_mode == material::FRACTION_MASS)
          {
            for (int i = 0; i < composition_names.size (); i++)
              {
                const element * a_elmt = 0;
                const material * a_matl = 0;
                if (composition_names[i] == name_)
                  {
                    ostringstream message;
                    message << "Self-referenced material with name '"
                            << composition_names[i] << "' for material '" << name_ << "' !";
                    delete matl;
                    DT_THROW_IF(true, std::logic_error, message.str ());
                  }
                element_dict_type::const_iterator found = elements_.find (composition_names[i]);
                if (found != elements_.end ())
                  {
                    a_elmt = found->second.get_ptr ();
                  }
                else
                  {
                    material_dict_type::const_iterator found2 = materials_.find (composition_names[i]);
                    if (found2 != materials_.end ())
                      {
                        DT_THROW_IF (found2->second.is_alias(),
                                     std::logic_error,
                                     "Cannot use material alias '" << composition_names[i]
                                     << "' as a component for material '" << name_ << "' !");
                        a_matl = found2->second.get_ptr ();
                      }
                    else
                      {
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
                if (a_elmt !=  0)
                  {
                    matl->add_element_by_mass (*a_elmt, f_mass);
                  }
                else
                  {
                    matl->add_material_by_mass (*a_matl, f_mass);
                  }
              }
          }

        // Some auxiliary properties:
        if (! formula.empty ())
          {
            matl->grab_properties ().store ("formula", formula);
          }

        if (! state.empty ())
          {
            matl->grab_properties ().store ("state", state);
          }

        if (temperature > 0.0)
          {
            matl->grab_properties ().store ("temperature", temperature);
          }

        if (pressure > 0.0)
          {
            matl->grab_properties ().store ("pressure", pressure);
          }

        // 2012-11-03 FM : support for various "Material Properties Tables"
        // ala Geant4 :
        config_.export_starting_with (matl->grab_properties (), "mpt.");

        matl->build ();
      }
    catch (exception & x)
      {
        //cerr << "materials::factory::create_material: " << "Exception : " << x.what () << endl;
        delete matl;
        matl = 0;
        throw x;
      }
    return matl;
  }

} // end of namespace materials

// end of factory.cc
