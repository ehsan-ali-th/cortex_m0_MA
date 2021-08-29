#ifndef INSTRPAIRTRACKER_H
#define INSTRPAIRTRACKER_H

#include "Instruction.h"
#include "tools.h"

#include <utility>
#include <vector>

using namespace std;
using namespace esi;

namespace esi {

class InstrPairTracker
{
    public:
        class InstPairStat {
            public:
                void addInstr (Instruction instr);
                vector <uint> getInstrsCount () const;
                Instruction getPairedInstruction(size_t pair_index) const;
            private:
                // There is a one-to-one relationship between m_paired_instructions and
                //      m_paired_instrs_count.
                vector <Instruction>    m_paired_instructions;
                vector <uint>           m_paired_instrs_count;
        };

        InstrPairTracker();
        ~InstrPairTracker();

        void addInstrPair(pair<Instruction, Instruction> InstrPair);
        vector <InstPairStat> getInstPairStat() const;
        vector <Instruction>  getInstructionsUsed() const;

    private:
        // Contains the instructions used in a function under analysis
        vector <Instruction>                        m_instructions_used;
        // every instruction has a stat. There is a one-to-one relationship between
        //      m_instructions_used and m_instPairStat
        vector <InstPairStat>                       m_instPairStat;
};

}

#endif // INSTRPAIRTRACKER_H
