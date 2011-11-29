#include <iostream>
#include <vector>

#include <datatools/utils/properties.h>

int main (void)
{

  {
    datatools::utils::properties config;
    
    // Set explicitely the values of some integer properties :
    config.store ("number", 5, "Some number of something");
    config.store_integer ("another_number", 8);

    if (config.has_key ("number") && 
	config.is_integer ("number") && 
	config.is_scalar ("number")  )
      {
	std::cout << "Integer scalar property 'number' exists !" << std::endl;
      }

    if (config.is_integer ("another_number"))
      {
	bool another_number = config.fetch_integer ("another_number");
	std::cout << "Fetched integer value for 'another_number' is : " 
		  << another_number << std::endl;
      }

    {
      std::vector<int32_t> some_numbers;
      some_numbers.assign (3, 0);
      some_numbers[0] = 3;
      some_numbers[1] = 1;
      some_numbers[2] = 4;
      config.store ("some_numbers", some_numbers, "An array of integers");
    }

    {
      std::vector<int32_t> some_numbers;
      if (config.has_key ("some_numbers") &&
	  config.is_integer ("some_numbers") && 
	  config.is_vector ("some_numbers"))
	{
	  config.fetch ("some_numbers", some_numbers);
	}
      for (int i = 0; i < some_numbers.size (); ++i)
	{
	  std::clog << some_numbers[i] << ' ';
	}
      std::clog << std::endl;

      std::cout << "Size of the array : " 
		<< config.size ("some_numbers") << std::endl;

      std::cout << "Value at index 1 in the array : " 
		<< config.fetch_integer ("some_numbers", 1) << std::endl;

      config.change ("number", 666);
      config.change ("some_numbers", 7, 1);

      std::cout << "Value at index 1 in the array : " 
		<< config.fetch_integer ("some_numbers", 1) << std::endl;
 
      config.update ("dummy", 9);
      config.update ("another_number", -7);

      }

    config.tree_dump (std::cout, "The properties container :");
  }

  return 0;
}
