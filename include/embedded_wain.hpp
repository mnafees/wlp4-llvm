#pragma once
#ifndef __WLP4_LLVM_EMBEDDED_WAIN
#define __WLP4_LLVM_EMBEDDED_WAIN

const auto intWain = R"(#include <stdlib.h>
#include <stdio.h>
extern int wain(int, int);
int main(int argc, char** argv) {
    int a,b,c;
    printf("Enter first integer: ");
    scanf("%d", &a);
    printf("Enter second integer: ");
    scanf("%d", &b);
    c = wain(a,b);
    printf("wain returned %d\n", c);
    return 0;
}
)";

const auto arrayWain = R"(#include <stdlib.h>
#include <stdio.h>
extern int wain(int*, int);
int main(int argc, char** argv) {
    int l, c;
    int* a;
    printf("Enter length of array: ");
    scanf("%d", &l);
    a = (int*) malloc(l*sizeof(int));
    for(int i = 0; i < l; i++) {
        printf("Enter value of array element %d: ", i);
        scanf("%d", a+i);
    }
    c = wain(a,l);
    printf("wain returned %d\n", c);
    return 0;
}
)";

#endif // __WLP4_LLVM_EMBEDDED_WAIN
