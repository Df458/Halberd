#ifndef QUADTREE_H
#define QUADTREE_H
#include <inttypes.h>
#include <string.h>

struct quadtree_node
{
    struct quadtree_node* children[4];
    size_t dimension;
};

typedef struct quadtree
{
    struct quadtree_node* root;
    size_t depth;
} quadtree;

quadtree* quadtree_make(size_t w, size_t h);
// TODO: Add a delete function to find a leaf and remove it recursively
void quadtree_insert(quadtree* tree, size_t x, size_t y);
void quadtree_destroy(quadtree* tree);

#endif
