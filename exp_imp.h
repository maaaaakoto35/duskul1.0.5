/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#if !defined(__EXP_IMP_H__)
#define __EXP_IMP_H__

#include <stdint.h>
#include "getitem.h"
#include "stnode_imp.h"

#define ExpTree     tok_ILL // mark of 'oprExpnode'

// Note: expnode, oprExpnode, and argExpnode have the common part:
// kind and prefix.

struct _expnode {
    token_t  kind;      // tok_id:variable, tok_num:int, tok_str: string-literal
    token_t  prefix;    // 0, sym_plus, sym_minus, sym_not
    union {
        int64_t     intvalue;   // if kind == tok_num, or tok_str
        varinfo     varinf;     // if kind == tok_id
    } v;
};

struct _oprExpnode {
    token_t  kind;      // operators: sym_ast, sym_plus, ...
    token_t  prefix;    // always ExpTree
    expnode  *operand[2];
};

struct _argExpnode {
    token_t  kind;      // sym_func
    token_t  prefix;    // 0, sym_plus, sym_minus, sym_not
    int16_t  index;     // index of functionsTable
    int16_t  count;     // number of arguments
    expnode  *args[1];
};

#endif /* !__EXP_IMP_H__ */
