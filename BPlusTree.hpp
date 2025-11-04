#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include <fstream>
#include <cstring>
#include <functional>

template<typename Key, typename Value, int M = 100>
class BPlusTree {
private:
    static constexpr int MAX_KEY = M;
    static constexpr int MIN_KEY = M / 2;
    
    struct Node {
        int size;
        bool isLeaf;
        int offset;
        Key keys[MAX_KEY + 1];
        int children[MAX_KEY + 2];
        int next;
        
        Node() : size(0), isLeaf(true), offset(-1), next(-1) {
            memset(children, -1, sizeof(children));
        }
    };
    
    std::fstream file;
    std::string filename;
    int root;
    int nodeCount;
    
    void writeNode(const Node& node, int offset) {
        file.seekp(offset);
        file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
        file.flush();
    }
    
    void readNode(Node& node, int offset) {
        file.seekg(offset);
        file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    }
    
    int allocateNode() {
        int offset = sizeof(int) * 2 + nodeCount * sizeof(Node);
        nodeCount++;
        return offset;
    }
    
    void writeHeader() {
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&root), sizeof(int));
        file.write(reinterpret_cast<char*>(&nodeCount), sizeof(int));
        file.flush();
    }
    
    void readHeader() {
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&root), sizeof(int));
        file.read(reinterpret_cast<char*>(&nodeCount), sizeof(int));
    }
    
    void insertNonFull(int offset, const Key& key, const Value& value, int childOffset = -1) {
        Node node;
        readNode(node, offset);
        
        if (node.isLeaf) {
            int i = node.size - 1;
            while (i >= 0 && key < node.keys[i]) {
                node.keys[i + 1] = node.keys[i];
                i--;
            }
            node.keys[i + 1] = key;
            node.size++;
            writeNode(node, offset);
        } else {
            int i = node.size - 1;
            while (i >= 0 && key < node.keys[i]) {
                i--;
            }
            i++;
            
            Node child;
            readNode(child, node.children[i]);
            
            if (child.size == MAX_KEY) {
                splitChild(offset, i);
                readNode(node, offset);
                if (key > node.keys[i]) {
                    i++;
                }
            }
            insertNonFull(node.children[i], key, value);
        }
    }
    
    void splitChild(int parentOffset, int childIndex) {
        Node parent, child, newChild;
        readNode(parent, parentOffset);
        readNode(child, parent.children[childIndex]);
        
        newChild.isLeaf = child.isLeaf;
        newChild.size = MIN_KEY;
        
        int mid = MIN_KEY;
        for (int i = 0; i < MIN_KEY; i++) {
            newChild.keys[i] = child.keys[mid + i];
            if (!child.isLeaf) {
                newChild.children[i] = child.children[mid + i];
            }
        }
        if (!child.isLeaf) {
            newChild.children[MIN_KEY] = child.children[MAX_KEY];
        }
        
        child.size = MIN_KEY;
        
        if (child.isLeaf) {
            newChild.next = child.next;
            int newOffset = allocateNode();
            child.next = newOffset;
            newChild.offset = newOffset;
        } else {
            int newOffset = allocateNode();
            newChild.offset = newOffset;
        }
        
        for (int i = parent.size; i > childIndex; i--) {
            parent.keys[i] = parent.keys[i - 1];
            parent.children[i + 1] = parent.children[i];
        }
        
        parent.keys[childIndex] = child.isLeaf ? newChild.keys[0] : child.keys[MIN_KEY];
        parent.children[childIndex + 1] = newChild.offset;
        parent.size++;
        
        writeNode(child, parent.children[childIndex]);
        writeNode(newChild, newChild.offset);
        writeNode(parent, parentOffset);
    }
    
public:
    BPlusTree(const std::string& fname) : filename(fname), root(-1), nodeCount(0) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        
        if (!file.is_open()) {
            file.clear();
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
            writeHeader();
        } else {
            readHeader();
        }
    }
    
    ~BPlusTree() {
        if (file.is_open()) {
            writeHeader();
            file.close();
        }
    }
    
    void insert(const Key& key, const Value& value) {
        if (root == -1) {
            Node node;
            node.isLeaf = true;
            node.size = 1;
            node.keys[0] = key;
            root = allocateNode();
            node.offset = root;
            writeNode(node, root);
            writeHeader();
            return;
        }
        
        Node rootNode;
        readNode(rootNode, root);
        
        if (rootNode.size == MAX_KEY) {
            Node newRoot;
            newRoot.isLeaf = false;
            newRoot.size = 0;
            int newRootOffset = allocateNode();
            newRoot.offset = newRootOffset;
            newRoot.children[0] = root;
            writeNode(newRoot, newRootOffset);
            
            splitChild(newRootOffset, 0);
            root = newRootOffset;
            writeHeader();
            insertNonFull(root, key, value);
        } else {
            insertNonFull(root, key, value);
        }
    }
    
    bool find(const Key& key, Value& value) {
        if (root == -1) return false;
        
        int currentOffset = root;
        while (true) {
            Node node;
            readNode(node, currentOffset);
            
            int i = 0;
            while (i < node.size && key >= node.keys[i]) {
                if (node.isLeaf && key == node.keys[i]) {
                    return true;
                }
                i++;
            }
            
            if (node.isLeaf) {
                return false;
            }
            currentOffset = node.children[i];
        }
    }
    
    void clear() {
        file.close();
        std::remove(filename.c_str());
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        root = -1;
        nodeCount = 0;
        writeHeader();
    }
    
    template<typename Func>
    void traverse(Func func) {
        if (root == -1) return;
        
        int currentOffset = root;
        Node node;
        readNode(node, currentOffset);
        
        while (!node.isLeaf) {
            currentOffset = node.children[0];
            readNode(node, currentOffset);
        }
        
        while (currentOffset != -1) {
            readNode(node, currentOffset);
            for (int i = 0; i < node.size; i++) {
                func(node.keys[i]);
            }
            currentOffset = node.next;
        }
    }
};

#endif
