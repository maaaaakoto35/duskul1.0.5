/* Duskul version 0.1.5,  2018.08.16,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.5,  2019.07.08 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "evaluate.h"
#include "exp_imp.h"
#include "statements.h"
#include "stnode_imp.h"
#include "strliteral.h"
#include "abort.h"

#define  STACK_SIZE     (1024 * 4)

/* extern */
long *stack, *globals;
// typical registers
int sp;             // stack pointer
int localbase = 0;  // base pointer (frame pointer)

typedef enum { ex_normal, ex_break, ex_return } ex_condition;

// prototypes
static ex_condition execStatements(const stnode *stptr);

static void execAssign(const assignnode *asp)
{
    int pos = asp->offset;
    long *target = asp->global ? &globals[pos] : &stack[localbase - pos];
    evaluate(asp->expr);
    *target = stack[sp++];
}

static void execReturn(const rtnnode *rtp)
{
    if (rtp->expr)
        evaluate(rtp->expr);    // stack[sp] has a return value
}

static ex_condition execIf(const ifnode *ifptr, int count)
{
    int i;
    for (i = 0; i < count; i++) {
        const expnode *ex = ifptr->clause[i].expr;
        if (ex == NULL) break; // else-clause
        evaluate(ex);
        if (stack[sp++] != 0) break;
    }
    if (i < count)
        return execStatements(ifptr->clause[i].body);
    return ex_normal;
}

static ex_condition execWhile(const whilenode *whp)
{
    ex_condition r = ex_normal;
    do {
        evaluate(whp->expr);
        if (stack[sp++] == 0) break;
        r = execStatements(whp->body);
    }while (r == ex_normal);
    return (r == ex_return) ? ex_return : ex_normal;
}

static ex_condition execFor(const fornode *frp)
{
    ex_condition rtncond = ex_normal;
    int upper, step;
    int pos = frp->offset;
    long *target = frp->global ? &globals[pos] : &stack[localbase - pos];
    evaluate(frp->exps[0]);
    *target = stack[sp++];
    evaluate(frp->exps[1]);
    upper = sp;
    step = upper - 1;
    if (frp->exps[2]) {
        evaluate(frp->exps[2]);
        assert(sp == step);
        if (stack[step] == 0) { goto RestoreSPAndExit; }
    }else {
        stack[--sp] = 1; // default value of step
    }
    while (1) {
        if (stack[step] > 0) {
            if (*target > stack[upper]) break;
        }else { // if stack[step] < 0 (note that stack[step] != 0)
            if (*target < stack[upper]) break;
        }
        rtncond = execStatements(frp->body);
        if (rtncond == ex_return) break;
        if (rtncond == ex_break) {
            rtncond = ex_normal;
            break;
        }
        *target += stack[step];
    }
RestoreSPAndExit:
    sp += 2;    // upper and step values are removed.
    return rtncond;
}

static ex_condition execStatements(const stnode *stptr)
{
    const stnode *stp = stptr;
    ex_condition r;
    while (stp) {
        switch (stp->kind) {
            case node_assign:
                execAssign((const assignnode *)stp); break;
            case node_call:
                execCall((const argnode *)stp, stp->count); break;
            case node_print:
                execPrint((const argnode *)stp, stp->count);
                break;
            case node_input:
                execInput((const argnode *)stp, stp->count); break;
            case node_return:
                execReturn((const rtnnode *)stp);
                return ex_return;
            case node_if:
                r = execIf((const ifnode *)stp, stp->count);
                if (r != ex_normal) return r;
                break;
            case node_while:
                r = execWhile((const whilenode *)stp);
                if (r != ex_normal) return r;
                break;
            case node_for:
                r = execFor((const fornode *)stp);
                if (r != ex_normal) return r;
                break;
            case node_break:
            default:
                return ex_break;
        }
        stp = stp->next;
    }
    return ex_normal;
}

#define  MAGIC_NUMBER   0xDEADBEEF

void subroutine(int index)
{
    funcinfo *finf = functionsTable[index];
    assert(CONTROL_INFO_SIZE >= 2);
    sp -= CONTROL_INFO_SIZE - 2;
    stack[--sp] = MAGIC_NUMBER;
    stack[--sp] = localbase;
    localbase = sp + finf->params + CONTROL_INFO_SIZE - 1;
    int locals = finf->localvars;
    if (sp - locals < STACK_LOW)
        abortMessage("stack overflow");
    while (locals--)
        stack[--sp] = 0;    // local vars

    (void)execStatements(finf->body);

    int bp;
    if (finf->rtntype) { // function
        long v = stack[sp++];
        sp += finf->localvars;
        bp = (int)stack[sp++];
        assert(stack[sp] == MAGIC_NUMBER);
        sp = localbase;
        stack[sp] = v;
    }else {
        sp += finf->localvars;
        bp = (int)stack[sp++];
        assert(stack[sp] == MAGIC_NUMBER);
        sp = localbase + 1;
    }
    localbase = bp;
}

int executeProgram(int mainidx)
{
    stack = malloc(sizeof(long) * STACK_SIZE);
    globals = malloc(sizeof(long) * numberOfStaticVars);
    sp = STACK_SIZE;
    funcinfo *finf = functionsTable[mainidx];
    for (int i = 0; i < finf->params; i++)
        stack[--sp] = 0;      // dummy arguments
    subroutine(mainidx);
    int rtncode = (sp < STACK_SIZE) ? (int)(stack[sp]) : 0;
    free(stack);
    free(globals);
    return rtncode;
}
