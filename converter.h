#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "config.h"
#include "ski-race.h"
#include "sympp.h"
#include <set>

void race2trace(const std::list<ski_rd_race>& races, const ksyms& sym, std::set<calltrace>& read, std::set<calltrace>& write);

#endif
