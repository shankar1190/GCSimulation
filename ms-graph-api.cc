/*
 * ms-graph-api.cc
 *
 *  Created on: 08-Apr-2012
 *      Author: shankar
		radhika
		neeraja

  In the naive mark-and-sweep method, each object in memory has a flag
  (typically a single bit) reserved for garbage collection use only.
  This flag   is always cleared, except during the collection cycle.
  The first stage of collection does a tree traversal of the entire
  'root set', marking each object that is pointed to as being 'in-use'.
  All objects that those objects point to, and so on, are marked as well,
  so that every object that is ultimately pointed to from the root set is
  marked. Finally, all memory is scanned from start to finish, examining
  all free or used blocks; those with the in-use flag still cleared are
  not reachable by any program or data, and their memory is freed.
  (For objects which are marked in-use, the in-use flag is cleared again,
  preparing for the next cycle.)

  This method has several disadvantages, the most notable being that
  the entire system must be suspended during collection; no mutation of the
  working set can be allowed. This will cause programs to 'freeze'
  periodically (and generally unpredictably), making real-time and
  time-critical applications impossible. In addition, the entire working
  memory must be examined, much of it twice, potentially causing problems
  in paged memory systems.
   
 */


#ifndef MSHEAPSIZE
#define MSHEAPSIZE 100
#endif

#include "ms-graph-api.h"


namespace ms_graph_api {

// default constructor
// initalizes the heap traversal pointers and the number of objects
// and heap-size
MSGraphUtil :: MSGraphUtil() {
	first = NULL;
	last = NULL;
	num_objects = 0;
	max_objects = MSHEAPSIZE;
}

// Allows the user to specify heap-size.
MSGraphUtil :: MSGraphUtil(int heap_size) {
	first = NULL;
	last = NULL;
	num_objects = 0;
	max_objects = heap_size;
}

// This is triggered when there is not enough space on the heap. In our
// simulation, that is when num_objects equals max_objects.
void MSGraphUtil :: TriggerGC() {
	for (int i = 0; i < int(roots.size()); i++) {
		DFSMark(roots[i]);
	}

	Sweep(first, NULL);
}

// Simple Depth First Search to mark the nodes.
void MSGraphUtil :: DFSMark (Object* root) {
	if (root == NULL)
	  return;
	
	root->seen = true;
	DFSMark(root->child);
}

// Sweeping the entire heap. We traverse the linked list in our simulation,
// resetting the "seen" flag then and there. Any object that does not have
// the seen flag set will be removed. Basic deletion of elements from a linked
// list.
void MSGraphUtil :: Sweep (Object* current, Object* prev) {
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

// Shows the total memory used and the free memory
void MSGraphUtil :: ShowMemoryUsage() {
	cout << "Used Memory: " << num_objects << endl;
	cout << "Free Memory: " << max_objects - num_objects << endl << "------------------\n";
}

// Creates a new reference object. Object* obj = new Object();. Essentially,
// creates an object in the heap and a reference in the roots vector.
void MSGraphUtil :: NewReference(const string& desc) {
    
    // if the heap is full, then we call TriggerGC() to free up some space.
    if (num_objects == max_objects) {
        TriggerGC();
    }
    
    // pushing the object into the heap is simulated as adding an element
    // at the end of a linked list.
    if (num_objects == 0) {
        	num_objects++;
        	Object* obj = new Object(desc);
        	roots.push_back(obj);
        	first = obj;
        	last = obj;
    } else if (num_objects < max_objects) {
  	    num_objects++;
	    Object* obj = new Object(desc);
	    roots.push_back(obj);
	    last->next = obj;
	    last = obj;
    } else {
	    cout << "Error! Unable to allocate memory!\n";
    }
}


// Creating a new reference and making it point to an existing object
// Object *obj = x;
void MSGraphUtil :: NewReference(Object* obj) {
	roots.push_back(obj);
}

// New object that would be pointed to by an existing pointer.
// This includes objects that are reffered to by other objects,
// a self-referencing object, like a binary tree structure or a
// linked-list.
Object* MSGraphUtil :: New(const string& desc, Object* parent) {
    Object* obj = NULL;
	if (num_objects == max_objects) {
      TriggerGC();
    }
    if (num_objects == 0) {
    	num_objects++;
    	obj = new Object(desc);
    	parent->child = obj;
    	first = obj;
    	last = obj;
    } else if (num_objects < max_objects) {
		num_objects++;
		obj = new Object(desc);
		parent->child = obj;
		last->next = obj;
		last = obj;
    } else {
        cout << "Error! Unable to allocate memory!\n";
    }
    return obj;
}

// Reassigning a pointer to a different object. No creation of objects
// involved.
void MSGraphUtil :: OldReference (Object* o1, Object* o2){
	o2 = o1;
}

// Getting rid of the root reference. This is typically when a pointer
// falls out of scope causing a memory leak.
void MSGraphUtil :: EndLifetime(Object* obj) {
    int pos = -1;
    for (int i = 0; i < int(roots.size()); i++) {
        if (roots[i] == obj) {
            pos = i;
            break;
        }
    }
    if (pos != -1) roots.erase(roots.begin() + pos);
}
} // ms_graph_api
