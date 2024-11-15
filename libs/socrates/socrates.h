// Socrates v1.0 - A single-file easy to use math library made for game development
// 
// To include the library in your project, copy this file (socrates.h) into the directory where you usually 
// keep your libraries. And then... that's it. No compilation, no setting up for shared or static libraries, and 
// no messing around with CMake or other build systems. Everything you'll need to know is in this one file you're 
// currently viewing. 
//
// If you have any inquires or potential bugs, please contact me: https://mohamedag2002.github.io/

#pragma once

#include <cmath>

namespace soc { // Start of soc

// Platform defines
///////////////////////////////////////////////////////////////

#ifdef _MSC_VER
// Windows inline keyword
#define SOC_INLINE   __forceinline 
#define SOC_NOINLINE __declspec(noinline)
#else 
// Non-Windows inline keyword
#define SOC_INLINE   inline 
#define SOC_NOINLINE 
#endif

///////////////////////////////////////////////////////////////

// Defines
///////////////////////////////////////////////////////////////

// The value of PI
#define SOC_PI 3.14159265359

// Radians to degrees multiplier  
#define SOC_RAD2DEG (180.0f / SOC_PI)

// Degrees to radians multiplier
#define SOC_DEG2RAD (SOC_PI / 180.0f)

// The epsilon which is the lowest possible decimal point value 
#define SOC_EPSILON 1.192092896e-07f

#define SOC_FLOAT_MIN -3.40282e+38F
#define SOC_FLOAT_MAX  3.40282e+38F

///////////////////////////////////////////////////////////////

// Typedefs
///////////////////////////////////////////////////////////////

// char
typedef char  int8;

// short
typedef short int16; 

// int
typedef int   int32; 

// long
typedef long  int64;

// unsigned char
typedef unsigned char  uint8;

// unsigned short
typedef unsigned short uint16; 

// unsigned int
typedef unsigned int   uint32; 

// unsigned long
typedef unsigned long  uint64;

// float
typedef float  float32;

// double
typedef double float64;

///////////////////////////////////////////////////////////////

// Socrates types
///////////////////////////////////////////////////////////////

// Two component vector
union Vector2 {
  float32 components[2];

  struct {
    float32 x, y;
  };

  struct {
    float32 u, v; 
  };
  
  struct {
    float32 r, g; 
  };
  
  // Default CTOR
  Vector2() {
    x = 0.0f; 
    y = 0.0f;
  }

  // Takes the `x` and `y` components of the vector 
  Vector2(float32 x, float32 y) 
    :x(x), y(y)
  {}

  // Fils all of the components with the given value 
  Vector2(float32 s)
    :x(s), y(s) 
  {}

  // Index operator overload into the components
  // NOTE: This indexing operator overload and all other overloads of this type 
  // will NOT check for out of bounds indices. Please keep this in mind
  float32 operator[](const uint32 index) {
    return components[index];
  }
  
  float32 operator[](const uint32 index) const {
    if(index < 0 || index > 2) {
      return 0.0f;
    }

    return components[index];
  }
};

// Three component vector
union Vector3 {
  float32 components[3];

  struct {
    float32 x, y, z;
  };

  struct {
    float32 u, v, w; 
  };
  
  struct {
    float32 r, g, b; 
  };
  
  // Default CTOR
  Vector3() {
    x = 0.0f; 
    y = 0.0f; 
    z = 0.0f;
  } 

  // Takes the `x`, `y`, and `z` components of the vector 
  Vector3(float32 x, float32 y, float32 z) 
    :x(x), y(y), z(z)
  {}

  // Takes a `Vector2` to fill the `x` and `y` components and also a `z` component
  Vector3(const Vector2& v, float32 z) 
    :x(v.x), y(v.y), z(z)
  {}
  
  // Fils all of the components with the given value 
  Vector3(float32 s)
    :x(s), y(s), z(s) 
  {}
  
  // Index operator overload into the components
  // NOTE: This indexing operator overload and all other overloads of this type 
  // will NOT check for out of bounds indices. Please keep this in mind
  float32 operator[](const uint32 index) {
    if(index < 0 || index > 3) {
      // @TODO: Should assert here
      return 0.0f;
    }

    return components[index];
  }
  
  float32 operator[](const uint32 index) const {
    if(index < 0 || index > 3) {
      // @TODO: Should assert here
      return 0.0f;
    }

    return components[index];
  }
};

// Four component vector 
union Vector4 {
  float32 components[4];

  struct {
    float32 x, y, z, w;
  };
  
  struct {
    float32 r, g, b, a; 
  };
  
  // Default CTOR
  Vector4() {
    x = 0.0f; 
    y = 0.0f; 
    z = 0.0f; 
    w = 0.0f;
  }

  // Takes the `x`, `y`, `z`, and `w` components of the vector 
  Vector4(float32 x, float32 y, float32 z, float32 w) 
    :x(x), y(y), z(z), w(w)
  {}
  
  // Takes a `Vector3` to fill the `x`, `y`, `w` components and a scalar to fill the `w` component
  Vector4(const Vector3& v, float32 w) 
    :x(v.x), y(v.y), z(v.z), w(w)
  {}
  
  // Fils all of the components with the given value 
  Vector4(float32 s)
    :x(s), y(s), z(s), w(s) 
  {}
  
  // Index operator overload into the components
  // NOTE: This indexing operator overload and all other overloads of this type 
  // will NOT check for out of bounds indices. Please keep this in mind
  float32 operator[](const uint32 index) {
    if(index < 0 || index > 4) {
      // @TODO: Should assert here
      return 0.0f;
    }

    return components[index];
  }
  
  float32 operator[](const uint32 index) const {
    if(index < 0 || index > 4) {
      // @TODO: Should assert here
      return 0.0f;
    }

    return components[index];
  }
};

// A 3x3 matrix 
struct Matrix3 {
  // Entries
  float32 data[9];

  // Default CTOR where the matrix gets initialized as an identity matrix
  Matrix3() {
    data[0] = 1; data[1] = 0; data[2] = 0;
    data[3] = 0; data[4] = 1; data[5] = 0;
    data[6] = 0; data[7] = 0; data[8] = 1;
  }

  // Takes in every single entry for the matrix
  Matrix3(float32 m0, float32 m1, float32 m2,
          float32 m3, float32 m4, float32 m5,
          float32 m6, float32 m7, float32 m8) {
    data[0] = m0; data[1] = m1; data[2] = m2;
    data[3] = m3; data[4] = m4; data[5] = m5;
    data[6] = m6; data[7] = m7; data[8] = m8;
  }

  // Takes in an array of floats and fills the entries of the matrix 
  // NOTE: The constructor does not check if the array of values given 
  // is valid or has the same size as the matrix. 
  Matrix3(float32* values) {
    for(uint8 i = 0; i < 9; i++) {
      data[i] = values[i];
    }
  }

