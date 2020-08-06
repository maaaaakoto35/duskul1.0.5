/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.4,  2020.06.23 */
#include <stdlib.h>
#include <assert.h>
#include "getsymbol.h"
#include "identifiers.h"
#include "abort.h"

static item unget_store;
static int unget_flag = false;

void ungetItem(item s)
{
    assert(unget_flag == false);
    unget_store = s;
    unget_flag = true;
}

/// if current_only: not search the global table for the identifier.
///     New identifier may be added to the current table.
/// else: the global table is also used.  No identifier is added.
static item get_identifier(int ch, bool current_only, TIN *tip)
{
    int index = 0;
    char buf[MAX_IDENTIFIER_LENGTH];
    int cc, attr;

    buf[index++] = ch;
    cc = nextch(tip);
    while ((attr = chAttribute(cc)) == ca_alpha || attr == ca_digit) {
        if (index >= MAX_IDENTIFIER_LENGTH - 1) {
            buf[MAX_IDENTIFIER_LENGTH - 1] = 0;
            abortMessageWithString("long ident", buf);
        }
        buf[index++] = cc;
        cc = nextch(tip);
    }
    if (cc != EOF) undoch(cc, tip);
    buf[index] = 0;

    struct string_info strinf = makeStringInfo(buf, current_only);
    item ent = identifierSearch(&strinf);
    if (ent.kind == id_undefined) { // name in 'buf' is undefined
        if (!current_only) // Error
            abortMessageWithString("undef id", buf);
        idRecord *rec = addIdentifier(&strinf);
        assert(rec != NULL);
        rec->kind = id_new;
        ent.a.recptr = rec;
    }
    return ent;
}

/// Get a token from the source program.
/// if current_only: not search the global table for the identifier.
item fgetItem(TIN *tip, bool current_only) {
    if (unget_flag) {
        unget_flag = false;
        if (unget_store.token == tok_id && unget_store.kind == id_new) {
            idRecord *rec = unget_store.a.recptr;
            assert(rec != NULL);
            unget_store.kind = rec->kind;
            unget_store.offset = rec->order;
        }
        return unget_store;
        // This is a preventive coding part.
        // For example, in the following case, the variable 'a' in
        // the second line may be regarded as an undefined identifier.
        // Note that this doesn't cause any problem with Duskul 1.0.3,
        // but it would be a difficult bug when extending language
        // features.
        //     var a
        //     a = 10
        // Immediately after reading 'a', the next token is gotten
        // by getItem(), and it is 'a' again. Then it is pushed back
        // by ungetItem(), because it isn't a '[' or something.
        // At this point, 'a' is handed back as undefined (id_new), so
        // the next getItem() needs to return the defined information.
    }

    item s;
    int ch;
    chattr_t attr;
    do {
        ch = nextch(tip);
        attr = chAttribute(ch);
    }while (attr == ca_blank);
    switch (attr) {
        case ca_sym:
            return getsymbol(ch, tip);
        case ca_quot:
            s.token = tok_str;
            s.a.value = get_string(tip);
            return s;
        case ca_digit:
            s.token = tok_num;
            s.a.value = getnumber(ch, tip);
            return s;
        case ca_alpha: return get_identifier(ch, current_only, tip);
        default:
            break;
    }
    if (isEOF(tip))
        abortMessage("unexp EOF");
    s.token = tok_ILL;
    abortMessage("illegal character");
    return s;
}

bool fgetEOF(TIN *tip) {
    int ch;
    do {
        ch = nextch(tip);
        if (ch == EOF) return true;
    }while (chAttribute(ch) == ca_blank);
    undoch(ch, tip);
    return false;
}
