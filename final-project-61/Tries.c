#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tries.h"

struct TrieNode *new_object()
{
    struct TrieNode *new_node = (struct TrieNode *)malloc(sizeof(struct TrieNode));

    new_node->wordEndCnt = 0;
    new_node->value = NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        new_node->childNode[i] = NULL;
    }
    // new_node->lock = PTHREAD_MUTEX_INITIALIZER;
    // new_node->writelock = PTHREAD_MUTEX_INITIALIZER;
    if (pthread_mutex_init(&(new_node->lock), NULL) != 0)
    {
        perror("pthread_mutex_init() failed");
        return NULL;
    }

    if (pthread_mutex_init(&(new_node->writelock), NULL) != 0)
    {
        perror("pthread_mutex_init() failed");
        pthread_mutex_destroy(&(new_node->lock));
        return NULL;
    }
    new_node->readers = 0;
    return new_node;
}

struct TrieNode *insert_key(struct TrieNode *root, char *key, char *value)
{
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            struct TrieNode *newNode = new_object();
            currentNode->childNode[key[i]] = newNode;
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    // Increment wordEndCnt for the last currentNode pointer as it points to the end of the key.
    currentNode->wordEndCnt++;

    // Set the value associated with the key
    if (currentNode->value != NULL)
    {
        free(currentNode->value); // Free previously allocated value if present
    }
    currentNode->value = strdup(value); // Copy the value into the node

    return root;
}

char *get_value(struct TrieNode *root, char *key)
{
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            return NULL; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0)
    {
        return currentNode->value; // Return the value associated with the key
    }

    return NULL; // Key not found
}

bool delete_key(struct TrieNode *root, char *key)
{
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            return false; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0)
    {
        currentNode->wordEndCnt--;

        // Clear the value associated with the key
        if (currentNode->value != NULL)
        {
            free(currentNode->value);
            currentNode->value = NULL;
        }

        return true;
    }

    return false; // Key not found
}

void freeTrie(struct TrieNode *root)
{
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        freeTrie(root->childNode[i]);
    }

    if (root->value != NULL)
    {
        free(root->value);
    }

    free(root);
}

// LOCK FUNCTIONS
pthread_mutex_t *get_lock(struct TrieNode *root, char *key)
{
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            return NULL; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0)
    {
        return &currentNode->lock; // Return the value associated with the key
    }

    return NULL; // Key not found
}

pthread_mutex_t *get_writelock(struct TrieNode *root, char *key)
{
    struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            return NULL; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0)
    {
        return &currentNode->writelock; // Return the value associated with the key
    }

    return NULL; // Key not found
}

struct  TrieNode* get_node(struct TrieNode *root, char *key){

     struct TrieNode *currentNode = root;
    int i = 0;

    while (key[i] != '\0')
    {
        if (currentNode->childNode[key[i]] == NULL)
        {
            return NULL; // Key not found
        }
        currentNode = currentNode->childNode[key[i]];
        i++;
    }

    if (currentNode != NULL && currentNode->wordEndCnt > 0)
    {
        return currentNode; // Return the value associated with the key
    }

    return NULL; // Key not found

    /* data */
}


void acquire_readlock(struct TrieNode *node)
{
    pthread_mutex_lock(&node->lock);
    node->readers++;
    if (node->readers == 1)
    {
        // DISABLE WRITERS TO ENTER
        pthread_mutex_lock(&node->writelock);
    }
    pthread_mutex_unlock(&node->lock);
}

void release_readlock(struct TrieNode *node)
{
    pthread_mutex_lock(&node->lock);
    node->readers--;
    if (node->readers == 0)
    {
        // ENABLE WRITERS TO ENTER
        pthread_mutex_unlock(&node->writelock);
    }
    pthread_mutex_unlock(&node->lock);
}

void acquire_writelock(struct TrieNode *node)
{
    pthread_mutex_lock(&node->writelock);
}
void release_writelock(struct TrieNode *node)
{
    pthread_mutex_unlock(&node->writelock);
}

// int main() {
//     struct TrieNode *root = new_object();

//     // Inserting key-value pairs
//     root = insert_key(root, "ap/.ple", "A fruit");
//     root = insert_key(root, "banana", "Another fruit");
//     root = insert_key(root, "app", "Application");

//     // Retrieving values
//     char *value1 = get_value(root, "ap/.ple");
//     char *value2 = get_value(root, "banana");
//     char *value3 = get_value(root, "app");

//     printf("Value for 'apple': %s\n", value1); // Output: Value for 'apple': A fruit
//     printf("Value for 'banana': %s\n", value2); // Output: Value for 'banana': Another fruit
//     printf("Value for 'app': %s\n", value3); // Output: Value for 'app': Application

//     // Deleting a key
//     bool deleted = delete_key(root, "ap/.ple");
//     if (deleted) {
//         printf("Deleted 'apple'\n");
//     }

//     // Retrieving the value after deletion
//     char *deletedValue = get_value(root, "ap/.ple");
//     if (deletedValue == NULL) {
//         printf("Value for 'apple' not found (after deletion)\n");
//     }

//     // Freeing the memory used by the Trie
//     freeTrie(root);

//     return 0;
// }
