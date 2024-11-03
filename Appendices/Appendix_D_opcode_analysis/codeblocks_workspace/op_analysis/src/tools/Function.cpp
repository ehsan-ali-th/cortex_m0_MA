/*
 * Function.cpp
 *
 *  Created on: Jul 30, 2020
 *      Author: esi
 */

#include "Function.h"

#include <iostream>
#include <iomanip>

namespace esi {

Function::Function() {
}

Function::~Function() {
}

void Function::addMachineBlock(MachineBlock MB) {
	m_MBs.push_back(MB);
}

void Function::clearMachineBlocks() {
	m_MBs.clear();
}

void Function::setName(string name) {
	m_name = name;
}

string Function::getName () const {
	return m_name;
}

void Function::listMBs() const {
	int i = 0;
	for (auto &MB : m_MBs) {
		cout << "------------------ MB #" <<  dec <<    i << endl;
		for (auto &instr: MB.getInstructions()) {
            if (instr.getSize() == '2')
                cout << hex << instr.getLocation() << "\t" << instr.getOpcode() << "\t" << instr.getSize() <<
                    "\t" << "Enc: " << setfill('0') << setw (4) << instr.getEncoding() << endl;
            else
                cout << hex << instr.getLocation() << "\t" << instr.getOpcode() << "\t" << instr.getSize() <<
                    "\t" << "Enc: " << setfill('0') << setw (8) << instr.getEncoding() << endl;
		}
		i++;
	}
}

int Function::count_MBs () const {
	return m_MBs.size();
}

vector<MachineBlock> Function::getMBs () const {
	return m_MBs;
}

unsigned int Function::getNumOfInstructions() const {
    uint total_instr = 0;
    for (auto &MB : m_MBs) {
        uint instr = MB.getNumOfInstructions();
        total_instr += instr;
    }
	return total_instr;
}

void Function::setLoc (uint32_t loc) {
	m_loc = loc;
}

uint32_t Function::getLoc () const {
	return m_loc;
}

}  // end of namespace esi

