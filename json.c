#include <stdlib.h>
#include <string.h>

#include "follow.h"
#include "parser.h"
#include "parsingtable.h"
/*
https://www.ietf.org/rfc/rfc4627.txt

ws = $ | x20 ws | x09 ws | x0a ws | x0d ws
begin-array = ws x5b ws
begin-object = ws %x7B ws
end-array = ws x5D ws
end-object = ws x7D ws
name-separator = ws x3A ws
value-separator = ws x2C ws
value = false | null | true | object | array | number | string
object = begin-object member end-object
member = $ | string name-separator value members
members = $ | value-separator members
array = begin-array array-value end-array
array-value = $ | value array-values
array-values = $ | value-separator  value
*/
const struct RULE_ENTRY rule_table[] = {
    {
        "ws", {
            {{1, "$"}},
            {{1, "\x20"}, {0, "ws"}},
            {{1, "\x09"}, {0, "ws"}},
            {{1, "\x0a"}, {0, "ws"}},
            {{1, "\x0d"}, {0, "ws"}},
        },
    },{
        "begin-array", {{{0, "ws"}, {1, "\x5b"}, {0, "ws"}}}
    },{
        "begin-object", {{{0, "ws"}, {1, "\x7b"}, {0, "ws"}}}
    },{
        "end-array", {{{0, "ws"}, {1, "\x5d"}, {0, "ws"}}}
    },{
        "end-object", {{{0, "ws"}, {1, "\x7d"}, {0, "ws"}}}
    },{
        "name-separator", {{{0, "ws"}, {1, "\x3a"}, {0, "ws"}}}
    },{
        "value-separator", {{{0, "ws"}, {1, "\x2c"}, {0, "ws"}}}
    },{
        "value", {
            {{1, "f"}, {1, "a"}, {1, "l"}, {1, "s"}, {1, "e"}},
            {{1, "t"}, {1, "r"}, {1, "u"}, {1, "e"}},
            {{1, "n"}, {1, "u"}, {1, "l"}, {1, "l"}},
            {{0, "object"}},
            {{0, "array"}},
            {{0, "number"}},
            {{0, "string"}},
            
        }
    }
};