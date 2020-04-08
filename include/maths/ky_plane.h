
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
 * @file     ky_plane.h
 * @brief    3D平面类
 *
 * @author   kunyang
 * @email    kunyang.yk@gmail.com
 * @version  1.0.0.1
 * @date     2014/04/09
 * @license  GNU General Public License (GPL)
 *
 * Change History :
 *    Date    |  Version  |  Author  |   Description
 * 2014/04/09 | 1.0.0.1   | kunyang  | 创建文件
 */
#ifndef ky_plane_H
#define ky_plane_H
#include "ky_vec4.h"
#include "ky_line3.h"
#include "tools/ky_vector.h"


/**
 * @brief a ky_plane class
 * The ky_plane is stored in hessian normal form: <br>
 * n*x + d = 0; <br>
 * n is the unit normal of this ky_plane pointing from the origin to this ky_plane <br>
 * (-d) >= 0 is the distance from the origin. <br>
 * <br>
 * Note: You can transform this ky_plane like this: <br>
 * myky_plane.toVec4() = myMatrix4 * myky_plane.toVec4(); <br>
 * <br>
 * Perhaps you want to do this: <br>
 * mat4 mv = tgt::getModelViewMatrix(); <br>
 * mat4 inv; <br>
 * mv.invert(inv); <br>
 * myky_plane.toVec4() = inv * myky_plane.toVec4(); <br>
*/
template<typename T>
struct ky_plane
{
    typedef T ElemType;

    typedef enum PlaneSide
    {
        FrontSide,
        BackSide,
        OnSide
    }ePlaneSides;

    /// 平面的法向量
    ky_vector3<T> norm;
    /// 平面的距离
    T             dist;

    ky_plane():norm(),dist((T)0) {}

    /// 法向量和距离构造
    ky_plane(T a, T b, T c, T d) :
        norm(a, b, c) ,
        dist(d)
    {

    }

    ky_plane(const ky_vector3<T>& n, T d):
        norm(n),
        dist(d)
    {

    }

    /// 三个顶点构造一个面
    ky_plane(const ky_vector3<T>& v1, const ky_vector3<T>& v2, const ky_vector3<T>& v3)
    {
        norm  = ky_normalize(ky_cross(v1 - v2, v2 - v3) );
        dist = -ky_dot(v1, n);

        // correct sign of d
        if (dist > T(0))
        {
            norm = -norm;
            dist = -dist;
        }
    }

    template<typename U>
    ky_plane(const ky_plane<U>& p):
        norm(p.norm),
        dist(T(p.dist))
    {

    }

    ky_plane(const ky_plane<T> &v):
        norm(v.norm),
        dist(v.dist)
    {
    }
    inline ky_plane<T> &operator = (const ky_plane<T> &c)
    {
        norm = c.norm;
        dist = c.dist;
        return *this;
    }

    inline const ky_vector4<T>& to_vec4() const
    {
        return *reinterpret_cast<ky_vector4<T>*>(const_cast<ky_plane<T>*>(this)->norm.elem);
    }

    inline ky_vector4<T>& to_vec4()
    {
        return *reinterpret_cast<ky_vector4<T>*>(norm.elem);
    }

    //!
    //! clip_polygon 将多边形裁剪到此平面内
    //! \param src
    //! \param vin  输入多边形
    //! \param vout  输出多边形
    //!
    inline void clip_polygon(const ky_vector<ky_vector3<T>>& vin, ky_vector<ky_vector3<T>>& vout);
    //!
    //! clip_polygon 将多边形裁剪到此平面内
    //! \param src
    //! \param vin  输入多边形
    //! \param vout  输出多边形
    //!
    inline void clip_polygon(const ky_vector3<T>* vin, int64 numIn,
                             ky_vector3<T>* vout, int64& numOut);

    //!
    //! classify_point 返回点在平面的那一边
    //! \param point
    //! \return ePlaneSides
    //!
    inline ePlaneSides classify_point(const ky_vector3<T>& point) const;

    /**
     * This method creates a polygon from this ky_plane.
     * It is ensured that this polygon is huge enough
     * so it may be clipped later against the axis aligned box
     * defined by llf and urb. <br>
     * Note that you can use this method with Vector, too: <br>
     * Vector\<vec3\> vout(4);
     * createQuad(llf, urb, &vout[0]);
     * @param llf lower left front of the input axis aligned box
     * @param urb upper right back of the input axis aligned box
     * @param v the result is written to this array which must be able to hold at least 4 vertices
     */
    inline void create_quad(ky_vector3<T> llf, ky_vector3<T> urb, ky_vector3<T>* v) const;

