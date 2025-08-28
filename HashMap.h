#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include <sstream>
#include "Hashing.h"
#include "HashNode.h"
#include <exception>

#define HashNode Node<keyType, valType>

using namespace std;


// Since for some reason `%` can return negatives
unsigned long long mod(long long x, unsigned int y) {
    return ((x % y) + y) % y;
}

/** @brief Hashes by value by default; to hash by identity, accept pointers as keys */
template<typename keyType, typename valType>
class HashMap {
public:
    HashMap() {
        buckets = new vector<HashNode*>(2);
        size = 0;

        const type_info& stringType = typeid(string);
        keyIsString = false;
        valIsString = false;
        if (typeid(keyType) == stringType) {
            keyIsString = true;
        }
        
        if (typeid(valType) == stringType) {
            valIsString = true;
        }
    }

    ~HashMap() {
        wipe();
    }

    /** @brief Get the value associated with the given key ONLY if it exists, otherwise throw an error */
    valType& operator[](const keyType& key) {
        HashNode* nodeOfInterest = getNode(key, false);

        if (nodeOfInterest == NULL) {
            throw out_of_range("Key not found");
        }

        return *(nodeOfInterest->val);
    }

    /** 
     * @brief Set a new value to the given key if it exists; if not, add the key and its value 
     * @return true if key was already present and value was updated
     * @return false if key and value were inserted 
     */
    bool insert(const keyType& key, const valType& val) {
        HashNode* nodeOfInterest = getNode(key, true);
        bool keyFound = (nodeOfInterest->val != NULL);

        nodeOfInterest->val = new valType(val);

        return keyFound;
    }

    unsigned int getSize() {
        return size;
    }

    bool containsKey(const keyType& key) {
        return getNode(key, false) != NULL;
    }

    void clear() {
        wipe();
        buckets = new vector<HashNode*>(2);
        size = 0;
    }

    /** Give summary of contents */
    string toString(bool verbose = false) {
        vector<string> lines;
        vector<unsigned int> load_factors;

        stringstream ss;
        
        bool extraLine = false;
        unsigned int loadFactor = 0;

        for (unsigned int i = 0; i < buckets->size(); i++) {
            if (verbose) {
                lines.push_back(to_string(i) + ':');
                loadFactor = 0;
            }
            loadFactor = addRestOfRowToString((*buckets)[i], lines, loadFactor);

            if (verbose) {
                load_factors.push_back(loadFactor);
                lines.push_back("");
            }
        }

        for (unsigned int i = 0; i < lines.size(); i++) {
            if (extraLine) {
                ss << endl;
            }
            ss << lines.at(i);
            extraLine = true;
        }

        double total = 0;
        if (verbose) {
            for (unsigned int l : load_factors) {
                total += l * ((l + 1) * 0.5);
            }

            ss << endl << "Avg. nodes to search through to find a value: ";
            if (this->size) {
                ss << total / this->size;
            }
            else {
                ss << "N/A";
            }
        }

        return ss.str();
    }

    /**
     * @return true if key was found and removed,
     * @return false if not
     */
    bool remove(const keyType& key) {
        return removeHelperFun(key);
    }

    long long hashTheMap() const {
        long long returnVal = 0;
        for (unsigned int i = 0; i < buckets->size(); i++) {
            if ((*buckets)[i] != NULL) {
                returnVal += (*buckets)[i]->hashWithSubsequent();
            }
        }

        return returnVal;
    }

private:

    bool removeHelperFun(const keyType& key) {
        long long hash = hashIt(key);
        unsigned int numOfBuckets = buckets->size();

        unsigned int bucketNum = mod(hash, numOfBuckets);


        // Search the bucket for the value

        HashNode* next = (*buckets)[bucketNum];
        HashNode* prev = NULL;

        while (next != NULL && next->getKey() != key) {
            prev = next;
            next = next->getNextNode();
        }

        if (next != NULL) { // If we actually found the value
            if (prev == NULL) { // If it's the first node in the linkedlist
                removeFirstNode(bucketNum);
            }
            else {
                removeNode(prev);
            }
            size--;
        }
        else {
            return false;
        }

        double loadFactor = (double)(size) / numOfBuckets;
        if (loadFactor < 0.5) {
            resizeDown();
        }

        return true;
    }

    /** Remove the node that comes after prevNode */
    void removeNode(HashNode* prevNode) {
        HashNode* nodeToRemove = prevNode->getNextNode();
        prevNode->setNext(nodeToRemove->getNextNode());
        delete nodeToRemove;
    }

