#ifndef _TOPOLOGY_H
#define _TOPOLOGY_H

#include "createSizes.h"
#include "simulator.h"
#include "csfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <string>

void mytopology(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void myAdditionalTopology(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void myPrint (struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void readMyTopology(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void ParsingFun(std::string line,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int returnNextPosition(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int type);
void readPopulation(std::string line);
void myPrint(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
bool is_ws(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
bool is_sw(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
bool is_srv(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
bool is_hb(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int pos_ws(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int pos_sw(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int pos_srv(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int pos_hb(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
#endif
