#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "quadtree.h"

quadtree* quadtree_make(size_t w, size_t h)
{
    quadtree* tree = malloc(sizeof(quadtree));

    // Finds the closest power of 2, roundiong up
    size_t d = w > h ? w : h;
    d--;
    d |= d >> 1;
    d |= d >> 2;
    d |= d >> 4;
    d |= d >> 8;
    d |= d >> 16;
    d++;
    tree->depth = log2(d);
    tree->root = malloc(sizeof(struct quadtree_node));
    tree->root->dimension = d;
    for(int i = 0; i < 4; ++i)
        tree->root->children[i] = 0;
    return tree;
}

bool is_root(struct quadtree_node* node)
{
    return !node->children[0] && !node->children[1] && !node->children[2] && !node->children[3];
}

uint8_t get_containing(size_t x, size_t y, size_t d)
{
    return (x < d / 2 ? 0 : 1) + (y < d / 2 ? 0 : 2);
}

void quadtree_insert(quadtree* tree, size_t x, size_t y)
{
    struct quadtree_node* node = tree->root;
    for(int i = 0; i < tree->depth; ++i) {
        uint8_t node_id = get_containing(x, y, node->dimension);
        x -= node->dimension / 2;
        y -= node->dimension / 2;

        if(!node->children[node_id]) {
            node->children[node_id] = malloc(sizeof(struct quadtree_node));
            node->children[node_id]->dimension = node->dimension / 2;
            for(int i = 0; i < 4; ++i)
                node->children[node_id]->children[i] = 0;
        }

        node = node->children[node_id];
    }
}

void quadtree_freestep(struct quadtree_node* node)
{
    if(!is_root(node)) {
        for(uint8_t i = 0; i < 4; ++i)
            if(node->children[i]) {
                quadtree_freestep(node->children[i]);
                node->children[i] = 0;
            }
    }
    free(node);
}

void quadtree_destroy(quadtree* tree)
{
    quadtree_freestep(tree->root);
    free(tree);
}