  // Sets all entries of the matrix to this scalar
  Matrix3(float32 scalar) {
    data[0] = scalar; data[1] = scalar; data[2] = scalar;
    data[3] = scalar; data[4] = scalar; data[5] = scalar;
    data[6] = scalar; data[7] = scalar; data[8] = scalar;
  }

  // Fills the columns of the matrix with the given vectors 
  Matrix3(const Vector3& col1, const Vector3& col2, const Vector3& col3) {
    data[0] = col1.x; data[1] = col1.y; data[2] = col1.z;
    data[3] = col2.x; data[4] = col2.y; data[5] = col2.z;
    data[6] = col3.x; data[7] = col3.y; data[8] = col3.z;
  }
  
  // Index operator overload into the components
  // NOTE: This indexing operator overload and all other overloads of this type 
  // will NOT check for out of bounds indices. Please keep this in mind
  float32 operator[](const uint32 index) {
    if(index > 9 || index < 0) {
      return 0.0f;
    }

    return data[index];
  }

  float32 operator[](const uint32 index) const {
    if(index > 9 || index < 0) {
      return 0.0f;
    }

    return data[index];
  }
};

// A 4x4 matrix
union Matrix4 {
  // Entries
  float32 data[16]; 
  
  // Default CTOR where the matrix gets initialized as an identity matrix
  Matrix4() {
    data[0]  = 1; data[1]  = 0; data[2]  = 0; data[3]  = 0;
    data[4]  = 0; data[5]  = 1; data[6]  = 0; data[7]  = 0;
    data[8]  = 0; data[9]  = 0; data[10] = 1; data[11] = 0;
    data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 1;
  }

  // Takes in every single entry for the matrix
  Matrix4(float32 m0, float32 m1, float32 m2, float32 m3,
          float32 m4, float32 m5, float32 m6, float32 m7,
          float32 m8, float32 m9, float32 m10, float32 m11,
          float32 m12, float32 m13, float32 m14, float32 m15) {
    data[0]  = m0;  data[1]  = m1;  data[2]  = m2;  data[3]  = m3; 
    data[4]  = m4;  data[5]  = m5;  data[6]  = m6;  data[7]  = m7; 
    data[8]  = m8;  data[9]  = m9;  data[10] = m10; data[11] = m11; 
    data[12] = m12; data[13] = m13; data[14] = m14; data[15] = m15; 
  }

  // Takes in an array of floats and fills the entries of the matrix 
  // NOTE: The constructor does not check if the array of values given 
  // is valid or has the same size as the matrix. 
  Matrix4(float32* values) {
    for(uint8 i = 0; i < 16; i++) {
      data[i] = values[i];
    }
  }

  // Sets all entries to this scalar
  Matrix4(float32 scalar) {
    data[0]  = scalar; data[1]  = scalar; data[2]  = scalar; data[3]  = scalar;
    data[4]  = scalar; data[5]  = scalar; data[6]  = scalar; data[7]  = scalar;
    data[8]  = scalar; data[9]  = scalar; data[10] = scalar; data[11] = scalar;
    data[12] = scalar; data[13] = scalar; data[14] = scalar; data[15] = scalar;
  }
  
  // Fills the columns of the matrix with the given vectors 
  Matrix4(const Vector4& col1, const Vector4& col2, const Vector4& col3, const Vector4& col4) {
    data[0]  = col1.x; data[1]  = col1.y; data[2]  = col1.z; data[3]  = col1.w;
    data[4]  = col2.x; data[5]  = col2.y; data[6]  = col2.z; data[7]  = col2.w;
    data[8]  = col3.x; data[9]  = col3.y; data[10] = col3.z; data[11] = col3.w;
    data[12] = col4.x; data[13] = col4.y; data[14] = col4.z; data[15] = col4.w;
  }
  
  // Index operator overload into the components
  // NOTE: This indexing operator overload and all other overloads of this type 
  // will NOT check for out of bounds indices. Please keep this in mind
  float32 operator[](const uint32 index) {
    if(index > 16 || index < 0) {
      return 0.0f;
    }

    return data[index];
  }
  
  float32 operator[](const uint32 index) const {
    if(index > 16 || index < 0) {
      return 0.0f;
    }

    return data[index];
  }
};

// A quaternion 
union Quaternion {
  struct {
    float32 x, y, z, w;
  };

  // Default CTOR where the Quaternion gets initialized as: `(0.0f, 0.0f, 0.0f, 1.0f)`
  Quaternion() 
    :x(0.0f), y(0.0f), z(0.0f), w(1.0f) 
  {}

  // Taking in the 4 components of the Quaternion 
  Quaternion(const float32 x, const float32 y, const float32 z, const float32 w) 
    :x(x), y(y), z(z), w(w) 
  {}

  // Fill the `x`, `y`, and `z` components with the given vector `vec` and 
  // fill the `w` component with given scalar `w`
  Quaternion(const Vector3& vec, const float32 w) 
    :x(vec.x), y(vec.y), z(vec.z), w(w)
  {}

  // Takes a `Vector4` to fill the components of the Quaternion
  Quaternion(const Vector4& vec) 
    :x(vec.x), y(vec.y), z(vec.z), w(vec.w)
  {}
};

///////////////////////////////////////////////////////////////

// Vector2 operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE Vector2 operator+(const Vector2& v1, const Vector2& v2) {
  return Vector2(v1.x + v2.x, v1.y + v2.y);
}

SOC_INLINE Vector2 operator+(const Vector2& v, const float32& s) {
  return Vector2(v.x + s, v.y + s);
}

SOC_INLINE Vector2 operator-(const Vector2& v1, const Vector2& v2) {
  return Vector2(v1.x - v2.x, v1.y - v2.y);
}

SOC_INLINE Vector2 operator-(const Vector2& v, const float32& s) {
  return Vector2(v.x - s, v.y - s);
}

SOC_INLINE Vector2 operator*(const Vector2& v1, const Vector2& v2) {
  return Vector2(v1.x * v2.x, v1.y * v2.y);
}

SOC_INLINE Vector2 operator*(const Vector2& v, const float32& s) {
  return Vector2(v.x * s, v.y * s);
}

SOC_INLINE Vector2 operator/(const Vector2& v1, const Vector2& v2) {
  return Vector2(v1.x / v2.x, v1.y / v2.y);
}

SOC_INLINE Vector2 operator/(const Vector2& v, const float32& s) {
  return Vector2(v.x / s, v.y / s);
}

SOC_INLINE void operator+=(Vector2& v1, const Vector2& v2) {
  v1 = v1 + v2;
}

SOC_INLINE void operator+=(Vector2& v, const float32& s) {
  v = v + s;
}

SOC_INLINE void operator-=(Vector2& v1, const Vector2& v2) {
  v1 = v1 - v2;
}

SOC_INLINE void operator-=(Vector2& v, const float32& s) {
  v = v - s;
}

