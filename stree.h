/*
 * stree.h, part of "trees" project.
 *
 *  Created on: 10.06.2015, 02:52
 *      Author: Vsevolod Lutovinov <klopp@yandex.ru>
 */

#ifndef STREE_H_
#define STREE_H_

#include "tree.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _STNode {
    int key;
    void *data;
    struct _STNode *left;
    struct _STNode *right;
} *STNode;

typedef void (*ST_Walk)(STNode node, void *data);

typedef struct _STree {
    Tree_Flags flags;
    Tree_Destroy destructor;
    size_t nodes;
    STNode head;
} *STree;

STree ST_create(Tree_Flags flags, Tree_Destroy destructor);
void ST_clear(STree tree);
void ST_destroy(STree tree);

size_t ST_depth(STree tree);

STNode ST_insert(STree tree, int key, void *data);
int ST_delete(STree tree, int key);
STNode ST_search(STree tree, int key);

void ST_walk(STree tree, ST_Walk walker, void *data);
int ST_dump(STree tree, Tree_DataDump dumper, FILE *handle);

#ifdef __cplusplus
}
#endif

#endif /* STREE_H_ */
