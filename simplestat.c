/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.3,  2019.06.01 */
#include <assert.h>
#include "getitem.h"
#include "identifiers.h"
#include "statements.h"
#include "stnode_imp.h"
#include "expression.h"
#include "abort.h"
#include "evaluate.h"
#include "expnode.h"

static void chechAssignment(idkind_t kind, const char *str)
{
    switch (kind) {
        case id_param:
            abortMessageWithString("assign param", str); break;
        case id_func:
        case id_proc:
            abortMessageWithString("assign func", str); break;
        case id_undefined:
        case id_new:
            abortMessageWithString("undef id", str); break;
        default:
            break;
    }
}

stnode *assignStatement(item ahead, symset_t terminator)
{
    chechAssignment(ahead.kind, "assign");

    item s = getItem();

    stnode *statmp = newNode(node_assign);
    assignnode *ap = (assignnode *)statmp;

    if(s.token == sym_pluseq || s.token == sym_minuseq ||s.token == sym_asteq || s.token == sym_slseq || s.token == sym_pcnteq){
        expnode *termcp = varTerm(BOOL(ahead.kind == id_static_v), ahead.offset);
        expnode *termp = expression();

        switch (s.token) {
            case sym_pluseq:
                ap->expr = newOprnode(sym_plus, termcp, termp);
                break;
            case sym_minuseq:
                ap->expr = newOprnode(sym_minus, termcp, termp);
                break;
            case sym_asteq:
                ap->expr = newOprnode(sym_ast, termcp, termp);
                break;
            case sym_slseq:
                ap->expr = newOprnode(sym_sls, termcp, termp);
                break;
            case sym_pcnteq:
                ap->expr = newOprnode(sym_pcnt, termcp, termp);
                break;
            default:
                break;
        }
        ap->global = BOOL(ahead.kind == id_static_v);
        ap->offset = ahead.offset;

    }else if(s.token == sym_eq){
        expnode *termp = expression();
        ap->expr = termp;
        ap->global = BOOL(ahead.kind == id_static_v);
        ap->offset = ahead.offset;
    }else if (s.token != sym_eq) {
        abortMessageWithToken("no equal", &s);
    }

    s = getItem();
    if (!symsetHas(terminator, s.token))
        abortMessageWithToken("illegal tail", &s);
    ungetItem(s);
    return statmp;
}

stnode *returnStatement(symset_t terminator)
{
    expnode *termp = valueIsReturned ? expression() : NULL;
    stnode *statmp = newNode(node_return);
    ((rtnnode *)statmp)->expr = termp;
    item s = getItem();
    if (!symsetHas(terminator, s.token))
        abortMessageWithToken("illegal tail", &s);
    ungetItem(s);
    return statmp;
}

stnode *inputStatement(void)
{
    varinfo buffer[PARAM_MAX];
    item s = getItem();
    if (s.token != sym_lpar)
        abortMessageWithToken("no left paren", &s);
    int args = 0;
    for ( ; ; ) {
        item v = getItem();
        if (v.token != tok_id)
            abortMessageWithToken("no id", &v);
        chechAssignment(v.kind, "intput");
        buffer[args].global = BOOL(v.kind == id_static_v);
        buffer[args].offset = v.offset;
        args++;
        s = getItem();
        if (s.token != sym_comma) break;
        if (args >= PARAM_MAX)
            abortMessageWithString("wrong arg num", "input");
    }
    if (s.token != sym_rpar)
        abortMessageWithToken("no right paren", &s);

    stnode *stp = newNodeExpand(node_input, args);
    stp->count = args;
    argnode *anp = (argnode *)stp;
    anp->offset = 0;
    if (args > 0) {
        varinfo *vlist = anp->p.vlist;
        for (int i = 0; i < args; i++)
            vlist[i] = buffer[i];
    }
    return stp;
}

stnode *forStatement(void)
{
    stnode *stp = newNode(node_for);
    fornode *fop = (fornode *)stp;
    blockNestPush();
    item s = getItem();
    if (s.token == sym_var) {
        s = getItemLocal();
        if (s.token != tok_id)
            abortMessageWithToken("no id", &s);
        assert(s.kind == id_undefined);
        idRecord *ent = s.a.recptr;
        ent->kind = id_local_v;
        ent->order = currentLocalOffset++;
        fop->global = false;
        fop->offset = ent->order;
    }else {
        if (s.token != tok_id)
            abortMessageWithToken("no id", &s);
        chechAssignment(s.kind, "for");
        fop->global = BOOL(s.kind == id_static_v);
        fop->offset = s.offset;
    }
    s = getItem();
    if (s.token != sym_eq)
        abortMessageWithToken("no equal", &s);
    fop->exps[0] = expression();
    s = getItem();
    if (s.token != sym_to)
        abortMessageWithToken("no to", &s);
    fop->exps[1] = expression();
    s = getItem();
    if (s.token == sym_step) {
        fop->exps[2] = expression();
        stp->count = 3;
        s = getItem();
    }else {
        fop->exps[2] = NULL;
        stp->count = 2;
    }
    if (s.token != sym_do)
        abortMessageWithToken("no do", &s);
    currentBreakNest++;
    fop->body = codeblock(end_set, false);
    currentBreakNest--;
    blockNestPop();
    (void)getItem();
    return stp;
}
