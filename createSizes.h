#ifndef _CREATESIZES_H
#define _CREATESIZES_H

#include "simulator.h"
#include "csfr.h"
#include "topology.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <string>

extern int total;
extern int ws_id;
extern int sw_id;
extern int hb_id;
extern int srv_id;
extern int ws_lnk;
extern int sw_lnk;
extern int hb_lnk;
extern int srv_lnk;
extern int arr_lnk;
extern int test_actions;
extern int total_actions;
extern int max_ws_lnk;
extern int max_sw_lnk;
extern int max_hb_lnk;
extern int max_srv_lnk;

void findSize();
void readPopulation(std::string line);
void make_fault_state_array();
void make_prior_prob_array();
void make_observ_array();
void make_cost_array();
void make_actions_array();
#endif
