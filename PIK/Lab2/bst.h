#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>

// Binary tree node definition
typedef struct node {
    float key;
    struct node *left;
    struct node *right;
} node_t;

// Float random number generator
float rand_float() {
    return (float)(rand()) / (float)(RAND_MAX);
}

// Create a new node
node_t *create_node(float key) {
    node_t *n = malloc(sizeof(node_t));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->key = key;
    n->left = NULL;
    n->right = NULL;
    return n;
}


// Sequential BST insert
node_t *bst_insert(node_t *root, float key) {
    if (root == NULL)
        return create_node(key);

    if (key < root->key)
        root->left = bst_insert(root->left, key);
    else if (key > root->key)
        root->right = bst_insert(root->right, key);

    return root;
}

// Deterministic randomized tree generation
node_t *generate_tree(int seed, int num_nodes) {
    node_t *root = NULL;

    srand(seed);

    for (int i = 0; i < num_nodes; i++) {
        float key = rand_float();
        root = bst_insert(root, key);
    }

    return root;
}

// Free the tree
void free_tree(node_t *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

#endif // BST_H