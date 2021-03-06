/*
 * OpcodeAnalyzer.h
 *
 *  Created on: Jul 28, 2020
 *      Author: esi
 */

#ifndef OPCODEANALYZER_H_
#define OPCODEANALYZER_H_

#include "Function.h"
#include "MachineBlock.h"
#include "Instruction.h"
#include "tools.h"
#include "InstrPairTracker.h"

#include <array>
#include <vector>
#include <fstream>
#include <utility>
#include <cstdint>

using namespace std;

namespace esi {

class OpcodeAnalyzer {
public:
	OpcodeAnalyzer(ifstream &opcodes_file, ifstream &exidx_file, ifstream &rodata_file);
	~OpcodeAnalyzer();

	int 		                    getTotalNoOpcodes () const;
	void 		                    CalcOpcodeOccurrence () const;
	int 		                    count_functions ();
	void 		                    list_functions ();
	Function	                    getFunction (int index) const;
	uint                            mineMachineBlock (MachineBlock &MB);
	uint                            mineFunction (Function &func);
	pair<Instruction, Instruction>	getMostFreqPair () const;
	vector <uint32_t>               getPCSensitivityList (uint function_index, pair<Instruction, Instruction> target_pair) const;
	vector <pair <Instruction, Instruction> >               genAccelMemVHDL (uint function_index, pair<Instruction, Instruction> target_pair) const;
    void                            genMainProgramCoeFile () const;
    void                            getRC_PC_sensivityFile (const vector <uint32_t>  PCSensivityList) const;
    void                            getPC_to_OP_File (const vector <uint32_t>  PCSensivityList, const vector <pair <Instruction, Instruction> >  inst_pairs) const;

private:
    // Reference to .dat file which contains which is the disassembly of final
    //      Cortex-M0 ELF file generated by issuing:
    // $ llvm-objdump -d fft_full.out  > opcodes.dat
	ifstream 									&m_opcodes_file;
	ifstream 									&m_exidx_file;
	ifstream 									&m_rodata_file;
	// Contains the functions present in ELF file.
	vector<Function>							m_Functions;
	vector< pair<Instruction, Instruction> > 	m_pairs;
	vector <uint>								m_pair_occurance;



	// Stores the location of all occurred pairs, e.g. 	[Pair 0, appears at 0]
	//									 				[Pair 0, appears at 69]
	//									 				[Pair 0, appears at 81]
	// means there is a pair at location 0, 69, and 81 : 3 occurrences.
	vector <array<uint, 2> >					m_pair_locs;

	InstrPairTracker                            m_instr_pair_tracker;

//	struct BL_instr {
//        uint32_t S : 1, imm10 : 10, :2, J1 : 1, :1, J2 : 1, imm11 : 11;
//	};

};

}  // end of namespace esi

#endif /* OPCODEANALYZER_H_ */
