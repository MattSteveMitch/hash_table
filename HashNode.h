#ifndef HASHNODE_H
#define HASHNODE_H

#include "Hashing.h"

/** @brief Class representing a key-value pair in hash table. This hash table uses chaining for
collision resolution, so they are called `Node` as in a linked list. */
template<typename keyType, typename valType>
class Node {

public:

/** @param v is simply here to indicate what type the "values" (as opposed to keys) in the hash 
table will be. Its specific value does not matter; it can be NULL. */
	Node(const keyType& key, const valType* v) {
		this->key = new keyType(key);
    	this->val = NULL;
	    nextNode = NULL;
	}

	~Node() {
		delete key;
		delete val;
	}

	keyType& getKey() {
		return *key;
	}

	Node* getNextNode() {
    	return nextNode;
	}

    void setNext(Node* node) {
    	nextNode = node;
	}

	/** Delete this node and subsequent nodes in linked list */
	void deleteWithSubsequent() {
		if (nextNode != NULL) {
			nextNode->deleteWithSubsequent();
		}
		delete this;
	}

	long long hashWithSubsequent() const {
		long long returnVal = 0;

		if (nextNode != NULL) {
			returnVal += nextNode->hashWithSubsequent();
		}
		
		returnVal += hashIt(*key) * (hashIt(*val) + 1726749251);

		return returnVal;
	}

    valType* val;

private:
	keyType* key;
    Node<keyType, valType>* nextNode;
};

#endif
