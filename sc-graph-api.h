/*
 * sc-graph-api.h
 *
 *  Created on: 10-Apr-2012
 *      Author: shankar
                radhika
                neeraja
 */

#ifndef SCGRAPHAPI_H_
#define SCGRAPHAPI_H_

#include <iostream>
#include <vector>

using namespace std;

namespace sc_graph_api {

class Object {
public:
  
  Object* next;
  Object* child;
  string desc;
  
  Object() {
	  child = NULL;
      desc = "";
  }

  Object (const string& description) {
  	child = NULL;
  	desc = description;
  }

};

class SCGraphUtil {
  public:
    
    // constructors
    SCGraphUtil();
    SCGraphUtil(int heap_size);
    
    // utility data members:
    // state explains which is the active and inactive heap
    // max_objects is the heap size.
    bool state;
    int max_objects;

    // List of all the roots
    vector <Object*> roots;

    // The two heaps, one which is the active component and the
    // other is the inactive component.
    vector <Object*> h0;
    vector <Object*> h1;

    // Utility functions
	  void DFSCopy(Object* root, vector<Object*> &to);
	  void Flush(vector<Object*> &v);
	  void TriggerGC();
	  void ShowMemoryUsage();

    // Functions dealing with memory allocation and references falling
    // out of scope
  	Object* New(const string& desc, Object* parent);
	  void NewReference(const string& desc);
	  void NewReference(Object* obj);
	  void EndLifetime(Object* reference);

};

} // sc_graph_api

#endif /* SCGRAPHAPI_H_ */
