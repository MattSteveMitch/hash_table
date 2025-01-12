#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include <sstream>
#include "HashNode.h"
#include "Hashing.h"
#include <exception>

using namespace std;



template<typename, typename> class HashMap;

int mod(int x, int y) { // Since for some reason "%" is NOT the mod operator
    return ((x % y) + y) % y;
}

/** @brief Hashes by value by default; to hash by identity, accept pointers as keys */
template<typename keyType, typename valType>
class HashMap {
public:
    HashMap() {
        buckets = {NULL, NULL};
        size = 0;
        string s;
        keyType k;
        keyIsPrimitiveOrString = false;
        keyIsString = false;
        keyIsPointer = false;
        if (typeid(k).name() == typeid(s).name()) {
            keyIsString = true;
        }
        if (is_fundamental<keyType>::value || keyIsString) {
            keyIsPrimitiveOrString = true;
        }
        if (is_pointer<keyType>::value) {
            keyIsPointer = true;
        }
    }

    ~HashMap() {
        clear();
    }

/** @brief Access the value slot associated with the given key, for either reading or writing. If the key does not yet exist in the hash map, a new value slot is created for it and filled beforehand with the default value for the specific value type.*/
    valType& operator[](keyType& key) {
        return *getVal(key, true);
    }

/** @brief Access the value slot associated with the given key (string literal), for either reading or writing. If the key does not yet exist in the hash map, a new value slot is created for it and filled beforehand with the default value for the specific value type.*/
    valType& operator[](const string& key) {
        string theKey = key;
        return *getVal(theKey, true);
    }

/** @brief Get the value associated with the given key ONLY if it exists, otherwise throw an error */
    valType get(keyType& key) {
        Node<keyType, valType>* nodeOfInterest;
        nodeOfInterest = getNode(key, false);

        if (nodeOfInterest == NULL) {
            throw out_of_range("Key not found");
        }

        return nodeOfInterest->data;
    }

    int getSize() {
        return size;
    }

    bool containsKey(keyType& key) {
        return getNode(key, false) != NULL;
    }

    bool containsKey(const string& key) {
        string theKey = key;
        return getNode(theKey, false) != NULL;
    }

    void clear() {
        for (int i = 0; i < buckets.size(); i++) {
            if (buckets.at(i) != NULL) {
                buckets.at(i)->deleteWithSubsequent();
                buckets.at(i) = NULL;
            }
        }
        buckets.resize(2);
        size = 0;
    }

    string toString() {
        vector<string> nodes;
        vector<int> load_factors;
        stringstream ss;
        bool semicolon = false;
        int* loadFactor = new int(0);
        double total = 0;
        for (int i = 0; i < buckets.size(); i++) {
            nodes.push_back('\n' + to_string(i) + ':');
            *loadFactor = 0;
            addRestOfRowToString(buckets.at(i), nodes, loadFactor);
            load_factors.push_back(*loadFactor);
        }
        
        for (int l : load_factors) {
            total += l * ((l + 1) * 0.5);
        }

        for (int i = 0; i < nodes.size(); i++) {
            if (semicolon) {
                ss << endl;
            }
            ss << nodes.at(i);
            semicolon = true;
        }
        ss << endl << "avg: " << total / this->size << endl;

        return ss.str();
    }

    bool remove(keyType& key) {
        return removeHelperFun(key);
    }

    bool remove(const string& key) {
        string theKey = key;
        return removeHelperFun(theKey);
    }

private:

    valType* getVal(keyType& key, bool insert) {
        auto node = getNode(key, insert);
        if (node == NULL) {
            return NULL;
        }
        return &(node->data);
    }
    
    bool removeHelperFun(keyType& key) {
        int hash = hashIt(key);
        int numOfBuckets = buckets.size();

        int bucketNum = mod(hash, numOfBuckets);

        Node<keyType, valType>* next = buckets.at(bucketNum);
        Node<keyType, valType>* last = NULL;
        bool found;

        while (next != NULL && next->getKey() != key) {
            last = next;
            next = next->getNextNode();
        }

        if (next != NULL) {
          //  cout << "1 " << endl;
            if (last == NULL) {
                removeFirstNode(bucketNum);
            }
            else {
                removeNode(last);
            }
            size--;
        }
        else {
            return false;
        }

        float loadFactor = (float)(size) / numOfBuckets;
        if (loadFactor < 0.5) {
            resizeDown();
        }

        return true;
    }

