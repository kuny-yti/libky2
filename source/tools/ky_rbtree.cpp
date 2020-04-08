
#include "ky_rbtree.h"


#define rb_color(r) ((r)->color)

#define rb_is_red(r)   ((r)->color == rb_node::cRed)
#define rb_is_black(r)  ((r)->color == rb_node::cBlack)

#define rb_set_black(r)  do { (r)->color = rb_node::cBlack; } while (0)
#define rb_set_red(r)  do { (r)->color = rb_node::cRed; } while (0)
#define rb_set_color(r, c)  do { (r)->color = (c); } while (0)

#define rb_left(r) ((r)->child[rb_node::cLeft])
#define rb_right(r) ((r)->child[rb_node::cRight])
#define rb_parent(r) ((r)->child[rb_node::cParent])
#define rb_set_left(r, l) do {(r)->child[rb_node::cLeft] = (l);} while(0)
#define rb_set_right(r, l) do {(r)->child[rb_node::cRight] = (l);} while(0)
#define rb_set_parent(r, p) do {(r)->child[rb_node::cParent] = (p);} while(0)

rb_node *rb_node::next()
{
    rb_node *take = this;

    if (rb_right(take))
    {
        take = rb_right(take);
        forever (rb_left(take))
            take = rb_left(take);
    }
    else
    {
        rb_node *tmp = rb_parent(take);
        forever (tmp && (take == rb_right(tmp)))
        {
            take = tmp;
            tmp = rb_parent(take);
        }
        take = tmp;
    }
    return take;
}
rb_node *rb_node::prev()
{
    rb_node *take = this;
    if (rb_left(take))
    {
        take = rb_left(take);
        forever (rb_right(take))
            take = rb_right(take);
    }
    else
    {
        rb_node *tmp = rb_parent(take);
        forever (tmp && (take == rb_left(tmp)))
        {
            take = tmp;
            tmp = rb_parent(take);
        }
        take = tmp;
    }
    return take;
}

void rb_tree::left_rotate(rb_node* x)
{
    rb_node *&rot = rb_left(&head);
    rb_node *y = rb_right(x);

    rb_set_right(x, rb_left(y));
    if (rb_left(y))
        rb_set_parent(rb_left(y), x);

    rb_set_parent(y, rb_parent(x));
    if (x == rot)
        rot = y;
    else if (x == rb_left(rb_parent(x)))
        rb_set_left(rb_parent(x), y);
    else
        rb_set_right(rb_parent(x), y);

    rb_set_left(y, x);
    rb_set_parent(x, y);
}

void rb_tree::right_rotate(rb_node* y)
{
    rb_node *&rot = rb_left(&head);
    rb_node *x = rb_left(y);

    rb_set_left(y, rb_right(x));
    if (rb_right(x))
        rb_set_parent(rb_right(x), y);

    rb_set_parent(x, rb_parent(y));
    if (y == rot)
        rot = x;
    else if (y == rb_right(rb_parent(y)))
        rb_set_right(rb_parent(y), x);
    else
        rb_set_left(rb_parent(y), x);

    rb_set_right(x, y);
    rb_set_parent(y, x);
}

void rb_tree::insert_rebalance(rb_node *x)
{
    rb_node *&rot = rb_left(&head);
    rb_set_red(x);
    forever (x != rot && rb_is_red(rb_parent(x)))
    {
        if (rb_parent(x) == rb_left(rb_parent(rb_parent(x))))
        {
            rb_node *y = rb_right(rb_parent(rb_parent(x)));
            if (y && rb_is_red(y))
            {
                rb_set_black(rb_parent(x));
                rb_set_black(y);
                rb_set_red(rb_parent(rb_parent(x)));
                x = rb_parent(rb_parent(x));
            }
            else
            {
                if (x == rb_right(rb_parent(x)))
                {
                    x = rb_parent(x);
                    left_rotate(x);
                }
                rb_set_black(rb_parent(x));
                rb_set_red(rb_parent(rb_parent(x)));
                right_rotate(rb_parent(rb_parent(x)));
            }
        }
        else
        {
            rb_node *y = rb_left(rb_parent(rb_parent(x)));
            if (y && rb_is_red(y))
            {
                rb_set_black(rb_parent(x));
                rb_set_black(y);
                rb_set_red(rb_parent(rb_parent(x)));
                x = rb_parent(rb_parent(x));
            }
            else
            {
                if (x == rb_left(rb_parent(x)))
                {
                    x = rb_parent(x);
                    right_rotate(x);
                }
                rb_set_black(rb_parent(x));
                rb_set_red(rb_parent(rb_parent(x)));
                left_rotate(rb_parent(rb_parent(x)));
            }
        }
    }
    rb_set_black(rot);
}

