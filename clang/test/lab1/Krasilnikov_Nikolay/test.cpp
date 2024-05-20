// RUN: %clang++ -cc1 -load %S/../../../lab1/Krasilnikov_Nikolay/AlwaysInline.so -plugin always-inline-plugin %s 2>&1 | FileCheck %s

// CHECK: __attribute__((always_inline)) void foo1() {
void foo1() {
  int x = 0;
}

// CHECK: __attribute__((always_inline)) void foo2() {
void foo2() {
  for (int i = 0; i < 10; ++i) {
    int x = i;
  }
}

// CHECK: __attribute__((always_inline)) void foo3() {
void foo3() {
  {
    int x = 0;
  }
}

// CHECK: __attribute__((always_inline)) void foo4() {
void foo4() {
}

// CHECK: __attribute__((always_inline)) void foo5() {
void foo5() {
  foo1();
}

// CHECK: __attribute__((always_inline)) void foo6() {
void foo6() {
  int x = (true) ? 1 : 0;
}

// CHECK: static __attribute__((always_inline)) void foo7() {
static void foo7() {
  int x = 0;
}

// CHECK: inline __attribute__((always_inline)) void foo8() {
inline void foo8() {
  int x = 0;
}

// CHECK: __attribute__((always_inline)) void foo9() {
void foo9() {
  for (int i = 0; i < 10; ++i) {
    if (i % 2 == 0) {
      int x = i;
    }
  }
}
  
// СHECK-NOT: __attribute__((always_inline)) void foo10() {
void foo10() {
  while (true) {
    if (true) {
      int x = 1;
    }
  }
}
// СHECK-NOT: __attribute__((always_inline)) void foo12() {
void foo12() {
  int x = 1;
  while (false) {
    if (true) {
      int x = 0;
    } 
  }
}

// СHECK-NOT: __attribute__((always_inline)) void foo11() {
void foo11() {
  if (true) {
    int x = 0;
  }
}

// CHECK: inline static __attribute__((always_inline)) int foo13() {
inline static int foo13() {
  int x = 0;
  for (;;) {
  }
  return 1;
}

// CHECK: inline static __attribute__((always_inline)) int foo14() {
inline static int foo14() {
  do {

  }
  while (false);
  
}


// CHECK-NOT: __attribute__((always_inline)) float foo16() {
float foo16() {
  do {
    if (true) {
      int x = 0;
    } 
  }
  while (false);
}