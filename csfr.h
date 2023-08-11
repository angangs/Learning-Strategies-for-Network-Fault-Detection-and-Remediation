#ifndef _CSFR_H
#define _CSFR_H

#include "createSizes.h"
#include "simulator.h"
#include "topology.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <vector>

#define TSIM		1000
#define fault_time	10

using namespace std;

struct array{
	int ID;
	std::vector<int> link;
	string color;
	int d;
	int p;
	int work;
	array();
};

struct workstation{
	int ID;
	std::vector<int> linkerID;
	int workFine;
	int visited;
	workstation();
};

struct server{
	int ID;
	std::vector<int> linkerID;
	int workFine;
	int visited;
	server();
};

struct hub{
	int ID;
	std::vector<int> linkerID;
	int workFine;
	int visited;
	hub();
};

struct switches{
	int ID;
	std::vector<int> linkerID;
	int workFine;
	int visited;
	switches();
};

//---------------------------------------------------				
extern int total_costM;
extern std::vector< std::vector<int> > Actions;
extern std::vector<int> FaultState;
extern std::vector<double> PriorProb;
extern std::vector<double> PriorProb_rl;
extern std::vector< std::vector<int> > Observ;
extern std::vector< std::vector<int> > Cost;
extern std::vector< std::vector<int> > Observ_rl;
extern std::vector< std::vector<int> > Cost_rl;
extern std::list <int>Act;
//---------------------------------------------------
int findDepth(struct array ar[],int num,int level);
void planning_manager(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy);
void rl_manager(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy,vector<double> &counter_episodes);
void training_rl_manager(int count_episodes,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
void computeObservAndCost(std::vector< std::vector<int> > &Observ,std::vector< std::vector<int> > &Cost,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]);
double getProb(int *);
double getProb_rl(int *);
double getCost(int *beliefstate,int rowAction);
double getCost_rl(int *beliefstate,int rowAction);
int checkUpdatebelief(int *beliefstate,int tot);
void defineSubbelief(int rowAction,int *beliefstate,int selection,int *b);
void defineSubbelief_rl(int rowAction,int *beliefstate,int selection,int *b);
void defineSubbelief_rlq(int rowAction,int *beliefstate,int selection,int *b);
void clearBelief(int *beliefstate,int tot);
double getVvalue(int *beliefstate,int depth, int *action,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy);
double getVvalue_rl(int *beliefstate,int depth, int *action,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy);

#endif
