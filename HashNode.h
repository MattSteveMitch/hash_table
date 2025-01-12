#ifndef HASHNODE_H
#define HASHNODE_H

#include <iostream>

template<typename, typename> class Node;

template<typename keyType, typename valType>
class Node {

public:
	Node(keyType& key, valType& dataVal) {
		this->key = new keyType(key);
    	data = dataVal;
	    nextNode = NULL;
	}

	Node(keyType* key, valType& dataVal, Node<keyType, valType>* nextNode) {
		this->key = key;
		data = dataVal;
		this->nextNode = nextNode;
	}

	Node() {
		this->key = new keyType();
    	data = valType();
	    nextNode = NULL;
	}

	~Node() {} // Do not call without taking care of the children! No memory leaks!

	/*
	* Returns the data that is stored in this node
	*
	* @return the data that is stored in this node.
	*/
	valType& getData() {
	    return data;
	}

	keyType& getKey() {
		return *key;
	}

	keyType* getKeyAddress() {
		return key;
	}

	keyType* getDataAddress() {
		return &data;
	}
	/*
	* Returns the left child of this node or null if it doesn't have one.
	*
	* @return the left child of this node or null if it doesn't have one.
	*/
	Node * getNextNode() {
    	return nextNode;
	}
    
    void setNext(keyType& key, valType data) {
    	nextNode = new Node(key, data);
	}

    void setNext(Node* child) { // Do not call without taking care of children! No memory leaks!
    	nextNode = child;
	}

	void deleteWithSubsequent() {
		if (nextNode != NULL) {
			nextNode->deleteWithSubsequent();
		}
		delete this;
	}
    
    /*void removeRightChild() {
	    delete right_child;
    	right_child = NULL;
	}*/

    void setData(valType dataVal) {
	    this->data = dataVal;
	}

	void setKey(keyType& key) {
		this->key = new keyType(key);
	}

    valType data;

private:
	keyType* key;
    Node<keyType, valType>* nextNode;
};

#endif