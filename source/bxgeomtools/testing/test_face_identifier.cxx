// test_face_identifier.cxx

// Ourselves:
#include <geomtools/face_identifier.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <geomtools/color.h>

int main (/*int argc_, char ** argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a sample program for class 'geomtools::face_identifier'!" << std::endl;

    {
      geomtools::face_identifier fid;
      fid.set_face_index(12);
      std::clog << "FID  = " << fid.to_string() << std::endl;
      fid.tree_dump(std::clog, "FID:");
      std::clog << std::endl;

      geomtools::face_identifier fid1;
      fid1.reset_face_index();
      std::clog << "FID1  = " << fid1.to_string() << std::endl;
      fid1.tree_dump(std::clog, "FID1:");
      std::clog << std::endl;

      geomtools::face_identifier fid2;
      fid2.set_face_bit(0x4);
      std::clog << "FID2 = " << fid2.to_string() << std::endl;
      fid2.tree_dump(std::clog, "FID2:");
      std::clog << std::endl;

      geomtools::face_identifier fid3;
      fid3.append_part(1);
      fid3.append_part(0);
      fid3.set_face_bits(0x16);
      std::clog << "FID3 = " << fid3.to_string() << std::endl;
      fid3.tree_dump(std::clog, "FID3:");
      std::clog << std::endl;

      geomtools::face_identifier fid4;
      fid4.append_part(1);
      fid4.append_part(geomtools::face_identifier::PART_INDEX_NONE);
      fid4.append_part(geomtools::face_identifier::PART_INDEX_ANY);
      fid4.append_part(0);
      fid4.reset_face_index();
      fid4.set_face_bits(0x16);
      std::clog << "FID4 = " << fid4.to_string() << std::endl;
      fid4.tree_dump(std::clog, "FID4:");
      std::clog << std::endl;

      geomtools::face_identifier fid5;
      fid5.append_part(1);
      fid5.append_part(geomtools::face_identifier::PART_INDEX_NONE);
      fid5.append_part(geomtools::face_identifier::PART_INDEX_ANY);
      fid5.append_part(0);
      fid5.set_face_index_any();
      std::clog << "FID5 = " << fid5.to_string() << std::endl;
      fid5.tree_dump(std::clog, "FID5:");
      std::clog << std::endl;

      geomtools::face_identifier fid6;
      fid6.append_part(1);
      fid6.append_part(geomtools::face_identifier::PART_INDEX_ANY);
      fid6.append_part(0);
      fid6.set_face_index_any();
      std::clog << "FID6 = " << fid6.to_string() << std::endl;
      fid6.tree_dump(std::clog, "FID6:");
      std::clog << std::endl;

      geomtools::face_identifier fid7;
      fid7.append_part(1);
      fid7.append_part(geomtools::face_identifier::PART_INDEX_ANY);
      fid7.append_part(0);
      fid7.set_face_index(12);
      std::clog << "FID7 = " << fid7.to_string() << std::endl;
      fid7.tree_dump(std::clog, "FID7:");
      std::clog << std::endl;

   }

    {
      std::vector<std::string> words = {
        "Z",
        "[!]",
        "[index=0]",
        "[index=a]",
        "[index=!]",
        "[index=*]",
        "[index=1]",
        "[index=9]",
        "[index=10]",
        "[index=01]",
        "[index=42]",
        "[bits=0]",
        "[bits=a]",
        "[bits=01]",
        "[bits=101]",
        "[bits=101]",
        "[parts=1.3:bits=101]",
        "[parts=1.*:bits=101]",
        "[parts=1.*:index=*]",
        "[parts=!.1.0:bits=00101]"
      };

      for (const auto & word : words) {
        geomtools::face_identifier fi;
        if (fi.parse(word)) {
          std::clog << "Parsing word '" << word << "' was successful: " << fi << std::endl;
          fi.tree_dump(std::clog, "Parsed face identifier:");
          std::clog << std::endl;
        } else {
          std::cerr << "Parsing word '" << word << "' failed!" << std::endl;
        }
        std::clog << std::endl;
      }

    }

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
