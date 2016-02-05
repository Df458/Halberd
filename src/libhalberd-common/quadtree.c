#include <dfgame-common.h>
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

void read_quad_node(FILE* cursor, const fpos_t* start, struct quadtree_node* node)
{
    info("Reading node(%ld)\n", ftell(cursor));
    uint32_t buffer[4];
    fread(buffer, sizeof(uint32_t), 4, cursor);

    for(int i = 0; i < 4; ++i) {
        if(buffer[i] == 0) {
            node->children[i] = 0;
        } else {
            node->children[i] = malloc(sizeof(struct quadtree_node));
            fsetpos(cursor, start);
            fseek(cursor, buffer[i], SEEK_CUR);
            read_quad_node(cursor, start, node->children[i]);
        }
    }
}

// TODO: Write the position of leaf data
void write_quad_node(FILE* cursor, const fpos_t* start, const struct quadtree_node* node, uint32_t* end_cursor)
{
    info("Writing node(%ld)\n", ftell(cursor));
    uint32_t buffer[4] = { 0 };
    if(node->children[0]) {
        buffer[0] = *end_cursor;
        *end_cursor += sizeof(uint32_t) * 4;
    }
    if(node->children[1]) {
        buffer[1] = *end_cursor;
        *end_cursor += sizeof(uint32_t) * 4;
    }
    if(node->children[2]) {
        buffer[2] = *end_cursor;
        *end_cursor += sizeof(uint32_t) * 4;
    }
    if(node->children[3]) {
        buffer[3] = *end_cursor;
        *end_cursor += sizeof(uint32_t) * 4;
    }
    fwrite(buffer, sizeof(uint32_t), 4, cursor);
    for(int i = 0; i < 4; ++i) {
        if(node->children[i]) {
            fsetpos(cursor, start);
            fseek(cursor, buffer[i], SEEK_CUR);
            write_quad_node(cursor, start, node->children[i], end_cursor);
        }
    }
}

// TODO: write dims
void quadtree_write(FILE* cursor, const quadtree* tree)
{
    // Write the tree depth
    fputc(tree->depth, cursor);
    uint32_t end_cursor = ftell(cursor) + (sizeof(uint32_t) * 4) + 1;
    fpos_t pos;
    fgetpos(cursor, &pos);
    write_quad_node(cursor, &pos, tree->root, &end_cursor);
}

// TODO: read dims
void quadtree_read(FILE* cursor, quadtree* tree)
{
    tree = malloc(sizeof(quadtree));
    tree->depth = fgetc(cursor);
    tree->root = malloc(sizeof(struct quadtree_node));

    fpos_t pos;
    fgetpos(cursor, &pos);
    read_quad_node(cursor, &pos, tree->root);
}