SOC_INLINE void operator*=(Vector2& v1, const Vector2& v2) {
  v1 = v1 * v2;
}

SOC_INLINE void operator*=(Vector2& v, const float32& s) {
  v = v * s;
}

SOC_INLINE void operator/=(Vector2& v1, const Vector2& v2) {
  v1 = v1 / v2;
}

SOC_INLINE void operator/=(Vector2& v, const float32& s) {
  v = v / s;
}

SOC_INLINE Vector2 operator-(const Vector2& v) {
  return Vector2(-v.x, -v.y);
}

///////////////////////////////////////////////////////////////

// Vector3 operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE Vector3 operator+(const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

SOC_INLINE Vector3 operator+(const Vector3& v, const float32 s) {
  return Vector3(v.x + s, v.y + s, v.z + s);
}

SOC_INLINE Vector3 operator-(const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

SOC_INLINE Vector3 operator-(const Vector3& v, const float32 s) {
  return Vector3(v.x - s, v.y - s, v.z - s);
}

SOC_INLINE Vector3 operator*(const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

SOC_INLINE Vector3 operator*(const Vector3& v, const float32 s) {
  return Vector3(v.x * s, v.y * s, v.z * s);
}

SOC_INLINE Vector3 operator*(const Vector3& v, const Matrix3& m) {
  return Vector3((v.x * m[0]) + (v.y * m[1]) + (v.z * m[2]), 
                 (v.x * m[3]) + (v.y * m[4]) + (v.z * m[5]), 
                 (v.x * m[6]) + (v.y * m[7]) + (v.z * m[8]));
}

SOC_INLINE Vector3 operator/(const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

SOC_INLINE Vector3 operator/(const Vector3& v, const float32 s) {
  return Vector3(v.x / s, v.y / s, v.z / s);
}

SOC_INLINE void operator+=(Vector3& v1, const Vector3& v2) {
  v1 = v1 + v2;
}

SOC_INLINE void operator+=(Vector3& v, const float32& s) {
  v = v + s;
}

SOC_INLINE void operator-=(Vector3& v1, const Vector3& v2) {
  v1 = v1 - v2;
}

SOC_INLINE void operator-=(Vector3& v, const float32& s) {
  v = v - s;
}

SOC_INLINE void operator*=(Vector3& v1, const Vector3& v2) {
  v1 = v1 * v2;
}

SOC_INLINE void operator*=(Vector3& v, const float32& s) {
  v = v * s;
}

SOC_INLINE void operator/=(Vector3& v1, const Vector3& v2) {
  v1 = v1 / v2;
}

SOC_INLINE void operator/=(Vector3& v, const float32& s) {
  v = v / s;
}

SOC_INLINE Vector3 operator-(const Vector3& v) {
  return Vector3(-v.x, -v.y, -v.z);
}

///////////////////////////////////////////////////////////////

// Vector4 operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE Vector4 operator+(const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

SOC_INLINE Vector4 operator+(const Vector4& v, const float32 s) {
  return Vector4(v.x + s, v.y + s, v.z + s, v.w + s);
}

SOC_INLINE Vector4 operator-(const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

SOC_INLINE Vector4 operator-(const Vector4& v, const float32 s) {
  return Vector4(v.x - s, v.y - s, v.z - s, v.w - s);
}

SOC_INLINE Vector4 operator*(const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

SOC_INLINE Vector4 operator*(const Vector4& v, const float32 s) {
  return Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
}

SOC_INLINE Vector4 operator*(const Vector4& v, const Matrix4& m) {
  return Vector4((v.x * m[0])  + (v.y * m[1])  + (v.z * m[2])  + (v.w * m[3]), 
                 (v.x * m[4])  + (v.y * m[5])  + (v.z * m[6])  + (v.w * m[7]), 
                 (v.x * m[8])  + (v.y * m[9])  + (v.z * m[10]) + (v.w * m[11]), 
                 (v.x * m[12]) + (v.y * m[13]) + (v.z * m[14]) + (v.w * m[15]));
}

SOC_INLINE Vector4 operator/(const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

SOC_INLINE Vector4 operator/(const Vector4& v, const float32 s) {
  return Vector4(v.x / s, v.y / s, v.z / s, v.w / s);
}

SOC_INLINE void operator+=(Vector4& v1, const Vector4& v2) {
  v1 = v1 + v2;
}

SOC_INLINE void operator+=(Vector4& v, const float32& s) {
  v = v + s;
}

SOC_INLINE void operator-=(Vector4& v1, const Vector4& v2) {
  v1 = v1 - v2;
}

SOC_INLINE void operator-=(Vector4& v, const float32& s) {
  v = v - s;
}

SOC_INLINE void operator*=(Vector4& v1, const Vector4& v2) {
  v1 = v1 * v2;
}

SOC_INLINE void operator*=(Vector4& v, const float32& s) {
  v = v * s;
}

SOC_INLINE void operator/=(Vector4& v1, const Vector4& v2) {
  v1 = v1 / v2;
}

SOC_INLINE void operator/=(Vector4& v, const float32& s) {
  v = v / s;
}

SOC_INLINE Vector4 operator-(const Vector4& v) {
  return Vector4(-v.x, -v.y, -v.z, -v.w);
}

///////////////////////////////////////////////////////////////

// Matrix3 operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE Matrix3 operator+(const Matrix3& m1, const Matrix3& m2) {
  Matrix3 result(0.0f);

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = m1[i] + m2[i];
  }

  return result;
}

SOC_INLINE Matrix3 operator+(const Matrix3& m, const float32 s) {
  Matrix3 result(0.0f);

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = m[i] + s;
  }

  return result;
}

SOC_INLINE Matrix3 operator-(const Matrix3& m1, const Matrix3& m2) {
  Matrix3 result(0.0f);

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = m1[i] - m2[i];
  }

  return result;
}

SOC_INLINE Matrix3 operator-(const Matrix3& m, const float32 s) {
  Matrix3 result(0.0f);

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = m[i] - s;
  }

  return result;
}

SOC_INLINE Matrix3 operator*(const Matrix3& m1, const Matrix3& m2) {
  return Matrix3(
                 (m1[0] * m2[0]) + (m1[1] * m2[3]) + (m1[2] * m2[6]), // m0 
                 (m1[0] * m2[1]) + (m1[1] * m2[4]) + (m1[2] * m2[7]), // m1 
                 (m1[0] * m2[2]) + (m1[1] * m2[5]) + (m1[2] * m2[8]), // m2 
                 
                 (m1[3] * m2[0]) + (m1[4] * m2[3]) + (m1[5] * m2[6]), // m3 
                 (m1[3] * m2[1]) + (m1[4] * m2[4]) + (m1[5] * m2[7]), // m4 
                 (m1[3] * m2[2]) + (m1[4] * m2[5]) + (m1[5] * m2[8]), // m5 
                 
                 (m1[6] * m2[0]) + (m1[7] * m2[3]) + (m1[8] * m2[6]), // m6 
                 (m1[6] * m2[1]) + (m1[7] * m2[4]) + (m1[8] * m2[7]), // m7 
                 (m1[6] * m2[2]) + (m1[7] * m2[5]) + (m1[8] * m2[8])  // m8 
                );
}

SOC_INLINE Matrix3 operator*(const Matrix3& m, const float32 s) {
  Matrix3 result(0.0f);

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = m[i] * s;
  }

  return result;
}

