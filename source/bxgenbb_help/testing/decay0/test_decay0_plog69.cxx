#include <iostream>
#include <cmath>
#include <genbb_help/decay0/plog69.h>
#include <genbb_help/decay0/divdif.h>

int main(void)
{
  double sl2[48];       
  sl2[ 1-1] = 2.2248;               // p = 0.1
  sl2[ 2-1] = 1.2634;               // p = 0.2
  sl2[ 3-1] = 1.0851;               // p = 0.3
  sl2[ 4-1] = 1.0275;               // p = 0.4
  sl2[ 5-1] = 1.0008;               // p = 0.5
  sl2[ 6-1] = 0.98693;              // p = 0.6
  sl2[ 7-1] = 0.97884;              // p = 0.7
  sl2[ 8-1] = 0.97426;              // p = 0.8
  sl2[ 9-1] = 0.97213;              // p = 0.9
  sl2[10-1] = 0.97128;              // p = 1.0
  sl2[11-1] = 0.97138;              // p = 1.2
  sl2[12-1] = 0.97276;              // p = 1.4
  sl2[13-1] = 0.9731;               // p = 1.6
  sl2[14-1] = 0.9728;               // p = 1.8
  sl2[15-1] = 0.9728;               // p = 2.0
  sl2[16-1] = 0.9731;               // p = 2.2
  sl2[17-1] = 0.9735;               // p = 2.4
  sl2[18-1] = 0.9740;               // p = 2.6
  sl2[19-1] = 0.9745;               // p = 2.8
  sl2[20-1] = 0.9750;               // p = 3.0
  sl2[21-1] = 0.9756;               // p = 3.2
  sl2[22-1] = 0.9762;               // p = 3.4
  sl2[23-1] = 0.9768;               // p = 3.6
  sl2[24-1] = 0.9774;               // p = 3.8
  sl2[25-1] = 0.9780;               // p = 4.0
  sl2[26-1] = 0.9794;               // p = 4.5
  sl2[27-1] = 0.9808;               // p = 5.0
  sl2[28-1] = 0.9821;               // p = 5.5
  sl2[29-1] = 0.9834;               // p = 6.0
  sl2[30-1] = 0.9846;               // p = 6.5
  sl2[31-1] = 0.9859;               // p = 7.0
  sl2[32-1] = 0.9870;               // p = 7.5
  sl2[33-1] = 0.9882;               // p = 8.0
  sl2[34-1] = 0.9903;               // p = 9.0
  sl2[35-1] = 0.9924;               // p = 10.0, E=4.625

  for (int i = 0; i < 35; i++)
    {
      std::cout << genbb::decay0::BJ69::plog69[i] << ' ' << sl2[i] << '\n';
    }
  std::cout << '\n' << '\n';

  {
    double pel = 0.15;
    double pellog = std::log(pel);
    double fpel = 
      genbb::decay0::decay0_divdif(sl2, 
                                   genbb::decay0::BJ69::plog69, 
                                   48, 
                                   pellog, 
                                   2);
    std::clog << pellog << ' ' << fpel << '\n';
  }
   
  for (double pel = 0.1; pel < 9.0; pel += 0.025)
    {
      double pellog = std::log(pel);
      double fpel = 
        genbb::decay0::decay0_divdif(sl2, 
                                     genbb::decay0::BJ69::plog69, 
                                     48, 
                                     pellog, 
                                     2);
      std::cout << pellog << ' ' << fpel << '\n';
    } 
  std::cout << '\n' << '\n';
  
  return 0;
}
