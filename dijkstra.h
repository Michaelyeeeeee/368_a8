#pragma once

#include <stdio.h>
#include <limits.h>

typedef struct _tnode
{
    short label;
    int distance;
    short predecessor;
} tnode;

typedef struct _gnode
{
    short label;
    short weight;
    struct gnode *next;
} gnode;