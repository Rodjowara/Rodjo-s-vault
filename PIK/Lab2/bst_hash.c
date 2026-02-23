#include "bst.h"
#include "custom_hash.h"
#include <stdio.h>
#include <omp.h>

hash_t tree_hash_seq(node_t *root){

    if (root == NULL)
        return 0;

    node_t *left = root->left;
    node_t *right = root->right;
    hash_t left_hash = hash_empty(); 
    hash_t right_hash = hash_empty();

    if(left != NULL){
        left_hash = tree_hash_seq(left);
    }

    if(right != NULL){
        right_hash = tree_hash_seq(right);
    }

    return hash_combine(left_hash, root->key, right_hash);
}

hash_t tree_hash_par(node_t *root, int depth){

    if (root == NULL)
        return 0;

    node_t *left = root->left;
    node_t *right = root->right;
    hash_t left_hash = hash_empty();
    hash_t right_hash = hash_empty();
    int depth_rem = depth - 1;

    if(depth_rem < 0){
        if(left != NULL){
            left_hash = tree_hash_seq(left);
        }

        if(right != NULL){
            right_hash = tree_hash_seq(right);
        }
    }else{
        #pragma omp task shared(left_hash)
        {
            if(left != NULL){
                left_hash = tree_hash_par(left, depth_rem);
            }
        }

        #pragma omp task shared(right_hash)
        {
            if(right != NULL){
                right_hash = tree_hash_par(right, depth_rem);
            }
        }

        #pragma omp taskwait
    }
    return hash_combine(left_hash, root->key, right_hash);
}

int main(int argc, char *argv[]){
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <seed> <num_nodes>\n", argv[0]);
        return 1;
    }

    int seed = atoi(argv[1]);
    int num_nodes = atoi(argv[2]);

    node_t *root = generate_tree(seed, num_nodes);
    double start_seq, end_seq, start_par, end_par;
    hash_t hash_seq, par_hash;

    start_seq = omp_get_wtime();
    hash_seq = tree_hash_seq(root);
    end_seq = omp_get_wtime();

    start_par = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            par_hash = tree_hash_par(root, 10);
        }
    }
    end_par = omp_get_wtime();

    printf("0x%016lx\n", par_hash);
    printf("%.6f\n", end_seq - start_seq);
    printf("%.6f\n", end_par - start_par);

    return 0;
}