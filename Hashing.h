#ifndef HASHING_H
#define HASHING_H

#include <iostream>
#include <vector>
#include <array>
#include <unordered_set>
#include <set>
#include <map>
#include <unordered_map>
#include <typeinfo>

using namespace std;


// Custom simple hashing functions for various data types

template<typename Type>
long long hashIt(const Type);

template<typename Type>
long long hashIt(Type*);

long long hashIt(const long long);

template<typename ArrayType>
long long hashArrayType(const ArrayType&);

template<typename VectElemType>
long long hashIt(const vector<VectElemType>&);

template<typename ElemType, size_t size>
long long hashIt(const array<ElemType, size>&);

long long hashIt(const string&);

template<typename SetType>
long long hashSetType(const SetType&);

template<typename keyType, typename valType>
long long hashIt(const pair<keyType, valType>&);

template<typename MapType>
long long hashTheMap(const MapType&);

template<typename keyType, typename valType>
long long hashIt(const unordered_map<keyType, valType>&);

template<typename keyType, typename valType>
long long hashIt(const map<keyType, valType>&);

template<typename SetElemType>
long long hashIt(const set<SetElemType>&);

template<typename USetElemType>
long long hashIt(const unordered_set<USetElemType>&);



/* For numbers (except for long long types, which lose some precision when coverted 
to a double). */
template<typename Type>
long long hashIt(const Type num) {
    double doubleFloatVal = num;
    long long* rawBits = (long long*)&doubleFloatVal;
    return *rawBits * 489259173496327321 + 381650824613849279;
}

template<typename Type>
long long hashIt(Type* ptr) {
    long long intVal = (long long)ptr;
    return intVal * 489259173496327321 + 381650824613849279;
}

long long hashIt(const long long num) {
    return 489259173496327321 * num + 381650824613849279;
}

template<typename ArrayType>
long long hashArrayType(const ArrayType& arr) {
    long long returnValSoFar = 0;
    int multiplier = 39324401;
    
    for (unsigned int i = 0; i < arr.size(); i++) {
        returnValSoFar += hashIt(arr[i]) * multiplier;
        multiplier *= 39324401;
    }

    return returnValSoFar + 738572087136357119;
}

template<typename VectElemType>
long long hashIt(const vector<VectElemType>& v) {
    return hashArrayType(v);
}

template<typename ElemType, size_t size>
long long hashIt(const array<ElemType, size>& a) {
    return hashArrayType(a);
}

long long hashIt(const string& s) {
    return hashArrayType(s);
}

template<typename SetType>
long long hashSetType(const SetType& theSet) {
    long long returnValSoFar = 0;

    for (auto& elem : theSet) {
        returnValSoFar += 39324401 * hashIt(elem);
    }

    return returnValSoFar + 738572087136357119;
}

template<typename keyType, typename valType>
long long hashIt(const pair<keyType, valType>& p) {
    return hashIt(p.first) * (hashIt(p.second) + 1726749251);
}

template<typename MapType>
long long hashTheMap(const MapType& m) {
    long long returnVal = 0;
    for (auto& thisPair : m) {
        returnVal += hashIt(thisPair);
    }

    return returnVal;
}

template<typename keyType, typename valType>
long long hashIt(const unordered_map<keyType, valType>& um) {
    return hashTheMap(um);
}

template<typename keyType, typename valType>
long long hashIt(const map<keyType, valType>& m) {
    return hashTheMap(m);
}

template<typename SetElemType>
long long hashIt(const set<SetElemType>& s) {
    return hashSetType(s);
}

template<typename USetElemType>
long long hashIt(const unordered_set<USetElemType>& s) {
    return hashSetType(s);
}

#endif
