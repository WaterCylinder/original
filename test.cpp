#include "array.h"
#include "chain.h"
#include "maths.h"

int main(){
    auto arr1 = original::array<int>(4);
    printf("%s", arr1.toString(true).c_str());
    original::array arr2 = {1, 4, 5, 3, 8};
    printf("%s", arr2.toString(true).c_str());
    printf("max(5,3):%d\n", original::max(5, 3));
    auto arr3 = arr2;
    printf("arr3:%p, arr2:%p\n", &arr3, &arr2);
    printf("arr3:%s\narr2:%s\n", arr3.toString(false).c_str(), arr2.toString(false).c_str());
    auto arr4 = original::array(arr2);
    printf("arr4:%p, arr2:%p\n", &arr4, &arr2);
    printf("arr4:%s\narr2:%s\n", arr4.toString(false).c_str(), arr2.toString(false).c_str());
    printf("arr2[2]:%d\n", arr2[2]);
    printf("arr3[-1]:%d\n", arr3[-1]);
    auto arr5 = original::array<int>(0);
    auto chain1 = original::chain(arr4);
    printf("chain1:%s", chain1.toCString(true));
    chain1.set(2, 10);
    printf("chain1:%s", chain1.toCString(true));
    for (int i = 0; i < chain1.size(); ++i) {
        printf("chain1[%d] = %d\n", i, chain1[i]);
    }
    for (original::iterator<int>* it = chain1.begins(); !it->isNull(); it->next()) {
        printf("chain1 element = %d, Iterator: %s\n", it->get(), it->toCString(false));
    }
    printf("\n");
    for (original::iterator<int>* it = chain1.ends(); !it->isNull(); it->prev()) {
        printf("chain1 element = %d, Iterator: %s\n", it->get(), it->toCString(false));
    }
    printf("\n");
    auto chain2 = original::chain({6, 7, 3, 9, 4, 2, 10, 14, -5});
    for (original::iterator<int> *l = chain2.begins(), *r = chain2.ends(); !l->equal(r) && !l->atNext(r); l->next(), r->prev()) {
        const int val = l->get();
        l->set(r->get());
        r->set(val);
    }
    for (int i = 0; i < chain2.size(); ++i) {
        printf("chain2[%d] = %d\n", i, chain2[i]);
    }
    printf("\n");
    printf("chain1 before:%s", chain1.toCString(true));
    chain1.foreach([](int& value) {
        value *= 2;
    });
    printf("chain1 after:%s", chain1.toCString(true));
    printf("\n");
    for (const int value : chain2) {
        printf("Element: %d\n", value);
    }
    printf("\n");
    for (int& value : chain2) {
        value *= 3;
    }
    printf("chain2 after:%s", chain2.toCString(true));
    auto chain3 = original::chain<int>();
    printf("chain3 phase1:%s", chain3.toCString(false));
    printf("\n");
    for (int i = 0; i < 21; i++){
        chain3.pushBegin(i);
    }
    printf("chain3 phase2:%s", chain3.toCString(true));
    for (int i = 1; i < chain3.size() - 1; i += 3){
        chain3.push(i, i);
    }
    printf("chain3 phase3:%s", chain3.toCString(false));
    return 0;
}
