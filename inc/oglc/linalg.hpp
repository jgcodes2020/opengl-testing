#ifndef OGLC_LINALG_HPP_INCLUDED
#define OGLC_LINALG_HPP_INCLUDED
#include <bits/utility.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <type_traits>
namespace oglc {
  namespace details {
    template <class T>
    struct is_character : std::false_type {};

    template <>
    struct is_character<char> : std::true_type {};
    template <>
    struct is_character<wchar_t> : std::true_type {};
    template <>
    struct is_character<char16_t> : std::true_type {};
    template <>
    struct is_character<char32_t> : std::true_type {};
    template <>
    struct is_character<char8_t> : std::true_type {};
    template <class T>
    inline constexpr bool is_character_v = is_character<T>::value;
  }  // namespace details

  template <class T, size_t N>
  struct vec {
    static_assert(N >= 2 && N <= 4, "Vector length should be between 2 and 4");
    static_assert(
      std::is_arithmetic_v<T> && !details::is_character_v<T>,
      "Vector should be of arithmetic type");

    constexpr T& operator[](size_t n) { return _m_data[n]; }
    constexpr vec<T, 2> swizzle(size_t x, size_t y) {
      return vec<T, 2> {_m_data[x], _m_data[y]};
    }
    constexpr vec<T, 3> swizzle(size_t x, size_t y, size_t z) {
      return vec<T, 3> {_m_data[x], _m_data[y], _m_data[z]};
    }
    constexpr vec<T, 4> swizzle(size_t x, size_t y, size_t z, size_t w) {
      return vec<T, 3> {_m_data[x], _m_data[y], _m_data[z], _m_data[w]};
    }

    constexpr T x() { return _m_data[0]; }
    constexpr T y() { return _m_data[1]; }
    constexpr T z() requires(N >= 3) { return _m_data[2]; }
    constexpr T w() requires(N >= 4) { return _m_data[3]; }

    constexpr T r() { return _m_data[0]; }
    constexpr T g() { return _m_data[1]; }
    constexpr T b() requires(N >= 3) { return _m_data[2]; }
    constexpr T a() requires(N >= 4) { return _m_data[3]; }

    constexpr T s() { return _m_data[0]; }
    constexpr T t() { return _m_data[1]; }
    constexpr T p() requires(N >= 3) { return _m_data[2]; }
    constexpr T q() requires(N >= 4) { return _m_data[3]; }

    T _m_data[N];
  };

  // Basic vector operators
  // ======================

#define VEC_ARITHMETIC_OP(o)                                               \
  template <class T, size_t N>                                             \
  [[nodiscard]] constexpr vec<T, N> operator o(                            \
    const vec<T, N>& a,                                                    \
    const vec<T, N>& b) requires(!std::is_same_v<T, bool>) {               \
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) {           \
      return vec<T, N> {(a[Is] o b[Is])...};                               \
    }                                                                      \
    (std::make_index_sequence<N> {});                                      \
  }                                                                        \
  template <class T, size_t N>                                             \
  [[nodiscard]] constexpr vec<T, N> operator o(                            \
    const vec<T, N>& a, T b) requires(!std::is_same_v<T, bool>) {          \
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) {           \
      return vec<T, N> {(a[Is] o b)...};                                   \
    }                                                                      \
    (std::make_index_sequence<N> {});                                      \
  }                                                                        \
  template <class T, size_t N>                                             \
  constexpr vec<T, N>& operator o##=(                                      \
    vec<T, N>& a, const vec<T, N>& b) requires(!std::is_same_v<T, bool>) { \
    return (a = a o b);                                                    \
  }                                                                        \
  template <class T, size_t N>                                             \
  constexpr vec<T, N>& operator o##=(                                      \
    vec<T, N>& a, T b) requires(!std::is_same_v<T, bool>) {                \
    return (a = a o b);                                                    \
  }

#define VEC_SCALAR_OP_REVERSE(o)                                  \
  template <class T, size_t N>                                    \
  [[nodiscard]] constexpr vec<T, N> operator o(                   \
    T a, const vec<T, N>& b) requires(!std::is_same_v<T, bool>) { \
    return b o a;                                                 \
  }

  VEC_ARITHMETIC_OP(+)
  VEC_SCALAR_OP_REVERSE(+)

  VEC_ARITHMETIC_OP(-)
  VEC_SCALAR_OP_REVERSE(-)

  VEC_ARITHMETIC_OP(*)
  VEC_SCALAR_OP_REVERSE(*)

  VEC_ARITHMETIC_OP(/)