    /**
     * This method cilps this ky_plane against an axis aligned box defined by llf and urb.
     * The output is written to vout. <br>
     * BEWARE: It is likely that you can gain performance by precomputing the ky_planes of the AAB once,
     * then calculate a proper polygon with createQuad and finally call a static clipAAB version
     * which takes the ky_planes an the polygon as input.
     * @param llf lower left front of the input axis aligned box
     * @param urb upper right back of the input axis aligned box
     * @param vout the result polygon
     */
    inline void clip_aab(const ky_vector3<T>& llf, const ky_vector3<T>& urb, ky_vector< ky_vector3<T> >& vout) const;

    static void create_cube_vertices(const ky_vector3<T>& llf, const ky_vector3<T>& urb, ky_vector3<T> cubeVertices[8]);

    static void create_cube_planes(const ky_vector3<T> cv[8], ky_plane<T> cube_planes[6]);

    /**
     * Creates ky_planes in the aabky_planes array from a axis aligned box given via llf and urb. The normal points to inside.
     * @param llf lower left front of the input axis aligned box
     * @param urb upper right back of the input axis aligned box
     * @param cubeky_planes the result is written to this array which must be able to hold at least 6 ky_planes
     */
    static void create_aab_planes(const ky_vector3<T>& llf, const ky_vector3<T>& urb, ky_plane<T> aabb_planes[6])
    {
        ky_vector3<T> cv[8];// cube vertices
        create_cube_vertices(llf, urb, cv);
        // cube ky_planes
        create_cube_planes(cv, aabb_planes);
    }

   /**
     * This routine clips a polygon against 6 ky_planes
     * @param aabky_planes against these 6 ky_planes is clipped
     * @param vin Points to the input vertices. <br>
     *      BEWARE: vin is changed after this call. <br>
     *      AND EVEN WORSE: The size should be enough. Buffer overruns are likely!!!
     * @param numIn number of vertices in vin
     * @param vout Points to the output vertices. This is the result. <br>
     *      BEWARE: The size should be enough. Buffer overruns are likely!!!
     * @param numOut Number of output vertices. This a result, too.
     */
    static void clip_aab(ky_plane<T> aab_planes[6], ky_vector3<T>* vin, int64 numIn,
                          ky_vector3<T>* vout, int64& numOut);

    /**
     * This routine clips a polygon against 6 ky_planes
     * @param aabky_planes against these 6 ky_planes is clipped
     * @param vin The input vertices. <br>
     *      BEWARE: vin is changed after this call.
     * @param vout Points to the output vertices. This is the result,
     */
    static void clip_aab(ky_plane<T> aab_planes[6], ky_vector< ky_vector3<T> >& vin, ky_vector< ky_vector3<T> >& vout);
};


//!
//! ky_distance 返回点到面的距离
//! \param p
//! \param l 检测的线条
//! \param f 有交点时，计算点的参数将从f返回
//!
template<typename T>
inline T ky_distance(const ky_plane<T> &p, const ky_vector3<T>& v)
{
    return ky_dot(p.norm, v) + p.dist;
}

//!
//! ky_normalize
//! \param p
//!
template<typename T>
inline ky_plane<T> & ky_normalize(ky_plane<T> &p)
{
    T scale = (T)1.0/ ky_length(p.norm);
    p.norm *= scale;
    p.dist *= scale;
    return p;
}

//!
//!ky_intersect 面是否和线相交
//! \param p
//! \param l 检测的线条
//! \param f 有交点时，计算点的参数将从f返回
//!
template<typename T>
inline bool ky_intersects(const ky_plane<T> &src, const ky_line3<T> &l, ky_vector3<T>* p = 0) const
{
    T f0 = ky_distance(src, l.position());
    T f1 = -ky_dot(src.norm, l.direction());

    if (ky_fabs(f1) > 0.00001f)
    {
        if (p)
            *p  = ky_interpolate(l.start(), l.end(), f0 / f1);
        return true;
    }
    return false;
}

//!
//! ky_intersect 面是否和面相交
//! \param src
//! \param p 检测的面
//! \param l 有交线时，计算点的参数将从l返回
//!
template<typename T>
inline bool ky_intersects(const ky_plane<T>& src, const ky_plane<T>& p, ky_line3<T> &l)
{
    ky_vector3<T> pn = p.norm;
    T n00 = ky_dot(src.norm,  src.norm);
    T n01 = ky_dot(src.norm, pn);
    T n11 = ky_dot(pn, pn);
    T det = n00 * n11 - n01 * n01;

    if (ky_fabs(det) > 0.00001f)
    {
        T inv_det = 1.0f / det;
        T c0 = (-n11 * -src.dist + n01 * -p.dist ) * inv_det;
        T c1 = (-n00 * -p.dist   + n01 * -src.dist) * inv_det;
        l.set_start(n * c0 + pn * c1);
        l.set_direction(ky_cross(src.norm, pn));
        return true;
    }
    return false;
}

