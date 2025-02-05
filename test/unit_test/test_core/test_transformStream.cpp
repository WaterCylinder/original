#include <gtest/gtest.h>
#include "transformStream.h"


namespace original {

    // 测试流操作符 +
    TEST(TransformStreamTest, CombinedOperatorPlusTest) {
        array<int> arr(5);

        const assignOptTransform init(1);
        const addOptTransform add(5);
        const addOptTransform minus(-7);
        const multiOptTransform mult1(2);
        const multiOptTransform mult2(4);

        arr.forEach(init + add + minus + mult1 + mult2);
        int res = (1 + 5 - 7) * 2 * 4;
        arr.forEach(
            [=](const auto& e) {
                ASSERT_EQ(res, e);
            }
        );
    }
}
