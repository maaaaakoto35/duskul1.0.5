/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#if !defined(__IDENTIFIERS_H__)
#define __IDENTIFIERS_H__

#include <stdint.h>
#include "getitem.h"

#define MAX_IDENTIFIER_LENGTH   127     // must be less than 255

struct _idtable;
typedef struct _idtable     idtable;

struct string_info {
    int32_t     hashvalue;
    uint8_t     length;
    uint8_t     to_be_defined;
    const char  *name;
};
typedef const struct string_info *stringInfoPtr;

struct _idrecord {
    uint8_t     kind;    // idkind_t
    uint8_t     length;  // length of identifier
    int16_t     order;   // index of variable, func, or proc
    // If the table is for reserved words, 'order' has the value of token_t.
    const char     *str;
};

extern int currentLocalOffset;
extern int maxLocalOffset;

const char *reservedWord(int sym);

void idtablesInitialize(void);
void idtablesFree(void);
struct string_info makeStringInfo(const char *str, bool newid);
item identifierSearch(stringInfoPtr strp);
idRecord *addIdentifier(stringInfoPtr strp);

void resetLocalList(void);
void blockNestPush(void);
void blockNestPop(void);

#endif /* !__IDENTIFIERS_H__ */
