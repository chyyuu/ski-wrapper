#include "config.h"
#include "sympp.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <pstreams/pstream.h>

#include <boost/format.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

#define ADDR2LINE_FORMAT "addr2line -e \"%s\" 0x" TARGET_ULONG_FMT

typedef list<pair<string, unsigned long>> srclines;

srclines addr2line(const char* exe, const char* prefix, target_ulong addr){
    srclines ret;
    int buf_num = snprintf(NULL, 0, ADDR2LINE_FORMAT, exe, addr);
    char cmdline[buf_num + 1];
    sprintf(cmdline, ADDR2LINE_FORMAT, exe, addr);
    redi::ipstream in(cmdline);
    string line;
    while(getline(in, line)){
        auto sep = line.rfind(':');
        assert(sep != string::npos);
        string filename = line.substr(0, sep);
        unsigned long number = strtoul(line.c_str() + sep + 1, nullptr, 0);
        assert(filename.size());
        if(filename[0] == '/'){
            assert(!strncmp(filename.c_str(), prefix, strlen(prefix)));
            filename.erase(0, strlen(prefix));
        }
        ret.push_back(make_pair(move(filename), number));
    }
    return ret;
}

int main(int, char** argv){
    auto sym = load_revsyms(argv[1]);

    set<calltrace> trace;
    ifstream trace_xml(argv[2]);
    boost::archive::xml_iarchive arch_trace(trace_xml);
    arch_trace >> BOOST_SERIALIZATION_NVP(trace);

    int max_len = snprintf(nullptr, 0, "%zu", trace.size());
    size_t i = 0;
    for(auto& t : trace){
        srclines lines;
        char filename[max_len + 1];
        sprintf(filename, "%0*zu", max_len, i++);
        for(auto& c : t){
            if(sym.find(c.first) == sym.end()){
                cerr << "[" << c.first << "] not found" << endl;
            }
            lines.splice(lines.end(), addr2line(argv[3], argv[4], sym[c.first] + c.second));
        }
        ofstream of(filename);
        for(auto& l : lines){
            of << l.first << ":" << l.second << endl;
        }
    }
    return 0;
}