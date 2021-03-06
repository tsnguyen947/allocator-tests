// -------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// -------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            my_allocator<int,    100>,
            my_allocator<double, 100>>
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const my_allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

TEST(TestAllocator2, index) {
    my_allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);}

TEST(TestAllocator2, invalid_size) {
    bool exceptionCaught = false;
    try{
        my_allocator<double, 8> x;
    } catch (std::bad_alloc){
        exceptionCaught = true;
    }
    ASSERT_TRUE(exceptionCaught);
}

TEST(TestAllocator2, zero_size) {
    bool exceptionCaught = false;
    try{
        my_allocator<int, 0> x;
    } catch (std::bad_alloc){
        exceptionCaught = true;
    }
    ASSERT_TRUE(exceptionCaught);}

TEST(TestAllocator2, deallocate_coalesce) {
    my_allocator<int, 100> x;
    int* first = x.allocate(3);
    int* middle = x.allocate(3);
    int* last = x.allocate(3);
    ASSERT_EQ(x[0], -12);
    ASSERT_EQ(x[20], -12);
    ASSERT_EQ(x[40], -12);
    x.deallocate(first, 3);
    x.deallocate(last, 3);
    x.deallocate(middle, 3);
    ASSERT_EQ(x[0], 92);
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            my_allocator<int,    100>,
            my_allocator<double, 100>>
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

    TYPED_TEST(TestAllocator3, over_allocate) {
        typedef typename TestFixture::allocator_type allocator_type;
        typedef typename TestFixture::value_type     value_type;

        allocator_type x;
        x.allocate(92 / sizeof(value_type));
        ASSERT_THROW(x.allocate(1), std::bad_alloc);
    }

    TYPED_TEST(TestAllocator3, deallocate_invalid) {
        typedef typename TestFixture::allocator_type allocator_type;
        typedef typename TestFixture::value_type     value_type;
        typedef typename TestFixture::size_type      size_type;
        typedef typename TestFixture::pointer        pointer;

        allocator_type x;
        const size_type      s = 5;
        const value_type     v = 2;
        pointer        b = x.allocate(s);
        pointer        p = b;
        for(unsigned int i = 0; i < s; i++) {
            x.construct(b, v);
            x.destroy(b);
            ++b;
        }
        ASSERT_THROW(x.deallocate(p + 1, s), std::invalid_argument);
    }

    TYPED_TEST(TestAllocator3, allocate_leftover) {
        typedef typename TestFixture::allocator_type allocator_type;
        typedef typename TestFixture::value_type     value_type;
        typedef typename TestFixture::size_type      size_type;
        typedef typename TestFixture::pointer        pointer;

        allocator_type x;
        const size_type s = (92 - 2 * sizeof(int) - sizeof(value_type)) / sizeof(value_type);
        pointer p = x.allocate(s + 1);
        ASSERT_THROW(x.allocate(1), std::bad_alloc);
    }
