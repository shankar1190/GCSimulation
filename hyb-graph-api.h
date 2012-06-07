/*
 * hyb-graph-api.h
 *
 *  Created on: 20-Apr-2012
 *      Author: shankar
                radhika
                neeraja
 */

#ifndef HYBGRAPHAPI_H_
#define HYBGRAPHAPI_H_

#include <iostream>
#include <vector>
#include <string>
#include "sc-graph-api.h"
#include "ms-graph-api.h"

using namespace std;

namespace hyb_graph_api {

class Object {
  public:
    bool seen;
    int age;
    Object* next;
    Object* child;
    string desc;
    Object() {
    	age = 0;
        next = NULL;
        child = NULL;
        desc = "";
    }
    Object (const string& description) {
    	age = 0;
    	next = NULL;
    	child = NULL;
    	desc = description;
    }
};

class HybGraphUtil {
  public:
    
    // constructors
    HybGraphUtil();
    HybGraphUtil(int ms_heap, int sc_heap, int thres);
    
    // utility data members
    int num_objects;
    int threshold;
    int ms_max_objects;
    
    // container for root objects
    vector <Object*> ms_roots;
    
    // pointers for scanning the heap
    Object* first;
    Object* last;
  	
    // utility functions for the mark-sweep component
    void DFSMark(Object* root);
  	void Sweep(Object* current, Object* prev);
   	void MSTriggerGC();
   	void MSShowMemoryUsage();
   	Object* MSNew(const string& desc, Object* parent);
   	void MSNewReference(const string& desc);
   	void MSNewReference(Object* obj);
   	void MSOldReference(Object* obj1, Object* obj2);
   	void MSEndLifetime(Object* reference);

   	
    // Utility data members for the stop-copy component
    bool state;
    int sc_max_objects;

    // container for the root objects
    vector <Object*> sc_roots;
    
    // the two heaps in the stop-copy component, the active and inactive
    // heap
    vector <Object*> h[2];
	  
    // Utility functions for the stop-copy component
    void DFSCopy(Object* root, vector<Object*> &to);
    void Flush(vector<Object*> &v);
    void SCShowMemoryUsage();
    void SCEndLifetime(Object* reference);
    void SCTriggerGC();


    // Integrative utility functions for the hybrid algorithm
    void DFSShift(Object *root);
    void ShowMemoryUsage();
    Object* New(const string& desc, Object* parent);
    void NewReference(const string& desc);
    void NewReference(Object* obj);
    void EndLifetime(Object* reference);
    void TriggerGC();
};

} // hyb_graph_api

#endif /* HYBGRAPHAPI_H_ */
