//============================================================================
// Name        : op_analysis.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "tools/tools.h"
#include "tools/OpcodeAnalyzer.h"

#include <iostream>
#include <iterator>
#include <fstream>


using namespace std;
using namespace esi;

int main(int argc, char *argv[]) {
    cout  << "Hi !" << endl;

	if (argc < 4) {
		cout << "Usage: ./op_analysis input_file1(opcodes.dat) exidx.dat rodata.dat" << endl;
		cout << "input_file1 (opcodes.dat) = should have .text      section content" << endl;
		cout << "input_file2 (exidx.dat)   = should have .ARM.exidx section content" << endl;
		cout << "input_file3 (rodata.dat)  = should have .rodata    section content" << endl;
		return -1;
	}

	string opcodes_filename = argv[1];
	string exidx_filename = argv[2];
	string rodata_filename = argv[3];


	cout << "- Analyzing : " << opcodes_filename  << " - " << exidx_filename << " - " << rodata_filename << endl;

	// Extract the filename by removing the extension
	size_t lastindex = opcodes_filename.find_last_of(".");
	if (lastindex == string::npos) {
		cout << "No extension in filename is specified. " <<
	    "Input filename must have .dat extension" << endl;
	    return -2;
	}

	string filename = opcodes_filename.substr(0, lastindex);
	string report_filename = filename + ".txt";
	string vhdl_filename = "miniature_accelerator.vhd";
	cout << "- VHDL Adaptive module: " << vhdl_filename << endl;
	cout << "- Report file: " << report_filename << endl;

	// Open the files
	ifstream opcodes_file_in (opcodes_filename.c_str(), ios::in);
	ifstream exidx_file_in (exidx_filename.c_str(), ios::in);
	ifstream rodata_file_in (rodata_filename.c_str(), ios::in);

	OpcodeAnalyzer opcodeAnalyzer(opcodes_file_in, exidx_file_in, rodata_file_in);

	opcodeAnalyzer.list_functions();
	cout << "Number of functions = " << opcodeAnalyzer.count_functions() << endl;

	for (int i = 0; i < opcodeAnalyzer.count_functions(); i++) {
        Function f = opcodeAnalyzer.getFunction(i);
        cout << "Function " << f.getName() << " has " << dec << f.count_MBs() << " Machine Blocks, with " <<
			f.getNumOfInstructions() << " instructions" <<  endl;
        f.listMBs();
	}

	Function func = opcodeAnalyzer.getFunction(0);

	opcodeAnalyzer.mineFunction(func);
	pair<Instruction, Instruction> mostFreqPair = opcodeAnalyzer.getMostFreqPair();
	cout << "Most Frequent pair is (" << mostFreqPair.first.getOpcode() << ", " <<
        mostFreqPair.second.getOpcode() << ")" << endl;

    vector <uint32_t> PCSensivityList = opcodeAnalyzer.getPCSensitivityList(0, mostFreqPair);

    opcodeAnalyzer.getRC_PC_sensivityFile(PCSensivityList);

    vector <pair <Instruction, Instruction> > intsr_pairs = opcodeAnalyzer.genAccelMemVHDL(0, mostFreqPair);
    opcodeAnalyzer.getPC_to_OP_File(PCSensivityList, intsr_pairs);
    opcodeAnalyzer.genMainProgramCoeFile();

	cout << "End of Program." << endl;
	return 0;
}


