// -*- mode: c++; -*- 
// test_version_id.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/version_id.h>

using namespace std;

void check_compare (const datatools::utils::version_id & v0_,
                    const datatools::utils::version_id & v1_,
                    bool major_only_ = false)
{
  if (datatools::utils::version_id::are_orderable (v0_, v1_, major_only_))
    {
      int comp = v0_.compare (v1_, major_only_);
      if (comp == 0)
        {
          clog << "  " << v0_ << " == " << v1_;
        }
      else if (comp < 0)
        {
          clog << "  " << v0_ << " < " << v1_;
        }
      else if (comp > 0)
        {
          clog << "  " << v0_ << " > " << v1_;
        }
      if (major_only_) clog << " (major only)";
      clog << endl;
    }
  else
    {
      clog << "  " << v0_ << " or " << v1_ << " is not orderable !" << endl;
    }
  return;
}

void check_match (const datatools::utils::version_id & v0_,
                  const string & rule_,
                  bool major_only_ = false)
{
  try
    {
      if (v0_.matches (rule_, major_only_))
        {
          clog << "  " << v0_ << " matches rule '" << rule_ << "'"; 
        }
      else
        {
          clog << "  " << v0_ << " does NOT match rule '" << rule_ << "'"; 
        
        }
      if (major_only_) clog << " (major only)";
      clog << endl;
    }
  catch (exception & x)
    {
      clog << "  " << "error: Cannot check matching for '" 
           << v0_ << "' with rule '" << rule_ << "' : " << x.what () << endl;
    } 
  return;
}

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'version_id' class..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
        {
          string arg = argv_[iarg];

          if ((arg == "-d") || (arg == "--debug")) debug = true;

          iarg++;
        }
    
      {
        datatools::utils::version_id v0;
        v0.dump (clog, "v0 : ");
   
        datatools::utils::version_id v1 (1);
        v1.dump (clog, "v1 : ");
   
        datatools::utils::version_id v2 (1,-1,0);
        v2.dump (clog, "v2 : ");
 
        datatools::utils::version_id v3 (3,1,4);
        v3.dump (clog, "v3 : ");

        datatools::utils::version_id v4 (3,1,4,"159-a");
        v4.dump (clog, "v4 : ");

        datatools::utils::version_id v5 ("159");
        v5.dump (clog, "v5 : ");
 
        try
          {
            datatools::utils::version_id v6 ("!1.5.9");
          }
        catch (exception & x)
          { 
            clog << "As expected there is an error: " << x.what () << endl; 
          }

        datatools::utils::version_id v7 (3,1,4, "159");
        v7.dump (clog, "v7 : ");

        datatools::utils::version_id v8 (3,-1,-1, "159");
        v8.dump (clog, "v8 : ");

        datatools::utils::version_id v9 (3,1,4, "alpha");
        v9.dump (clog, "v9 : ");

        datatools::utils::version_id v10 (3,1,4, "rc0");
        v10.dump (clog, "v10 : ");

        datatools::utils::version_id v11 (3,1,4, "rc666");
        v11.dump (clog, "v11 : ");
        }
      
      {
        clog << "Format : " << endl;
        datatools::utils::version_id v0 (3, 1, 4);
        clog << "  v0 = " << v0 << endl;
   
        datatools::utils::version_id v1 (3, 1, 4);
        clog << "  v1 = " << v1 << endl;
       
        datatools::utils::version_id v2 (3, 1, 5);
        clog << "  v2 = " << v2 << endl;
       
        datatools::utils::version_id v3 (3, 1, 5, "159");
        clog << "  v3 = " << v3 << endl;
      
        datatools::utils::version_id v4 (3, 1, 5, "test");
        clog << "  v4 = " << v4 << endl;
      
        datatools::utils::version_id v5 (3, 1, 5, "alpha");
        clog << "  v5 = " << v5 << endl;
      
        datatools::utils::version_id v6 (3, 1, 5, "beta");
        clog << "  v6 = " << v6 << endl;
      
        datatools::utils::version_id v7 (3, 1, 5, "rc0");
        clog << "  v7 = " << v7 << endl;
     
        datatools::utils::version_id v8 (3, 1, 5, "rc1");
        clog << "  v8 = " << v8 << endl;
    
        datatools::utils::version_id v9 (3);
        clog << "  v9 = " << v9 << endl;

        clog << "Equality : " << endl;
        if (v0 == v1)
          {
            clog << "  " << v0 << " == " << v1 << endl;
          }
        else
          {
            clog << "  " << v0 << " != " << v1 << endl;
          }

        if (v0 == v2)
          {
            clog << "  " << v0 << " == " << v2 << endl;
          }
        else
          {
            clog << "  " << v0 << " != " << v2 << endl;
          }

        if (v0 == v3)
          {
            clog << "  " << v0 << " == " << v3 << endl;
          }
        else
          {
            clog << "  " << v0 << " != " << v3 << endl;
          }

        if (v0 == v4)
          {
            clog << "  " << v0 << " == " << v4 << endl;
          }
        else
          {
            clog << "  " << v0 << " != " << v4 << endl;
          }

        clog << "Comparison : " << endl;
        check_compare (v0, v1);
        check_compare (v0, v2);
        check_compare (v0, v3);
        check_compare (v3, v0);
        check_compare (v3, v3);
        try
          {
            int comp = v0.compare (v4);
          }
        catch (exception & x)
          { 
            clog << "  As expected : " << x.what () << endl; 
          } 
        check_compare (v0, v4);
        check_compare (v0, v5);
        check_compare (v5, v6);
        check_compare (v6, v7);
        check_compare (v7, v8);
        check_compare (v5, v7);
        check_compare (v2, v5);
        check_compare (v3, v9);
        check_compare (v3, v9, true);
     
      }

      {
         clog << "Parsing : " << endl;

         vector<string> tokens;
         tokens.push_back ("?");
         tokens.push_back ("1");
         tokens.push_back ("1.");
         tokens.push_back ("-aaa");
         tokens.push_back ("1-3");
         tokens.push_back ("4.1-3");
         tokens.push_back ("1-3test");
         tokens.push_back ("1-test");
         tokens.push_back ("1a");
         tokens.push_back ("1.2");
         tokens.push_back ("1.2a");
         tokens.push_back ("1.2-a");
         tokens.push_back ("1.-2");
         tokens.push_back ("1.2.");
         tokens.push_back ("1.2.-a");
         tokens.push_back ("1.2.a");
         tokens.push_back ("1.2.3");
         tokens.push_back ("1.2.3-0");
         tokens.push_back ("1.2.3.4");
         tokens.push_back ("3.1.4-159");
         tokens.push_back ("3.1.4-");
         tokens.push_back ("3.1.4-1a");
         tokens.push_back ("4.5.2-8ubuntu4");
         tokens.push_back ("4.5.2-alpha");
         tokens.push_back ("4.5.2-beta");
         tokens.push_back ("4.5.2-rc");
         tokens.push_back ("4.5.2-rc0");
         tokens.push_back ("4.5.2-rc13");
         tokens.push_back ("Natty");
         tokens.push_back ("Oneric");
         tokens.push_back ("Precise");
         tokens.push_back ("Tiger");
         tokens.push_back ("0azerty");
         tokens.push_back ("a-banana");

         for (int i = 0; i < tokens.size (); i++)
           {
             datatools::utils::version_id v;
             istringstream v_iss (tokens[i]);
             clog << "  Token '" << tokens[i] << "' : ";
             v_iss >> v;
             if (v_iss)
               { 
                 clog << v << " (ok)";
               }
             else
               {
                 clog << v << " (invalid format)";
               }
             clog << endl;
           }

      }

      {
         clog << "Matching : " << endl;

         datatools::utils::version_id v0 (3, 1, 4);
         check_match (v0, "> 1");
         check_match (v0, ">3");
         check_match (v0, ">3",true);
         check_match (v0, ">=3");
         check_match (v0, ">=3",true);
         check_match (v0, ">4");
         check_match (v0, ">4",true);
         check_match (v0, "<3");
         check_match (v0, "<3",true);
         check_match (v0, "<=3");
         check_match (v0, "<=3",true);
         check_match (v0, "<3.2");
         check_match (v0, "!= 3.1");
         check_match (v0, "<= 3.0.3-alpha");
         check_match (v0, "<= 3.0.3-test");
         check_match (v0, "<= 3.1.4-test");
         check_match (v0, "> 3.1.4-rc3");
         check_match (v0, "!=3.1.4");
         check_match (v0, "=3.1.4");
         check_match (v0, "== 3.1.4");
         check_match (v0, "!3.1.4");

         datatools::utils::version_id v1 (3, 1);
      }

    }
  catch (exception & x)
    { 
      clog << "error: " << x.what () << endl; 
      error_code =  EXIT_FAILURE;
    }
  catch (...) 
    { 
      clog << "error: " << "unexpected error!" << endl;  
      error_code = EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_version_id.cxx 
  
