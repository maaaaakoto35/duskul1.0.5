/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#if !defined(__ABORT_H__)
#define __ABORT_H__

struct _item;

const char *shorten(const char *s, int limit);

void abortMessage(const char *msg);
void abortMessageWithToken(const char *msg, const struct _item *s);
void abortMessageWithString(const char *msg, const char *str);

#endif /* !__ABORT_H__ */
