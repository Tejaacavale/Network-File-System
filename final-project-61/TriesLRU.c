#include "Tries.h"
#include "LRU.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct TrieNode* new_object() {
    struct TrieNode *new_node = (struct TrieNode*)malloc(sizeof(struct TrieNode));

    new_node->wordEndCnt = 0;
    new_node->value = NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        new_node->childNode[i] = NULL;
    }

    return new_node;
}

struct TrieNode* insert_key(struct TrieNode *root, char *key, const char *value) {
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0') {
        if (currentNode->childNode[key[i]] == NULL) {
            struct TrieNode *newNode = new_object();
            currentNode->childNode[key[i]] = newNode;
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    // Increment wordEndCnt for the last currentNode pointer as it points to the end of the key.
    currentNode->wordEndCnt++;

    // Set the value associated with the key
    if (currentNode->value != NULL) {
        free(currentNode->value); // Free previously allocated value if present
    }
    currentNode->value = strdup(value); // Copy the value into the node

    return root;
}

char* get_value(struct TrieNode *root, char *key) {
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0') {
        if (currentNode->childNode[key[i]] == NULL) {
            return NULL; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0) {
        return currentNode->value; // Return the value associated with the key
    }

    return NULL; // Key not found
}

bool delete_key(struct TrieNode *root, char *key) {
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0') {
        if (currentNode->childNode[key[i]] == NULL) {
            return false; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0) {
        currentNode->wordEndCnt--;

        // Clear the value associated with the key
        if (currentNode->value != NULL) {
            free(currentNode->value);
            currentNode->value = NULL;
        }

        return true;
    }

    return false; // Key not found
}

void freeTrie(struct TrieNode *root) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(root->childNode[i]);
    }

    if (root->value != NULL) {
        free(root->value);
    }

    free(root);
}

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

int hashFunc(const char* key) {
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

void printCache(LRUCache* obj) {
    Node* currentNode = obj->head->next;
    while (currentNode != obj->tail) {
        printf("%s %s\n", currentNode->key, currentNode->value);
        currentNode = currentNode->next;
    }
}

void printWord(char* str, int n)
{
    printf("\n");
    for(int i=0; i<n; i++)
    {
        printf("%c",str[i]);
    }
}

void printAllWords(struct TrieNode* root, char* wordArray, int pos)
{
   if(root == NULL)
      return;
   if(root->wordEndCnt)
   {
      printWord(wordArray, pos);
   }
   for(int i=0; i<ALPHABET_SIZE; i++)
   {
      if(root->childNode[i] != NULL)
      {
         wordArray[pos] = i;
         printAllWords(root->childNode[i], wordArray, pos+1);
      }
   }
}

char* Get_value(struct TrieNode *root, const char *key, LRUCache* obj){
    char* value=get(obj,key);
    if(value != NULL){
        return value;
    }
    value=get_value(root,key);
    put(obj,key,value);
    return value;
}

void Delete_key(struct TrieNode *root, const char *key, LRUCache* obj){
    delete_key(root,key);
    delete(obj,key);
}

int main(){
    LRUCache* cache=createLRUCache(5);
    struct Trienode* root=new_object();
    insert_key(root,"key1","value1");
    printf("%s\n",Get_value(root,"key1",cache));
    printCache(cache);
    printf("\n");
    insert_key(root,"key2","value2");
    Get_value(root,"key2",cache);
    printCache(cache);
    printf("\n");
    Delete_key(root,"key1",cache);
    printCache(cache);
    printf("\n");
}