template<typename T>
ky_plane<T>::ePlaneSides ky_plane<T>::classify_point(const ky_vector3<T>& point) const
{
    const T dist = ky_distance(*this, point);

    if (dist > T(.001))
        return Plane_Front;
    else if (dist < -T(.001))
        return Plane_Back;

    return Plane_On;
}

template<typename T>
void ky_plane<T>::clip_polygon(const ky_vector<ky_vector3<T>>& vin, ky_vector<ky_vector3<T>>& vout)
{
    ky_vector<T> vd;
    for (int64 i = 0; i < vin.count(); ++i)
        vd.append (ky_distance(*this, vin[i]));

    for (int64 i1 = 0, i2 = 1; i1 < vin.count(); ++i1, ++i2)
    {
        i2 = ( i2 == vin.count() ) ? 0 : i2;

        // inside-inside
        if (vd[i1] < T(0) && vd[i2] < T(0))
            vout.append ( vin[i2] );
        // inside-outside OR outside-inside
        else if (vd[i1] * vd[i2] < 0.f)
        {
            T scale = vd[i1] / (vd[i1] - vd[i2]);
            // interpolate vertex
            vout.append (ky_vector3<T>(vin[i1] + (vin[i2] - vin[i1]) * scale));

            // outside-inside
            if (vd[i1] > T(0) && vd[i2] < T(0))
                vout.append( vin[i2] );
        }
        // do nothing for outside-outside
    }
}

template<typename T>
void ky_plane<T>::clip_polygon(const ky_vector3<T>* vin, int64 numIn,
                               ky_vector3<T>* vout, int64& numOut)
{
    T* vd = kyNew(T[numIn]) ;
    numOut = 0;

    for (int64 i = 0; i < numIn; ++i)
        vd[i] = ky_distance(*this, vin[i]);

    for (int64 i1 = 0, i2 = 1; i1 < numIn; ++i1, ++i2)
    {
        i2 = ( i2 == numIn ) ? 0 : i2;

        // inside-inside
        if (vd[i1] < T(0) && vd[i2] < T(0))
        {
            vout[numOut] = vin[i2];
            ++numOut;
        }
        // inside-outside OR outside-inside
        else if (vd[i1] * vd[i2] < 0.f)
        {
            T scale = vd[i1] / (vd[i1] - vd[i2]);
            // interpolate vertex
            vout[numOut] = vin[i1] + (vin[i2] - vin[i1]) * scale;
            ++numOut;

            // outside-inside
            if (vd[i1] > T(0) && vd[i2] < T(0))
            {
                vout[numOut] = vin[i2];
                ++numOut;
            }
        }
        // do nothing for outside-outside
    }

    // clean up
    kyDelete([] vd);
}

template<typename T>
void ky_plane<T>::create_cube_vertices(const ky_vector3<T>& llf, const ky_vector3<T>& urb,
                                       ky_vector3<T> cubeVertices[8])
{
    cubeVertices[0] = ky_vector3<T>(llf.x, llf.y, urb.z);// llb 0
    cubeVertices[1] = ky_vector3<T>(urb.x, llf.y, urb.z);// lrb 1
    cubeVertices[2] = ky_vector3<T>(urb.x, urb.y, urb.z);// urb 2
    cubeVertices[3] = ky_vector3<T>(llf.x, urb.y, urb.z);// ulb 3

    cubeVertices[4] = ky_vector3<T>(llf.x, llf.y, llf.z);// llf 4
    cubeVertices[5] = ky_vector3<T>(urb.x, llf.y, llf.z);// lrf 5
    cubeVertices[6] = ky_vector3<T>(urb.x, urb.y, llf.z);// urf 6
    cubeVertices[7] = ky_vector3<T>(llf.x, urb.y, llf.z);// ulf 7
}

template<typename T>
void ky_plane<T>::create_cube_planes(const ky_vector3<T> cv[8], ky_plane<T> cube_planes[6])
{
    cube_planes[0] = ky_plane<T>( cv[2], cv[1], cv[0] );// front
    cube_planes[1] = ky_plane<T>( cv[0], cv[3], cv[7] );// left
    cube_planes[2] = ky_plane<T>( cv[4], cv[5], cv[6] );// back
    cube_planes[3] = ky_plane<T>( cv[6], cv[2], cv[1] );// right
    cube_planes[4] = ky_plane<T>( cv[5], cv[1], cv[0] );// bottom
    cube_planes[5] = ky_plane<T>( cv[7], cv[3], cv[2] );// top
}

