/* SPDX-License-Identifier: Unlicense */

#include "xlist.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

xlist_declare(item);

struct item
{
    int               value;
    struct xlist_item link;
};

xlist_define(item, struct item, link);

static void
test_init_empty(void)
{
    struct xlist_item head;
    xlist_init_item(&head);
    assert(xlist_empty_item(&head));
    assert(head.next == &head);
    assert(head.prev == &head);
}

static void
test_single_insert(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item a = {.value = 1};
    xlist_insert_after_item(&head, &a);

    assert(!xlist_empty_item(&head));
    assert(head.next == &a.link);
    assert(head.prev == &a.link);
    assert(a.link.next == &head);
    assert(a.link.prev == &head);

    struct item *p = xlist_ptr_item(head.next);
    assert(p == &a);
    assert(p->value == 1);
}

static void
test_multiple_insert_order(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item a = {.value = 1};
    struct item b = {.value = 2};
    struct item c = {.value = 3};

    xlist_insert_after_item(&head, &a);
    xlist_insert_after_item(&head, &b);
    xlist_insert_after_item(&head, &c);

    int          expected[] = {3, 2, 1};
    int          i = 0;
    struct item *pos;
    xlist_foreach(item, &head, pos)
    {
        assert(pos->value == expected[i]);
        i++;
    }
    assert(i == 3);
}

static void
test_insert_after_non_head(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item a = {.value = 1};
    struct item b = {.value = 2};
    struct item c = {.value = 3};

    xlist_insert_after_item(&head, &a);
    xlist_insert_after_item(&a.link, &b);
    xlist_insert_after_item(&a.link, &c);

    int          expected[] = {1, 3, 2};
    int          i = 0;
    struct item *pos;
    xlist_foreach(item, &head, pos)
    {
        assert(pos->value == expected[i]);
        i++;
    }
    assert(i == 3);
}

static void
test_unlink(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item a = {.value = 1};
    struct item b = {.value = 2};
    struct item c = {.value = 3};

    xlist_insert_after_item(&head, &a);
    xlist_insert_after_item(&head, &b);
    xlist_insert_after_item(&head, &c);

    xlist_unlink_item(&b);

    assert(b.link.next == &b.link);
    assert(b.link.prev == &b.link);

    int          expected[] = {3, 1};
    int          i = 0;
    struct item *pos;
    xlist_foreach(item, &head, pos)
    {
        assert(pos->value == expected[i]);
        i++;
    }
    assert(i == 2);

    xlist_unlink_item(&a);
    xlist_unlink_item(&c);
    assert(xlist_empty_item(&head));
}

static void
test_foreach_safe_remove_all(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item items[5];
    for (int i = 0; i < 5; i++)
    {
        items[i].value = i;
        xlist_insert_after_item(&head, &items[i]);
    }

    struct item *pos;
    int          count = 0;
    xlist_foreach_safe(item, &head, pos)
    {
        xlist_unlink_item(pos);
        count++;
    }
    assert(count == 5);
    assert(xlist_empty_item(&head));
}

static void
test_foreach_safe_remove_selective(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item items[6];
    for (int i = 0; i < 6; i++)
    {
        items[i].value = i;
        xlist_insert_after_item(&head, &items[i]);
    }

    struct item *pos;
    xlist_foreach_safe(item, &head, pos)
    {
        if (pos->value % 2 == 0)
            xlist_unlink_item(pos);
    }

    int expected[] = {5, 3, 1};
    int i = 0;
    xlist_foreach(item, &head, pos)
    {
        assert(pos->value == expected[i]);
        i++;
    }
    assert(i == 3);
}

static void
test_reinsert_after_unlink(void)
{
    struct xlist_item head;
    xlist_init_item(&head);

    struct item a = {.value = 1};
    xlist_insert_after_item(&head, &a);
    xlist_unlink_item(&a);
    assert(xlist_empty_item(&head));

    xlist_insert_after_item(&head, &a);
    assert(!xlist_empty_item(&head));
    struct item *p = xlist_ptr_item(head.next);
    assert(p == &a);
}

int
main(void)
{
    test_init_empty();
    test_single_insert();
    test_multiple_insert_order();
    test_insert_after_non_head();
    test_unlink();
    test_foreach_safe_remove_all();
    test_foreach_safe_remove_selective();
    test_reinsert_after_unlink();
    return EXIT_SUCCESS;
}
