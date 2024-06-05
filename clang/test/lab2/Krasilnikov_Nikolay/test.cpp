// RUN: %clang_cc1 -load %S/../../../lab2/Krasilnikov_Nikolay/InliningPlugin.so -plugin inlining-plugin %s 2>&1 | FileCheck %s

// CHECK-NOT: void foo1() {
// CHECK-NOT: int a = 10;
// CHECK-NOT: }
void foo1() {
    int a = 10;
}

void caller1() {
    // CHECK: {
    // CHECK-NEXT: int a = 10;
    // CHECK-NEXT: }
    foo1();
}

// CHECK-NOT: void foo2() {
// CHECK-NOT: int x = 2;
// CHECK-NOT: int a = x + 1;
// CHECK-NOT: }
void foo2() {
    int x = 2;
    int a = x + 1;
}

void caller2() {
    // CHECK: {
    // CHECK: int x = 2;
    // CHECK: int a = x + 1;
    // CHECK: }
    foo2();
}

// CHECK: int foo3(int x) {
// CHECK-NEXT: return x;
// CHECK-NEXT: }
int foo3(int x) {
    return x;
}

void caller3() {
    // CHECK: int y = foo3(20);
    int y = foo3(20);
}

// CHECK: float foo4(float x) {
// CHECK-NEXT: int b = 5 + 1;
// CHECK-NEXT: for (int i = 0; i < x; ++i) {};
// CHECK-NEXT: return b / x;
// CHECK-NEXT: }
float foo4(float x) {
    int b = 5 + 1;
    for (int i = 0; i < x; ++i) {}
    return b / x;
}

void caller4() {
    // CHECK: float y = foo4(20);
    float y = foo4(20);
}