void rb_tree::remove_rebalance(rb_node *x, rb_node *x_parent)
{
    rb_node *&rot = rb_left(&head);
    forever (x != rot && (!x || rb_is_black(x)))
    {
        if (x == rb_left(x_parent))
        {
            rb_node *w = rb_right(x_parent);
            if (rb_is_red(w))
            {
                rb_set_black(w);
                rb_set_red(x_parent);
                left_rotate(x_parent);
                w = rb_right(x_parent);
            }
            if ((!rb_left(w) || rb_is_black(rb_left(w))) &&
                (!rb_right(w) || rb_is_black(rb_right(w))))
            {
                rb_set_red(w);
                x = x_parent;
                x_parent = rb_parent(x_parent);
            }
            else
            {
                if (!rb_right(w) || rb_is_black(rb_right(w)))
                {
                    if (rb_left(w))
                        rb_set_black(rb_left(w));
                    rb_set_red(w);
                    right_rotate(w);
                    w = rb_right(x_parent);
                }
                rb_set_color(w, rb_color(x_parent));
                rb_set_black(x_parent);
                if (rb_right(w))
                    rb_set_black(rb_right(w));
                left_rotate(x_parent);
                break;
            }
        }
        else
        {
            rb_node *w =  rb_left(x_parent);
            if (rb_is_red(w))
            {
                rb_set_black(w);
                right_rotate(x_parent);
                w = rb_left(x_parent);
            }
            if ((!rb_right(w) || rb_is_black(rb_right(w))) &&
                (!rb_left(w) || rb_is_black(rb_left(w))))
            {
                rb_set_red(w);
                x = x_parent;
                x_parent = rb_parent(x_parent);
            }
            else
            {
                if (!rb_left(w) || rb_is_black(rb_left(w)))
                {
                    if (rb_right(w))
                        rb_set_black(rb_right(w));
                    rb_set_red(w);
                    left_rotate(w);
                    w = rb_left(x_parent);
                }
                rb_set_color(w, rb_color(x_parent));
                rb_set_black(x_parent);
                if (rb_left(w))
                    rb_set_black(rb_left(w));
                right_rotate(x_parent);
                break;
            }
        }
    }
    if (x)
        rb_set_black(x);
}

void rb_tree::remove(rb_node *z)
{
    rb_node *&rot = rb_left(&head);
    rb_node *y = z;
    rb_node *x = 0;
    rb_node *x_parent = 0;
    if (!rb_left(y))
    {
        x = rb_right(y);
        if (y == most_left)
        {
            if (x)
                most_left = x;
            else
                most_left = rb_parent(y) ;
        }
    }
    else
    {
        if (!rb_right(y))
            x = rb_left(y);
        else
        {
            y = rb_right(y);
            while (rb_left(y))
                y = rb_left(y);
            x = rb_right(y);
        }
    }

    if (y != z)
    {
        rb_set_parent(rb_left(z), y);
        rb_set_left(y, rb_left(z));
        if (y != rb_right(z))
        {
            x_parent = rb_parent(y);
            if (x)
                rb_set_parent(x, rb_parent(y));
            rb_set_left(rb_parent(y), x);
            rb_set_right(y, rb_right(z));
            rb_set_parent(rb_right(z), y);
        }
        else
            x_parent = y;

        if (rot == z)
            rot = y;
        else if (rb_left(rb_parent(z)) == z)
            rb_set_left(rb_parent(z), y);
        else
            rb_set_right(rb_parent(z), y);
        rb_set_parent(y, rb_parent(z));

        ky_swap(rb_color(y), rb_color(z));
        y = z;
    }
    else
    {
        x_parent = rb_parent(y);
        if (x)
            rb_set_parent(x, rb_parent(y));
        if (rot == z)
            rot = x;
        else if (rb_left(rb_parent(z)) == z)
            rb_set_left(rb_parent(z), x);
        else
            rb_set_right(rb_parent(z), x);
    }

    if (rb_is_black(y))
        remove_rebalance(x, x_parent);
}

void rb_tree::insert (rb_node *n)
{
    rb_node *rot = head.left();
    rb_node *par = &head;
    bool  is_left = true;

    rb_set_red(n);
    rb_set_left(n, 0);
    rb_set_right(n, 0);

    forever (rot)
    {
        par = rot;
        is_left = (n->compare(rot) < 0) ? true : false;
        if (is_left)
            rot = rot->left();
        else
            rot = rot->right();
    }

    rb_set_parent(n, par);
    if (is_left)
    {
        rb_set_left(par, n);
        if (par == most_left)
            most_left = n;
    }
    else
        rb_set_right(par, n);
    insert_rebalance(n);
}
