#include "Caching.h"
char* Get_value(struct TrieNode *root, char *key, LRUCache* obj){
    char* value=get(obj,key);
    if(value != NULL){
        return value;
    }
    value=get_value(root,key);
    put(obj,key,value);
    return value;
}

void Delete_key(struct TrieNode *root, char *key, LRUCache* obj){
    delete_key(root,key);
    delete(obj,key);
}
