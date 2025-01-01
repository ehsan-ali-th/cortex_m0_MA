#include "InstrPairTracker.h"

#include <string>

using namespace std;
using namespace esi;

namespace esi {

InstrPairTracker::InstrPairTracker()
{

}

InstrPairTracker::~InstrPairTracker()
{
}

void InstrPairTracker::addInstrPair(pair<Instruction, Instruction> InstrPair) {

    // First search if the instruction is already added.
    if (m_instructions_used.size() == 0) {
        // It is the first run so just add the new instruction
        m_instructions_used.push_back(InstrPair.first);
        InstPairStat instPairStat;
        instPairStat.addInstr(InstrPair.second);
        m_instPairStat.push_back(instPairStat);
    }
    else {
        size_t instrFoundIndex = 0;
        for (size_t i = 0; i < m_instructions_used.size(); i++) {
            Instruction instr = m_instructions_used[i];
            int res = InstrPair.first.getOpcode().compare(instr.getOpcode());
            if (res == 0) {
                instrFoundIndex = i;
                //  Instruction is already added
                break;
            }
        }

        if (instrFoundIndex == 0) {
            m_instructions_used.push_back(InstrPair.first);
            InstPairStat instPairStat;
            instPairStat.addInstr(InstrPair.second);
            m_instPairStat.push_back(instPairStat);

        }
        else {
             m_instPairStat[instrFoundIndex].addInstr(InstrPair.second);
        }
    }
}

void InstrPairTracker::InstPairStat::addInstr (Instruction instr_to_add) {
    bool pair_found = false;

    // First search if the pair exists
    for (size_t i = 0; i < m_paired_instructions.size(); i++) {
        Instruction inst = m_paired_instructions[i];
        int res = inst.getOpcode().compare(instr_to_add.getOpcode());
        if (res == 0) {
            // Instruction pair is already there so we just need to increment its count
            m_paired_instrs_count[i] ++;
            pair_found = true;
            break;

        }
    }

    if (! pair_found) {
       m_paired_instructions.push_back(instr_to_add);
       m_paired_instrs_count.push_back(1); // set count to 1
    }
}

vector <uint> InstrPairTracker::InstPairStat::getInstrsCount () const {
    return m_paired_instrs_count;
}

vector <InstrPairTracker::InstPairStat> InstrPairTracker::getInstPairStat () const {
    return m_instPairStat;
}

vector <Instruction>  InstrPairTracker::getInstructionsUsed() const {
    return m_instructions_used;
}

Instruction InstrPairTracker::InstPairStat::getPairedInstruction(size_t pair_index) const {
    return m_paired_instructions[pair_index];
}

}
