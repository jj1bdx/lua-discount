#include <stdio.h>
#include <ctype.h>

#include "lua.h"
#include "lauxlib.h"

#include "mkdio.h"

static const char *const discount_opts[] = {
  "nolinks",
  "noimages",
  "nopants",
  "nohtml",
  "strict",
  "tagtext",
  "noext",
  "cdata",
  "notables",
  "toc",
  "compat",
  "autolink",
  "safelink",
  "embed",
  NULL
};

static const int discount_opts_codes[] = {
  MKD_NOLINKS,
  MKD_NOIMAGE,
  MKD_NOPANTS,
  MKD_NOHTML,
  MKD_STRICT,
  MKD_TAGTEXT,
  MKD_NO_EXT,
  MKD_CDATA,
  MKD_NOTABLES,
  MKD_TOC,
  MKD_1_COMPAT,
  MKD_AUTOLINK,
  MKD_SAFELINK,
  MKD_EMBED
};

static int ldiscount(lua_State *L) {
  size_t len;
  const char *str = luaL_checklstring(L, 1, &len);
  int flags = 0;
  int num_args = lua_gettop(L);
  MMIOT *doc;
  int i;

  for (i = 2; i <= num_args; i++) {
    int opt_index = luaL_checkoption(L, i, NULL, discount_opts);
    flags |= discount_opts_codes[opt_index];
  }

  doc = mkd_string(str, len, MKD_TABSTOP|MKD_NOHEADER);
  if (mkd_compile(doc, flags)) {
    char *result;
    int result_size = mkd_document(doc, &result);
    lua_pushlstring(L, result, result_size);
    mkd_cleanup(doc);
    return 1;
  } else {
    mkd_cleanup(doc);
    return luaL_error(L, "error converting document to markdown");
  }
}

static int ldiscount__call(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_remove(L, 1);
  return ldiscount(L);
}

static const struct luaL_reg ldiscount_meta[] = {
  {"__call", ldiscount__call},
  {NULL, NULL}
};

static const struct luaL_reg ldiscount_funcs[] = {
  {"to_html", ldiscount},
  {NULL, NULL}
};

LUALIB_API int luaopen_discount(lua_State *L) {
  // Give the discount table a metatable
  luaL_register(L, "discount", ldiscount_funcs);
  lua_newtable(L);
  luaL_register(L, NULL, ldiscount_meta);
  lua_setmetatable(L, -2);
  return 1;
}
