// -*- mode: c++; -*-
// plog69.cc

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>

#include <genbb_help/decay0/plog69.h>

namespace genbb {
  namespace decay0 {

    /*
    const double  BJ69::get_plog69(int i) {
      static std::vector<double> pl69;
      if (pl69.empty()) {
        pl69.push_back(-2.302585);
        pl69.push_back(-1.609438);
        pl69.push_back(-1.203973);
        pl69.push_back(-0.9162907);
        pl69.push_back(-0.6931472);
        pl69.push_back(-0.5108256);
        pl69.push_back(-0.3566750);
        pl69.push_back(-0.2231435);
        pl69.push_back(-0.1053605);
        pl69.push_back(0.0000000);
        pl69.push_back(0.1823216);
        pl69.push_back(0.3364722);
        pl69.push_back(0.4700036);
        pl69.push_back(0.5877866);
        pl69.push_back(0.6931472);
        pl69.push_back(0.7884574);
        pl69.push_back(0.8754688);
        pl69.push_back(0.9555114);
        pl69.push_back(1.029619);
        pl69.push_back(1.098612);
        pl69.push_back(1.163151);
        pl69.push_back(1.223776);
        pl69.push_back(1.280934);
        pl69.push_back(1.335001);
        pl69.push_back(1.386294);
        pl69.push_back(1.504077);
        pl69.push_back(1.609438);
        pl69.push_back(1.704748);
        pl69.push_back(1.791759);
        pl69.push_back(1.871802);
        pl69.push_back(1.945910);
        pl69.push_back(2.014903);
        pl69.push_back(2.079442);
        pl69.push_back(2.197225);
        pl69.push_back(2.302585);
        pl69.push_back(2.397895);
        pl69.push_back(2.484907);
        pl69.push_back(2.564949);
        pl69.push_back(2.639057);
        pl69.push_back(2.890372);
        pl69.push_back(2.995732);
        pl69.push_back(3.218876);
        pl69.push_back(3.401197);
        pl69.push_back(3.555348);
        pl69.push_back(3.688879);
        pl69.push_back(3.806663);
        pl69.push_back(3.912023);
      }
      return pl69[i];
    }
    */

    const double BJ69::plog69[48] = {
        -2.302585,
        -1.609438,
        -1.203973,
        -0.9162907,
        -0.6931472,
        -0.5108256,
        -0.3566750,
        -0.2231435,
        -0.1053605,
        0.0000000,
        0.1823216,
        0.3364722,
        0.4700036,
        0.5877866,
        0.6931472,
        0.7884574,
        0.8754688,
        0.9555114,
        1.029619,
        1.098612,
        1.163151,
        1.223776,
        1.280934,
        1.335001,
        1.386294,
        1.504077,
        1.609438,
        1.704748,
        1.791759,
        1.871802,
        1.945910,
        2.014903,
        2.079442,
        2.197225,
        2.302585,
        2.397895,
        2.484907,
        2.564949,
        2.639057,
        2.890372,
        2.995732,
        3.218876,
        3.401197,
        3.555348,
        3.688879,
        3.806663,
        3.912023
      };

  } // end of namespace decay0
} // end of namespace genbb
