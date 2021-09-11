#include "ring_buffer.h"
#include "test.h"
//#include <sstream>
#include <iostream>
#include <assert.h>


void test_empty1()
{
    int value, rc;
    char buf[8];
    ring_buffer rb(buf, sizeof buf);

    rc = rb.empty();
    ASSERT(rc, rc);

    value = rb.pop();
    ASSERT(value == -1, value);
    rc = rb.empty();
    ASSERT(rc, rc);


    rb.push(1);
    rc = rb.empty();
    ASSERT(rc == 0, rc);

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
    int value, rc;
    char buf[8];
    ring_buffer rb(buf, sizeof buf);

    rc = rb.empty();
    ASSERT(rc, rc);

    for (int k = 0; k < 9; ++k) {
        rb.push(k);
        rc = rb.empty();
        ASSERT(rc == 0, rc);
    }

    for (int k = 0; k < 8; ++k) {
        rc = rb.empty();
        value = rb.pop();
        ASSERT(value == k + 1, value, k);
        ASSERT(rc == 0, rc, k);
    }
    rc = rb.empty();
    ASSERT(rc, rc);

    value = rb.pop();
    ASSERT(value == -1, value);
}


void test_push()
{
    int value;
    char buf[8];
    ring_buffer rb(buf, sizeof buf);

    rb.push(0);
    value = rb.pop();
    ASSERT(value == 0, value);

    rb.push(1);
    value = rb.pop();
    ASSERT(value == 1, value);

    for (int k = 2; k < 10; ++k)
        rb.push(k);

    value = rb.pop();
    ASSERT(value == 2, value);
    value = rb.pop();
    ASSERT(value == 3, value);

    rb.push(11);
    rb.push(12);
    rb.push(13); // Overwrites 4.

    value = rb.pop();
    ASSERT(value == 5, value);
}

int main(int , char* [])
{
    test_empty1();
    test_empty2();

    test_push();

    return status;
}
