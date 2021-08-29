/*
 * Instruction.cpp
 *
 *  Created on: Aug 3, 2020
 *      Author: esi
 */

#include "Instruction.h"

namespace esi {

Instruction::Instruction() :
	m_opcode("NONE"),
	m_size('0') {

}
Instruction::Instruction(uint32_t encoding, uint32_t loc, string opcode, uchar size) :
    m_encoding (encoding),
	m_loc (loc),
	m_opcode(opcode),
	m_size(size) {
}

Instruction::~Instruction() {
}

string Instruction::getOpcode() const {
	return m_opcode;
}

unsigned char Instruction::getSize() const {
	return m_size;
}

void Instruction::setOpcode(string opcode) {
	m_opcode = opcode;
}

void Instruction::setSize(unsigned char size) {
	m_size = size;
}

bool Instruction::operator==(const Instruction& rhs) const {
    return (m_opcode == rhs.m_opcode) && (m_size == rhs.m_size);
}

void Instruction::setLocation(uint32_t loc) {
	m_loc = loc;
}

uint32_t Instruction::getLocation () const {
	return m_loc;
}

void Instruction::setEncoding (uint32_t encoding) {
    m_encoding = encoding;
}

uint32_t Instruction::getEncoding () const {
    return m_encoding;
}


}

