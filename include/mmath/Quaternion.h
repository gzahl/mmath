// BSD 2-Clause License
//
// Copyright (c) 2019, Sean Zheng
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef MMATH_QUATERNION_H_
#define MMATH_QUATERNION_H_

#include <cstdint>

#include "Vector3.h"
#include "util.h"

namespace mmath {

template <typename T>
class Quaternion {
 public:
  union {
    T buf[4];
    struct {
      T w, x, y, z;
    };
  };

  constexpr Quaternion();
  constexpr Quaternion(const Quaternion &v);
  constexpr Quaternion(T w, T x, T y, T z);
  constexpr Quaternion(const Vector<3, T> axis, const T angle);

  constexpr Vector<3, T> ToEulerXYZ();
  constexpr void FromEulerXYZ(const Vector<3, T> &e);
  constexpr T Dot(const Quaternion &q);
  constexpr T Angle(const Quaternion &q);
  constexpr T norm();
};

template <typename T>
constexpr Quaternion<T> operator*(const Quaternion<T> &q1,
                                  const Quaternion<T> &q2);

template <typename T>
constexpr Quaternion<T> operator/(const Quaternion<T> &q1, const T &scalar);

template <typename T>
constexpr Quaternion<T>::Quaternion() : w(1), x(0), y(0), z(0) {}

template <typename T>
constexpr Quaternion<T>::Quaternion(const Quaternion &v)
    : w(v.w), x(v.x), y(v.y), z(v.z) {}

/**
 * q = cos(a/2) + i ( x * sin(a/2)) + j (y * sin(a/2)) + k ( z * sin(a/2))
 * https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/index.htm
 */
template <typename T>
constexpr Quaternion<T>::Quaternion(const Vector<3, T> axis, const T angle)
    : w(cos(angle / 2.)),
      x(axis.x * sin(angle / 2.)),
      y(axis.y * sin(angle / 2.)),
      z(axis.z * sin(angle / 2.)) {}

template <typename T>
constexpr Quaternion<T>::Quaternion(T w, T x, T y, T z)
    : w(w), x(x), y(y), z(z) {}

template <typename T>
constexpr Vector<3, T> Quaternion<T>::ToEulerXYZ() {
  T ex = 0.0, ey = 0.0, ez = 0.0;

  T x2 = this->x + this->x;
  T y2 = this->y + this->y;
  T z2 = this->z + this->z;
  T xx = this->x * x2;
  T xy = this->x * y2;
  T xz = this->x * z2;
  T yy = this->y * y2;
  T yz = this->y * z2;
  T zz = this->z * z2;
  T wx = this->w * x2;
  T wy = this->w * y2;
  T wz = this->w * z2;

  T sx = 1;
  T sy = 1;
  T sz = 1;

  T m11 = (1 - (yy + zz)) * sx;  // Matrix4[0][0] (Matrix4.Element[0])
  T m12 = (xy - wz) * sy;        // Matrix4[1][0] (Matrix4.Element[4])
  T m13 = (xz + wy) * sz;        // Matrix4[2][0] (Matrix4.Element[8])
  T m22 = (1 - (xx + zz)) * sy;  // Matrix4[1][1] (Matrix4.Element[5])
  T m23 = (yz - wx) * sz;        // Matrix4[2][1] (Matrix4.Element[9])
  T m32 = (yz + wx) * sy;        // Matrix4[1][2] (Matrix4.Element[6])
  T m33 = (1 - (xx + yy)) * sz;  // Matrix4[2][2] (Matrix4.Element[10])

  ey = asin(clamp(m13, -1, 1));

  if (abs(m13) < 0.9999999) {
    ex = atan2(-m23, m33);
    ez = atan2(-m12, m11);
  } else {
    ex = atan2(m32, m22);
    ez = 0;
  }

  return Vector<3, T>(ex, ey, ez);
}

template <typename T>
constexpr void Quaternion<T>::FromEulerXYZ(const Vector<3, T> &e) {
  T c1 = cos(e.x / 2.0);
  T c2 = cos(e.y / 2.0);
  T c3 = cos(e.z / 2.0);

  T s1 = sin(e.x / 2.0);
  T s2 = sin(e.y / 2.0);
  T s3 = sin(e.z / 2.0);

  x = s1 * c2 * c3 + c1 * s2 * s3;
  y = c1 * s2 * c3 - s1 * c2 * s3;
  z = c1 * c2 * s3 + s1 * s2 * c3;
  w = c1 * c2 * c3 - s1 * s2 * s3;
}

template <typename T>
constexpr T Quaternion<T>::Dot(const Quaternion<T> &q) {
  Quaternion<T> qt = *this * q;
  return qt.w + qt.x + qt.y + qt.z;
}

template <typename T>
constexpr T Quaternion<T>::Angle(const Quaternion<T> &q) {
  return acos(abs(clamp(Dot(q), -1.0, 1.0))) * 2.0;
}

template <typename T>
constexpr T Quaternion<T>::norm() {
  return sqrt(this->w * this->w + this->x * this->x + this->y * this->y +
              this->z * this->z);
}

template <typename T>
constexpr Quaternion<T> operator*(const Quaternion<T> &q1,
                                  const Quaternion<T> &q2) {
  Quaternion<T> ret;
  ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  ret.x = q1.x * q2.w + q1.w * q2.x + q1.y * q2.z - q1.z * q2.y;
  ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  return ret;
}

template <typename T>
constexpr Quaternion<T> operator/(const Quaternion<T> &q, const T &scalar) {
  return Quaternion<T>(q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar);
}

using QuaternionF = Quaternion<float>;
using Quat32 = Quaternion<float>;
using Quat = Quaternion<float>;
using QuaternionLF = Quaternion<double>;
using Quat64 = Quaternion<double>;

}  // namespace mmath

#endif
