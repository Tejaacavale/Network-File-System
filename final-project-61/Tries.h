// Implementation of Trie functions.
// Tries are preferred over hashmaps because they are superior in space complexity for large datasets.
#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <pthread.h>
#define ALPHABET_SIZE 128

struct TrieNode
{
    struct TrieNode *childNode[ALPHABET_SIZE];
    int wordEndCnt;
    char *value;

    pthread_mutex_t lock;
    pthread_mutex_t writelock;

    int readers;
};

// Function to create a new Trie node
struct TrieNode *new_object();

// Function to insert a key-value pair into the Trie
struct TrieNode *insert_key(struct TrieNode *root,   char *key,   char *value);

// Function to get the value associated with a given key in the Trie
// Returns the value if key exists, otherwise NULL
char *get_value(struct TrieNode *root,   char *key);

// Function to delete a key-value pair from the Trie
// Returns true if key found and deleted, false otherwise
bool delete_key(struct TrieNode *root,   char *key);

// Function to free memory used by the Trie
void freeTrie(struct TrieNode *root);

struct  TrieNode *get_node(struct TrieNode *root,   char *key);

// LOCK FUNCTIONS
pthread_mutex_t *get_lock(struct TrieNode *root,   char *key);
pthread_mutex_t *get_writelock(struct TrieNode *root,   char *key);

void acquire_readlock(struct TrieNode *node);
void release_readlock(struct TrieNode *node);

void acquire_writelock(struct TrieNode *node);
void release_writelock(struct TrieNode *node);

#endif /* TRIE_H */
