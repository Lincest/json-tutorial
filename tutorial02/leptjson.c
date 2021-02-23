#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <errno.h>
#include <stdio.h>
#include <math.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/*重构以下代码为lept_parse_literal*/
/*
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
 */

static int lept_parse_literal(lept_context* c, lept_value* v, lept_type t, const char* literal) {
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; ++i) {
        if (c->json[i] != literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += i;
    v->type = t;
    return LEPT_PARSE_OK;
}

/* 用于校验数字语法的两个宏 */
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '0' && (ch) <='9')
static int lept_parse_number(lept_context* c, lept_value* v) {
    /*------------校验部分------------*/
    /* p指向当前解析位置 */
    const char *p = c->json;
    /* 1. 必须以-号或者1-9的数字打头, 或者是单个0 */
    if (*p == '-') p++;
    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        /*移动到小数点或者是指数的位置*/
        for (p++; ISDIGIT(*p); p++);
    }
    /* 小数部分 */
    if (*p == '.') {
        /* 小数点后没有值或者值不是0-9 */
        if (!ISDIGIT(*(p + 1))) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    /* 指数部分 */
    if (*p == 'E' || *p == 'e') {
        /* 跳过e/E和+-号 */
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    /*------------校验部分------------*/

    /* C 库函数 double strtod(const char *str, char **endptr) 把参数 str 所指向的字符串转换为一个浮点数（类型为 double 型）。
    如果 endptr 不为空，则指向转换中最后一个字符后的字符的指针会存储在 endptr 引用的位置。 */
    errno = 0;
    v->n = strtod(c->json, NULL);
    /* 如果number过大超出表示范围 */
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL)) return LEPT_PARSE_NUMBER_TOO_BIG;

    c->json = p;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v, LEPT_TRUE, "true");
        case 'f':  return lept_parse_literal(c, v, LEPT_FALSE, "false");
        case 'n':  return lept_parse_literal(c, v, LEPT_NULL, "null");
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
