/*
 * hyb-graph-api.cc
 *
 *  Created on: 21-Apr-2012
 *      Author: shankar
                radhika
                neeraja

 The Hybrid Garbage Collection algorithm aims to balance trade-offs in stop-copy
 and the mark-sweep alsorithms and try to harness the benifits of both, trying
 to minimize the disadvantages. We consider both algorithms running on two segments
 of the heap memory.

 The copying collector runs on one segment, typically for 'new' objects, assumed
 that they are short-lived. As the objects age, they are assumed they are long-
 lived and they are moved to the other heap where mark and sweep collector runs.

 Since mark and sweep is better for long-lived objects and stop-copy is better
 for short-lived objects, we predict that this collector would indeed perform
 better for the average case, provided important bottlenecks are handled and the
 program is large enough to make the copying across heaps an insignificant investment.

 The latency does turn out to be lesser than the other two algorithms in the
 average case.

 This algorithm is of course a crude implementation, which can be improved by
 estimating the percentage of long-lived and short lived objects before hand, while
 parsing or writing an adaptive mechanism where it intelligently assigns the
 objects directly to their respective heaps.

 */

#include "hyb-graph-api.h"

#ifndef SCHEAPSIZE
#define SCHEAPSIZE 100
#endif

#ifndef MSHEAPSIZE
#define MSHEAPSIZE 100
#endif

#ifndef THRESHOLD
#define THRESHOLD 3
#endif


