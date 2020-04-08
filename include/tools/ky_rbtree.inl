

template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::node::left() const
{
    return (node *)rb_node::left();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::node::right() const
{
    return (node *)rb_node::right();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::node::next()
{
    return (node*)rb_node::next();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::node::prev()
{
    return (node*)rb_node::prev();
}
template<typename T, typename Alloc>
char ky_rbtree<T, Alloc>::node::compare (rb_node *rhs) const
{
    node *z = (node*)rhs;
    if (key < z->key)
        return -1;
    else if (key > z->key)
        return 1;
    return 0;
}
template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::node::destroy()
{
    complex_destruct<T>(key);
    destroy_child(const_int<bool, tComplex>::value);
}
template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::node::destroy_child(false_type_t)
{

}

template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::node::destroy_child(true_type_t)
{
    if (left()) left()->destroy();
    if (right()) right()->destroy();
}

template<typename T, typename Alloc>
ky_rbtree<T, Alloc>::ky_rbtree()
{
    node_count = 0;
    ky_memory::zero(&head, sizeof(head));
}
template<typename T, typename Alloc>
ky_rbtree<T, Alloc>::~ky_rbtree()
{
    if (root())
        root()->destroy();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::root() const
{
    return (node *)head.left();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::const_node *ky_rbtree<T, Alloc>::end() const
{
    return (const_node *)&head;
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::end()
{
    return (node *)&head;
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::const_node *ky_rbtree<T, Alloc>::begin() const
{
    if (root())
        return (const_node*)most_left;
    return end();
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::begin()
{
    if (root())
        return (node *)most_left;
    return end();
}

template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::find (const T &k)const
{
    node* n = (node*)begin();
    while ((n != end()) && (n->key != k))
        n = n->next ();
    return n == end() ? 0 : n;
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node *ky_rbtree<T, Alloc>::search(const T &k)const
{
    return find(k);
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node* ky_rbtree<T, Alloc>::create_node(false_type_t, const T &v)
{
    node *n = Alloc::alloc(sizeof(node));
    new (n) node();
    n->key = v;
    return n;
}
template<typename T, typename Alloc>
typename ky_rbtree<T, Alloc>::node* ky_rbtree<T, Alloc>::create_node(true_type_t, const T &v)
{
    node *n = Alloc::alloc(sizeof(node));
    new (n) node();
    new (&n->key) T(v);
    return n;
}
template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::destory_node(false_type_t, node *n)
{
    n->~node();
    Alloc::destroy(n);
}
template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::destory_node(true_type_t, node *n)
{
    n->key.~T();
    n->~node();
    Alloc::destroy(n);
}
template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::insert(const T &v)
{
    rb_tree::insert(create_node(const_int<bool, tComplex>::value, v));
    ++node_count;
}

template<typename T, typename Alloc>
void ky_rbtree<T, Alloc>::remove(const T &v)
{
    node *n = find(v);
    rb_tree::remove(n);
    destory_node(const_int<bool, tComplex>::value, n);
    --node_count;
}

template<typename T, typename Alloc>
const T ky_rbtree<T, Alloc>::minimum()const
{
    node *x = begin();
    forever (x->left())
            x = x->left();
    return x ? x->key : T();
}
template<typename T, typename Alloc>
const T ky_rbtree<T, Alloc>::maximum()const
{
    node *x = begin();
    forever (x->right())
            x = x->right();
    return x ? x->key : T();
}
