#include "ky_linked.h"

const _ky_linked_ _ky_linked_::shared_null =
{
    (_ky_linked_*)&_ky_linked_::shared_null,
    (_ky_linked_*)&_ky_linked_::shared_null,
    ky_ref::Static, 0
};

bool _ky_linked_::is_null (intptr p)
{
    return p == (intptr)&shared_null;
}
intptr _ky_linked_::null()
{
    return (intptr)&shared_null;
}
