#include "ring_buffer.h"
#include "test.h"
//#include <sstream>
#include <iostream>
#include <assert.h>


void test_empty1()
{
    int value, rc, size;
    int buf[8];
    ring_buffer rb(buf, (sizeof buf)/(sizeof *buf));

    rc = rb.empty();
    ASSERT(rc, rc);
    size = rb.size();
    ASSERT(size == 0, size);
    rb.print(std::cout);

    value = rb.pop();
    ASSERT(value == -1, value);
    rc = rb.empty();
    ASSERT(rc, rc);


    rb.push(1);
    rc = rb.empty();
    ASSERT(rc == 0, rc);
    size = rb.size();
    ASSERT(size == 1, size);
    rb.print(std::cout);

    value = rb.pop();
    ASSERT(value == 1, value);
    rc = rb.empty();
    ASSERT(rc, rc);

    value = rb.pop();
    ASSERT(value == -1, value);
    rc = rb.empty();
    ASSERT(rc, rc);
}

void test_empty2()
{
    int value, rc, size, cap, full;
    int buf[8];
    ring_buffer rb(buf, (sizeof buf)/(sizeof *buf));

    rc = rb.empty();
    ASSERT(rc, rc);
    size = rb.size();
    ASSERT(size == 0, size);
    rb.print(std::cout);

    cap = rb.capacity();
    ASSERT(cap == sizeof buf, cap);

    for (int k = 1; k < 10; ++k) {
        rb.push(k);
        rc = rb.empty();
        ASSERT(rc == 0, rc, k);
        size = rb.size();
        full = rb.full();

        if (k <= cap)
            ASSERT(size == k, size, k);

        if (k < cap) {
            ASSERT(full == 0, full, size, k, cap);
        } else {
            ASSERT(size == cap, size, k, cap);
            ASSERT(full, full, size, k, cap);
        }
        rb.print(std::cout);
    }
    size = rb.size();
    ASSERT(size == cap, size, cap);
    full = rb.full();
    ASSERT(full, full, size, cap);

    for (int k = 1; k < 9; ++k) {
        rc = rb.empty();
        ASSERT(rc == 0, rc, k);
        value = rb.pop();
        ASSERT(value == k+1, value, k);
        size = rb.size();
        ASSERT(size == cap - k, size, k, cap);
        full = rb.full();
        ASSERT(full == 0, full, size, k, cap);
        rb.print(std::cout);
    }
    rc = rb.empty();
    ASSERT(rc, rc);
    size = rb.size();
    ASSERT(size == 0, size);
    full = rb.full();
    ASSERT(full == 0, full, size);

    value = rb.pop();
    ASSERT(value == -1, value);
}

void test_capacity()
{
    int cap;
    int buf[8];
    ring_buffer rb(buf, (sizeof buf)/(sizeof *buf));

    cap = rb.capacity();
    ASSERT(cap == (sizeof buf)/(sizeof *buf), cap);
}

void test_clear()
{
    int rc, size;
    int buf[8];
    ring_buffer rb(buf, (sizeof buf)/(sizeof *buf));

    rb.push(1);

    rc = rb.empty();
    ASSERT(rc == 0, rc, rb.size());
    rb.clear();
    rc = rb.empty();
    ASSERT(rc, rc, rb.size());
    size = rb.size();
    ASSERT(size == 0, size);
}

void test_push()
{
    int value;
    int buf[8];
    ring_buffer rb(buf, (sizeof buf)/(sizeof *buf));

    rb.push(0);
    rb.print(std::cout);
    value = rb.pop();
    ASSERT(value == 0, value);

    rb.push(1);
    rb.print(std::cout);
    value = rb.pop();
    ASSERT(value == 1, value);

    for (int k = 2; k < 10; ++k) {
        rb.push(k);
        rb.print(std::cout);
    }

    value = rb.pop();
    ASSERT(value == 2, value);
    rb.print(std::cout);
    value = rb.pop();
    ASSERT(value == 3, value);
    rb.print(std::cout);

    rb.push(11);
    rb.push(12);
    rb.push(13); // Overwrites 4.
    rb.print(std::cout);

    value = rb.pop();
    ASSERT(value == 5, value);
    rb.print(std::cout);
}

int main(int , char* [])
{
    test_capacity();
    test_empty1();
    test_empty2();

    test_clear();
    test_push();

    return status;
}