SOC_INLINE Matrix3 operator-(const Matrix3& m) {
  Matrix3 result(0.0f); 

  for(uint32 i = 0; i < 9; i++) {
    result.data[i] = -m[i];
  }

  return result;
}

SOC_INLINE void operator+=(Matrix3& m1, const Matrix3& m2) {
  m1 = m1 + m2;
}

SOC_INLINE void operator+=(Matrix3& m, const float32 s) {
  m = m + s;
}

SOC_INLINE void operator-=(Matrix3& m1, const Matrix3& m2) {
  m1 = m1 - m2;
}

SOC_INLINE void operator-=(Matrix3& m, const float32 s) {
  m = m - s;
}

SOC_INLINE void operator*=(Matrix3& m1, const Matrix3& m2) {
  m1 = m1 * m2;
}

SOC_INLINE void operator*=(Matrix3& m, const float32 s) {
  m = m * s;
}

//////////////////////////////////////////////////////////////

// Matrix4 operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE Matrix4 operator+(const Matrix4& m1, const Matrix4& m2) {
  Matrix4 result(0.0f);

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = m1[i] + m2[i];
  }

  return result;
}

SOC_INLINE Matrix4 operator+(const Matrix4& m, const float32 s) {
  Matrix4 result(0.0f);

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = m[i] + s;
  }

  return result;
}

SOC_INLINE Matrix4 operator-(const Matrix4& m1, const Matrix4& m2) {
  Matrix4 result(0.0f);

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = m1[i] - m2[i];
  }

  return result;
}

SOC_INLINE Matrix4 operator-(const Matrix4& m, const float32 s) {
  Matrix4 result(0.0f);

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = m[i] - s;
  }

  return result;
}

SOC_INLINE Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
  return Matrix4((m2[0]  * m1[0]) + (m2[1]  * m1[4]) + (m2[2]  * m1[8])  + (m2[3]  * m1[12]), // m0
                 (m2[0]  * m1[1]) + (m2[1]  * m1[5]) + (m2[2]  * m1[9])  + (m2[3]  * m1[13]), // m1
                 (m2[0]  * m1[2]) + (m2[1]  * m1[6]) + (m2[2]  * m1[10]) + (m2[3]  * m1[14]), // m2
                 (m2[0]  * m1[3]) + (m2[1]  * m1[7]) + (m2[2]  * m1[11]) + (m2[3]  * m1[15]), // m3
                 (m2[4]  * m1[0]) + (m2[5]  * m1[4]) + (m2[6]  * m1[8])  + (m2[7]  * m1[12]), // m4
                 (m2[4]  * m1[1]) + (m2[5]  * m1[5]) + (m2[6]  * m1[9])  + (m2[7]  * m1[13]), // m5
                 (m2[4]  * m1[2]) + (m2[5]  * m1[6]) + (m2[6]  * m1[10]) + (m2[7]  * m1[14]), // m6
                 (m2[4]  * m1[3]) + (m2[5]  * m1[7]) + (m2[6]  * m1[11]) + (m2[7]  * m1[15]), // m7
                 (m2[8]  * m1[0]) + (m2[9]  * m1[4]) + (m2[10] * m1[8])  + (m2[11] * m1[12]), // m8
                 (m2[8]  * m1[1]) + (m2[9]  * m1[5]) + (m2[10] * m1[9])  + (m2[11] * m1[13]), // m9
                 (m2[8]  * m1[2]) + (m2[9]  * m1[6]) + (m2[10] * m1[10]) + (m2[11] * m1[14]), // m10
                 (m2[8]  * m1[3]) + (m2[9]  * m1[7]) + (m2[10] * m1[11]) + (m2[11] * m1[15]), // m11
                 (m2[12] * m1[0]) + (m2[13] * m1[4]) + (m2[14] * m1[8])  + (m2[15] * m1[12]), // m12
                 (m2[12] * m1[1]) + (m2[13] * m1[5]) + (m2[14] * m1[9])  + (m2[15] * m1[13]), // m13
                 (m2[12] * m1[2]) + (m2[13] * m1[6]) + (m2[14] * m1[10]) + (m2[15] * m1[14]), // m14
                 (m2[12] * m1[3]) + (m2[13] * m1[7]) + (m2[14] * m1[11]) + (m2[15] * m1[15])  // m15
                 );
}

SOC_INLINE Matrix4 operator*(const Matrix4& m, const float32 s) {
  Matrix4 result(0.0f);

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = m[i] * s;
  }

  return result;
}

SOC_INLINE Matrix4 operator-(const Matrix4& m) {
  Matrix4 result(0.0f); 

  for(uint32 i = 0; i < 16; i++) {
    result.data[i] = -m[i];
  }

  return result;
}

SOC_INLINE void operator+=(Matrix4& m1, const Matrix4& m2) {
  m1 = m1 + m2;
}

SOC_INLINE void operator+=(Matrix4& m, const float32 s) {
  m = m + s;
}

SOC_INLINE void operator-=(Matrix4& m1, const Matrix4& m2) {
  m1 = m1 - m2;
}

SOC_INLINE void operator-=(Matrix4& m, const float32 s) {
  m = m - s;
}

SOC_INLINE void operator*=(Matrix4& m1, const Matrix4& m2) {
  m1 = m1 * m2;
}

SOC_INLINE void operator*=(Matrix4& m, const float32 s) {
  m = m * s;
}

///////////////////////////////////////////////////////////////

// Quaternion operator overloading
///////////////////////////////////////////////////////////////

SOC_INLINE const Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
  return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

SOC_INLINE const void operator+=(Quaternion& q1, const Quaternion& q2) {
  q1 = q1 + q2; 
}

SOC_INLINE const Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
  return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

SOC_INLINE const void operator-=(Quaternion& q1, const Quaternion& q2) {
  q1 = q1 - q2; 
}