    void removeNode(Node<keyType, valType>* prevNode) {
        Node<keyType, valType>* nodeToRemove = prevNode->getNextNode();
        prevNode->setNext(nodeToRemove->getNextNode());
        delete nodeToRemove;
    }

    void removeFirstNode(int bucketNum) {
        Node<keyType, valType>* nodeToRemove = buckets.at(bucketNum);
        buckets.at(bucketNum) = nodeToRemove->getNextNode();
        delete nodeToRemove;
    }

    Node<keyType, valType>* getNode(keyType& key, bool insert) {
        int hash;
        int numOfBuckets = buckets.size();

        float loadFactor = (float)(size + 1) / numOfBuckets;
        if (loadFactor > 2) {
            resizeUp();
        }

        numOfBuckets = buckets.size();

        hash = hashIt(key);

        int bucketNum = mod(hash, numOfBuckets);

        Node<keyType, valType>* next = buckets.at(bucketNum);
        Node<keyType, valType>* last = NULL;
        Node<keyType, valType>* nodeOfInterest;

        while (next != NULL && next->getKey() != key) {
            last = next;
            next = next->getNextNode();
        }

        if (next != NULL) {
          //  cout << "1 " << endl;
            nodeOfInterest = next;
        }
        else if (last == NULL) {
            //cout << "2 " << endl;
            if (!insert) {
                return NULL;
            }
            valType v = valType();
            auto newNode = new Node(key, v);
            buckets.at(bucketNum) = newNode;
            size++;
            nodeOfInterest = newNode;
        }
        else {
            if (!insert) {
                return NULL;
            }
            valType v = valType();
            auto newNode = new Node(key, v);
            //cout << "3 " << endl;
            last->setNext(newNode);
            size++;
            nodeOfInterest = newNode;
        }

        return nodeOfInterest;
    }

    void moveNode(Node<keyType, valType>* node, vector<Node<keyType, valType>*>& newBuckets) {
        int hash = hashIt(node->getKey());

        int bucket = mod(hash, newBuckets.size());

        Node<keyType, valType>* firstNode = newBuckets.at(bucket);
        newBuckets.at(bucket) = new Node(node->getKeyAddress(), node->data, firstNode);
    }

    void addRestOfRow(Node<keyType, valType>* startNode, vector<Node<keyType, valType>*>& newBuckets) {
        if (startNode == NULL) {
            return;
        }
        moveNode(startNode, newBuckets);

        auto next = startNode->getNextNode();
        addRestOfRow(next, newBuckets);
        delete startNode;
    }

    void addRestOfRowToString(Node<keyType, valType>* startNode, vector<string>& nodes, int* numOfNodes) {
        stringstream ss;
        if (startNode == NULL) {
            return;
        }
        *numOfNodes = *numOfNodes + 1;
        ss << "key: ";
        if (keyIsString) {
            ss << '"' << startNode->getKey() << '"';
        }
        else {
            ss << startNode->getKey();
        }
        ss << ", val: " << startNode->data;
        auto next = startNode->getNextNode();
        nodes.push_back(ss.str());

        addRestOfRowToString(next, nodes, numOfNodes);
    }

    void resizeUp() {
  //      cout << "resizing up" << endl;
        int newSize = 2 * buckets.size() + 1;
        resize(newSize);
    }

    void resizeDown() {
    //    cout << "resizing down" << endl;
        int newSize = buckets.size() / 2 + 1;
        resize(newSize);
    }

    void resize(int newSize) {
        vector<Node<keyType, valType>*> newBuckets;
        newBuckets.resize(newSize);

        for (int i = 0; i < buckets.size(); i++) {
            addRestOfRow(buckets.at(i), newBuckets);
        }

        buckets = newBuckets;
    }
    
    int size;
    bool keyIsPrimitiveOrString;
    bool keyIsString;
    bool keyIsPointer;

protected:
    vector<Node<keyType, valType>*> buckets;
};


#endif
