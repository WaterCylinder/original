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
    printf("arr3:%s\narr2:%s\n", arr3.toString().c_str(), arr2.toString().c_str());
    auto arr4 = original::array(arr2);
    printf("arr4:%p, arr2:%p\n", &arr4, &arr2);
    printf("arr4:%s\narr2:%s\n", arr4.toString().c_str(), arr2.toString().c_str());
    printf("arr2[2]:%d\n", arr2[2]);
    printf("arr3[-1]:%d\n", arr3[-1]);
    auto arr5 = original::array<int>(0);
    auto chain1 = original::chain({1, 2, 3, 4, 5});
    printf("%s", chain1.toString(true).c_str());
    printf("%d\n", chain1[2]);
    // 'class original::chain<int>::chainNode' is private within this context
    // auto node = original::chain<int>::chainNode(10);
    const auto it1 = chain1.begins();
    printf("iterator1 from chain1:%p\n", it1);
    printf("**it1=%d\n", it1->operator*());
    return 0;
}
