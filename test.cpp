#include "array.h"
#include "chain.h"
#include "maths.h"
#include "vector.h"

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
    chain1.forEach([](int &value) {
        value *= 2;
    });
    printf("chain1 after:%s", chain1.toCString(true));
    printf("\n");
    for (const int value : chain2) {
        printf("chain2 element: %d\n", value);
    }
    printf("\n");
    printf("chain2 before:%s", chain2.toCString(true));
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
    printf("chain3 phase3:%s", chain3.toCString(true));
    printf("Does chain3 contains 100: bool(%d)\n", chain3.contains(100));
    printf("-3**-3=%f, 0**4=%f, 2**0=%f, 5.2**6=%f\n",
        original::pow(-3, -3), original::pow(0, 4), original::pow(2, 0), original::pow(5.2, 6));
    auto vector1 = original::vector<double>({1.3, 2.7, 5, 8.9, 4.1, 8, 9.5, 11.45, -0.7, -2, -5.8, 6.4, 23, 56, 65, 0.03, 2.07});
    printf("vector1: %s", vector1.toCString(true));
    printf("index of 9.5 in vector1: %llu\n", vector1.indexOf(9.5));
    // for (auto &e : vector1) // 检查内部包装类的地址连续性和首尾的前后指向是否为nullptr
    // {
    //     e *= 3.5;
    // }
    // vector1.forEach([](auto &e)
    // {
    //     e *= 3.5;
    // });
    printf("vector1: %s", vector1.toCString(true));
    return 0;
}