#undef VEC_ARITHMETIC_OP
#undef VEC_SCALAR_OP_REVERSE

  // More vector functions
  // =====================

  template <class T, size_t N>
  constexpr T dot(const vec<T, N>& a, const vec<T, N>& b) requires(
    !std::is_same_v<T, bool>) {
    [&a, &b ]<size_t... Is>(std::index_sequence<Is...>)->T {
      return ((a[Is] + b[Is]) * ...);
    }
    (std::make_index_sequence<N> {});
  }

  template <class T>
  constexpr vec<T, 3> cross(const vec<T, 3>& a, const vec<T, 3>& b) requires(
    !std::is_same_v<T, bool>) {
    return vec<T, 3> {
      a[1] * b[2] - a[2] * b[1],
      a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0],
    };
  }

  template <class T, size_t N>
  constexpr T length(const vec<T, N>& a) requires(std::is_floating_point_v<T>) {
    return std::sqrt(dot(a, a));
  }
  
  template <class T, size_t N>
  constexpr vec<T, N> norm(const vec<T, N>& a) requires(std::is_floating_point_v<T>) {
    return a / length(a);
  }

  template <class T, size_t C, size_t R = C>
  struct mat {
    static_assert(
      std::is_floating_point_v<T>, "Only floating-point matrices are allowed");
    static_assert(
      C >= 2 && C <= 4 && R >= 2 && R <= 4,
      "Matrix dimensions should be between 2x2 and 4x4");

    constexpr vec<T, C>& operator[](size_t n) { return _m_data[n]; }

    constexpr vec<T, R> row(size_t n) {
      [ this, n ]<size_t... Is>(std::index_sequence<Is...>) {
        return vec<T, R> {(_m_data[Is][n])...};
      }
      (std::make_index_sequence<R> {});
    }

    vec<T, C> _m_data[R];
  };

  // Basic matrix operations
  // =======================

#define MAT_COMP_OP(o)                                           \
  template <class T, size_t C, size_t R>                         \
  constexpr mat<T, C, R> operator o(                             \
    const mat<T, C, R>& a, const mat<T, C, R>& b) {              \
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) { \
      return mat<T, C, R> {(a[Is] o b[Is])...};                  \
    }                                                            \
    (std::make_index_sequence<R> {});                            \
  }
#define MAT_SCALAR_OP(o)                                          \
  template <class T, size_t C, size_t R>                          \
  constexpr mat<T, C, R> operator o(const mat<T, C, R>& a, T b) { \
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) {  \
      return mat<T, C, R> {(a[Is] o b)...};                       \
    }                                                             \
    (std::make_index_sequence<R> {});                             \
  }
#define MAT_SCALAR_OP_REVERSE(o)                                  \
  template <class T, size_t C, size_t R>                          \
  constexpr mat<T, C, R> operator o(T a, const mat<T, C, R>& b) { \
    return b o a;                                                 \
  }

  MAT_COMP_OP(+)
  MAT_SCALAR_OP(+)
  MAT_SCALAR_OP_REVERSE(+)

  MAT_COMP_OP(-)
  MAT_SCALAR_OP(-)
  MAT_SCALAR_OP_REVERSE(-)

  // Multiplication only has scalar product
  // since it is special for mat * mat and mat * vec
  MAT_SCALAR_OP(*)
  MAT_SCALAR_OP_REVERSE(*)

  template <class T, size_t C, size_t R>
  constexpr vec<T, R> operator*(const mat<T, C, R>& a, const vec<T, R>& b) {
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) {
      return vec<T, R> {dot(a.row(Is), b)...};
    }
    (std::make_index_sequence<R> {});
  }

  template <class T, size_t R0, size_t C1, size_t M>
  constexpr mat<T, C1, R0> operator*(
    const mat<T, M, R0>& a, const mat<T, C1, M>& b) {
    return [&a, &b ]<size_t... Is>(std::index_sequence<Is...>) {
      // each column of the result matrix is equal to
      // the matrix-vector product of the 1st matrix and the
      // corresponding column in the 2nd matrix
      return mat<T, C1, R0> {(a * b[Is])...};
    }
    (std::make_index_sequence<C1> {});
  }

  MAT_COMP_OP(/)
  MAT_SCALAR_OP(/)

#undef MAT_COMP_OP
#undef MAT_SCALAR_OP
#undef MAT_SCALAR_OP_REVERSE

  // Typedefs to match GLSL
  // ======================

#define VEC_DEF(l, T)        \
  using l##vec2 = vec<T, 2>; \
  using l##vec3 = vec<T, 3>; \
  using l##vec4 = vec<T, 4>;
#define MAT_DEF(C, R) using mat##C##x##R = mat<float, C, R>;

  using vec2 = vec<float, 2>;
  using vec3 = vec<float, 3>;
  using vec4 = vec<float, 4>;
  VEC_DEF(b, bool)
  VEC_DEF(d, double)
  VEC_DEF(i, int32_t)
  VEC_DEF(u, uint32_t)

  MAT_DEF(2, 2)
  MAT_DEF(2, 3)
  MAT_DEF(2, 4)

  MAT_DEF(3, 2)
  MAT_DEF(3, 3)
  MAT_DEF(3, 4)

  MAT_DEF(4, 2)
  MAT_DEF(4, 3)
  MAT_DEF(4, 4)

  using mat2 = mat2x2;
  using mat3 = mat3x3;
  using mat4 = mat4x4;

#undef VEC_DEF
#undef MAT_DEF
}  // namespace oglc
#endif