template<typename T>
void ky_plane<T>::create_quad(ky_vector3<T> llf, ky_vector3<T> urb, ky_vector3<T>* v) const
{
    // copy of llf and urb is needed here
    T param;
    ky_line3<T> l;

    // scale to ensure that a polygon is created that is huge enough
    urb.x += T(5);
    urb.y += T(5);
    urb.z -= T(5);
    llf.x -= T(5);
    llf.y -= T(5);
    llf.z += T(5);

    // check whether the ky_plane's normal lies in the x-y ky_plane
    if (ky_abs(norm).z < T(0.000001))
    {
        ky_vector3<T> llb(llf.x, llf.y, urb.z);
        ky_vector3<T> urf(urb.x, urb.y, llf.z);

        // each found vector must be scaled hence a overlapping polygon is guaranteed
        l = ky_line3<T>(urb, urb + ky_vector3<T>(T(1), T(0), T(0)));
        ky_intersects(*this, l, &param);
        v[0] = l.interpolates (param);

        l = ky_line3<T>(urf, urf + ky_vector3<T>(T(1), T(0), T(0)));
        ky_intersects(*this, l, &param);
        v[1] = l.interpolates(param);

        l = ky_line3<T>(llf, llf + ky_vector3<T>(T(1), T(0), T(0)));
        ky_intersects(*this, l, &param);
        v[2] = l.interpolates(param);

        l = ky_line3<T>(llb, llb + ky_vector3<T>(T(1), T(0), T(0)));
        ky_intersects(*this, l, &param);
        v[3] = l.interpolates(param);
    }
    else
    {
        ky_vector3<T> lrf(urb.x, llf.y, llf.z);
        ky_vector3<T> ulb(llf.x, urb.y, urb.z);

        // each found vector must be scaled hence a overlapping polygon is guaranteed
        l = ky_line3<T>(ulb, ulb + ky_vector3<T>(T(0), T(0), T(1)));
        ky_intersects(*this, l, &param);
        v[0] = l.interpolates(param);

        l = ky_line3<T>(urb, urb + ky_vector3<T>(T(0), T(0), T(1)));
        ky_intersects(*this, l, &param);
        v[1] = l.interpolates(param);

        l = ky_line3<T>(lrf, lrf + ky_vector3<T>(T(0), T(0), T(1)));
        ky_intersects(*this, l, &param);
        v[2] = l.interpolates(param);

        l = ky_line3<T>(llf, llf + ky_vector3<T>(T(0), T(0), T(1)));
        ky_intersects(*this, l, &param);
        v[3] = l.interpolates(param);
    }
}

template<typename T>
void ky_plane<T>::clip_aab(ky_plane<T> aabb_planes[6], ky_vector3<T>* vin, int64 numIn,
                            ky_vector3<T>* vout, int64& numOut)
{
    for (int64 i = 0; i < 6; ++i)
    {
        aabb_planes[i].clip_polygon(vin, numIn, vout, numOut);

        if (i == 5)
            return;

        // swap
        ky_vector3<T>* tmp = vin;
        vin = vout;
        vout = tmp;
        numIn = numOut;
    }
}

template<typename T>
void ky_plane<T>::clip_aab(ky_plane<T> aabb_planes[6], ky_vector<ky_vector3<T> > &vin,
                            ky_vector<ky_vector3<T> > &vout)
{
    for (int64 i = 0; i < 6; ++i)
    {
        aabb_planes[i].clip_polygon(vin, vout);

        if (i == 5)
            return;

        ky_swap(vin, vout);
        vout.clear();
    }
}

template<typename T>
void ky_plane<T>::clip_aab(const ky_vector3<T>& llf, const ky_vector3<T>& urb, ky_vector<ky_vector3<T> > &vout) const
{
    // 1. compute AAB ky_planes
    ky_plane<T> aab_planes[6];
    create_aab_planes(llf, urb, aab_planes);

    // 2. calculate a polygon which is huge enough
    ky_vector< ky_vector3<T> > vin(4);
    create_quad(llf, urb, &vin[0]);

    // 3. clip against all 6 ky_planes of the AAB
    clip_aab(aab_planes, vin, vout);
}

typedef ky_plane<int>      iplane;
typedef ky_plane<float>    fplane;
typedef ky_plane<real>     dplane;


/// ostream-operator
template <typename T>
std::ostream& operator << (std::ostream& s, const ky_plane<T>& p) {
    return (s << p.n << " * x + " << p.d << " = 0");
}



template<typename T>
ky_streamb &operator << (ky_streamb &in, const ky_plane<T> &v)
{
    in << v.d;
    in << v.n;
    return in;
}

template<typename T>
ky_streamb &operator >> (ky_streamb &out, ky_plane<T> &v)
{
    out >> v.d;
    out >> v.n;
    return out;
}
template<typename T>
ky_streamt &operator << (ky_streamt &in, const ky_plane<T> &v)
{
    in << v.d;
    in << v.n;
    return in;
}

template<typename T>
ky_streamt &operator >> (ky_streamt &out, ky_plane<T> &v)
{
    out >> v.d;
    out >> v.n;
    return out;
}
#endif // ky_plane_H
