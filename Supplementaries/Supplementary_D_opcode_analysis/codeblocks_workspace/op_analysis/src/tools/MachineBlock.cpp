/*
 * MachineBlock.cpp
 *
 *  Created on: Jul 30, 2020
 *      Author: esi
 */

#include "MachineBlock.h"

namespace esi {

MachineBlock::MachineBlock() {
}

MachineBlock::~MachineBlock() {
}

void MachineBlock::addInstruction(const Instruction instr) {
	m_instructions.push_back(instr);
}

vector<Instruction> MachineBlock::getInstructions() const {
	return m_instructions;
}

void MachineBlock::clearInstructions() {
	m_instructions.clear ();
}

uint MachineBlock::getNumOfInstructions() const {
    return m_instructions.size ();
}

}  // end of namespace esi

