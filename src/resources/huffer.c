#include "huffer.h"
#include <stdlib.h>
#define MAX_PROB 0xFFFF

void freeTree(Node* tree) {
    if(tree) {
        struct Node* leftChild = tree->leftChild;
        struct Node* rightChild = tree->rightChild;
        free(tree);
        freeTree(leftChild);
        freeTree(rightChild);
    }
}


Node* addKeyToBinaryTree(char key) {
    if (!KeysRoot) {
        KeysRoot = (Node*)malloc(sizeof(Node));
        if (!KeysRoot) {
            fprintf(stderr,"\nmalloc error on binary tree root initialisation\n");
            exit(EXIT_FAILURE);
        }
        KeysRoot->key.content = key;
        KeysRoot->key.probability++;
        KeysRoot->leftChild = NULL;
        KeysRoot->rightChild = NULL;
        return KeysRoot;
    }

    Node* pred = NULL;
    Node* iterator = KeysRoot;
    while (iterator != NULL) {
        pred = iterator;
        if (key < iterator->key.content)
            iterator = iterator->leftChild;
        else if (key == iterator->key.content) {
            iterator->key.probability++;
            return iterator;
        } else iterator = iterator->rightChild;
    }

    Node* newNode = (Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        fprintf(stderr,"\nmalloc error on adding a new node");
        freeTree(KeysRoot);
        exit(EXIT_FAILURE);
    }
    newNode->key.content = key;
    newNode->key.probability = 1;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;

    if (key < pred->key.content)
        pred->leftChild = newNode;
    else
        pred->rightChild = newNode;
    return newNode;
}

void removeKeyFromBinaryTree(char key) {
    if (KeysRoot == NULL)
        return;
    Node* iterator = KeysRoot;
    Node* pred = NULL;
    while (iterator != NULL) {
        if (iterator->key.content > key) {
            pred = iterator;
            iterator = iterator->leftChild;
        }
        else if (iterator->key.content < key) {
            pred = iterator;
            iterator = iterator->rightChild;
        }
        else {
            if (!iterator->leftChild && !iterator->rightChild) {
                if (pred == NULL) {
                    KeysRoot = NULL;
                    return;
                }
                free(iterator);
                if (key < pred->key.content)
                    pred->leftChild = NULL;
                else pred->rightChild = NULL;
            } else if (iterator->leftChild && iterator->rightChild) {
                Node *leftIterator = iterator->leftChild;
                Node *leftPredIterator = NULL;
                while (leftIterator != NULL) {
                    leftPredIterator = leftIterator;
                    leftIterator = leftIterator->rightChild;
                }
                char newKey = leftPredIterator->key.content;
                int newProbability = leftPredIterator->key.probability;
                removeKeyFromBinaryTree(newKey);
                iterator->key.content = newKey;
                iterator->key.probability = newProbability;
            } else {
                if (iterator->leftChild) {
                    iterator->key.content = iterator->leftChild->key.content;
                    iterator->key.probability = iterator->leftChild->key.probability;
                    Node* leftChild = iterator->leftChild->leftChild;
                    Node* rightChild = iterator->leftChild->rightChild;
                    free(iterator->leftChild);
                    iterator->leftChild = leftChild;
                    iterator->rightChild = rightChild;

                } else {
                    iterator->key.content = iterator->rightChild->key.content;
                    iterator->key.probability = iterator->rightChild->key.probability;
                    Node* leftChild = iterator->rightChild->leftChild;
                    Node* rightChild = iterator->rightChild->rightChild;
                    free(iterator->rightChild);
                    iterator->leftChild = leftChild;
                    iterator->rightChild = rightChild;
                }
            }
        }
    }
}

Node* getSmallestKey(Node* tree) {
    if (tree) {
        Node* leftTree = getSmallestKey(tree->leftChild);
        Node* rightTree = getSmallestKey(tree->rightChild);
        int leftProbability = (leftTree != NULL) ? (leftTree->key.probability) : MAX_PROB;
        int rightProbability = (rightTree != NULL) ? (rightTree->key.probability) : MAX_PROB;
        if (tree->key.probability < rightProbability) {
            if (tree->key.probability < leftProbability)
                return tree;
            else return leftTree;
        } else if (leftProbability <= rightProbability)
            return leftTree;
          else return rightTree;
    } else return NULL;


}

void addData(Node* addedNode) {
    if (addedNode == NULL)
        return;
    data[numberOfData].node = addedNode;
    numberOfData++;
}

void removeData(Node* searchedNode) {
    for (int i = 0; i < numberOfData; i++) {
        if (data[i].node == searchedNode) {
            for (int j = i; j < numberOfData; j++)
                data[j] = data[j + 1];
            numberOfData--;
            break;
        }
    }
}

NodeData getSmallestData() {
    int smallestDataProbability = MAX_PROB;
    NodeData smallestData;
    smallestData.node = NULL;
    for (int i = 0; i < numberOfData; i++) {
        if (data[i].node->key.probability < smallestDataProbability) {
            smallestDataProbability = data[i].node->key.probability;
            smallestData = data[i];
        }
    }
    return smallestData;
}

Node* parseKeysTree() {
    if (KeysRoot == NULL)
        return NULL;
    while (KeysRoot) {
        Node* smallestNode = getSmallestKey(KeysRoot);
        Node* copyNode = (Node* )malloc(sizeof(Node));
        copyNode->key.content = smallestNode->key.content;
        copyNode->key.probability = smallestNode->key.probability;
        copyNode->leftChild = NULL;
        copyNode->rightChild = NULL;
        addData(copyNode);
        removeKeyFromBinaryTree(smallestNode->key.content);
    }
    do {
        NodeData smallestNode = getSmallestData();
        removeData(smallestNode.node);
        NodeData secondSmallestNode = getSmallestData();
        removeData(secondSmallestNode.node);
        Node* newRoot = (Node *)malloc(sizeof(Node));
        newRoot->key.content = '*';
        newRoot->key.isSpecialNode = 1;
        newRoot->leftChild = smallestNode.node;
        newRoot->rightChild = secondSmallestNode.node;
        int leftProbability = (newRoot->leftChild) ? newRoot->leftChild->key.probability : 0;
        int rightProbability = (newRoot->rightChild) ? newRoot->rightChild->key.probability : 0;
        newRoot->key.probability = leftProbability + rightProbability;
        HuffmanRoot = newRoot;
        addData(HuffmanRoot);
    }while(numberOfData > 1);

    return HuffmanRoot;
}

void viewTree(Node* tree) {
    if (tree) {
        viewTree(tree->leftChild);
        printf("%c ", tree->key.content);
        viewTree(tree->rightChild);
    }
}
