#include "ky_object.h"
#include "ky_thread.h"
#include "thread/thread_dispatch.h"

namespace impl {

class object : public ky_ref // 本对象的引用计数
{
public:
    ky_object           *parent;  // 父对象
    ky_list<ky_object *> child;   // 本对象的所有子对象
    ky_thread           *thread;  // 本对象所处的线程

    object():
        ky_ref(OneDetach),
        parent(0),
        child(),
        thread(ky_thread::current ())
    {
    }
};
}


ky_object::ky_object(ky_object *parent):
    impl(kyNew (impl::object()))
{
    impl->parent = parent;
    if (parent)
        parent->append(this);
}
ky_object::~ky_object()
{
    if (impl->parent)
        impl->parent->remove (this);
    kyDelete(impl);
}
ky_thread *ky_object::thread() const
{
    return impl->thread;
}
bool ky_object::move_thread(ky_thread *t)
{
    // 父线程处于其他线程，则无法切换线程
    if ((impl->parent != 0) && (impl->parent->thread () != t))
        return false;

    if (ky_threadsafe::move_thread (t))
    {
        impl->thread = t;
        for (child slt = impl->child.begin(); slt != impl->child.end(); ++slt)
            (*slt)->move_thread (t);

        return true;
    }
    return false;
}
bool ky_object::event(ievent *)
{
    return true;
}

void ky_object::registered(intptr fd, eNotifyFlags flag)
{
    ky_thread *cur = impl->thread;
    cur->dispatch->registered(fd, flag, this);
}
void ky_object::unregister(intptr fd)
{
    ky_thread *cur = impl->thread;
    cur->dispatch->unregister(fd);
}

void ky_object::modify(intptr fd, eNotifyFlags flag, bool active)
{
    ky_thread *cur = impl->thread;
    cur->dispatch->modify (fd, flag, active);
}

void ky_object::addref()
{
    impl->addref();
}
void ky_object::lessref()
{
    if (impl->lessref())
    {
        //! 在父对象中去掉自己
        if (parent())
            parent()->remove(this);

        //! 处理孩子
        for (child slt = impl->child.begin(); slt != impl->child.end(); ++slt)
            (*slt)->lessref();
    }
}
void ky_object::set_parent(const ky_object *p)
{
    if (impl->parent)
        impl->parent->remove(this);
    impl->parent = (ky_object *)p;
    impl->parent->append(this);
}
ky_object *ky_object::parent()const
{
    return impl->parent;
}
uint ky_object::child_count()const
{
    return impl->child.count();
}
void ky_object::append(ky_object *obj)
{
    for (child it = begin(); it != end(); ++it)
        if (obj == *it)
            return ;
    impl->child.append(obj);
}
void ky_object::remove(const child &it)
{
    impl->child.erase(it);
}
void ky_object::remove(ky_object *obj)
{
    for (child it = begin(); it != end(); ++it)
        if (obj == *it)
        {
            remove(it);
            return ;
        }
}
ky_object::child ky_object::begin()
{
    return impl->child.begin();
}
const ky_object::child ky_object::begin()const
{
    return impl->child.begin();
}
ky_object::child ky_object::end()
{
    return impl->child.end();
}
const ky_object::child ky_object::end()const
{
    return impl->child.end();
}
