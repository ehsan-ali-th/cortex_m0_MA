#ifndef TOOLS_H_
#define TOOLS_H_

#include <cstdint>
#include <fstream>

using namespace std;

namespace esi {

typedef unsigned int uint;
typedef unsigned char uchar;

void print_binary (const uint32_t value, ofstream & os);

}

#endif
