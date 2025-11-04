#ifndef CORE_HPP
#define CORE_HPP

#include <cstring>
#include <cstdio>

// Maximum sizes
const int MAX_USERS = 20000;
const int MAX_TRAINS = 5000;
const int MAX_ORDERS = 100000;
const int MAX_DATES = 92;  // June-August

// Simple pair template
template<typename T1, typename T2>
struct Pair {
    T1 first;
    T2 second;
    
    Pair() : first(), second() {}
    Pair(const T1& f, const T2& s) : first(f), second(s) {}
};

// Simple dynamic array
template<typename T>
class Array {
private:
    T* data;
    int cap;
    int len;
    
    void expand() {
        cap = cap == 0 ? 4 : cap * 2;
        T* newData = new T[cap];
        for (int i = 0; i < len; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    
public:
    Array() : data(nullptr), cap(0), len(0) {}
    
    ~Array() { delete[] data; }
    
    void add(const T& val) {
        if (len >= cap) expand();
        data[len++] = val;
    }
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    
    int size() const { return len; }
    void clear() { len = 0; }
    
    // Sort using simple bubble sort (sufficient for small arrays)
    template<typename Comp>
    void sort(Comp comp) {
        for (int i = 0; i < len - 1; i++) {
            for (int j = i + 1; j < len; j++) {
                if (comp(data[j], data[i])) {
                    T temp = data[i];
                    data[i] = data[j];
                    data[j] = temp;
                }
            }
        }
    }
};

// Simple hash map using linear probing
template<typename K, typename V, int SIZE = 10007>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        bool used;
        
        Entry() : used(false) {}
    };
    
    Entry table[SIZE];
    
    unsigned int hash(const char* str) {
        unsigned int h = 0;
        while (*str) {
            h = h * 131 + *str++;
        }
        return h % SIZE;
    }
    
public:
    void insert(const K& key, const V& value) {
        unsigned int h = hash(key);
        for (int i = 0; i < SIZE; i++) {
            int pos = (h + i) % SIZE;
            if (!table[pos].used) {
                table[pos].key = key;
                table[pos].value = value;
                table[pos].used = true;
                return;
            }
            if (strcmp(table[pos].key, key) == 0) {
                table[pos].value = value;
                return;
            }
        }
    }
    
    bool find(const K& key, V& value) {
        unsigned int h = hash(key);
        for (int i = 0; i < SIZE; i++) {
            int pos = (h + i) % SIZE;
            if (!table[pos].used) {
                return false;
            }
            if (strcmp(table[pos].key, key) == 0) {
                value = table[pos].value;
                return true;
            }
        }
        return false;
    }
    
    bool exists(const K& key) {
        V dummy;
        return find(key, dummy);
    }
    
    void clear() {
        for (int i = 0; i < SIZE; i++) {
            table[i].used = false;
        }
    }
};

#endif
