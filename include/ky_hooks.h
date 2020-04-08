
/**
 * Basic tool library
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * @file     ky_hooks.h
 * @brief    装配枚举类型为旗语标志
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2020/02/12
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2020/02/12 | 1.0.0.1   | kunyang  | 创建文件
 *
 */
#ifndef KY_HOOKS_H
#define KY_HOOKS_H

#include "ky_define.h"
/**
 * 为每个钩子保留一个函数指针（允许为NULL）。
 * 传递给 app_hooks_update。
 **/
struct ky_hooks
{
    void (*override_upload_caps)();
    const OsPath& (*get_log_dir)();
    void (*bundle_logs)(FILE* f);
    const char* (*translate)(const char* text);
    void (*translate_free)(const char* text);
    void (*log)(const char* text);
    eErrorReactionInternals (*display_error)(const char* text, uint64 flags);

    static void regisered(ky_hooks *hook);
    static void override_upload();
    static eErrorReactionInternals err_display(const char* text, uint64 flags);
    static const char *translate(const char *text);
};

#endif
