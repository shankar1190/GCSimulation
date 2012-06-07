/*
 * sc-graph-api.cc
 *
 *  Created on: 12-Apr-2012
 *      Author: shankar
                radhika
                neeraja
  
  Stop-copy is one of the most popular garbage collection algorithms.
  
  It works as follows:
  The heap is divided into two parts, the active part and the inactive
  part. All the memory allocation takes part in the active part. When
  it gets full, the Garbage Collection is triggered. In this case, it
  runs a Depth First Search from all the roots to all the objects
  reachable by the roots. All these objects are copied from the active
  to the inactive heap. Then the active and inactive heaps swap names.
  Ie, the other heap becomes the active heap now. The objects that weren't
  copied are overwritten next time Garbage collection runs.

  The overhead of copying too much is a drawback. But it has the potential
  to free up a lot of memory with little overhead when there are many
  short-lived objects.
 */

#include "sc-graph-api.h"

#ifndef SCHEAPSIZE
#define SCHEAPSIZE 50
#endif

namespace sc_graph_api {

// default constructor
// initializes the state and heap size
SCGraphUtil :: SCGraphUtil() {
	state = 0;
	max_objects = SCHEAPSIZE;
}

// Lets the user specify heap-size
SCGraphUtil :: SCGraphUtil(int heap_size) {
	state = 0;
	max_objects = heap_size;
}

void SCGraphUtil :: TriggerGC() {
	for(int i = 0; i < (int)roots.size(); i++) {
		(state == 0) ? DFSCopy(roots[i], h1) : DFSCopy(roots[i], h0);
	}
	(state == 0) ? Flush(h0) : Flush(h1);
	state = !state;
}

void SCGraphUtil :: DFSCopy(Object* root, vector<Object*> &to) {
	if (root == NULL) return;

	to.push_back(root);
	DFSCopy(root->child, to);
}

// All the elements of the heap are dropped, the destructors of the objects are
// called and we retain the heap (vector) of size 0. Hence freeing the heap.
void SCGraphUtil :: Flush(vector <Object*>& v) {
	v.clear();
}


// Displays the total memory used and the free memory.
void SCGraphUtil :: ShowMemoryUsage() {
	cout << "Used Memory: " << ((state == 0) ? h0.size() : h1.size()) << endl;
	cout << "Free Memory: " << max_objects - ((state == 0) ? h0.size() : h1.size()) << endl << "------------------\n";
}


// Creates a new reference object. Object* obj = new Object();. Essentially,
// creates an object in the heap and a reference in the roots vector.
void SCGraphUtil :: NewReference(const string& desc) {
    int num_objects = (int)((state == 0) ? h0.size() : h1.size());

    // if the heap is full, then we call TriggerGC() to free up some space.
    if (num_objects == max_objects) {
        TriggerGC();
    }
    
    num_objects = (int)((state == 0) ? h0.size() : h1.size());
    
    if (num_objects == 0) {
        num_objects++;
        Object* obj = new Object(desc);
        roots.push_back(obj);
        (state == 0) ? h0.push_back(obj) : h1.push_back(obj);
     } else if (num_objects < max_objects) {
  	    num_objects++;
	    Object* obj = new Object(desc);
	    roots.push_back(obj);
	    (state == 0) ? h0.push_back(obj) : h1.push_back(obj);
	 } else {
	    cout << "Error! Unable to allocate memory!\n";
    }
}

// Creating a new reference and making it point to an existing object
// Object *obj = x;
void SCGraphUtil :: NewReference(Object* obj) {
	roots.push_back(obj);
}

// New object that would be pointed to by an existing pointer.
// This includes objects that are reffered to by other objects,
// a self-referencing object, like a binary tree structure or a
// linked-list.
Object* SCGraphUtil :: New(const string& desc, Object* parent) {
    Object* obj = NULL;
    
    int num_objects = (int)((state == 0) ? h0.size() : h1.size());
	if (num_objects == max_objects) {
		TriggerGC();
    }
    
    num_objects = (int)((state == 0) ? h0.size() : h1.size());
    
    if (num_objects == 0) {
    	num_objects++;
    	obj = new Object(desc);
    	(state == 0) ? h0.push_back(obj) : h1.push_back(obj);
    	parent->child = obj;
    } else if (num_objects < max_objects) {
	num_objects++;
	obj = new Object(desc);
	(state == 0) ? h0.push_back(obj) : h1.push_back(obj);
	parent->child = obj;
    } else {
        cout << "Error! Unable to allocate memory!\n";
    }
    
    return obj;
}

// Getting rid of the root reference. This is typically when a pointer
// falls out of scope causing a memory leak.
void SCGraphUtil :: EndLifetime(Object* obj) {
    int pos = -1;
    for (int i = 0; i < int(roots.size()); i++) {
        if (roots[i] == obj) {
            pos = i;
            break;
        }
    }
    if (pos != -1) roots.erase(roots.begin() + pos);
}

} // sc_graph_api
