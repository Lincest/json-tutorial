#ifndef LEPTJSON_H__
#define LEPTJSON_H__

/*
 * 因为 C 语言没有 C++ 的命名空间（namespace）功能，一般会使用项目的简写作为标识符的前缀。
 * 通常枚举值用全大写（如 LEPT_NULL），而类型及函数则用小写（如 lept_type）。
 * 接下来，我们声明 JSON 的数据结构。JSON 是一个树形结构，我们最终需要实现一个树的数据结构，
 * 每个节点使用 lept_value 结构体表示，我们会称它为一个 JSON 值（JSON value）。
 * 在此单元中，我们只需要实现 null, true 和 false 的解析，因此该结构体只需要存储一个 lept_type。之后的单元会逐步加入其他数据。
 */
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct {
    lept_type type;
}lept_value;

enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

/*
 * 一般用法:
 * lept_value v;
 * const char json[] = ...;
 * int ret = lept_parse(&v, json);
*/
int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

#endif /* LEPTJSON_H__ */
