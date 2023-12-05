
#include "Tries.h"
#include "LRU.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
void Delete_key(struct TrieNode *root, char *key, LRUCache* obj);

char* Get_value(struct TrieNode *root, char *key, LRUCache* obj);