SOC_INLINE const Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
  return Quaternion(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y, 
                    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x, 
                    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w, 
                    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

SOC_INLINE const void operator*=(Quaternion& q1, const Quaternion& q2) {
  q1 = q1 * q2; 
}

SOC_INLINE const Quaternion operator*(const Quaternion& q, const float32 s) {
  return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
}

SOC_INLINE const void operator*=(Quaternion& q, const float32 s) {
  q = q * s; 
}

///////////////////////////////////////////////////////////////

// Misc. functions
///////////////////////////////////////////////////////////////

// Returns a float scalar between `min` and `max`
SOC_INLINE float32 float_clamp(const float32 value, const float32 min, const float32 max) {
  if(value > max) {
    return max;
  }
  else if(value < min) {
    return value;
  }

  return value;
}

// Returns a int scalar between `min` and `max`
SOC_INLINE int32 int_clamp(const int32 value, const int32 min, const int32 max) {
  if(value > max) {
    return max;
  }
  else if(value < min) {
    return value;
  }

  return value;
}

// Sin of `x`
SOC_INLINE float64 sin(const float64 x) {
  return std::sin(x);
}

// Cos of `x`
SOC_INLINE float64 cos(const float64 x) {
  return std::cos(x);
}

// Tan of `x`
SOC_INLINE float64 tan(const float64 x) {
  return std::tan(x);
}

// Atan of `x`
SOC_INLINE float64 atan(const float64 x) {
  return std::atan(x);
}

// Atan2 of `y` and `x`
SOC_INLINE float64 atan2(const float64 y, const float64 x) {
  return std::atan2(y, x);
}

// Floor of `x`
SOC_INLINE float64 floor(const float64 x) {
  return std::floor(x);
}

// Square root of `x` 
SOC_INLINE float64 sqrt(const float64 x) {
  return std::sqrt(x);
}

// Returns the smallest number between `x` and `y` 
SOC_INLINE float64 min(const float64 x, const float64 y) {
  return std::min(x, y);
}

// Returns the biggest number between `x` and `y` 
SOC_INLINE float64 max(const float64 x, const float64 y) {
  return std::max(x, y);
}

// Returns the linear interpolation with the given `start`, `end`, and `amount` 
SOC_INLINE float32 lerp(const float32 start, const float32 end, const float32 amount) {
  return start + amount * (end - start);
}

// Remaps the given `value` from the old range to a new range 
SOC_INLINE float32 remap(const float32 value, const float32 old_min, const float32 old_max, const float32 new_min, const float32 new_max) {
  return (value - old_min) / (old_max - old_min) * (new_max - new_min) + new_max;
}

///////////////////////////////////////////////////////////////

// Vector2 functions
///////////////////////////////////////////////////////////////

// Returns the dot product of `v1` and `v2` 
SOC_INLINE const float32 vec2_dot(const Vector2& v1, const Vector2& v2) {
  return (v1.x * v2.x) + (v1.y * v2.y);
}

// Returns the length/magnitude of `v`
SOC_INLINE const float32 vec2_length(const Vector2& v) {
  return sqrt((v.x * v.x) + (v.y * v.y));
}

// Returns the normalized version of the vector `v`
SOC_INLINE const Vector2 vec2_normalize(const Vector2& v) {
  return v / vec2_length(v);
}

// Returns a vector between `min` and `max`
SOC_INLINE const Vector2 vec2_clamp(const Vector2& value, const Vector2& min, const Vector2& max) {
  if((value.x > max.x) && (value.y > max.y)) {
    return max;
  }
  else if((value.x < min.x) && (value.y < min.y)) {
    return min;
  }
  else {
    return value;
  }
}

// Returns the distance between `v1` and `v2` 
SOC_INLINE const float32 vec2_distance(const Vector2& v1, const Vector2& v2) {
  soc::Vector2 diff = v2 - v1;
  return vec2_length(diff);
}

// Returns the linear interpolation between `start` and `end` by `amount` 
SOC_INLINE const Vector2 vec2_lerp(const Vector2& start, const Vector2& end, const float32 amount) {
  return Vector2(lerp(start.x, end.x, amount), lerp(start.y, end.y, amount));
}

// Retruns the smallest vector between `v1` and `v2` 
SOC_INLINE const Vector2 vec2_min(const Vector2& v1, const Vector2& v2) {
  float32 v1_sum = v1.x + v1.y;
  float32 v2_sum = v2.x + v2.y;

  // `v2` is the smallest vector
  if(v1_sum > v2_sum) {
    return v2;
  }

  return v1;
}

// Retruns the biggest vector between `v1` and `v2` 
SOC_INLINE const Vector2 vec2_max(const Vector2& v1, const Vector2& v2) {
  float32 v1_sum = v1.x + v1.y;
  float32 v2_sum = v2.x + v2.y;

  // `v1` is the biggest vector
  if(v1_sum > v2_sum) {
    return v1;
  }

  return v2;
}

// Returns the angle in RADIANS between `point1` and `point2` 
SOC_INLINE const float32 vec2_angle(const Vector2& point1, const Vector2& point2) {
  soc::Vector2 diff = point2 - point1;
  return atan2(diff.y, diff.x);
}

///////////////////////////////////////////////////////////////

// Vector3 functions 
///////////////////////////////////////////////////////////////

// Returns the dot product of `v1` and `v2` 
SOC_INLINE const float32 vec3_dot(const Vector3& v1, const Vector3& v2) {
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

// Returns the length/magnitude of `v`
SOC_INLINE const float32 vec3_length(const Vector3& v) {
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// Returns the normalized version of the vector `v`
SOC_INLINE const Vector3 vec3_normalize(const Vector3& v) {
  return v / vec3_length(v);
}

// Returns the cross product of `v1` and `v2`
SOC_INLINE const Vector3 vec3_cross(const Vector3& v1, const Vector3& v2) {
  return Vector3((v1.y * v2.z) - (v1.z * v2.y), 
                 (v1.z * v2.x) - (v1.x * v2.z), 
                 (v1.x * v2.y) - (v1.y * v2.x));
}

// Returns a vector between `min` and `max`
SOC_INLINE const Vector3 vec3_clamp(const Vector3& value, const Vector3& min, const Vector3& max) {
  if((value.x > max.x) && (value.y > max.y) && (value.z > max.z)) {
    return max;
  }
  else if((value.x < min.x) && (value.y < min.y) && (value.z < min.z)) {
    return min;
  }
  else {
    return value;
  }
}

// Returns the distance between `v1` and `v2` 
SOC_INLINE const float32 vec3_distance(const Vector3& v1, const Vector3& v2) {
  soc::Vector3 diff = v2 - v1;
  return vec3_length(diff);
}

// Returns the linear interpolation between `start` and `end` by `amount` 
SOC_INLINE const Vector3 vec3_lerp(const Vector3& start, const Vector3& end, const float32 amount) {
  return Vector3(lerp(start.x, end.x, amount), lerp(start.y, end.y, amount), lerp(start.z, end.z, amount));
}

// Retruns the smallest vector between `v1` and `v2` 
SOC_INLINE const Vector3 vec3_min(const Vector3& v1, const Vector3& v2) {
  float32 v1_sum = v1.x + v1.y + v1.z;
  float32 v2_sum = v2.x + v2.y + v2.z;

  // `v2` is the smallest vector
  if(v1_sum > v2_sum) {
    return v2;
  }

  return v1;
}

// Retruns the biggest vector between `v1` and `v2` 
SOC_INLINE const Vector3 vec3_max(const Vector3& v1, const Vector3& v2) {
  float32 v1_sum = v1.x + v1.y + v1.z;
  float32 v2_sum = v2.x + v2.y + v2.z;

  // `v1` is the biggest vector
  if(v1_sum > v2_sum) {
    return v1;
  }

  return v2;
}

// Returns the angle in RADIANS between `point1` and `point2` 
SOC_INLINE const float32 vec3_angle(const Vector3& point1, const Vector3& point2) {
  soc::Vector3 diff = point2 - point1;
  return atan2(diff.y, diff.x);
}

///////////////////////////////////////////////////////////////

// Vector4 functions
///////////////////////////////////////////////////////////////

// Returns the dot product of `v1` and `v2` 
SOC_INLINE const float32 vec4_dot(const Vector4& v1, const Vector4& v2) {
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

// Returns the length/magnitude of `v`
SOC_INLINE const float32 vec4_length(const Vector4& v) {
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

// Returns the normalized version of the vector `v`
SOC_INLINE const Vector4 vec4_normalize(const Vector4& v) {
  return v / vec4_length(v);
}

// Returns a vector between `min` and `max`
SOC_INLINE const Vector4 vec4_clamp(const Vector4& value, const Vector4& min, const Vector4& max) {
  if((value.x > max.x) && (value.y > max.y) && (value.z > max.z) && (value.w > max.w)) {
    return max;
  }
  else if((value.x < min.x) && (value.y < min.y) && (value.z < min.z) && (value.w < min.w)) {
    return min;
  }
  else {
    return value;
  }
}

// Returns the linear interpolation between `start` and `end` by `amount` 
SOC_INLINE const Vector4 vec4_lerp(const Vector4& start, const Vector4& end, const float32 amount) {
  return Vector4(lerp(start.x, end.x, amount), lerp(start.y, end.y, amount), lerp(start.z, end.z, amount), lerp(start.w, end.w, amount));
}

// Retruns the smallest vector between `v1` and `v2` 
SOC_INLINE const Vector4 vec3_min(const Vector4& v1, const Vector4& v2) {
  float32 v1_sum = v1.x + v1.y + v1.z + v1.w;
  float32 v2_sum = v2.x + v2.y + v2.z + v2.w;

  // `v2` is the smallest vector
  if(v1_sum > v2_sum) {
    return v2;
  }

  return v1;
}

// Retruns the biggest vector between `v1` and `v2` 
SOC_INLINE const Vector4 vec4_max(const Vector4& v1, const Vector4& v2) {
  float32 v1_sum = v1.x + v1.y + v1.z + v1.w;
  float32 v2_sum = v2.x + v2.y + v2.z + v2.w;

  // `v1` is the biggest vector
  if(v1_sum > v2_sum) {
    return v1;
  }

  return v2;
}

///////////////////////////////////////////////////////////////

// Matrix3 functions
///////////////////////////////////////////////////////////////

// Returns the determinant of the matrix `m`
SOC_INLINE const float32 mat3_det(const Matrix3& m) {
  return (m[0] * m[4] * m[8]) + (m[1] * m[5] * m[6]) + (m[2] * m[3] * m[7]) -
         (m[0] * m[5] * m[7]) - (m[1] * m[3] * m[8]) - (m[2] * m[4] * m[6]);
}

// Returns the transposed (rows and columns switched) version of the matrix `m`
SOC_INLINE const Matrix3 mat3_transpose(const Matrix3& m) {
  return Matrix3(m[0], m[3], m[6], 
                 m[1], m[4], m[7], 
                 m[2], m[5], m[8]);
}

// Returns the inverse matrix of the given `m` matrix
SOC_INLINE const Matrix3 mat3_inverse(const Matrix3& m) {
  Vector3 v1(m[0], m[3], m[6]); 
  Vector3 v2(m[1], m[4], m[7]); 
  Vector3 v3(m[2], m[5], m[8]); 

  Vector3 r0 = vec3_cross(v2, v3);
  Vector3 r1 = vec3_cross(v3, v1);
  Vector3 r2 = vec3_cross(v1, v2);

  float32 inv_det = 1.0f / vec3_dot(r2, v3);

  return Matrix3(r0.x * inv_det, r1.x * inv_det, r2.x * inv_det,
                 r0.y * inv_det, r1.y * inv_det, r2.y * inv_det,
                 r0.z * inv_det, r1.z * inv_det, r2.z * inv_det);
}

// Using the given `angle`, returns the rotation matrix on the X-axis
SOC_INLINE const Matrix3 mat3_rotate_x(const float32 angle) {
  float32 c = cos(angle);
  float32 s = sin(angle);

  return Matrix3(1.0f, 0.0f, 0.0f, 
                 0.0f, c,    -s, 
                 0.0f, s,    c);
}

// Using the given `angle`, returns the rotation matrix on the Y-axis
SOC_INLINE const Matrix3 mat3_rotate_y(const float32 angle) {
  float32 c = cos(angle);
  float32 s = sin(angle);

  return Matrix3(c,    0.0f, s, 
                 0.0f, 1.0f, 0.0f, 
                 -s,   0.0f, c);
}

// Using the given `angle`, returns the rotation matrix on the Y-axis
SOC_INLINE const Matrix3 mat3_rotate_z(const float32 angle) {
  float32 c = cos(angle);
  float32 s = sin(angle);

  return Matrix3(c,   -s,    0.0f, 
                 s,    c,    0.0f, 
                 0.0f, 0.0f, 1.0f);
}

// Returns rotation matrix around the given `axis` by `angle` RADIANS
// NOTE: The given `axis` vector gets normalized inside the function so there's no 
// need to normalize it yourself.
SOC_INLINE const Matrix3 mat3_rotate(const Vector3& axis, const float32 angle) {
  float32 c = cos(angle);
  float32 s = sin(angle);
  float32 d = (1.0f - c);

  // Need to normalize the axis 
  Vector3 norm_axis = vec3_normalize(axis);

  float32 cx = norm_axis.x * d; 
  float32 cy = norm_axis.y * d; 
  float32 cz = norm_axis.z * d;

  return Matrix3(c    + cx * norm_axis.x, // 0
                 cx * norm_axis.y + s  * norm_axis.z, // 1
                 cx * norm_axis.z - s  * norm_axis.y, // 2

                 cy * norm_axis.x - s  * norm_axis.z, // 3
                 c    + cy * norm_axis.y, // 4
                 cy * norm_axis.z + s  * norm_axis.x, // 5

                 cz * norm_axis.x + s  * norm_axis.y, // 6
                 cz * norm_axis.y - s  * norm_axis.x, // 7
                 c    + cz * norm_axis.z);// 8
}

// Returns the scale matrix on the given `axis` 
SOC_INLINE const Matrix3 mat3_scale(const Vector3& scale) {
  Vector3 s = (scale - 1.0f);
  
  float32 sx = s.x * scale.x;
  float32 sy = s.y * scale.y;
  float32 sz = s.z * scale.z;

  float32 axay = sx * scale.y;
  float32 axaz = sx * scale.z; 
  float32 ayaz = sy * scale.z;

  return Matrix3(sx * scale.x + 1.0f, axay, axaz, 
                 axay, sy * scale.y + 1.0f, ayaz, 
                 axaz, ayaz, sz * scale.z + 1.0f);
}

// Returns the reflection matrix from the given `point`
SOC_INLINE const Matrix3 mat3_reflect(const Vector3& point) {
  float32 x = -2.0f * point.x; 
  float32 y = -2.0f * point.y; 
  float32 z = -2.0f * point.z; 

  float32 axay = x * point.y;
  float32 axaz = x * point.z; 
  float32 ayaz = y * point.z;

  return Matrix3(x * point.x + 1.0f, axay, axaz, 
                 axay, y * point.y + 1.0f, ayaz, 
                 axaz, ayaz, z * point.z + 1.0f);
}

// Returns the skew matrix on the given `axis` towards the given `direction` by `angle` in RADIANS
SOC_INLINE const Matrix3 mat3_skew(const Vector3& axis, const Vector3& direction, const float32 angle) {
  float t = tan(angle);

  float32 x = axis.x * t; 
  float32 y = axis.y * t; 
  float32 z = axis.z * t; 

  return Matrix3(x * direction.x + 1.0f, x * direction.y, x * direction.z, 
                 y * direction.x, y * direction.y + 1.0f, y * direction.z, 
                 z * direction.x, z * direction.y, z * direction.z + 1.0f);
}

// Converts the given `m` Matrix3 into a Matrix4
// NOTE: The last row of the final matrix is: 0.0f, 0.0f, 0.0f, 1.0f
SOC_INLINE const Matrix4 mat3_to_mat4(const Matrix3& m) {
  return Matrix4(m[0], m[1], m[2], 0.0f, 
                 m[3], m[4], m[5], 0.0f, 
                 m[6], m[7], m[8], 0.0f, 
                 0.0f, 0.0f, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////

// Matrix4 functions 
///////////////////////////////////////////////////////////////

// Returns the determinant of the given `m` matrix
SOC_INLINE const float32 mat4_det(const Matrix4& m) {
  return (m[3] * m[6] * m[9]  * m[12]) - (m[2] * m[7] * m[9]  * m[12]) - 
         (m[3] * m[5] * m[10] * m[12]) + (m[1] * m[7] * m[10] * m[12]) + 
         (m[2] * m[5] * m[11] * m[12]) - (m[1] * m[6] * m[11] * m[12]) - 
         (m[3] * m[6] * m[8]  * m[13]) + (m[2] * m[7] * m[8]  * m[13]) + 
         (m[3] * m[4] * m[10] * m[13]) - (m[0] * m[7] * m[10] * m[13]) - 
         (m[2] * m[4] * m[11] * m[13]) + (m[0] * m[6] * m[11] * m[13]) + 
         (m[3] * m[5] * m[8]  * m[14]) - (m[1] * m[7] * m[8]  * m[14]) - 
         (m[3] * m[4] * m[9]  * m[14]) + (m[0] * m[7] * m[9]  * m[14]) + 
         (m[1] * m[4] * m[11] * m[14]) - (m[0] * m[5] * m[11] * m[14]) - 
         (m[2] * m[5] * m[8]  * m[15]) + (m[1] * m[6] * m[8]  * m[15]) + 
         (m[2] * m[4] * m[9]  * m[15]) - (m[0] * m[6] * m[9]  * m[15]) - 
         (m[1] * m[4] * m[10] * m[15]) + (m[0] * m[5] * m[10] * m[15]); 
}

// Returns the transposed (rows and columns switched) version of the matrix `m`
SOC_INLINE const Matrix4 mat4_transpose(const Matrix4& m) {
  return Matrix4(m[0], m[4], m[8],  m[12], 
                 m[1], m[5], m[9],  m[13], 
                 m[2], m[6], m[10], m[14], 
                 m[3], m[7], m[11], m[15]);
}

// Returns the inverse matrix of the given `m` matrix
SOC_INLINE const Matrix4 mat4_inverse(const Matrix4& m) {
  Vector3 a(m[0], m[4], m[8]);
  Vector3 b(m[1], m[5], m[9]);
  Vector3 c(m[2], m[6], m[10]);
  Vector3 d(m[3], m[7], m[11]);

  float32 x = m[12];
  float32 y = m[13];
  float32 z = m[14];
  float32 w = m[15];

  Vector3 s = vec3_cross(a, b);
  Vector3 t = vec3_cross(c, d);
  Vector3 u = a * y - b * x; 
  Vector3 v = c * w - d * z; 

  float32 inv_det = 1.0f / (vec3_dot(s, v) + vec3_dot(t, u));
  s *= inv_det;
  t *= inv_det;
  u *= inv_det;
  v *= inv_det;

  Vector3 r0 = vec3_cross(b, v) + t * y;
  Vector3 r1 = vec3_cross(v, a) - t * x;
  Vector3 r2 = vec3_cross(d, u) + s * w;
  Vector3 r3 = vec3_cross(u, c) - s * z;

  return Matrix4(r0.x, r1.x, r2.x, r3.x, 
                 r0.y, r1.y, r2.y, r3.y,  
                 r0.z, r1.z, r2.z, r3.z, 
                -vec3_dot(b, t), vec3_dot(a, t), -vec3_dot(d, s), vec3_dot(c, s));
}

// Returns the translation matrix using the given `position`
// NOTE: The function uses an identity matrix (the diagonal entries of the matrix are all 1) 
// to calculate the transformation.
SOC_INLINE const Matrix4 mat4_translate(const Vector3& position) {
  Matrix4 iden_mat;
  
  float32 cx1 = iden_mat[0] * position.x;
  float32 cy1 = iden_mat[1] * position.x;
  float32 cz1 = iden_mat[2] * position.x;
  
  float32 cx2 = iden_mat[4] * position.y;
  float32 cy2 = iden_mat[5] * position.y;
  float32 cz2 = iden_mat[6] * position.y;
  
  float32 cx3 = iden_mat[8]  * position.z;
  float32 cy3 = iden_mat[9]  * position.z;
  float32 cz3 = iden_mat[10] * position.z;

  iden_mat.data[12] = cx1 + cx2 + cx3;
  iden_mat.data[13] = cy1 + cy2 + cy3;
  iden_mat.data[14] = cz1 + cz2 + cz3;

  return iden_mat;
}

// Returns the rotation matrix around the given `axis` by `angle` in RADIANS
// NOTE: The function uses an identity matrix (the diagonal entries of the matrix are all 1) 
// to calculate the transformation.
SOC_INLINE const Matrix4 mat4_rotate(const Vector3& axis, const float32 angle) {
  Matrix3 mat = mat3_rotate(axis, angle); 
  return Matrix4(mat[0], mat[1], mat[2], 0.0f, 
                 mat[3], mat[4], mat[5], 0.0f, 
                 mat[6], mat[7], mat[8], 0.0f, 
                 0.0f,   0.0f,   0.0f,   1.0f);
}

// Returns the scale matrix on the given `scale`
// NOTE: The function uses an identity matrix (the diagonal entries of the matrix are all 1) 
// to calculate the transformation.
SOC_INLINE const Matrix4 mat4_scale(const Vector3& scale) {
  Matrix4 iden_mat;

  iden_mat.data[0] *= scale.x;
  iden_mat.data[1] *= scale.x;
  iden_mat.data[2] *= scale.x;

  iden_mat.data[4] *= scale.y;
  iden_mat.data[5] *= scale.y;
  iden_mat.data[6] *= scale.y;
  
  iden_mat.data[8]  *= scale.z;
  iden_mat.data[9]  *= scale.z;
  iden_mat.data[10] *= scale.z;

  return iden_mat;
}

// Converts the given 'm' Matrix4 to a Matrix3
SOC_INLINE const Matrix3 mat4_to_mat3(const Matrix4& m) {
  return Matrix3(m[0], m[1], m[2], 
                 m[4], m[5], m[6], 
                 m[8], m[9], m[10]);
}

// Generates a 4x4 matrix for a symmetric perspective view frustrum, using the left hand coordinate system 
// NOTE: The fov is in radians
SOC_INLINE const Matrix4 mat4_perspective(const float32 fov, const float32 aspect_ratio, const float32 near, const float32 far) {
  float32 tangent = tan(fov / 2.0f); 

  return Matrix4(1 / (aspect_ratio * tangent), 0.0f, 0.0f, 0.0f, 
                 0.0f, 1 / tangent, 0.0f, 0.0f, 
                 0.0f, 0.0f, -(far + near) / (far - near), -1.0f, 
                 0.0f, 0.0f, -(2 * far * near) / (far - near), 0.0f);
};

// Generates a 4x4 matrix for a orthographic view frustrum, using the left hand coordinate system
SOC_INLINE const Matrix4 mat4_orthographic(const float32 left, const float32 right, const float32 bottom, const float32 top) {
  Matrix4 mat;
  mat.data[0] = 2.0f / (right - left);
  mat.data[5] = 2.0f / (top - bottom);
  mat.data[10] = -1.0f; 
  mat.data[12] = -(right + left) / (right - left);
  mat.data[13] = -(top + bottom) / (top - bottom);

  return mat;
}

// Generates a 4x4 look at view matrix, using ther left handed coordinate system 
SOC_INLINE const Matrix4 mat4_look_at(const Vector3& eye, const Vector3& target, const Vector3& up) {
  Matrix4 mat;

  Vector3 forward = vec3_normalize(eye - target);
  Vector3 left = vec3_normalize(vec3_cross(up, forward)); 
  Vector3 up_dir = vec3_cross(forward, left);

  mat.data[0] = left.x;    mat.data[4] = left.y;    mat.data[8] = left.z; 
  mat.data[1] = up_dir.x;  mat.data[5] = up_dir.y;  mat.data[9] = up_dir.z; 
  mat.data[2] = forward.x; mat.data[6] = forward.y; mat.data[10] = forward.z;

  mat.data[12] = -vec3_dot(left, eye);
  mat.data[13] = -vec3_dot(up_dir, eye);
  mat.data[14] = -vec3_dot(forward, eye);

  return mat;
}

///////////////////////////////////////////////////////////////

// Quaternion functions
///////////////////////////////////////////////////////////////

// Returns the rotation matrix calculated from the quaternion `q`
SOC_INLINE const Matrix3 quat_get_mat3(const Quaternion& q) {
  float32 x2 = q.x * q.x; 
  float32 y2 = q.y * q.y; 
  float32 z2 = q.z * q.z;

  float32 xy = q.x * q.y; 
  float32 xz = q.x * q.z;
  float32 yz = q.y * q.z;
  float32 wx = q.w * q.x;
  float32 wy = q.w * q.y;
  float32 wz = q.w * q.z;

  return Matrix3(1.0f - 2.0f * (y2 + z2), 2.0f * (xy + wz), 2.0f * (xz - wy), 
                 2.0f * (xy - wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz + wx), 
                 2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (x2 + y2));
}

// The same as `quat_get_mat3` but returns a Matrix4 instead
SOC_INLINE const Matrix4 quat_get_mat4(const Quaternion& q) {
  return soc::mat3_to_mat4(quat_get_mat3(q));
}

// Sets and returns the rotation of a quaternion using the given matrix `m`
SOC_INLINE const Quaternion quat_set_mat3(const Matrix3& m) {
  Quaternion q; 

  float32 m00 = m[0];
  float32 m11 = m[4];
  float32 m22 = m[8];
  float32 sum = m00 + m11 + m22;

  if(sum > 0) {
    q.w = sqrt(sum + 1.0f) * 0.5f; 
    float32 f = 0.25f / q.w;

    q.x = (m[5] - m[7]) * f;
    q.y = (m[6] - m[2]) * f;
    q.z = (m[1] - m[3]) * f;
  }
  else if((m00 > m11) && (m00 > m22)) {
    q.x = sqrt(m00 - m11 - m22 + 1.0f) * 0.5f; 
    float f = 0.25f / q.x;

    q.y = (m[1] + m[3]) * f;
    q.z = (m[6] + m[2]) * f;
    q.w = (m[5] - m[7]) * f;
  }
  else if(m11 > m22) {
    q.y = sqrt(m11 - m00 - m22 + 1.0f) * 0.5f;
    float f = 0.25f / q.y;
    
    q.x = (m[1] + m[3]) * f;
    q.z = (m[5] + m[7]) * f;
    q.w = (m[6] - m[2]) * f;
  }
  else {
    q.z = sqrt(m22 - m00 - m11 + 1.0f) * 0.5f;
    float f = 0.25f / q.z;
    
    q.x = (m[6] + m[2]) * f;
    q.y = (m[5] + m[7]) * f;
    q.w = (m[1] - m[3]) * f;
  }

  return q;
}

// Same as `quat_set_mat3` but uses a Matrix4 instead
SOC_INLINE const Quaternion quat_set_mat4(const Matrix4& m) {
  return quat_set_mat3(mat4_to_mat3(m));
}

// Rotates and returns a quaternion using the given `axis` by `angle` in radians
SOC_INLINE const Quaternion quat_axis_angle(const Vector3& axis, const float32 angle) {
  float32 s = sin(angle * 0.5f);
  return Quaternion(axis * s, cos(angle * 0.5f));
}

///////////////////////////////////////////////////////////////

} // End of soc
