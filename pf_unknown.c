// RUN: %sea pf %s --inline 2>&1 | OutputCheck %s
// CHECK: ^unknown$

/**
 * This example, when inlined and run against pf, results in
 *     Finished context-insensitive global analysis.
 *     Running AllocSiteInfo pass on function main
 *     WARNING: initializing DSA node due to memset()
 *     WARNING: initializing DSA node due to memset()
 *     unknown
 */

#include "seahorn/seahorn.h"
struct Map {
  char set[2];
  char data[2];
  char nd_data;
};
extern char nd();
char Read_Map(struct Map *arr) {
  for (char i = 0; i < 2; ++i) {
    if ((arr->set)[i] != 1) {
      (arr->set)[i] = 1;
      return (arr->data)[i];
    }
  }
  return nd();
}
struct Map Init_Map(void) {
  struct Map tmp;
  tmp.nd_data = 0;
  for (char i = 0; i < 2; ++i) {
    tmp.set[i] = 0;
    tmp.data[i] = 0;
  }
  return tmp;
}
int main(int argc, const char **argv) {
  struct Map m;
  m = Init_Map();
  while (nd()) {
    sassert(Read_Map(&(m)) >= 0);
  }
  return 0;
}