    /** Remove the first node in the given bucket */
    void removeFirstNode(unsigned int bucketNum) {
        HashNode* nodeToRemove = (*buckets)[bucketNum];
        (*buckets)[bucketNum] = nodeToRemove->getNextNode();
        delete nodeToRemove;
    }

    /** @param insert Indicates whether we insert the key if not found in the map*/
    HashNode* getNode(const keyType& key, bool insert) {
        long long hash;
        unsigned int numOfBuckets = buckets->size();
        double loadFactor;
        
        if (insert) {
            loadFactor = (double)(size + 1) / numOfBuckets;
            if (loadFactor > 2) {
                resizeUp();
            }

            numOfBuckets = buckets->size();
        }

        hash = hashIt(key);

        unsigned int bucketNum = mod(hash, numOfBuckets);


        // Search the bucket for the value

        HashNode* next = (*buckets)[bucketNum];
        HashNode* prev = NULL;
        HashNode* nodeOfInterest;

        while (next != NULL && next->getKey() != key) {
            prev = next;
            next = next->getNextNode();
        }

        if (next != NULL) { // If we actually found the node
            nodeOfInterest = next;
        }
        else {
            if (!insert) {
                return NULL;
            }

            HashNode* newNode = new Node(key, valTypeIndicator);
            
            if (prev == NULL) { // If we didn't find the value because the bucket is empty
                (*buckets)[bucketNum] = newNode;
            } 
            /* If we didn't find the value, but the bucket is not empty, add the value
            to the end of the linked list */
            else {
                prev->setNext(newNode);
            }

            size++;
            nodeOfInterest = newNode;
        }

        return nodeOfInterest;
    }

    /** Move a node into the new set of buckets */
    void moveNode(HashNode* node, vector<HashNode*>* newBuckets) {
        long long hash = hashIt(node->getKey());

        unsigned int bucket = mod(hash, newBuckets->size());

        HashNode* firstNode = (*newBuckets)[bucket];
        (*newBuckets)[bucket] = node;
        node->setNext(firstNode);
    }

    /** Add the current node and subsequent nodes in the current bucket's linked list, to the new
    set of buckets */
    void addRestOfRow(HashNode* startNode, vector<HashNode*>* newBuckets) {
        if (startNode == NULL) {
            return;
        }

        HashNode* next = startNode->getNextNode();
        moveNode(startNode, newBuckets);
        addRestOfRow(next, newBuckets);
    }

    int addRestOfRowToString(HashNode* startNode, vector<string>& lines, unsigned int numOfNodes) {
        if (startNode == NULL) {
            return numOfNodes;
        }

        stringstream ss;

        numOfNodes++;

        ss << "key: ";
        if (keyIsString) {
            ss << '"' << startNode->getKey() << '"';
        }
        else {
            ss << startNode->getKey();
        }

        ss << ", val: ";
        if (valIsString) {
            ss << '"' << *(startNode->val) << '"';
        }
        else {
            ss << *(startNode->val);
        }

        HashNode* next = startNode->getNextNode();
        lines.push_back(ss.str());

        numOfNodes = addRestOfRowToString(next, lines, numOfNodes);
        return numOfNodes;
    }

    void resizeUp() {
        unsigned int newSize = 2 * buckets->size() + 1;
        resize(newSize);
    }

    void resizeDown() {
        unsigned int newSize = buckets->size() / 2 + 1;
        if (newSize == buckets->size()) {
            return;
        }
        resize(newSize);
    }

    /** Increase or decrease the number of buckets and reorganize all nodes appropriately */
    void resize(unsigned int newSize) {
        vector<HashNode*>* newBuckets = new vector<HashNode*>(newSize);

        // Move all nodes into the new set of buckets
        for (unsigned int i = 0; i < buckets->size(); i++) {
            addRestOfRow((*buckets)[i], newBuckets);
        }

        delete buckets;
        buckets = newBuckets;
    }

    void wipe() {
        for (unsigned int i = 0; i < buckets->size(); i++) {
            if ((*buckets)[i] != NULL) {
                (*buckets)[i]->deleteWithSubsequent();
            }
        }
        delete buckets;
        buckets = NULL;
    }

protected:
    vector<HashNode*>* buckets;
    const valType* valTypeIndicator = NULL;
    unsigned int size;
    bool keyIsString;
    bool valIsString;
};


template<typename keyType, typename valType>
long long hashIt(const HashMap<keyType, valType>& map) {
    return map.hashTheMap();
};

#endif
