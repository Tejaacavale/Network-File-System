#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LRU.h"

Node* createNode(char* key, char* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

LRUCache* createLRUCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->cache = (Node**)malloc(sizeof(Node*) * 10000); // Assuming key values are less than 10000
    cache->head = createNode("head", "");
    cache->tail = createNode("tail", "");
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    return cache;
}

void removeNode(Node* node) {
    Node* prev = node->prev;
    Node* nextNode = node->next;
    prev->next = nextNode;
    nextNode->prev = prev;
}

void addToHead(Node* node, Node* head) {
    Node* nextNode = head->next;
    head->next = node;
    node->prev = head;
    node->next = nextNode;
    nextNode->prev = node;
}

void delete(LRUCache* obj, char* key) {
    int hash = hashFunc(key);
    if (obj->cache[hash] != NULL) {
        Node* node = obj->cache[hash];
        if (strcmp(node->key, key) == 0) {
            removeNode(node);
            obj->cache[hash] = NULL;
            free(node->key);
            free(node->value);
            free(node);
            obj->capacity++;
        }
    }
}

int hashFunc(char* key) {
    unsigned long hash = 5381; // Initializing hash with a prime number
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return (int)(hash % 10000); // Assuming key values are less than 10000
}

char* get(LRUCache* obj, char* key) {
    int hash = hashFunc(key);
    if (obj->cache[hash] != NULL) {
        Node* node = obj->cache[hash];
        if (strcmp(node->key, key) == 0) {
            removeNode(node);
            addToHead(node, obj->head);
            return node->value;
        }
    }
    return NULL;
}

void put(LRUCache* obj, char* key, char* value) {
    int hash = hashFunc(key);
    if (obj->cache[hash] != NULL) {
        Node* node = obj->cache[hash];
        if (strcmp(node->key, key) == 0) {
            free(node->value);
            node->value = strdup(value);
            removeNode(node);
            addToHead(node, obj->head);
            return;
        }
    }
    Node* newNode = createNode(key, value);
    if (obj->capacity <= 0) {
        Node* tailPrev = obj->tail->prev;
        removeNode(tailPrev);
        obj->cache[hashFunc(tailPrev->key)] = NULL;
        free(tailPrev->key);
        free(tailPrev->value);
        free(tailPrev);
        obj->capacity++;
    }
    addToHead(newNode, obj->head);
    obj->cache[hash] = newNode;
    obj->capacity--;
}

void freeLRUCache(LRUCache* obj) {
    for (int i = 0; i < 10000; i++) {
        if (obj->cache[i] != NULL) {
            free(obj->cache[i]->key);
            free(obj->cache[i]->value);
            free(obj->cache[i]);
        }
    }
    free(obj->cache);
    free(obj->head->key);
    free(obj->head->value);
    free(obj->tail->key);
    free(obj->tail->value);
    free(obj->head);
    free(obj->tail);
    free(obj);
}

// int main() {
//     LRUCache* cache = createLRUCache(5);
// // Hash size should be less than 10000.
//     put(cache, "key1", "value1");
//     put(cache, "key2", "value2");
//     printf("%s\n", get(cache, "key1")); 
//     put(cache, "key3", "value3");
//     printf("%s\n", get(cache, "key2")); 
//     put(cache, "key4", "value4");
//     printf("%s\n", get(cache, "key1")); 
//     printf("%s\n", get(cache, "key3")); 
//     printf("%s\n", get(cache, "key4")); 

//     freeLRUCache(cache);
//     return 0;
// }
