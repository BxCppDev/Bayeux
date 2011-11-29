#include <iostream>

#include <datatools/utils/properties.h>

int main (void)
{
  {
    datatools::utils::properties config ("A test properties container");
    config.store_flag ("test", "A boolean value");
    config.store ("debug.level", 1, "The debug level");
    config.store ("pi", 3.14159, "The approximated value of pi");
    config.store ("name", "Monty");
    
    // ASCII formatting in the terminal :
    datatools::utils::properties::config writer;
    std::cout << "ASCII formatted printing of the properties container : "<< endl;
    std::cout << "----------------------------------------------------" << endl;
    writer.write (std::cout, config);
    std::cout << "----------------------------------------------------" << endl;
    std::cout << endl;

    // save in an ASCII configuration file :
    datatools::utils::properties::write_config ("properties_4.conf",
						config);
  }

  {
    datatools::utils::properties restored_config;
    // read (restore) from an ASCII configuration file :
    datatools::utils::properties::read_config ("properties_4.conf",
						restored_config);
    restored_config.tree_dump (std::cout, 
			       "The properties container restored from the file :"); 
  }

  return 0;
}
