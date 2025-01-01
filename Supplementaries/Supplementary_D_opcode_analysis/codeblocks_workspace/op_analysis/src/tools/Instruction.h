/*
 * Instruction.h
 *
 *  Created on: Aug 3, 2020
 *      Author: esi
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "tools.h"

#include <string>
#include <cstdint>

using namespace std;
using namespace esi;

namespace esi {

class Instruction {
public:
	Instruction();
	Instruction(uint32_t encoding, uint32_t loc, string opcode, uchar size);
	~Instruction();

	void setOpcode(string opcode);
	void setSize(uchar size);
	void setLocation(uint32_t loc);
	uint32_t getLocation () const;
	string getOpcode() const;
	uchar getSize() const;
	bool operator==(const Instruction& rhs) const;
	void setEncoding (uint32_t encoding);
	uint32_t getEncoding () const;

private:
    uint32_t        m_encoding;
	uint32_t		m_loc;
	string 			m_opcode;
	uchar 			m_size;

};

}
#endif /* INSTRUCTION_H_ */
