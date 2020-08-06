/* Duskul version 1.0.2,  2018.03.25,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#ifndef _STNODE_IMP_H_
#define _STNODE_IMP_H_
#include <stdint.h> // int16_t = short, etc.
#include "stnode.h"
#include "expnode.h"
#include "getitem.h"


struct _funcinfo {
    const char  *ident;
    stnode      *body;
    uint8_t     withbody;    // not prototype
    uint8_t     rtntype;     // 0:proc, 1:func
    int16_t     params;      // number of parameters
    long        defaultValue; // default value by fujiwaramakoto
    bool        hasDefaultValue;
    int16_t     localvars;   // number of local variables
};

struct _assign {                    // assign
    struct _stnode _stnode_common;  // caution!
    uint8_t     global;
    uint16_t    offset;
    int compope;
    expnode     *expr;
};

struct _rtnnode {                   // return
    struct _stnode _stnode_common;  // caution!
    expnode     *expr;
};

struct ifclause {
    expnode        *expr;
    struct _stnode *body;
};

struct _ifnode {                    // if-statement
    struct _stnode _stnode_common;  // caution!
    struct ifclause  clause[1];
};

struct _whilenode {                 // while-statement
    struct _stnode _stnode_common;  // caution!
    expnode        *expr;
    struct _stnode *body;
};

struct _fornode {                   // for-statement
    struct _stnode _stnode_common;  // caution!
    uint8_t         global;         // var is global ?
    uint16_t        offset;         // offset of var
    expnode         *exps[3];
    struct _stnode *body;           // code block
};

struct _varinfo {
    uint8_t     global;     // var is global ?
    uint16_t    offset;     // offset of var list
};

struct _argnode {
    struct _stnode _stnode_common;  // caution!
    uint16_t       offset;          // index of proc / println ?
    union {
        expnode        *xlist[1];   // call, print, println
        varinfo        vlist[1];    // input
    } p; // parameters
};

#endif /* _STNODE_IMP_H_ */
