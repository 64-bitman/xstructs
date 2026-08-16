/* SPDX-License-Identifier: Unlicense */

#ifdef NDEBUG
#    undef NDEBUG
#endif

#include "xarray.h"
#include <stdio.h>

/*
 * Thanks Claude...
 */

xarray_create(int, int, uint32_t, 4, 2.0);

struct point
{
    int x, y;
};
xarray_create(struct point, point, uint32_t, 2, 1.5);

static void
test_init_uninit(void)
{
    struct xarray_int arr;

    xarray_init_int(&arr);
    assert(arr.size == 0);
    assert(arr.len == 0);
    assert(arr.data == NULL);

    xarray_uninit_int(&arr);
    assert(arr.size == 0);
    assert(arr.len == 0);
    assert(arr.data == NULL);
}

static void
test_add_and_access(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    for (int i = 0; i < 10; i++)
        assert(xarray_add_int(&arr, i));

    assert(xarray_len_int(&arr) == 10);
    for (int i = 0; i < 10; i++)
        assert(xarray_val_int(&arr, i) == i);

    int *data = xarray_data_int(&arr);
    for (int i = 0; i < 10; i++)
        assert(data[i] == i);

    *xarray_ptr_int(&arr, 0) = 42;
    assert(xarray_val_int(&arr, 0) == 42);

    xarray_uninit_int(&arr);
}

static void
test_grow_behavior(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    // First add should allocate at least 'initial' (4) slots
    assert(xarray_add_int(&arr, 1));
    assert(arr.size >= 4);
    assert(arr.len == 1);

    // Fill up to capacity without triggering another grow
    uint32_t size_after_first = arr.size;
    while (arr.len < size_after_first)
        assert(xarray_add_int(&arr, 0));
    assert(arr.size == size_after_first);

    // One more add should trigger growth by at least grow_factor (2.0)
    assert(xarray_add_int(&arr, 99));
    assert(arr.size >= size_after_first * 2);

    xarray_uninit_int(&arr);
}

static void
test_shrink_behavior(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    assert(xarray_set_size_int(&arr, 20));
    assert(arr.size == 20);
    arr.len = 20;

    for (int i = 0; i < 15; i++)
        xarray_del_int(&arr, 0);

    assert(arr.size == 10);

    xarray_uninit_int(&arr);
}

static void
test_set_size(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    assert(xarray_set_size_int(&arr, 100));
    assert(arr.size == 100);
    assert(arr.len == 0);

    // set_size should not use "excess" growth - exact size requested
    assert(xarray_set_size_int(&arr, 100));
    assert(arr.size == 100);

    /* Growing further explicitly */
    assert(xarray_set_size_int(&arr, 150));
    assert(arr.size == 150);

    // Shrinking request should not shrink allocation
    assert(xarray_set_size_int(&arr, 10));
    assert(arr.size == 150);

    xarray_uninit_int(&arr);
}

static void
test_concat(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    int vals1[] = {1, 2, 3};
    assert(xarray_concat_int(&arr, vals1, 3));
    assert(xarray_len_int(&arr) == 3);
    for (int i = 0; i < 3; i++)
        assert(xarray_val_int(&arr, i) == vals1[i]);

    int vals2[] = {4, 5};
    assert(xarray_concat_int(&arr, vals2, 2));
    assert(xarray_len_int(&arr) == 5);
    int expected[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
        assert(xarray_val_int(&arr, i) == expected[i]);

    xarray_uninit_int(&arr);
}

static void
test_clear(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    for (int i = 0; i < 5; i++)
        assert(xarray_add_int(&arr, i));
    assert(xarray_len_int(&arr) == 5);

    xarray_clear_int(&arr);
    assert(xarray_len_int(&arr) == 0);
    assert(arr.size == 4);
    assert(arr.data != NULL);

    assert(xarray_add_int(&arr, 99));
    assert(xarray_len_int(&arr) == 1);
    assert(xarray_val_int(&arr, 0) == 99);

    xarray_uninit_int(&arr);
}

static void
test_del(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    for (int i = 0; i < 5; i++)
        assert(xarray_add_int(&arr, i));
    // arr: 0 1 2 3 4

    xarray_del_int(&arr, 2);
    // arr: 0 1 3 4
    assert(xarray_len_int(&arr) == 4);
    int expected[] = {0, 1, 3, 4};
    for (int i = 0; i < 4; i++)
        assert(xarray_val_int(&arr, i) == expected[i]);

    xarray_del_int(&arr, 3);
    assert(xarray_len_int(&arr) == 3);
    int expected2[] = {0, 1, 3};
    for (int i = 0; i < 3; i++)
        assert(xarray_val_int(&arr, i) == expected2[i]);

    xarray_del_int(&arr, 0);
    assert(xarray_len_int(&arr) == 2);
    int expected3[] = {1, 3};
    for (int i = 0; i < 2; i++)
        assert(xarray_val_int(&arr, i) == expected3[i]);

    xarray_uninit_int(&arr);
}

static void
test_foreach(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);
    for (int i = 0; i < 5; i++)
        assert(xarray_add_int(&arr, i * 2));

    int sum = 0;
    int count = 0;
    xarray_foreach(&arr, i)
    {
        sum += xarray_val_int(&arr, i);
        count++;
    }
    assert(count == 5);
    assert(sum == (0 + 2 + 4 + 6 + 8));

    xarray_uninit_int(&arr);
}

static void
test_foreach_mutation(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);
    for (int i = 0; i < 5; i++)
        assert(xarray_add_int(&arr, i));

    xarray_foreach(&arr, i) { *xarray_ptr_int(&arr, i) *= 10; }

    int expected[] = {0, 10, 20, 30, 40};
    for (int i = 0; i < 5; i++)
        assert(xarray_val_int(&arr, i) == expected[i]);

    xarray_uninit_int(&arr);
}

static void
test_struct_type(void)
{
    struct xarray_point arr;
    xarray_init_point(&arr);

    struct point p1 = {1, 2};
    struct point p2 = {3, 4};
    assert(xarray_add_point(&arr, p1));
    assert(xarray_add_point(&arr, p2));

    assert(xarray_len_point(&arr) == 2);
    assert(xarray_val_point(&arr, 0).x == 1);
    assert(xarray_val_point(&arr, 0).y == 2);
    assert(xarray_val_point(&arr, 1).x == 3);
    assert(xarray_val_point(&arr, 1).y == 4);

    struct point *ptr = xarray_ptr_point(&arr, 0);
    ptr->x = 99;
    assert(xarray_val_point(&arr, 0).x == 99);

    xarray_uninit_point(&arr);
}

static void
test_empty_array_foreach(void)
{
    struct xarray_int arr;
    xarray_init_int(&arr);

    int count = 0;
    xarray_foreach(&arr, i) { count++; }
    assert(count == 0);

    xarray_uninit_int(&arr);
}

int
main(void)
{
    test_init_uninit();
    test_add_and_access();
    test_grow_behavior();
    test_shrink_behavior();
    test_set_size();
    test_concat();
    test_clear();
    test_del();
    test_foreach();
    test_foreach_mutation();
    test_struct_type();
    test_empty_array_foreach();
    return EXIT_SUCCESS;
}
