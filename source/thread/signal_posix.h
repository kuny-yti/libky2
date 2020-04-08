#ifndef SIGNAL_POSIX_H
#define SIGNAL_POSIX_H

#include "posix_fd.h"

struct signal_posix : posix_fd
{
    signal_posix();
    virtual ~signal_posix();

    //!
    //! \brief addset 向信号集中添加信号
    //! \param signo
    //!
    void addset(int signo);
    //!
    //! \brief delset 向信号集中删除信号
    //! \param signo
    //!
    void delset(int signo);

    virtual void despatch();

    void update();

    sigset_t mask;
    u32      signo;
};

#endif // SIGNAL_POSIX_H
