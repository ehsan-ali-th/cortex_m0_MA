/*
 * MachineBlock.h
 *
 *  Created on: Jul 30, 2020
 *      Author: esi
 */

#ifndef MACHINEBLOCK_H_
#define MACHINEBLOCK_H_

#include "Instruction.h"

#include <vector>

namespace esi {

class MachineBlock {
public:
	MachineBlock();
	virtual ~MachineBlock();

	void 				addInstruction(const Instruction instr);
	vector<Instruction>	getInstructions() const;
	uint                getNumOfInstructions() const;
	void 				clearInstructions();

private:
	vector<Instruction> 	m_instructions;
};

}  // end of namespace esi

#endif /* MACHINEBLOCK_H_ */
