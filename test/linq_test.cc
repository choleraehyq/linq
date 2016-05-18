#include "include/linq.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace linq;

TEST(LinqTest, select_and_where) {
    std::vector<int> v = {1, 2, 3, 4};
    auto r = from(v).select([](int i){ return i+2; }).where([](int i){ return i>2; }).max();
    EXPECT_EQ(r, 6);
}

TEST(LinqTest, aggregate) {
    std::vector<std::string> strv = { "a", "b", "c" };
    std::string ret = from(strv).aggregate([](const std::string& str1, const std::string& str2){
		return str1 + str2;
	});
    EXPECT_EQ(ret, std::string("abc"));
}

TEST(LinqTest, math_operation) {
    std::vector<int>v = { 0, 1, 2, 3, 4, 5, 3, 6};
	auto count = from(v).count();
	auto sum = from(v).sum();
	auto average = from(v).average();
	auto min = from(v).min();
	auto max = from(v).max();
    EXPECT_EQ(count, 8);
    EXPECT_EQ(sum, 24);
    EXPECT_EQ(average, 3);
    EXPECT_EQ(min, 0);
    EXPECT_EQ(max, 6);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}