/*
 * main.cc
 *
 *  Created on: 08-Apr-2012
 *      Author: shankar
 *
 */

#include "ms-graph-api.h"
#include "sc-graph-api.h"
#include "hyb-graph-api.h"


#define CANCELMSTEST // comment if you want to run MS test
#define CANCELSCTEST // comment if you want to run SC test
//#define CANCELHYBTEST // comment if you want to run hybrid test


using namespace std;

int main() {

#ifndef CANCELMSTEST
#define CANCELMSTEST

	ms_graph_api::MSGraphUtil msgc;
	for (int i = 0; i < 25; i++) {
		msgc.NewReference("root");
	}
	cout << "\nMark and Sweep.\n";
	msgc.ShowMemoryUsage();

	for (int i = 0; i < 25; i++) {
		ms_graph_api::Object* obj1 = msgc.New("", msgc.roots[i]);
		ms_graph_api::Object* obj2 = msgc.New("", obj1);
		msgc.New("", obj2);
	}

	msgc.ShowMemoryUsage();

	for (int i = 0; i < 25; i++) {
		msgc.EndLifetime(msgc.roots[0]);
		msgc.TriggerGC();
		msgc.ShowMemoryUsage();
	}

#endif

#ifndef CANCELSCTEST
#define CANCELSCTEST

	  using namespace sc_graph_api;
    SCGraphUtil scgc(100);
		for (int i = 0; i < 25; i++) {
			scgc.NewReference("root");
		}
		cout << "\nStop-Copy.\n";
		scgc.ShowMemoryUsage();

		for (int i = 0; i < 25; i++) {
			sc_graph_api::Object* obj1 = scgc.New("", scgc.roots[i]);
			sc_graph_api::Object* obj2 = scgc.New("", obj1);
			scgc.New("", obj2);
		}

		scgc.ShowMemoryUsage();

		for (int i = 0; i < 25; i++) {
			scgc.EndLifetime(scgc.roots[0]);
			scgc.TriggerGC();
			scgc.ShowMemoryUsage();
		}

#endif

#ifndef CANCELHYBTEST
#define CANCELHYBTEST
	
  using namespace hyb_graph_api;
	HybGraphUtil gc;
	for (int i = 0; i < 25; i++) {
		gc.NewReference("root");
	}
	cout << "\nHybrid.\n";
	gc.ShowMemoryUsage();
  cout << "MarkSweep heap:\n";
  gc.MSShowMemoryUsage();
  cout << "StopCopy heap:\n";
  gc.SCShowMemoryUsage();
  cout << endl;
	for (int i = 0; i < 25; i++) {
		Object* obj1 = gc.New("", gc.sc_roots[i]);
		Object* obj2 = gc.New("", obj1);
		gc.New("", obj2);
	}

  gc.ShowMemoryUsage();
  cout << "MarkSweep heap:\n";
  gc.MSShowMemoryUsage();
  cout << "StopCopy heap:\n";
  gc.SCShowMemoryUsage();
  cout << endl;
  for (int i = 0; i < 25; i++) {
    if (gc.sc_roots.size() > 0) gc.sc_roots.pop_back(); else gc.ms_roots.pop_back();
    gc.TriggerGC();
    gc.ShowMemoryUsage();
    cout << "MarkSweep heap:\n";
    gc.MSShowMemoryUsage();
    cout << "StopCopy heap:\n";
    gc.SCShowMemoryUsage();
    cout << endl;
  }
#endif

  return 0;
}
