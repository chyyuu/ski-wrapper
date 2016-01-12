#include "sympp.h"
#include <fstream>
#include <sstream>

#include <boost/format.hpp>

using namespace std;

ksyms load_syms(const char* _sysmap) {
	ksyms ret;
	ifstream sysmap(_sysmap);
	string line;
	while (getline(sysmap, line)) {
		stringstream ss(line);
		target_ulong addr;
		char type;
		ss >> hex >> addr >> type;
		ss >> ret[addr];
	}
	return ret;
}
krsyms load_revsyms(const char* _sysmap){
	krsyms ret;
	ifstream sysmap(_sysmap);
	string line;
	while (getline(sysmap, line)) {
		stringstream ss(line);
		target_ulong addr;
		char type;
		string sym;
		ss >> hex >> addr >> type >> sym;
		ret[sym] = addr;
	}
	return ret;
}

calltrace convert_addr(const list<target_ulong>& trace, const ksyms& sym) {
	calltrace ret;
	for (auto& raddr : trace) {
		if(!raddr){
			continue;
		}
		auto isym = sym.upper_bound(raddr);
		if(isym == sym.begin()){
			cerr << boost::format("address " TARGET_ULONG_FMT " not found!!") % raddr << endl;
			continue;
		}
		--isym;
		ret.push_back(make_pair(isym->second, raddr - isym->first));
	}
	return ret;
}

void print_map(const ksyms& sym, ostream& os){
	for(auto& e : sym){
		os << boost::format("%s " TARGET_ULONG_FMT) % e.second.c_str() % e.first << endl;
	}
}
