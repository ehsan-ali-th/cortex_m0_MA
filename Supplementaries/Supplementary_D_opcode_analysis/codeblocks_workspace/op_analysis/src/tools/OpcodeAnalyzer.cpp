/*
 * OpcodeAnalyzer.cpp
 *
 *  Created on: Jul 28, 2020
 *      Author: esi
 */


#include "OpcodeAnalyzer.h"
#include "tools.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <array>
#include <cassert>
#include <iterator>
#include <iomanip>
#include <utility>


using namespace std;

namespace esi {

OpcodeAnalyzer::OpcodeAnalyzer(ifstream &opcodes_file, ifstream &exidx_file, ifstream &rodata_file) :
	m_opcodes_file(opcodes_file),
	m_exidx_file(exidx_file),
	m_rodata_file(rodata_file) {

	m_opcodes_file.clear();
	m_opcodes_file.seekg(0, ios::beg);

	bool first_function_parse = true;
	string line;
	string func_name;
	uint32_t function_loc;
	Function function;
	MachineBlock MB;
	bool data_section_detected = false;
	uint line_number = 1;


	// iterate through the lines and add the opcodes
	while (getline (m_opcodes_file, line)) {
		// skip the first 6 lines.
		if (line_number < 17) {
			line_number ++;
			continue;
		}

		stringstream line_ss(line);
		string token;
		vector<string> tokens;
		while (line_ss >> token) {
			tokens.push_back(token);
		}
		if (first_function_parse) {
			if (! data_section_detected) {
				if (tokens.capacity() == 2) {
					// Either function start point or data section start point
					first_function_parse = false;
					if (tokens[1][1] == '$') {
						// This is a data section
						data_section_detected = true;

					} else {
						// This is a function
						data_section_detected = false;
						stringstream location_ss;
						location_ss << hex << tokens[0].substr(0, 8); // extract the low 4-bits
						location_ss >> function_loc;
						func_name = tokens[1];
					}
				}
				else {
                    continue;
				}
			}
		}
		else {
			if (tokens.capacity() == 2) {
                // when previously data section is detected then this time it is a function
                if (data_section_detected) {
                    data_section_detected = false;
                    stringstream location_ss;
                    location_ss << hex << tokens[0].substr(0, 8); // extract the low 4-bits
                    location_ss >> function_loc;
                    func_name = tokens[1];
                }
                else {
                    // Either function start point or data section start point
                    function.setName(func_name);
                    function.setLoc(function_loc);
                    function.addMachineBlock(MB);	// Save the last function
                    m_Functions.push_back(function);
                    stringstream location_ss;
                    location_ss << hex << tokens[0].substr(0, 8); // extract the low 4-bits
                    location_ss >> function_loc;
                    func_name = tokens[1];
                }

                if (tokens[1][1] == '$') {
                    // This is a data section
                    data_section_detected = true;

                } else {
                    // This is a function
                    data_section_detected = false;
                    stringstream location_ss;
                    location_ss << hex << tokens[0].substr(0, 8); // extract the low 4-bits
                    location_ss >> function_loc;
                    func_name = tokens[1];
                }

                function.clearMachineBlocks();
                first_function_parse = false;
                MB.clearInstructions();
			}
			else {
                // Then it can be empty line or instruction line.
                if (! data_section_detected) { // ignore the line if this is data section line
                    if (tokens.capacity() > 4) { // Just to be sure it is not a data section start line
                        stringstream location_ss;
                        stringstream encoding_ss;
                        location_ss << hex << tokens[0].substr(0, 5); // extract the low 4-bits
                        uint32_t encoding;
                        uint32_t instr_loc;
                        location_ss >> instr_loc;
                        uchar size;
                        string opcode;

                        string line_till_tab = line.substr(0, line.find('\t'));
                        istringstream line_till_tab_ss (line_till_tab);
                        uint words = distance(istream_iterator<string>{line_till_tab_ss}, istream_iterator<string>{});
                        if (words == 5) {
                            // instruction size is 4
                            encoding_ss << hex << tokens[2] + tokens[1] + tokens[4] + tokens[3];
                            size = '4';
                            opcode = tokens[5];
                        }
                        else {
                            // instruction size is 2
                            encoding_ss << hex << tokens[2] + tokens[1];
                            size = '2';
                            opcode = tokens[3];
                        }
                        encoding_ss >> encoding;
                        Instruction instr(encoding, instr_loc, opcode, size);
                        if (opcode.at(0) == 'b' ) {
                            // It is a branch instruction
                            MB.addInstruction(instr);
                            function.addMachineBlock(MB);
                            MB.clearInstructions();
                        }
                        else
                        MB.addInstruction(instr);

                    }
                    else {
                        // empty line
                        continue;
                    }
                }
			}
        }
	}
}

OpcodeAnalyzer::~OpcodeAnalyzer() {
}

int OpcodeAnalyzer::count_functions () {
	return m_Functions.size();
}

void OpcodeAnalyzer::list_functions () {
	cout << "List of Functions:" << endl;
	for (auto &func : m_Functions) {
		cout << func.getName() << endl;
	}
}

Function OpcodeAnalyzer::getFunction (int index) const {
	return m_Functions[index];
}

uint OpcodeAnalyzer::mineMachineBlock (MachineBlock &MB) {
	// clear the data from possible previous mining
	m_pairs.clear();
	m_pair_locs.clear();

	vector<Instruction> instructions = MB.getInstructions();
	vector< pair<string, string> > pairs;

	// First make initial pairs
	for (vector<Instruction>::iterator it = instructions.begin(); it !=  instructions.end(); ++it) {
		pair <Instruction, Instruction> mypair;
		if (it + 1 == instructions.end()) {
			Instruction Inst(0, 0, "NONE", '0');
			mypair = make_pair(*it, Inst);

		} else
			mypair = make_pair(*it, *(it + 1));
		m_pairs.push_back(mypair);
	}
	cout << "Total pairs = " << m_pairs.size() << endl;

	m_pair_occurance.resize(m_pairs.size());


	 bool *pair_index_used = new bool[m_pairs.size()];

	// set all elements to false
	for (int i = 0; i < (int) m_pairs.size(); i++)
		*(pair_index_used + i) = false;

	uint i = 0;
	uint j = 0;

	// Then see if any pair matches
	for (vector< pair<Instruction, Instruction> >::iterator it = m_pairs.begin();
			it !=  m_pairs.end(); ++it) {
		if (! *(pair_index_used + i))
			for (vector< pair<Instruction, Instruction> >::iterator it2 = m_pairs.begin();
					it2 !=  m_pairs.end(); ++it2) {
				if (*it == *it2) {
					cout << "pair matched  #" << i << "\t" <<
							"(" << (*it).first.getOpcode() << "," << (*it).second.getOpcode() << ")" << " matched with " <<
							"j= " << j << " (" << (*it2).first.getOpcode() << "," << (*it2).second.getOpcode() << ")" << endl;
					*(pair_index_used + j)  = true;
					m_pair_occurance[i] = m_pair_occurance[i] + 1;
					// save the pair occurrence location
					array<unsigned int, 2> loc = {i, j};
					m_pair_locs.push_back(loc);
				}
				j++;
			}
		i++;
		j = 0;
	}

	// return the index of the most frequent pair
	uint max = 0;
	int pair_index = 0;
	i = 0;
	for (auto &pair_occurance: m_pair_occurance) {
		if (pair_occurance > max) {
			pair_index = i;
			max = pair_occurance;
		}
		i++;
	}

	cout << "Mining finished." << endl;

	delete[] pair_index_used;

	return pair_index;
}

/*
 * The getMostFreqPair() function return the most frequent pair.
 */
pair<Instruction, Instruction>	OpcodeAnalyzer::getMostFreqPair () const {
	uint max = 0;
	uint pair_index = 0;
	uint instruction_index = 0;
	vector <InstrPairTracker::InstPairStat> instPairStat = m_instr_pair_tracker.getInstPairStat();
	vector <Instruction> instructions_used = m_instr_pair_tracker.getInstructionsUsed();
	for (size_t i = 0; i < instPairStat.size(); i++) {
        // Get counts for each pair associated with the instruction
        vector <uint> instr_count = instPairStat[i].getInstrsCount();
        for (size_t j = 0; j < instr_count.size(); j++) {
            int is_branch = instructions_used[i].getOpcode().compare("bl");
            if (is_branch == 0) break;
            // make sure instruction is not branvh when replacing max value
            if (instr_count[j] > max) {
                instruction_index = i;
                pair_index = j;
                max = instr_count[j];
            }
        }
	}

    cout << "The pair frequency is " << dec << instPairStat[instruction_index].getInstrsCount().at(pair_index)<< endl;
	return make_pair(instructions_used[instruction_index],
                    instPairStat[instruction_index].getPairedInstruction(pair_index));
}

vector <uint32_t> OpcodeAnalyzer::getPCSensitivityList (uint function_index,
                                    pair<Instruction, Instruction> target_pair) const {
    Function target_func = getFunction(function_index);
    vector<MachineBlock> MBs = target_func.getMBs();
    vector <uint32_t> PCs;

    for (auto const &MB : MBs) {
        vector<Instruction> Instrs = MB.getInstructions();
        for (size_t i = 0; i < Instrs.size(); i ++) {
            Instruction Instr_left  = Instrs[i];
            Instruction Instr_right;
            if (i == Instrs.size() - 1) {
                // Last instruction
                Instr_right = Instrs[i];
            } else {
                Instr_right = Instrs[i + 1];
            }
            int res = Instr_left.getOpcode().compare(target_pair.first.getOpcode());
            if (res == 0) {
                // first instruction matches
                int res2 = Instr_right.getOpcode().compare(target_pair.second.getOpcode());
                if (res2 == 0)
                    // second instruction matches, therefore this is a pair and its location must be saved
                    PCs.push_back(Instr_left.getLocation());
            }
        }
    }

	return PCs;
}

// Ouputs the Accelerator Memory VHDL module
// Output filename: RC_accel_mem.vhd
// Return the instructions pair
vector <pair <Instruction, Instruction> > OpcodeAnalyzer::genAccelMemVHDL (uint function_index,
                            pair<Instruction, Instruction> target_pair) const {
    Function target_func = getFunction(function_index);
    vector<MachineBlock> MBs = target_func.getMBs();
    vector <pair <Instruction, Instruction> > opcodes;

    ofstream vhd_file;
    vhd_file.open ("RC_accel_mem.vhd");

    vhd_file << "-- Miniature Accelerator Memory VHDL File:" << endl;
    vhd_file << "library IEEE;" << endl;
    vhd_file << "use IEEE.STD_LOGIC_1164.ALL;" << endl;
    vhd_file << "use IEEE.Numeric_Std.all;" << endl << endl;

    vhd_file << "entity accel_rom is" << endl;
    vhd_file << "    port (" << endl;
    vhd_file << "        clk   : in  std_logic;" << endl;
    vhd_file << "        addr : in  std_logic_vector (3 downto 0);" << endl;
    vhd_file << "        dataout : out std_logic_vector (7 downto 0)" << endl;
    vhd_file << "    );" << endl;
    vhd_file << "end entity accel_rom;" << endl;
    vhd_file << endl;
    vhd_file << "architecture RTL of accel_rom is" << endl;
    vhd_file << endl;
    vhd_file << "type ram_type is array (0 to (2**addr'length)-1) of std_logic_vector (dataout'range);" << endl;
    vhd_file << "signal ram : ram_type := (" << endl;

    for (auto const &MB : MBs) {
        vector<Instruction> Instrs = MB.getInstructions();
        for (size_t i = 0; i < Instrs.size(); i ++) {
            Instruction Instr_left  = Instrs[i];
            Instruction Instr_right;
            pair <Instruction, Instruction> instr_pair;
            if (i < Instrs.size() - 1) {  // not last instruction
                Instr_right = Instrs[i + 1];
                int res = Instr_left.getOpcode().compare(target_pair.first.getOpcode());
                if (res == 0) {
                    // first instruction matches
                    int res2 = Instr_right.getOpcode().compare(target_pair.second.getOpcode());
                    if (res2 == 0) {
                        // second instruction matches, therefore this is a pair and its encoding must be printed
                        uint32_t op = (0x00FF & Instr_left.getEncoding());
                        vhd_file << " \tx\"" << setw(2) << setfill('0') << hex << op << "\"," <<  endl;
                        instr_pair = make_pair (Instr_left, Instr_right);
                        opcodes.push_back(instr_pair);
                    }
                }
            }
        }
    }

    // Just fill the last memory element with 0x00
    vhd_file << "\tx\"00\"" << endl;
    vhd_file << ");" << endl;
    vhd_file << "signal read_addr : std_logic_vector(addr'range);" << endl;
    vhd_file << endl;
    vhd_file << "begin" << endl;
    vhd_file << endl;
    vhd_file << "    RamProc: process(clk) is begin" << endl;
    vhd_file << "        if rising_edge(clk) then" << endl;
    vhd_file << "            read_addr <= addr;" << endl;
    vhd_file << "        end if;" << endl;
    vhd_file << "    end process RamProc;" << endl;
    vhd_file << endl;
    vhd_file << "    dataout <= ram(to_integer(unsigned(read_addr)));" << endl;
    vhd_file << endl;
    vhd_file << "end architecture RTL;" << endl;
    vhd_file << "-- Miniature Accelerator Memory VHDL File ends here" << endl;

    vhd_file.close();

    return opcodes;
}

// Goes through every MB of a given function and then all possible pairs that are present in each MB.
//      It then add pairs to m_instr_pair_tracker
uint OpcodeAnalyzer::mineFunction (Function &func) {
    cout << "Mining " << func.getName() << endl;

 	uint MB_no = 0;
 	// Go through each MB
	for (auto &MB: func.getMBs()) {
        cout << "\t" << "Mining MB #" << dec << MB_no << endl;

        vector<Instruction> instructions = MB.getInstructions();

        // First make initial pairs using this formula (n, n+1) is a pair
        for (vector<Instruction>::iterator it = instructions.begin(); it !=  instructions.end(); ++it) {
            pair <Instruction, Instruction> mypair;
            if (it + 1 == instructions.end()) {
                Instruction Inst(0, 0, "NONE", '0');
                mypair = make_pair(*it, Inst);
            } else
                mypair = make_pair(*it, *(it + 1));
            m_instr_pair_tracker.addInstrPair(mypair);
        }
        MB_no ++;
	}

    cout << "Mining finished." << endl;
	return 0;
}

void OpcodeAnalyzer::genMainProgramCoeFile () const {
    int total_bytes_written = 0;

    ofstream coe_file;
    coe_file.open ("program.coe");
    coe_file << "memory_initialization_radix=16;" << endl;
    coe_file << "memory_initialization_vector=";
    // Emit Vector Table:
    // 1st 4 bytes sets SP main
    // 2nd one is reset handler
    // 11th one is SVCall
    coe_file <<  "20001F00 00000195 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000054 00000000 00000000 00000000 00000000 ";

    total_bytes_written += 64; // Vector table is 64 bytes.

    // Reset opcodes.dat file
    m_opcodes_file.clear();
	m_opcodes_file.seekg(0, ios::beg);

	string line;
    uint line_number = 1;

    // bytes_available stores the number of bytes available currently for the current line
    // If we read a 2 bytes instruction then we subtract bytes_available by 2 which then
    // tells us the bytes_available is 2.
    // After that if we read another 2 bytes instruction we subtract bytes_available by 2
    // which gives us 0. A bytes_available = 0 indicates that we must insert a space and then
    // reset bytes_available tp 4.
    // If bytes_available =2 and we read a 4 bytes instruction then we subtract it by 4 and
    // we get -2 which indicates we can write 2 bytes of current instruction now, then insert
    // a space and write the other 2 bytes,
    // Rules:
    int bytes_available = 4;

	// iterate through the lines and emit the hex bytes
	while (getline (m_opcodes_file, line)) {
		// skip the first 6 lines.
		if (line_number < 17) {
			line_number ++;
			continue;
		}

		stringstream line_ss(line);
		string token;
		vector<string> tokens;
		while (line_ss >> token) {
			tokens.push_back(token);
		}

		// be sure the line is not empty or function or data start line
        if (tokens.capacity() != 2 && tokens.capacity() != 0) {
            int instr_size;
            string line_till_tab = line.substr(0, line.find('\t'));
            istringstream line_till_tab_ss (line_till_tab);
            uint words = distance(istream_iterator<string>{line_till_tab_ss}, istream_iterator<string>{});
            // insert a space after every 4 bytes
            if (words == 5 || words == 1)
                instr_size = 4;
            else
                instr_size = 2;

            total_bytes_written += instr_size;

            if (bytes_available == 4) {
                if (instr_size == 2) {
                    coe_file << tokens[2] + tokens[1];
                    bytes_available = 2;
                }
                else {
                    // instr_size == 4
                    coe_file <<  tokens[2] + tokens[1] << tokens[4] + tokens[3] << " " ;
                    bytes_available = 4;
                }
            }
            else if (bytes_available == 2) {
                if (instr_size == 2) {
                    coe_file << tokens[2] + tokens[1] << " ";
                    bytes_available = 4;
                }
                else {
                    // instr_size == 4
                    coe_file  << tokens[2] + tokens[1] + " " + tokens[4] + tokens[3];
                    bytes_available = 2;
                }
            }
        }
	}

	// Reset seek pointer
    m_exidx_file.clear();
	m_exidx_file.seekg(0, ios::beg);
	line_number = 1;
	// iterate through the lines and emit the hex bytes
	while (getline (m_exidx_file, line)) {
		// skip the first 6 lines.
		if (line_number < 5) {
			line_number ++;
			continue;
		}

		stringstream line_ss(line);
		string token;
		vector<string> tokens;
		while (line_ss >> token) {
			tokens.push_back(token);
		}
        coe_file << tokens[1] << " " << tokens[2] << " " << tokens[3] << " " << tokens[4] << " ";
    }

    total_bytes_written += 16; // 16 bytes in .ARM.exidx section

    // After writing .text and .ARM.exidx section we have to fill the memory with zeros until memory address 0x20000 = 131072
    int no_of_words_to_be_filled = (131072 - total_bytes_written) / 4;
    for (int i = 0; i < no_of_words_to_be_filled ; i++) {
        coe_file << "00000000 ";  // 4 bytes filled with 0
    }

    // Output .rodata section
    m_rodata_file.clear();
	m_rodata_file.seekg(0, ios::beg);
	line_number = 1;
	// iterate through the lines and emit the hex bytes
	while (getline (m_rodata_file, line)) {
		// skip the first 4 lines.
		if (line_number < 5) {
			line_number ++;
			continue;
		}

		stringstream line_ss(line);
		string token;
		vector<string> tokens;
		while (line_ss >> token) {
			tokens.push_back(token);
		}
		// Tokens with size 6 represent a full line like this:
		// 20000 fb21f93f fb210940 7cd91240 fb211940  .!.?.!.@|..@.!.@
		if (tokens.size() >= 6) {
            coe_file << tokens[1] << " " << tokens[2] << " " << tokens[3] << " " << tokens[4] << " ";
        }
        else {
            // tokens.size() is less than 6, it can be 3, 4 or 5
            switch (tokens.size()) {
                case 3: coe_file << tokens[1] << " ";  break;
                case 4: coe_file << tokens[1] << " " << tokens[2] << " ";  break;
                case 5: coe_file << tokens[1] << " " << tokens[2] << " " << tokens[3] << " ";  break;
                default: throw "Unexpected situation.";
            }
        }
    }

    coe_file << ";";
    coe_file.close();

    cout << "total_bytes_written = " << total_bytes_written << endl;
}

void OpcodeAnalyzer::getRC_PC_sensivityFile (const vector <uint32_t>  PCSensivityList) const {
    ofstream pc_sensicity_vhd_file;
    pc_sensicity_vhd_file.open ("RC_PC_sensivity.vhd");
    pc_sensicity_vhd_file << "library IEEE;" << endl;
    pc_sensicity_vhd_file << "use IEEE.STD_LOGIC_1164.ALL;" << endl << endl;


    pc_sensicity_vhd_file << "entity m0_RC_PC_sensivity is" << endl;
    pc_sensicity_vhd_file << "Port (" << endl;
    pc_sensicity_vhd_file << "    HADDR : in std_logic_vector(31 downto 0);" << endl;
    pc_sensicity_vhd_file << "    invoke_accelerator : out std_logic" << endl;
    pc_sensicity_vhd_file << " );" << endl;
    pc_sensicity_vhd_file << "end m0_RC_PC_sensivity;" << endl << endl;;

    pc_sensicity_vhd_file << "architecture Behavioral of m0_RC_PC_sensivity is" << endl << endl;

    pc_sensicity_vhd_file << "begin" << endl;

    pc_sensicity_vhd_file << "invoke_accelerator_p: process(HADDR) begin" << endl;
    pc_sensicity_vhd_file << "    case HADDR is" << endl;
    pc_sensicity_vhd_file << "      when " << endl;


    for (vector <uint32_t>::const_iterator it = PCSensivityList.begin();
         it != PCSensivityList.end(); it++) {
         if (next(it) == PCSensivityList.end())
            pc_sensicity_vhd_file << "           x\"" << setw(8) << setfill('0') <<  hex << *it << "\" " << endl;
         else
            pc_sensicity_vhd_file << "           x\"" << setw(8) << setfill('0') <<  hex << *it << "\" |" << endl;
    }

    pc_sensicity_vhd_file << "                            =>  invoke_accelerator <= '1';" << endl;
    pc_sensicity_vhd_file << "        when others         =>  invoke_accelerator <= '0';" << endl;
    pc_sensicity_vhd_file << "    end case;" << endl;
    pc_sensicity_vhd_file << "end process;" << endl;
    pc_sensicity_vhd_file << "end Behavioral;" << endl;

    pc_sensicity_vhd_file.close ();
}

void OpcodeAnalyzer::getPC_to_OP_File (const vector <uint32_t>  PCSensivityList, const vector <pair <Instruction, Instruction> >  inst_pairs) const {
    ofstream pc_op_vhd_file;
    pc_op_vhd_file.open ("PC_OP.vhd");
    pc_op_vhd_file << "library IEEE;" << endl;
    pc_op_vhd_file << "use IEEE.STD_LOGIC_1164.ALL;" << endl;
    pc_op_vhd_file << "entity m0_PC_OP is" << endl;
    pc_op_vhd_file << "Port (" << endl;
    pc_op_vhd_file << "    PC     : in  std_logic_vector(31 downto 0);" << endl;
    // Add some comments
    pc_op_vhd_file << "    -- Operand: 8-bit + 25-bit = 33-bit" << endl;
    pc_op_vhd_file << "    -- 8-bit is the mov operand, 25-bit is the BL target address operand" << endl;
    pc_op_vhd_file << "    -- [32-31-30-29 - 28-27-26-25]" << endl;
    // Operands of a 16-bit instruction with 5-bit opcode can be maximum 11-bit
    pc_op_vhd_file << "    operand : out std_logic_vector(32 downto 0)" << endl;
    pc_op_vhd_file << " );" << endl;
    pc_op_vhd_file << "end m0_PC_OP;" << endl << endl;;

    pc_op_vhd_file << "architecture Behavioral of m0_PC_OP is" << endl << endl;

    pc_op_vhd_file << "begin" << endl;
    pc_op_vhd_file << "PC_to_OP_p: process(PC) begin" << endl;
    pc_op_vhd_file << "  case PC is" << endl;

    uint32_t i = 0;
     for (vector <uint32_t>::const_iterator it = PCSensivityList.begin();
         it != PCSensivityList.end(); it++) {
            pair <Instruction, Instruction> instr_pair = inst_pairs[i];
            Instruction left_instr = instr_pair.first;
            Instruction right_instr = instr_pair.second;
            uint32_t op1 = (0x00FF & left_instr.getEncoding());
            uint32_t op2_original = right_instr.getEncoding();
            uint32_t S = (op2_original >> 26) & 1;
            uint32_t imm10 = (op2_original >> 16) & 0x03FF;
            uint32_t imm11 = op2_original & 0x07FF;
            uint32_t J1 = (op2_original >> 13) & 1;
            uint32_t J2 = (op2_original >> 11) & 1;

            uint32_t I1 = (~ (J1 ^ S)) & 1;
            uint32_t I2 = (~ (J2 ^ S)) & 1;

            // 00 00 0000 0000 0000 0000 000 0
            //    imm10        imm11
            uint32_t op2 =  (S << 24) | (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
            cout << "op = " << hex << op2_original << "\t op2 = " << op2 << endl;


         if (next(it) == PCSensivityList.end()) {
            pc_op_vhd_file << "      when x\"" << setw(8) << setfill('0') <<  hex << *it << "\" => operand <= x\"" <<
              setw(2) << setfill('0') << hex << op1 << "\" & B\"";
            print_binary (op2, pc_op_vhd_file);
            pc_op_vhd_file << "\";" << endl;
         }
         else {
            pc_op_vhd_file << "      when x\"" << setw(8) << setfill('0') <<  hex << *it << "\" => operand <= x\"" <<
              setw(2) << setfill('0') << hex << op1 << "\" & B\"";
            print_binary (op2, pc_op_vhd_file);
            pc_op_vhd_file << "\";" << endl;
         }
         i++;
    }

    pc_op_vhd_file << "    when others =>  operand <= (others => '0');" << endl;
    pc_op_vhd_file << "  end case;" << endl;
    pc_op_vhd_file << "end process;" << endl;
    pc_op_vhd_file << "end Behavioral;" << endl;

    pc_op_vhd_file.close();
}


} // end of namespace esi
