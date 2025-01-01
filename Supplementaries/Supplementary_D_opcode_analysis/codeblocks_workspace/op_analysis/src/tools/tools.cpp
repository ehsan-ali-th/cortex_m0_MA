/*
 * tools.cpp
 *
 *  Created on: Jul 28, 2020
 *      Author: esi
 */

#include "tools.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

namespace esi {

    void print_binary (const uint32_t value, ofstream & os) {

        int i;
        for (i = 1 << 24; i > 0; i = i / 2)
        {
          if((value & i) != 0)
            os << "1";
          else
            os << "0";
        }
    }
}

