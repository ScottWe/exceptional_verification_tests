// RUN: %sea exe-cex %s -O0 --inline --verify 2>&1 | OutputCheck %s
// CHECK: ^__VERIFIER_error was executed$
// XFAIL: true

/*
 * This produces an erroneous counterexample when executed with,
 *   sea exe-cex -O0 --inline -o cexexe
 * However, it will work if line 21 is changed to int and line 33 is changed to
 * `tmp.d_nd = 0;`. This is interesting, as d_nd is only referenced at lines 21
 * and 33.
 *
 * From examining the LLVM bytecode, it seems that when the nested structure is
 * present, the LLVM memcpy is not elided.
 */

#include "stdint.h"
#include "seahorn/seahorn.h"

extern unsigned int nd_addr(void);
extern int nd_bool(void);
extern int nd_call(void);
extern int nd_int(void);

struct sol_int256 { int64_t v; };
typedef struct sol_int256 sol_int256_t;
static inline sol_int256_t Init_sol_int256_t(int64_t v) {
  sol_int256_t tmp;
  tmp.v = v;
  return tmp;
}

struct Map {
  int m_set;
  unsigned int m_curr;
  int d_;
  sol_int256_t d_nd;
};
struct PositiveMap {
  struct Map d_entries;
  unsigned int d_j;
  unsigned int d_k;
};

struct Map Init_Map(void) {
  struct Map tmp;
  tmp.m_set = 0;
  tmp.m_curr = 0;
  tmp.d_ = 0;
  tmp.d_nd.v = 0;
  return tmp;
}
int Read_Map(struct Map *a, unsigned int idx) {
  if (a->m_set != 1) {
    a->m_curr = idx;
    a->m_set = 1;
  }
  return (idx != a->m_curr) ? nd_int() : a->d_;
}
int Method_PositiveMap_Funcinvr(struct PositiveMap *self, unsigned int addr) {
  return (Read_Map(&(self->d_entries), addr) >= 0);
}
void Write_Map(struct Map *a, unsigned int idx, int d) {
  if (a->m_set != 1) {
    a->m_curr = idx;
    a->m_set = 1;
  }
  if (idx == a->m_curr) a->d_ = d;
}
struct PositiveMap Init_PositiveMap(unsigned int _j, unsigned int _k) {
  assume(_j != _k);
  struct PositiveMap tmp;
  tmp.d_entries = Init_Map();
  tmp.d_j = _j;
  tmp.d_k = _k;
  sassert(Read_Map(&(tmp.d_entries), _k) >= 0);
  return tmp;
}
void Method_PositiveMap_Funcdeposit(struct PositiveMap *self, unsigned int sender, int amt) {
  assume(sender == self->d_j || sender == self->d_k);
  assume(sender != self->d_j || Read_Map(&(self->d_entries), self->d_j) >= 0);
  Write_Map(&(self->d_entries), sender, Read_Map(&(self->d_entries), sender) + amt);
  sassert(Read_Map(&(self->d_entries), self->d_k) >= 0);
}
int main(int argc, const char **argv) {
  struct PositiveMap contract = Init_PositiveMap(nd_addr(), nd_addr());
  while (nd_bool()) {
    switch (nd_call()) {
    case 1: {
      Method_PositiveMap_Funcdeposit(&contract, nd_addr(), nd_int());
      break;
    }
    default: { assume(0); }
    }
  }
  return 0;
}
