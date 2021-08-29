/*
 * Function.h
 *
 *  Created on: Jul 30, 2020
 *      Author: esi
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "MachineBlock.h"

#include <vector>
#include <string>
#include <cstdint>

namespace esi {

class Function {
public:
	Function();
	virtual ~Function();

	void 					addMachineBlock(MachineBlock MB);
	void 					clearMachineBlocks();
	void 					setName(string name);
	string 					getName() const;
	unsigned int			getNumOfInstructions() const;
	void 					listMBs() const;
	int 					count_MBs() const;
	vector<MachineBlock> 	getMBs() const;
	void					setLoc (uint32_t loc);
	uint32_t				getLoc () const;

private:
	string 					m_name;
	vector<MachineBlock> 	m_MBs;
	uint32_t				m_loc;
};

}  // end of namespace esi

#endif /* FUNCTION_H_ */
