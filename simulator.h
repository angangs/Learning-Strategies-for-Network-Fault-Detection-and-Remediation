#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include "csfr.h"
#include "createSizes.h"
#include "topology.h"
#include "precsfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>

using namespace std;

extern int fault_node;
extern int total_episodes;

void createTree(struct array2tree arr[],struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);							
void connectionSim(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int depth,int *sel_str);
void myInitialize(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void insertFault(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[], int select_op,vector<int>&selF);
int search(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int searchSpec(int c1,int c2,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
int checkFine(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void insertCustomizeFault(int fault_node,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void uninsertCustomizeFault(int fault_node,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void putInArray(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],struct array ar[]);
void returnArray(struct array ar[],int node,int count);
void printPath(struct array *nd, int x1, int x, list<int> *pathlist, int print_operation);
#endif
