#include "pch.h"
#include "CppUnitTest.h"
#include "Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mytest1
{
    TEST_CLASS(mytest1)
    {
    public:

        TEST_METHOD(TestSum)
        {
            std::vector<int> arr = { 1, 2, 3, 4, 5 };
            int result = sum_array(arr);
            Assert::AreEqual(15, result);
        }

        TEST_METHOD(TestAverage)
        {
            std::vector<int> arr = { 2, 4, 6, 8 };
            double result = average_array(arr);
            Assert::AreEqual(5.0, result, 0.0001); 
        }

        TEST_METHOD(TestReverse)
        {
            std::vector<int> arr = { 1, 2, 3, 4, 5 };
            reverse_array(arr);
            std::vector<int> expected = { 5, 4, 3, 2, 1 };
            for (size_t i = 0; i < arr.size(); ++i) {
                Assert::AreEqual(expected[i], arr[i]);
            }
        }

        TEST_METHOD(TestReplaceMinMax)
        {
            std::vector<int> arr = { 2, 7, 3, 9, 4 };
            double avg = 5.0;
            replace_min_max_with_avg(arr, 2, 9, avg);
            std::vector<int> expected = { 5, 7, 3, 5, 4 };
            for (size_t i = 0; i < arr.size(); ++i) {
                Assert::AreEqual(expected[i], arr[i]);
            }
        }
    };
}
