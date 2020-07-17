/* Duskul version 1.0.2,  2018.03.25,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#ifndef _STNODE_H_
#define _STNODE_H_

#include <stdint.h>

typedef enum {
    node_assign, node_call, node_print, node_input,
    node_break, node_return, node_if, node_while, node_for,
} nodetype;

typedef struct _stnode {
    struct _stnode *next;
    uint8_t     kind;    // nodetype
    uint16_t    count;   // the number of args / vars
} stnode;

struct _funcinfo;
typedef struct _funcinfo    funcinfo;
struct _assign;
typedef struct _assign      assignnode;
struct _rtnnode;
typedef struct _rtnnode     rtnnode;
struct _ifnode;
typedef struct _ifnode      ifnode;
struct _whilenode;
typedef struct _whilenode   whilenode;
struct _fornode;
typedef struct _fornode     fornode;
struct _varinfo;
typedef struct _varinfo     varinfo;
struct _argnode;
typedef struct _argnode     argnode;

#endif /* _STNODE_H_ */
