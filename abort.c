/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getitem.h"
#include "identifiers.h"
#include "strliteral.h"
#include "abort.h"

#define SHORTEN_LEN  80

/// Shorten strings, taking UTF-8 encoding into account.
const char *shorten(const char *s, int limit)
{
    static char buf[SHORTEN_LEN + 8];
    int i, cc = 0;
    int lim = (SHORTEN_LEN < limit) ? SHORTEN_LEN : limit;
    for (i = 0; (cc = s[i]) != 0 && (i < lim || (cc & 0xc0) == 0x80); i++)
        buf[i] = cc; /* Refer to the encoding of UTF-8 */
    if (cc)
        for (int k = 0; k < 3; k++) buf[i++] = '.';
    buf[i] = 0;
    return buf;
}

static void itemToString(char *buffer, const item *p)
{
    const char *str;
    token_t t = p->token;
    switch (t) {
        case tok_id:
            sprintf(buffer, "[id \"%s\"]", p->a.recptr->str);
            break;
        case tok_num:
            sprintf(buffer, "[int %ld]", p->a.value); break;
        case tok_str:
            str = shorten(literalString((int)p->a.value), 20);
            sprintf(buffer, "[string \"%s\"]", str); break;
        default:
            if (t < combined_symbol_0)
                sprintf(buffer, "[symbol '%c']", (int)p->kind);
            else if (t < reserved_word_0) {
                static const char *tab[] = { "<=", ">=", "==", "<>" , "+=", "-=", "*=", "/=", "%=", "&&", "||"};
                sprintf(buffer, "[symbol '%s']", tab[t - combined_symbol_0]);
            }else if (t < all_normal_symbols) {
                sprintf(buffer, "[token '%s']", reservedWord(t));
            }else
                sprintf(buffer, "[token ILL/EOF]");
            break;
    }
}

#if !defined(NDEBUG)
void printItem(const item *p)
{
    char buffer[80];
    itemToString(buffer, p);
    fprintf(stderr, "%s\n", buffer);
}
#endif

/* defined in errmessages.c */
const char *getErrorMessage(const char *key);
/**
 エラーメッセジをstderrに出力してプログラムを終了
 */
static void abortMessage_string(const char *msg, const char *str)
{
    fprintf(stderr, "ERROR: %s", getErrorMessage(msg));
    if (str)
        fprintf(stderr, " %s", str);
    fputc('\n', stderr);
    if (stdtxin) {
        printTextBuffer(stderr, stdtxin);
        freeTextBuffer(stdtxin);
        stdtxin = NULL;
    }
    exit(1); // === EXIT THE PROGRAM ===
}

void abortMessage(const char *msg) {
    abortMessage_string(msg, NULL);
}

void abortMessageWithToken(const char *msg, const item *s)
{
    char buffer[80];
    itemToString(buffer, s);
    abortMessage_string(msg, buffer);
}

void abortMessageWithString(const char *msg, const char *str)
{
    char buffer[40];
    sprintf(buffer, "'%s'", shorten(str, 32));
    abortMessage_string(msg, buffer);
}
