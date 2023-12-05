#ifndef LRUCACHE_H
#define LRUCACHE_H

struct Node {
    char* key;
    char* value;
    struct Node* prev;
    struct Node* next;
};

typedef struct Node Node;

struct LRUCache {
    int capacity;
    Node* head;
    Node* tail;
    Node** cache;
};

typedef struct LRUCache LRUCache;

Node* createNode(char* key, char* value);
LRUCache* createLRUCache(int capacity);
void removeNode(Node* node);
void addToHead(Node* node, Node* head);
int hashFunc(char* key);
char* get(LRUCache* obj, char* key);
void put(LRUCache* obj, char* key, char* value);
void freeLRUCache(LRUCache* obj);
void delete(LRUCache* obj, char* key);
#endif /* LRUCACHE_H */
