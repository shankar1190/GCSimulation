/*
 * ms-graph-api.h
 *
 *  Created on: 08-Apr-2012
 *      Author: shankar
		radhika
		neeraja
 */



#ifndef MSGRAPHAPI_H_
#define MSGRAPHAPI_H_

#include <string>
#include <vector>
#include <cctype>
#include <iostream>

// Utility Macros.
#define CHECK(x) if(!(x)){cerr<<"Check not satisfied! Aborting...\n";exit(1);}

using namespace std;

namespace ms_graph_api {

class Object {
  public:
    bool seen;
    Object* next;
    Object* child;
    string desc;
    Object() {
        next = NULL;
        child = NULL;
        desc = "";
    }
    Object (const string& description) {
    	next = NULL;
    	child = NULL;
    	desc = description;
    }
};

class MSGraphUtil {
  public:
    
    // constructors
    MSGraphUtil();
    MSGraphUtil(int heap_size);
    
    // utility data members
    int num_objects;
    int max_objects;
    
    // container for root items
    vector <Object*> roots;
    
    // pointers for scanning the heap
    Object* first;
    Object* last;

    // utility functions
    void DFSMark(Object* root);
    void Sweep(Object* current, Object* prev);
    void TriggerGC();
    void ShowMemoryUsage();
    
    // Object allocation and reference lifetime
    Object* New(const string& desc, Object* parent);
    void NewReference(const string& desc);
    void NewReference(Object* obj);
    void OldReference(Object* obj1, Object* obj2);
    void EndLifetime(Object* reference);
};

} // ms_graph_api
#endif /* MSGRAPHAPI_H_ */
