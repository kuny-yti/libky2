
/**
 * Basic tool library
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
 * @file     ky_camera.h
 * @brief    三维相机
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2019/08/19
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2019/08/19 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_CAMERA_H
#define ky_CAMERA_H
#include "maths/ky_vec4.h"
#include "maths/ky_quat.h"
#include "maths/ky_mat4x4.h"
#include "maths/ky_frustum.h"

template<typename T>
struct ky_camera : public ky_frustum<T> ///< 相机视锥
{
    typedef enum
    {
        Orthogonal ,  ///< 正交投影
        Perspective,  ///< 透视投影
        Frustum       ///< 视锥投影
    }ProjectionMode;

    ProjectionMode _mode;         ///< 相机模式
    ky_vector3<T>  _position;     ///< 相机眼睛位置
    ky_vector3<T>  _focus;        ///< 相机焦点位置(目标位置)
    ky_vector3<T>  _up;           ///< 相机方向矢量

    ky_camera():ky_frustum<T>(), _mode(Perspective),_position(),_focus(),_up(){}
    ky_camera(const ky_vector3<T> &pos, const ky_vector3<T> &focus, const ky_vector3<T> &up,
              const T &fovy, const T &near, const T &far, const ky_vector2<T> &size,
              ProjectionMode mode = Perspective):
        ky_frustum<T>(fovy, T(size.width / size.height), near, far),
        _mode(mode),
        _position(pos),
        _focus(focus),
        _up(ky_normalize(up))
    {
    }

    //!
    //! \brief position 返回相机眼睛位置
    //! \return
    //!
    inline ky_vector3<T> position()const {return _position;}
    //!
    //! \brief focus 返回相机焦点位置
    //! \return
    //!
    inline ky_vector3<T> focus()const {return _focus;}
    //!
    //! \brief up 返回相机方向矢量
    //! \return
    //!
    inline ky_vector3<T> up()const {return _up;}
    //!
    //! \brief mode 返回相机模式
    //! \return
    //!
    inline ProjectionMode mode()const {return _mode;}
    //!
    //! \brief frustum 返回相机的视锥
    //! \return
    //!
    inline ky_frustum<T> frustum()const{return *this;}

    //!
    //! \brief look 观看位置
    //! \return
    //!
    inline ky_vector3<T> look()const { return ky_normalize(_focus - _position); }
    //!
    //! \brief strafe
    //! \return
    //!
    inline ky_vector3<T> strafe()const { return ky_normalize(ky_cross(look(), _up)); }
    //!
    //! \brief focal_length 相机的焦距
    //! \return
    //!
    inline T focal_length() const {return ky_distance(_focus, _position);}

    inline void set_mode(ProjectionMode pm)
    {
        if(pm == Perspective)
        {
            set_right(-ky_frustum<T>::left());
            set_top(-ky_frustum<T>::bottom());
        }
        mode = pm;
    }
    inline void reset(const ky_vector3<T> &pos, const ky_vector3<T> &focus, const ky_vector3<T> &up)
    {
        set_position(pos);
        set_focus(focus);
        set_up(up);
    }
    //!
    //! \brief set_focal_length 设置相机的焦距
    //! \param f
    //!
    inline void set_focal_length(T f) {set_focus(_position + f * look());}
    //!
    //! \brief position 设置相机眼睛位置
    //! \param pos 位置
    //!
    inline void set_position(const ky_vector3<T>& pos) {_position = pos;}
    //!
    //! \brief focus 设置相机焦点位置
    //! \param foc 焦点
    //!
    inline void set_focus(const ky_vector3<T>& foc)  {_focus = foc;}
    //!
    //! \brief up 设置相机方向矢量
    //! \param up 矢量
    //!
    inline void set_up(const ky_vector3<T>& up)  {_up = ky_normalize(up);}

    //!
    //! \brief set_fovy 设置相机视角
    //! \param fovy 角度
    //!
    inline void set_fovy(T fovy)
    {
        if(mode == Perspective)
        {
            T oldRatio = ky_frustum<T>::ratio();
            ky_frustum<T>::set_fovy(fovy);
            ky_frustum<T>::set_ratio(oldRatio);
        }
    }
    //!
    //! \brief set_ratio 设置宽高比
    //! \param ratio
    //!
    inline void set_ratio(T ratio)
    {
        if(mode == Perspective)
            ky_frustum<T>::set_ratio(ratio);
    }
    inline void set_near(T neardist)
    {
        if(mode == Perspective)
        {
            T oldFovy = ky_frustum<T>::fovy();
            T oldRatio = ky_frustum<T>::ratio();
            ky_frustum<T>::set_near(neardist);
            ky_frustum<T>::set_fovy(oldFovy);
            ky_frustum<T>::set_ratio(oldRatio);
        }
        else
            ky_frustum<T>::set_near(neardist);
    }
    inline void set_left(T v)
    {
        ky_frustum<T>::set_left(v);
        if(modes == Perspective)
            ky_frustum<T>::set_right(-v);
    }
    inline void set_right(T v)
    {
        ky_frustum<T>::set_right(v);
        if(modes == Perspective)
            ky_frustum<T>::set_left(-v);
    }
    inline void set_top(T v)
    {
        ky_frustum<T>::set_top(v);
        if(modes == Perspective)
            ky_frustum<T>::set_bottom(-v);
    }
    inline void set_bottom(T v)
    {
        ky_frustum<T>::set_bottom(v);
        if(modes == Perspective)
            ky_frustum<T>::set_top(-v);
    }

    //!
    //! \brief generate_projection 生成投影矩阵
    //! \return
    //!
    ky_matrix4X4<T> generate_projection()const
    {
        const T aspect_ratio = ky_frustum<T>::ratio();
        if (mode == Orthogonal)
        {
            if(aspect_ratio > T(1))
                return ky_matrix4X4<T>::create_ortho(left(), right(),
                                                     top(), bottom(),
                                                     -near(), far());

            return ky_matrix4X4<T>::create_ortho(left(), right(),
                                                 top() * (T(1) / aspect_ratio),
                                                 bottom() * (T(1) / aspect_ratio),
                                                 -near(), far());
        }
        else if (mode = Perspective)
        {
            const T fovy = ky_frustum<T>::fovy();
            if(fovy < T(6))
                fovy = T(6);
            if(fovy > T(175))
                fovy = T(175);

            if (aspect_ratio >= T(1))
                return ky_matrix4X4<T>::create_perspective(ky_deg2rad(fovy), aspect_ratio,
                                                           near(), far());
            else
                return ky_matrix4X4<T>::create_perspective(ky_atan(ky_tan(ky_deg2rad(fovy/T(2))) / ratio()*2),
                                                           ratio(), near(), far());
        }
        return ky_frustum<T>::generate_matrix();
    }

    //!
    //! \brief generate_projection 生成视图矩阵
    //! \return
    //!
    ky_matrix4X4<T> generate_view()const
    {
        return ky_matrix4X4<T>::create_lookAt(_position, _focus, _up);
    }
    //!
    //! \brief generate_projection 生成方向矩阵
    //! \return
    //!
    ky_matrix4X4<T> generate_orientation()const
    {
        ky_matrix4X4<T> result;
        generate_view().invert(result);
        return result;
    }
    //!
    //! \brief to_quaternion 转换到四元数
    //! \return
    //!
    ky_quaternion<T> to_quaternion() const
    {
        return generate_quaternion(generate_view().get_rotational());
    }

    void update(const ky_aabb<T> &scissor)
    {
        const ky_matrix4X4<T> mat = ky_transpose(generate_projection() * generate_view());
        const ky_vector3<T> smax = scissor.max();
        const ky_vector3<T> smin = scissor.min();

        // get the RIGHT plane
        ky_plane<T> &right = ky_frustum<T>::_plane[ky_frustum::Right];
        right.norm.x = smax.x * mat.t03 - mat.t00;
        right.norm.y = smax.x * mat.t13 - mat.t10;
        right.norm.z = smax.x * mat.t23 - mat.t20;
        right.dist   = smax.x * mat.t33 - mat.t30;

        // get the LEFT plane
        ky_plane<T> &left = ky_frustum<T>::_plane[ky_frustum::Left];
        left.norm.x = -smin.x * mat.t03 + mat.t00;
        left.norm.y = -smin.x * mat.t13 + mat.t10;
        left.norm.z = -smin.x * mat.t23 + mat.t20;
        left.dist   = -smin.x * mat.t33 + mat.t30;

        // get the BOTTOM plane
        ky_plane<T> &bottom = ky_frustum<T>::_plane[ky_frustum::Bottom];
        bottom.norm.x = -smin.y * mat.t03 + mat.t01;
        bottom.norm.y = -smin.y * mat.t13 + mat.t11;
        bottom.norm.z = -smin.y * mat.t23 + mat.t21;
        bottom.dist   = -smin.y * mat.t33 + mat.t31;

        // get the TOP plane
        ky_plane<T> &top = ky_frustum<T>::_plane[ky_frustum::Top];
        top.norm.x = smax.y * mat.t03 - mat.t01;
        top.norm.y = smax.y * mat.t13 - mat.t11;
        top.norm.z = smax.y * mat.t23 - mat.t21;
        top.dist   = smax.y * mat.t33 - mat.t31;

        // get the FAR plane
        ky_plane<T> &far = ky_frustum<T>::_plane[ky_frustum::Far];
        far.norm.x = smax.z * mat.t03 - mat.t02;
        far.norm.y = smax.z * mat.t13 - mat.t12;
        far.norm.z = smax.z * mat.t23 - mat.t22;
        far.dist   = smax.z * mat.t33 - mat.t32;

        // get the NEAR plane
        ky_plane<T> &near = ky_frustum<T>::_plane[ky_frustum::Near];
        near.norm.x = -smin.z * mat.t03 + mat.t02;
        near.norm.y = -smin.z * mat.t13 + mat.t12;
        near.norm.z = -smin.z * mat.t23 + mat.t22;
        near.dist   = -smin.z * mat.t33 + mat.t32;

        for (int i = 0; i < kyArraySizeOf(ky_frustum<T>::_plane); ++i)
            ky_frustum<T>::_plane[i] = ky_normalize(ky_frustum<T>::_plane[i]);
    }

    void set_matrix_view(const ky_matrix4X4<T>& mvMat)
    {
        ky_matrix4X4<T> inv;
        if (mvMat.invert(inv))
        {
            // preserve the focallength
            T focallength = focal_length();// length(focuss - positions);

            // calculate world-coordinates
            vec4 pos   = (inv * vec4(0.f, 0.f,  0.f, 1.f));
            vec4 look  = (inv * vec4(0.f, 0.f, -1.f, 0.f));
            vec4 focus = pos + focallength * look;
            vec4 up    = (inv * vec4(0.f, 1.f,  0.f, 0.f));

            reset(pos.xyz(), focus.xyz(), up.xyz());
        }
    }


    //!
    //! \brief clone 克隆一个相机
    //! \return
    //!
    inline ky_camera clone() const ;
};


template<typename T>
inline generate_frustum_normal (const ky_camera<T> &camera)
{
    const ky_frustum<T> frustum = camera.frustum();

    ky_vector3<T> normal[ky_frustum::Count];

    normal[ky_frustum::Far]  = camera.look();
    normal[ky_frustum::Near] = T(-1) * normal[ky_frustum::Far];

    ky_vector3<T> cam2near = normal[ky_frustum::Far] * frustum.near();

    nearp_ = camera.position() + cam2near;
    farp_  = camera.position() + normal[ky_frustum::Far] * frustum.far();

    ky_vector3<T> strafe = camera.strafe();
    ky_vector3<T> tmp;

    tmp = cam2near + frustum.top() * ky_cross(strafe, camera.look());
    normal[ky_frustum::Top] = ky_normalize(ky_cross(strafe, tmp));

    tmp = cam2near + frustum.bottom() * ky_cross(strafe, camera.look());
    nnormal[ky_frustum::Bottom] = ky_normalize(ky_cross(tmp, strafe));

    tmp = cam2near + frustum.left() * strafe;
    nnormal[ky_frustum::Left] = ky_normalize(ky_cross(ky_cross(strafe, camera.look()), tmp));

    tmp = cam2near + frustum.right() * strafe;
    nnormal[ky_frustum::right()] = ky_normalize(ky_cross(tmp, ky_cross(strafe, camera.look())));

}

#endif
