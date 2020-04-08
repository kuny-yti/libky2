#ifndef PIPE_POSIX_H
#define PIPE_POSIX_H

#include "posix_fd.h"

struct pipe_posix : posix_fd
{
    pipe_posix();
    virtual ~pipe_posix();

    //!
    //! \brief active 激活管道
    //! \return
    //!
    bool active();

    //!
    //! \brief cancel 取消激活
    //! \return
    //!
    bool cancel();

    //!
    //! \brief is_active 管道是否被激活
    //! \return
    //!
    bool is_active()const;

    void despatch();

    int  inl_priv[2];
    int  inl_flag;
};

#endif // PIPE_POSIX_H
