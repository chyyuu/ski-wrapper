#include "sympp.h"
#include <fstream>
#include <sstream>

using namespace std;

ksyms load_syms(const char* _sysmap){
	ksyms ret;
	ifstream sysmap(_sysmap);
	string line;
	while(getline(sysmap, line)){
		stringstream ss(line);
		uint32_t addr;
		char type;
		ss >> hex >> addr >> type;
		ss >> ret[addr];
	}
	return ret;
}
