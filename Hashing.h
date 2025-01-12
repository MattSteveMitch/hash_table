#ifndef HASHING_H
#define HASHING_H

using namespace std;


int hashIt(string str) {
    int returnVal = 0;
    int multiplier = 1;
    for (int i = 0; i < str.length(); i++) {
        returnVal += str.at(i) * multiplier;
        multiplier *= 101;
    }
    return returnVal;
}

int hashIt(double num) {
    long long intNum = num;
    double partial = intNum - num;
    long long returnVal = num + (partial * 3903257311);
    return returnVal;
}

int hashIt(void* key) {
    unsigned long long intAddress = reinterpret_cast<uintptr_t>(key);
    intAddress += intAddress * 0.28478231;
    return intAddress;
}

#endif
