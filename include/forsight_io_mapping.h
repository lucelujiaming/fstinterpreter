#ifndef FORSIGHT_IO_MAP_H
#define FORSIGHT_IO_MAP_H

#include "forsight_basint.h"

#define IO_NAMES   "ai;ao;di;do;ri;ro;si;so;ui;uo"
extern map<string, string> g_io_mapper;

int append_io_mapping();

#endif
