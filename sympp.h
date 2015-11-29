#ifndef SYMPP_H_
#define SYMPP_H_

#include <list>
#include <map>
#include <string>
#include <cstdint>

typedef std::map<uint32_t, std::string> ksyms;
ksyms load_syms(const char* _sysmap);

#endif /* SYMPP_H_ */
