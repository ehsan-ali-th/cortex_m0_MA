//============================================================================
// Name        : trace_trimmer.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================



#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>


using namespace std;


int main(int argc, char *argv[]) {
    cout  << "Hi !" << endl;

	if (argc < 2) {
		cout << "Usage: trace_trimmer trace_file.txt" << endl;
		return -1;
	}

	string input_filename = argv[1];
	cout << "- Analyzing : " << input_filename << endl;

	// Extract the filename by removing the extension
	size_t lastindex = input_filename.find_last_of(".");
	if (lastindex == string::npos) {
		cout << "No extension in filename is specified. " <<
	    "Input filename must have .txt extension" << endl;
	    return -2;
	}

	string filename = input_filename.substr(0, lastindex);
	string trace_filename = filename + ".trc";
	cout << "- Trace Output file: " << trace_filename << endl;

	// Open the trace .txt input file
	ifstream file_in (input_filename.c_str(), ios::in);

	string line;

	ofstream trace_output_file;
    trace_output_file.open (trace_filename);

    string delimiter = ".";

	// iterate through the lines and add the opcodes
	while (getline (file_in, line)) {
		stringstream line_ss(line);
		string token;
		vector<string> tokens;
		while (line_ss >> token) {
			tokens.push_back(token);
		}

		if (tokens.size() >= 4) {
            if (tokens[0] == "e") {
                // remove anythign after .
                string token_3 = tokens[3].substr(0, tokens[3].find(delimiter));
                // make sure tokens[3] is 5 characters
                token_3.resize(5, ' ');   // resize to 5 characters and fill the added ones with space.

                trace_output_file << token_3 << endl;
            }
            else {
                // remove anythign after .
                string token_2 = tokens[2].substr(0, tokens[2].find(delimiter));
                token_2.resize(5, ' ');   // resize to 5 characters and fill the added ones with space.
                trace_output_file << token_2 << endl;
            }

		}
    }




	cout << "End of Program." << endl;

	// close open files
	file_in.close();
	trace_output_file.close();

	return 0;
}


