
/**
 * Universal interface header
 * Copyright (C) 2014 kunyang kunyang.yk@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file     ievent.h
 * @brief    事件基础接口
 *       1.触控板、触摸屏、键盘、鼠标、操纵杆的输入事件定义.
 *       2.系统窗口、系统分发、系统剪切板、系统拖拽的系统事件定义.
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.3.1.1
 * @date     2013/05/11
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2013/05/11 | 1.0.0.1   | kunyang  | 创建文件
 * 2013/06/19 | 1.0.1.0   | kunyang  | 键盘、鼠标、系统窗口事件定义
 * 2014/04/06 | 1.1.0.1   | kunyang  | 重构类将其分为实现部分和基础宏部分
 * 2014/09/22 | 1.1.1.0   | kunyang  | 增加系统事件分发和系统拖拽事件
 * 2016/11/20 | 1.2.0.1   | kunyang  | 修改占位标志，并增加操纵杆定义
 * 2017/03/10 | 1.3.0.1   | kunyang  | 增加触摸板和触控板定义，并修改字段宏
 * 2018/12/13 | 1.3.0.2   | kunyang  | 将系统事件标志修改为通知事件
 * 2019/09/02 | 1.3.1.1   | kunyang  | 为了兼容所有系统将窗口事件定义为系统事件
 */
#ifndef IEVENT_H
#define IEVENT_H

#include "ky_define.h"
#include "ky_flags.h"

/* 标志位宽 */
#define iFlagsWide 0x01

/* 事件定义宏 */
#define iEventDefine(val, field) ((val) << (field))
/* 事件定义获取宏 */
#define iEventGetDef(val, field) ((val) >> (field))

/* 通知事件标志位定义 */
#define iNotifyFlags 31
/* 通知事件掩码段定义 */
#define iNotifyMaskField (iFlagsWide << iNotifyFlags)

/* 系统事件标志位定义 */
#define iSystemFlags 30
/* 系统事件掩码段定义 */
#define iSystemMaskField (iFlagsWide << iSystemFlags)

/* 键盘事件标志位定义 */
#define iKeybordFlags 29
/* 键盘事件掩码段定义 */
#define iKeybordMaskField (iFlagsWide << iKeybordFlags)

/* 鼠标事件标志位定义 */
#define iMouseFlags 28
/* 鼠标事件掩码段定义 */
#define iMouseMaskField (iFlagsWide << iMouseFlags)

/* 触摸屏事件标志位定义 */
#define iTouchScreenFlags 27
/* 触摸屏事件掩码段定义 */
#define iTouchScreenMaskField (iFlagsWide << iTouchScreenFlags)

/* 触控板事件标志位定义 */
#define iTouchpadFlags 26
/* 触控板事件掩码段定义 */
#define iTouchpadMaskField (iFlagsWide << iTouchpadFlags)

/* 操纵杆事件标志位定义 */
#define iJoystickFlags 25
/* 操纵杆事件掩码段定义 */
#define iJoystickMaskField (iFlagsWide << iJoystickFlags)

/* 剪切板事件标志位定义 */
#define iClipboardFlags 24
/* 剪切板事件掩码段定义 */
#define iClipboardMaskField (iFlagsWide << iClipboardFlags)

/* 拖放事件标志位定义 */
#define iDragDropFlags 23
/* 拖放事件掩码段定义 */
#define iDragDropMaskField (iFlagsWide << iDragDropFlags)

typedef enum
{
    Event_Notify      = iEventDefine(1, iNotifyFlags), //通知(系统消息，套接字，异步IO)
    Event_System      = iEventDefine(1, iSystemFlags),
    Event_Keybord     = iEventDefine(1, iKeybordFlags),
    Event_Mouse       = iEventDefine(1, iMouseFlags),
    Event_TouchScreen = iEventDefine(1, iTouchScreenFlags),
    Event_Touchpad    = iEventDefine(1, iTouchpadFlags),
    Event_Joystick    = iEventDefine(1, iJoystickFlags),
    Event_Clipboard   = iEventDefine(1, iClipboardFlags),
    Event_DragDrop    = iEventDefine(1, iDragDropFlags)
}eEvents;
kyDeclareFlags(eEvents, eEventFlags);

/* 通知事件定义宏 */
#define iNotifyEventMacro(val) (Event_Notify | (val))

/* 系统事件定义宏 */
#define iSystemEventMacro(val) (Event_System | (val))

/* 键盘事件定义宏 */
#define iKeybordEventMacro(val) (Event_Keybord | (val))

/* 鼠标事件定义宏 */
#define iMouseEventMacro(val) (Event_Mouse | (val))

/* 触摸屏事件定义宏 */
#define iTouchScreenEventMacro(val) (Event_TouchScreen | (val))

/* 触控板事件定义宏 */
#define iTouchpadEventMacro(val) (Event_Touchpad | (val))

/* 操纵杆事件定义宏 */
#define iJoystickEventMacro(val) (Event_Joystick | (val))

/* 剪切板事件定义宏 */
#define iClipboardEventMacro(val) (Event_Clipboard | (val))

/* 拖放事件定义宏 */
#define iDragDropEventMacro(val) (Event_DragDrop | (val))

/* 是否有evt事件 */
#define iEventIs(code, evt) ((code) & (evt))
/* 是否匹配evt事件 */
#define iEventMatch(code, evt) (iEventIs((code), (evt)) == (evt))

struct ievent
{
    explicit ievent(int v = 0):_code(v){}

    virtual ~ievent(){}

    int code()const{return _code;}

    i64 &timestamp() {return _times;}
    i64 timestamp()const {return _times;}

    bool is_notify()const{return iEventIs(code(), Event_Notify);}
    bool is_keybord()const{return iEventIs(code(), Event_Keybord);}
    bool is_mouse()const{return iEventIs(code(), Event_Mouse);}
    bool is_system()const{return iEventIs(code(), Event_System);}
    bool is_touchscreen()const{return iEventIs(code(), Event_TouchScreen);}
    bool is_touchpad()const{return iEventIs(code(), Event_Touchpad);}
    bool is_joystick()const{return iEventIs(code(), Event_Joystick);}
    bool is_clipboard()const{return iEventIs(code(), Event_Clipboard);}
    bool is_dragdrop()const{return iEventIs(code(), Event_DragDrop);}

    int _code;
    i64 _times;
};

#endif // IEVENT_H