namespace hyb_graph_api {

// default constructor, initializing heap sizes of both the stop-copy and
// mark-sweep heaps. We also initialize "threshold", which is an age
// threshold after which objects will move from the stop-copy heap to
// the mark-sweep heap.
HybGraphUtil :: HybGraphUtil() {
	ms_max_objects = MSHEAPSIZE;
	sc_max_objects = SCHEAPSIZE;
	state = false;
	first = NULL;
	last = NULL;
	threshold = THRESHOLD;
	num_objects = 0;
}

// Lets the user define mark-sweep, stop-copy heap sizes and age
// threshold.
HybGraphUtil :: HybGraphUtil(int ms_heap, int sc_heap, int thres) {
	ms_max_objects = ms_heap;
	sc_max_objects = sc_heap;
	threshold = thres;
	state = false;
	first = NULL;
	last = NULL;
	num_objects = 0;
}

// Function to start the Garbage Collection process. Sometimes used to
// force garbage collection, but it is usually called when the heap is
// full and it has to be freed up.
void HybGraphUtil :: TriggerGC() {
	if (int(h[state].size()) > 0) 
    SCTriggerGC();
	else 
    MSTriggerGC();
}


// Function to start the Garbage Collection process in the stop-copy
// heap. It is called by the TriggerGC() function. 
void HybGraphUtil :: SCTriggerGC() {
	for(int i = 0; i < (int)sc_roots.size(); i++) {
		
    sc_roots[i]->age++;
		if (sc_roots[i]->age > threshold) {
			DFSShift(sc_roots[i]);
			i--;
		} else {
			DFSCopy(sc_roots[i], h[!state]);
    }

	}
	Flush(h[state]);
	state = !state;
}


// This is one of the most important functionalities of the hybrid
// algorithm. This function performs a DFS from each of the root objects
// in the stop-copy heap and then copies them to mark-sweep heap. This
// copying is done to those roots whose age is greater than or equal to
// the age threshold specified. The process is essentially creating a
// new root object in the Mark-sweep heap and then copying all the chained
// objects one by one. This is called during a Garbage Collection call,
// therefore the heap of stop-copy is automatically flushed by another
// utility function.
void HybGraphUtil :: DFSShift(Object *root) {
	SCEndLifetime(root);
  MSNewReference("moved");
  EndLifetime(root);
  Object* new_root = ms_roots[ms_roots.size() - 1];
	root = root->child;
	while (root != NULL) {
		Object *parent = new_root;
		// Moving the children of the moved root.
		// For facilitating mark and sweep.
		new_root = MSNew("desc", parent);
		root = root->child;
	}
}


// This is a basic utility function of the stop-copy algorithm. This function
// copies live objects, ie, objects reachable from the roots from one heap to
// another. The other heap is flushed, thereby reclaiming space occupied by the
// "garbage".
void HybGraphUtil :: DFSCopy(Object* root, vector<Object*> &to) {
	if (root == NULL) return;

	to.push_back(root);
	DFSCopy(root->child, to);
}


// All the elements of the heap are dropped, the destructors of the objects are
// called and we retain the heap (vector) of size 0. Hence freeing the heap.
void HybGraphUtil :: Flush(vector <Object*>& v) {
	v.clear();
}


// Shows memory usage for the stop-copy component of the heap
void HybGraphUtil :: SCShowMemoryUsage() {
	cout << "Used Memory: " << (h[state].size()) << endl;
	cout << "Free Memory: " << sc_max_objects - (h[state].size()) << endl << "------------------\n";

}

// Shows memory usage for the entire heap, both the mark-sweep component
// and stop copy component
void HybGraphUtil :: ShowMemoryUsage() {
	int used = num_objects + h[state].size();
	int free = sc_max_objects + ms_max_objects - used;
	cout << "Used Memory: " << used << endl;
	cout << "Free Memory: " << free << endl << "------------------\n";
}


// Creates a new reference object. Object* obj = new Object();. Essentially,
// creates an object in the heap and a reference in the roots vector.
void HybGraphUtil :: NewReference(const string& desc) {
    int num_objects = (int)(h[state].size());

  // if the heap is full, then we call TriggerGC() to free up some space.
  // If there is not enough space in the stop-copy heap, the GC runs
  // enough number of times so that some of the objects age and then move to
  // the mark-sweep heap.
  // By default, new objects are assumed to be short-lived because of the nested
  // block structure of any functional / object-oriented language. New objects are
  // allocated into the stop-copy heap and older (long-lived by extension of logic)
  // are moved to the mark-sweep heap.
	if (num_objects == sc_max_objects) {
        for (int z = 0; z < threshold && num_objects == sc_max_objects; z++) {
			TriggerGC();
        }
	}
	num_objects = (int)h[state].size();
    
    // pushing the object into the heap is simulated as adding an element
    // at the end of a linked list.
    if (num_objects == 0) {
        num_objects++;
        Object* obj = new Object(desc);
        sc_roots.push_back(obj);
        h[state].push_back(obj);
     } else if (num_objects < sc_max_objects) {
  	    num_objects++;
	    Object* obj = new Object(desc);
	    sc_roots.push_back(obj);
	    h[state].push_back(obj);
	 } else {
	    cout << "SC Error! Unable to allocate memory!\n";
     }
}


// Creating a new reference and making it point to an existing object
// Object *obj = x;
void HybGraphUtil :: NewReference(Object* obj) {
	sc_roots.push_back(obj);
}

// New object that would be pointed to by an existing pointer.
// This includes objects that are reffered to by other objects,
// a self-referencing object, like a binary tree structure or a
// linked-list.
Object* HybGraphUtil :: New(const string& desc, Object* parent) {
    Object* obj = NULL;
    int num_objects = (int)(h[state].size());
	if (num_objects == sc_max_objects) {
        for (int z = 0; z <= threshold && num_objects == sc_max_objects; z++) {
        	TriggerGC();
        }
    }
	num_objects = (int)h[state].size();
    if (num_objects == 0) {
    	num_objects++;
    	obj = new Object(desc);
    	h[state].push_back(obj);
    	parent->child = obj;
    } else if (num_objects < sc_max_objects) {
		num_objects++;
		obj = new Object(desc);
		h[state].push_back(obj);
		parent->child = obj;
	} else {
        cout << "sc Error! Unable to allocate memory!\n";
    }
    return obj;
}

// Getting rid of the root reference. This is typically when a pointer
// falls out of scope causing a memory leak.
void HybGraphUtil :: EndLifetime(Object *obj) {
	int pos = -1;
	for (int i = 0; i < int(sc_roots.size()); i++) {
	    if (sc_roots[i] == obj) {
	        pos = i;
	        break;
	    }
	}
	if (pos != -1) sc_roots.erase(sc_roots.begin() + pos);
	else {
		for (int i = 0; i < int(ms_roots.size()); i++) {
		    if (ms_roots[i] == obj) {
		        pos = i;
		        break;
		    }
		}
		if (pos != -1) ms_roots.erase(ms_roots.begin() + pos);
	}
}

// Deleting root items for when they fall out of scope. Specifically for
// the roots in the stop-copy heap.
void HybGraphUtil :: SCEndLifetime(Object* obj) {
    int pos = -1;
    for (int i = 0; i < int(sc_roots.size()); i++) {
        if (sc_roots[i] == obj) {
            pos = i;
            break;
        }
    }
    if (pos != -1) sc_roots.erase(sc_roots.begin() + pos);
}


// Utility function to Trigger the Garbage Collection mechanism
// in the mark and sweep component. This is generally called when
// the mark and sweep heap is full when we shift from the stop-copy
// heap.
void HybGraphUtil :: MSTriggerGC() {
	for (int i = 0; i < int(ms_roots.size()); i++) {
		DFSMark(ms_roots[i]);
	}
	Sweep(first, NULL);
}

// Simple Depth First Search to mark the nodes.
void HybGraphUtil :: DFSMark (Object* root) {
	if (root == NULL) return;
	root->seen = true;
	DFSMark(root->child);
}

// Sweeping the entire heap. We traverse the linked list in our simulation,
// resetting the "seen" flag then and there. Any object that does not have
// the seen flag set will be removed. Basic deletion of elements from a linked
// list.
void HybGraphUtil :: Sweep (Object* current, Object* prev) {
	if (current == NULL) return;

	if (current->seen == true) {
		current->seen = false;
		Sweep(current->next, current);
	} else {
		if ((current->next != NULL) & (prev != NULL)) {
			// Object to be removed is neither the
			// first nor the last item
			Object* obsolete = current;
			prev->next = current->next;
			delete obsolete;
			num_objects--;
			Sweep(current->next, prev);
		} else if (prev == NULL) {
			// Object to be removed is the first item
			Object* temp = current;
			first = current->next;
			delete temp;
			num_objects--;
			Sweep(first, prev);
		} else if (current->next == NULL) {
			// Object to be removed is the last item
			prev->next = NULL;
			last = prev;
			Object* obsolete = current;
			delete obsolete;
			num_objects--;
		}
	}
}


// Shows memory usage for the Mark and sweep component of the heap
void HybGraphUtil :: MSShowMemoryUsage() {
	cout << "Used Memory: " << num_objects << endl;
	cout << "Free Memory: " << ms_max_objects - num_objects << endl << "------------------\n";
}

// Creating a new root item in the mark and sweep heap. This is called
// during the shifting phase of aged objects
void HybGraphUtil :: MSNewReference(const string& desc) {
    if (num_objects == ms_max_objects) {
        TriggerGC();
    }
    if (num_objects == 0) {
        	num_objects++;
        	Object* obj = new Object(desc);
        	ms_roots.push_back(obj);
        	first = obj;
        	last = obj;
    } else if (num_objects < ms_max_objects) {
  	    num_objects++;
	    Object* obj = new Object(desc);
	    ms_roots.push_back(obj);
	    last->next = obj;
	    last = obj;
    } else {
	    cout << "MS Error! Unable to allocate memory!\n";
    }
}

// Creating a new reference and making it point to an existing object
// Object *obj = x;
void HybGraphUtil :: MSNewReference(Object* obj) {
	ms_roots.push_back(obj);
}

// New object that would be pointed to by an existing pointer.
// This includes objects that are reffered to by other objects,
// a self-referencing object, like a binary tree structure or a
// linked-list.
Object* HybGraphUtil :: MSNew(const string& desc, Object* parent) {
    Object* obj = NULL;

    if (num_objects == ms_max_objects) {
      TriggerGC();
    }
    
   if (num_objects == 0) {
    	num_objects++;
    	obj = new Object(desc);
    	parent->child = obj;
    	first = obj;
    	last = obj;
    } else if (num_objects < ms_max_objects) {
	num_objects++;
	obj = new Object(desc);
	parent->child = obj;
	last->next = obj;
	last = obj;
    } else {
        cout << "ms Error! Unable to allocate memory!\n";
    }

    return obj;
}

// Reassigning a pointer to a different object. No creation of objects
// involved.
void HybGraphUtil :: MSOldReference (Object* o1, Object* o2){
	o2 = o1;
}

// Deleting root items for when they fall out of scope.
void HybGraphUtil :: MSEndLifetime(Object* obj) {
    int pos = -1;
    for (int i = 0; i < int(ms_roots.size()); i++) {
        if (ms_roots[i] == obj) {
            pos = i;
            break;
        }
    }
    if (pos != -1) ms_roots.erase(ms_roots.begin() + pos);
}

} // hyb_graph_api
