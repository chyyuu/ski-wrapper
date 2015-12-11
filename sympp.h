#ifndef SYMPP_H_
#define SYMPP_H_

#include "config.h"

#include <list>
#include <map>
#include <string>
#include <cstdint>

typedef std::map<target_ulong, std::string> ksyms;
typedef std::map<std::string, target_ulong> krsyms;
typedef std::pair<std::string, target_ulong> callsite;
typedef std::list<callsite> calltrace;

ksyms load_syms(const char* sysmap);
krsyms load_revsyms(const char* sysmap);
calltrace convert_addr(const std::list<target_ulong>& trace, const ksyms& sym);
void print_map(const ksyms& sym, std::ostream& os);

#endif /* SYMPP_H_ */
