#include <gtest/gtest.h>
#include "arkoi_language/utils/ordered_set.hpp"

TEST(OrderedSetTest, InsertAndContains) {
    OrderedSet<int> set;
    
    EXPECT_TRUE(set.insert(1));
    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.insert(1));  // Duplicate insert should return false
    EXPECT_EQ(set.size(), 1);
    
    EXPECT_TRUE(set.insert(2));
    EXPECT_TRUE(set.insert(3));
    EXPECT_EQ(set.size(), 3);
}

TEST(OrderedSetTest, EraseBasic) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_TRUE(set.erase(2));
    EXPECT_FALSE(set.contains(2));
    EXPECT_EQ(set.size(), 2);
    
    EXPECT_FALSE(set.erase(2));  // Erasing non-existent element should return false
    EXPECT_EQ(set.size(), 2);
}

TEST(OrderedSetTest, EraseLastElement) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_TRUE(set.erase(3));  // Erase last element
    EXPECT_FALSE(set.contains(3));
    EXPECT_EQ(set.size(), 2);
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(2));
}

TEST(OrderedSetTest, EraseMiddleElement) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);
    
    EXPECT_TRUE(set.erase(2));  // Erase middle element
    EXPECT_FALSE(set.contains(2));
    EXPECT_EQ(set.size(), 3);
    
    // Remaining elements should still be present
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(3));
    EXPECT_TRUE(set.contains(4));
}

TEST(OrderedSetTest, EraseFirstElement) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_TRUE(set.erase(1));  // Erase first element
    EXPECT_FALSE(set.contains(1));
    EXPECT_EQ(set.size(), 2);
    EXPECT_TRUE(set.contains(2));
    EXPECT_TRUE(set.contains(3));
}

TEST(OrderedSetTest, Clear) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    set.clear();
    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.contains(1));
}

TEST(OrderedSetTest, EmptyAndSize) {
    OrderedSet<int> set;
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
    
    set.insert(1);
    EXPECT_FALSE(set.empty());
    EXPECT_EQ(set.size(), 1);
}

TEST(OrderedSetTest, Iteration) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    std::vector<int> values;
    for (const auto& val : set) {
        values.push_back(val);
    }
    
    // At least all values should be present
    EXPECT_EQ(values.size(), 3);
    EXPECT_TRUE(std::find(values.begin(), values.end(), 1) != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), 2) != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), 3) != values.end());
}

TEST(OrderedSetTest, IterationAfterErase) {
    OrderedSet<int> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    set.insert(4);
    
    set.erase(2);
    
    std::vector<int> values;
    for (const auto& val : set) {
        values.push_back(val);
    }
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_TRUE(std::find(values.begin(), values.end(), 1) != values.end());
    EXPECT_FALSE(std::find(values.begin(), values.end(), 2) != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), 3) != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), 4) != values.end());
}

TEST(OrderedSetTest, StringType) {
    OrderedSet<std::string> set;
    
    EXPECT_TRUE(set.insert("hello"));
    EXPECT_TRUE(set.insert("world"));
    EXPECT_FALSE(set.insert("hello"));  // Duplicate
    
    EXPECT_TRUE(set.contains("hello"));
    EXPECT_TRUE(set.contains("world"));
    EXPECT_FALSE(set.contains("foo"));
    
    EXPECT_TRUE(set.erase("hello"));
    EXPECT_FALSE(set.contains("hello"));
    EXPECT_TRUE(set.contains("world"));
}

TEST(OrderedSetTest, MultipleErases) {
    OrderedSet<int> set;
    for (int i = 0; i < 10; ++i) {
        set.insert(i);
    }
    
    EXPECT_EQ(set.size(), 10);
    
    // Erase every other element
    for (int i = 0; i < 10; i += 2) {
        EXPECT_TRUE(set.erase(i));
    }
    
    EXPECT_EQ(set.size(), 5);
    
    // Check remaining elements
    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) {
            EXPECT_FALSE(set.contains(i));
        } else {
            EXPECT_TRUE(set.contains(i));
        }
    }
}
