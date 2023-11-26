#pragma once

#include "quaternion.h"
#include "utils.h"
#include "vector4.h"

#include <array>
#include <numbers>
#include <ostream>
#include <tuple>

namespace muse
{

/**
 *
 *  Class that represents matrix 4x4.
 *
 */
class Matrix4
{
  public:
    /**
     *
     *  Create a matrix with each individual element set.
     *
     *  @param x0 Row 0 Column 0 element.
     *  @param x1 Row 0 Column 1 element.
     *  @param x2 Row 0 Column 2 element.
     *  @param x3 Row 0 Column 3 element.
     *  @param y0 Row 1 Column 0 element.
     *  @param y1 Row 1 Column 1 element.
     *  @param y2 Row 1 Column 2 element.
     *  @param y3 Row 1 Column 3 element.
     *  @param z0 Row 2 Column 0 element.
     *  @param z1 Row 2 Column 1 element.
     *  @param z2 Row 2 Column 2 element.
     *  @param z3 Row 2 Column 3 element.
     *  @param w0 Row 3 Column 0 element.
     *  @param w1 Row 3 Column 1 element.
     *  @param w2 Row 3 Column 2 element.
     *  @param w3 Row 3 Column 3 element.
     *
     */
    constexpr Matrix4(float x0, float x1, float x2, float x3,
                      float y0, float y1, float y2, float y3,
                      float z0, float z1, float z2, float z3,
                      float w0, float w1, float w2, float w3)
        : elements_({x0, x1, x2, x3,
                     y0, y1, y2, y3,
                     z0, z1, z2, z3,
                     w0, w1, w2, w3})
    {
    }

    /**
     *
     *  Create scale matrix.
     *
     *  @param scale Scale in all 3 dimensions.
     *
     */
    constexpr Matrix4(float scale)
        : elements_({scale, 0.0f, 0.0f, 0.0f,
                     0.0f, scale, 0.0f, 0.0f,
                     0.0f, 0.0f, scale, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f})
    {
    }

    /**
     *
     *  Create a indentity matrix, so any matrix multiplied with it equals that matrix.
     *
     */
    constexpr Matrix4()
        : Matrix4(1.0f)
    {
    }

    /**
     *
     *  Get reference to element by index.
     *
     *  @param i Index of element.
     *
     *  @return Reference to that element.
     *
     */
    constexpr float &operator[](std::size_t i)
    {
        return elements_[i];
    }

    /**
     *
     *  Get copy of element by index.
     *
     *  @param i Index of element.
     *
     *  @return Copy of element.
     *
     */
    constexpr float operator[](std::size_t i) const
    {
        return elements_[i];
    }

    /**
     *
     *  Get row of a matrix (accounts for translation).
     *
     *  @param row Index of a row to get, needs to be in range [0, 3].
     *
     */
    constexpr Vector4 row(std::size_t row) const
    {
        const auto row_4 = row * 4;
        return Vector4{elements_[row_4], elements_[row_4 + 1], elements_[row_4 + 2], elements_[row_4 + 3]};
    }

    /**
     *
     *  Get column of a matrix (accounts for last row).
     *
     *  @param col Inde of a column to get, needs to be in range [0, 3].
     *
     */
    constexpr Vector4 column(std::size_t col) const
    {
        return Vector4{elements_[col], elements_[col + 4], elements_[col + 8], elements_[col + 12]};
    }

    /**
     *
     *  Multiply this matrix by some other matrix.
     *
     *  @param m Matrix to multiply by.
     *
     *  @return Reference to this matrix.
     *
     */
    constexpr Matrix4 &operator*=(const Matrix4 &m)
    {
        const auto element = [&](std::size_t row, std::size_t col)
        {
            auto row_elements = this->row(row);
            auto col_elements = m.column(col);

            return Vector4::dot(row_elements, col_elements);
        };

        Matrix4 result{*this};

        result[0] = element(0, 0);
        result[1] = element(0, 1);
        result[2] = element(0, 2);
        result[3] = element(0, 3);

        result[4] = element(1, 0);
        result[5] = element(1, 1);
        result[6] = element(1, 2);
        result[7] = element(1, 3);

        result[8] = element(2, 0);
        result[9] = element(2, 1);
        result[10] = element(2, 2);
        result[11] = element(2, 3);

        result[12] = element(3, 0);
        result[13] = element(3, 1);
        result[14] = element(3, 2);
        result[15] = element(3, 3);

        return *this = std::move(result);
    }

    /**
     *
     *  Multiply two matrices and return the result.
     *
     *  @param m Matrix to multiply by.
     *
     *  @return Result of multiplication.
     *
     */
    constexpr Matrix4 operator*(const Matrix4 &m) const
    {
        return Matrix4(*this) *= m;
    }

    /**
     *
     *  Multiply this matrix by scalar value (useful for getting the inverse).
     *
     *  @param scale Scalar.
     *
     *  @return Reference to this matrix.
     *
     */
    constexpr Matrix4 &operator*=(float scale)
    {
        for (auto i = 0u; i < 16; i++)
        {
            elements_[i] *= scale;
        }

        return *this;
    }

    /**
     *
     *  Multiply this matrix matrix by scalar and return the result.
     *
     *  @param scale Scalar.
     *
     *  @return Result of multiplication.
     *
     */
    constexpr Matrix4 operator*(float scale) const
    {
        return Matrix4(*this) *= scale;
    }

    /**
     *
     *  Multiply a vector 3d by matrix.
     *
     *  @param v Vector 3d to multiply.
     *
     *  @return Result of multiplication.
     *
     */
    constexpr Vector3 operator*(const Vector3 &v) const
    {
        return Vector3{elements_[0] * v.x + elements_[1] * v.y + elements_[2] * v.z + elements_[3],
                       elements_[4] * v.x + elements_[5] * v.y + elements_[6] * v.z + elements_[7],
                       elements_[8] * v.x + elements_[9] * v.y + elements_[10] * v.z + elements_[11]};
    }

    /**
     *
     *  Multiply a vector 4d by matrix.
     *
     *  @param v Vector 4d to multiply.
     *
     *  @return Result of multiplication.
     *
     */
    constexpr Vector4 operator*(const Vector4 &v) const
    {
        return Vector4{elements_[0] * v.x + elements_[1] * v.y + elements_[2] * v.z + elements_[3] * v.w,
                       elements_[4] * v.x + elements_[5] * v.y + elements_[6] * v.z + elements_[7] * v.w,
                       elements_[8] * v.x + elements_[9] * v.y + elements_[10] * v.z + elements_[11] * v.w,
                       elements_[12] * v.x + elements_[13] * v.y + elements_[14] * v.z + elements_[15] * v.w};
    }

    /**
     *
     *  Get inverse of matrix.
     *
     *  @return Inverse of this matrix if it exists, otherwise indentity matrix.
     *
     */
    constexpr Matrix4 inverse() const
    {
        Matrix4 inv{1.0f};

        inv[0] = elements_[5] * elements_[10] * elements_[15] -
                 elements_[5] * elements_[11] * elements_[14] -
                 elements_[9] * elements_[6] * elements_[15] +
                 elements_[9] * elements_[7] * elements_[14] +
                 elements_[13] * elements_[6] * elements_[11] -
                 elements_[13] * elements_[7] * elements_[10];

        inv[4] = -elements_[4] * elements_[10] * elements_[15] +
                 elements_[4] * elements_[11] * elements_[14] +
                 elements_[8] * elements_[6] * elements_[15] -
                 elements_[8] * elements_[7] * elements_[14] -
                 elements_[12] * elements_[6] * elements_[11] +
                 elements_[12] * elements_[7] * elements_[10];

        inv[8] = elements_[4] * elements_[9] * elements_[15] -
                 elements_[4] * elements_[11] * elements_[13] -
                 elements_[8] * elements_[5] * elements_[15] +
                 elements_[8] * elements_[7] * elements_[13] +
                 elements_[12] * elements_[5] * elements_[11] -
                 elements_[12] * elements_[7] * elements_[9];

        inv[12] = -elements_[4] * elements_[9] * elements_[14] +
                  elements_[4] * elements_[10] * elements_[13] +
                  elements_[8] * elements_[5] * elements_[14] -
                  elements_[8] * elements_[6] * elements_[13] -
                  elements_[12] * elements_[5] * elements_[10] +
                  elements_[12] * elements_[6] * elements_[9];

        inv[1] = -elements_[1] * elements_[10] * elements_[15] +
                 elements_[1] * elements_[11] * elements_[14] +
                 elements_[9] * elements_[2] * elements_[15] -
                 elements_[9] * elements_[3] * elements_[14] -
                 elements_[13] * elements_[2] * elements_[11] +
                 elements_[13] * elements_[3] * elements_[10];

        inv[5] = elements_[0] * elements_[10] * elements_[15] -
                 elements_[0] * elements_[11] * elements_[14] -
                 elements_[8] * elements_[2] * elements_[15] +
                 elements_[8] * elements_[3] * elements_[14] +
                 elements_[12] * elements_[2] * elements_[11] -
                 elements_[12] * elements_[3] * elements_[10];

        inv[9] = -elements_[0] * elements_[9] * elements_[15] +
                 elements_[0] * elements_[11] * elements_[13] +
                 elements_[8] * elements_[1] * elements_[15] -
                 elements_[8] * elements_[3] * elements_[13] -
                 elements_[12] * elements_[1] * elements_[11] +
                 elements_[12] * elements_[3] * elements_[9];

        inv[13] = elements_[0] * elements_[9] * elements_[14] -
                  elements_[0] * elements_[10] * elements_[13] -
                  elements_[8] * elements_[1] * elements_[14] +
                  elements_[8] * elements_[2] * elements_[13] +
                  elements_[12] * elements_[1] * elements_[10] -
                  elements_[12] * elements_[2] * elements_[9];

        inv[2] = elements_[1] * elements_[6] * elements_[15] -
                 elements_[1] * elements_[7] * elements_[14] -
                 elements_[5] * elements_[2] * elements_[15] +
                 elements_[5] * elements_[3] * elements_[14] +
                 elements_[13] * elements_[2] * elements_[7] -
                 elements_[13] * elements_[3] * elements_[6];

        inv[6] = -elements_[0] * elements_[6] * elements_[15] +
                 elements_[0] * elements_[7] * elements_[14] +
                 elements_[4] * elements_[2] * elements_[15] -
                 elements_[4] * elements_[3] * elements_[14] -
                 elements_[12] * elements_[2] * elements_[7] +
                 elements_[12] * elements_[3] * elements_[6];

        inv[10] = elements_[0] * elements_[5] * elements_[15] -
                  elements_[0] * elements_[7] * elements_[13] -
                  elements_[4] * elements_[1] * elements_[15] +
                  elements_[4] * elements_[3] * elements_[13] +
                  elements_[12] * elements_[1] * elements_[7] -
                  elements_[12] * elements_[3] * elements_[5];

        inv[14] = -elements_[0] * elements_[5] * elements_[14] +
                  elements_[0] * elements_[6] * elements_[13] +
                  elements_[4] * elements_[1] * elements_[14] -
                  elements_[4] * elements_[2] * elements_[13] -
                  elements_[12] * elements_[1] * elements_[6] +
                  elements_[12] * elements_[2] * elements_[5];

        inv[3] = -elements_[1] * elements_[6] * elements_[11] +
                 elements_[1] * elements_[7] * elements_[10] +
                 elements_[5] * elements_[2] * elements_[11] -
                 elements_[5] * elements_[3] * elements_[10] -
                 elements_[9] * elements_[2] * elements_[7] +
                 elements_[9] * elements_[3] * elements_[6];

        inv[7] = elements_[0] * elements_[6] * elements_[11] -
                 elements_[0] * elements_[7] * elements_[10] -
                 elements_[4] * elements_[2] * elements_[11] +
                 elements_[4] * elements_[3] * elements_[10] +
                 elements_[8] * elements_[2] * elements_[7] -
                 elements_[8] * elements_[3] * elements_[6];

        inv[11] = -elements_[0] * elements_[5] * elements_[11] +
                  elements_[0] * elements_[7] * elements_[9] +
                  elements_[4] * elements_[1] * elements_[11] -
                  elements_[4] * elements_[3] * elements_[9] -
                  elements_[8] * elements_[1] * elements_[7] +
                  elements_[8] * elements_[3] * elements_[5];

        inv[15] = elements_[0] * elements_[5] * elements_[10] -
                  elements_[0] * elements_[6] * elements_[9] -
                  elements_[4] * elements_[1] * elements_[10] +
                  elements_[4] * elements_[2] * elements_[9] +
                  elements_[8] * elements_[1] * elements_[6] -
                  elements_[8] * elements_[2] * elements_[5];

        float det = elements_[0] * inv[0] + elements_[1] * inv[4] + elements_[2] * inv[8] + elements_[3] * inv[12];

        if (det != 0.0f)
        {
            inv *= 1.0f / det;
            return inv;
        }

        return Matrix4{1.0f};
    }

    /**
     *
     *  Static method to get inverse of given matrix.
     *
     *  @param m Matrix to get inverse of.
     *
     *  @return Inverse of given matrix.
     *
     */
    static constexpr Matrix4 inverse(const Matrix4 &m)
    {
        return m.inverse();
    }

    /**
     *
     *  Get pointer to underlying array of floats.
     *
     *  @return Pointer to data.
     *
     */
    constexpr const float *data() const
    {
        return elements_.data();
    }

    /**
     *
     *  Make a translation matrix.
     *
     *  @param translation Translation in world space for given matrix.
     *
     *  @return Translation matrix.
     *
     */
    static constexpr Matrix4 translate(const Vector3 &translation)
    {
        return Matrix4{1.0f, 0.0f, 0.0f, translation.x,
                       0.0f, 1.0f, 0.0f, translation.y,
                       0.0f, 0.0f, 1.0f, translation.z,
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    /**
     *
     *  Make a rotation matrix out of quaternion.
     *
     *  @param rotation Rotation.
     *
     *  @return Rotation matrix 4x4.
     *
     */
    static constexpr Matrix4 rotate(const Quaternion &rotation)
    {
        return Matrix4{1.0f - 2.0f * rotation.y * rotation.y - 2.0f * rotation.z * rotation.z, 2.0f * rotation.x * rotation.y - 2.0f * rotation.w * rotation.z, 2.0f * rotation.x * rotation.z - 2.0f * rotation.w * rotation.y, 0.0f,
                       2.0f * rotation.x * rotation.y + 2.0f * rotation.w * rotation.z, 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.z * rotation.z, 2.0f * rotation.y * rotation.z + 2.0f * rotation.w * rotation.x, 0.0f,
                       2.0f * rotation.x * rotation.z - 2.0f * rotation.w * rotation.y, 2.0f * rotation.y * rotation.z - 2.0f * rotation.w * rotation.x, 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.y * rotation.y, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    /**
     *
     *  Make a scale matrix.
     *
     *  @param scale Scale vector.
     *
     *  @return Scale matrix.
     *
     */
    static constexpr Matrix4 scale(const Vector3 &scale)
    {
        return Matrix4{scale.x, 0.0f, 0.0f, 0.0f,
                       0.0f, scale.y, 0.0f, 0.0f,
                       0.0f, 0.0f, scale.z, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    /**
     *
     *  Create a perspective projection matrix.
     *
     *  @param fov Field of view (in degrees).
     *  @param width Width of projection (usually width of window or render target).
     *  @param height Height of projection (usually height of window or render target).
     *  @param near_plane Near plane of frustum.
     *  @param far_plane Far plane of frustum.
     *  @param is_opengl Is projection matrix created for OpenGL API (by default true).
     *
     */
    static constexpr Matrix4 perspective_projection(float fov, float width, float height, float near_plane, float far_plane, bool is_opengl = true)
    {
        if (is_opengl)
        {
            return perspective_projection_opengl(fov, width, height, near_plane, far_plane);
        }
        else
        {
            return perspective_projection_vulkan(fov, width, height, near_plane, far_plane);
        }
    }

    /**
     *
     *  Create a perspective projection with infinite far plane by taking a limit as far_plane approaches infinity.
     *
     *  @param fov Field of view (in degrees).
     *  @param width Width of projection (usually width of window or render target).
     *  @param height Height of projection (usually height of window or render target).
     *  @param near_plane Near plane of frustum.
     *  @param epsilon Epsilon (Smallest float value to actually change a float).
     *  @param is_opengl Is projection matrix created for OpenGL API (by default true).
     *
     */
    static constexpr Matrix4 infinite_perspective_projection(float fov,
                                                             float width,
                                                             float height,
                                                             float near_plane,
                                                             float epsilon = std::numeric_limits<float>::epsilon(),
                                                             bool is_opengl = true)
    {
        if (is_opengl)
        {
            return infinite_perspective_projection_opengl(fov, width, height, near_plane, epsilon);
        }
        else
        {
            // FIXME: Create a infinite perspective matrix for vulkan too
            return Matrix4{1.0f};
        }
    }

    /**
     *
     *  Create an orthographic projection matrix.
     *
     *  @param width Width of projection (usually width of window or render target).
     *  @param height Height of projection (usually height of window or render target).
     *  @param depth Depth of projection (how much you want to see in front of you).
     *  @param is_opengl Is projection matrix created for OpenGL API (by default true).
     *
     */
    static constexpr Matrix4 orthographic_projection(float width, float height, float depth, bool is_opengl = true)
    {
        if (is_opengl)
        {
            return orthographic_projection_opengl(width, height, depth);
        }
        else
        {
            return orthographic_projection_vulkan(width, height, depth);
        }
    }

    /**
     *
     *  Create a view or look at matrix.
     *
     *  @param eye Position of viewer.
     *  @param target Direction to look at
     *  @param up Up vector (most of the times is just (0, 1, 0), if you support tilting, then you need to calculate it yourself).
     *  @param is_opengl Whether view matrix is created for OpenGL API (by default true).
     *
     */
    static constexpr Matrix4 look_at(const Vector3 &eye, const Vector3 &target, const Vector3 &up, bool is_opengl = true)
    {
        if (is_opengl)
        {
            return look_at_opengl(eye, target, up);
        }
        else
        {
            return look_at_opengl(eye, target, up);
        }
    }

    /**
     *
     *  Convert matrix to quaternion (expecting that this matrix has extracted translation and was divided it's own column by it's corresponding scalar).
     *
     *  @return Quaternion.
     *
     */
    constexpr Quaternion to_quat() const
    {
        Quaternion q{};

        auto trace = elements_[0] + elements_[5] + elements_[10];
        auto root = 0.0f;

        if (trace > 0.0f)
        {
            root = 0.5f / std::sqrt(1.0f + trace);
            q.w = 0.25f / root;
            q.x = (elements_[6] - elements_[9]) * root;
            q.y = (elements_[8] - elements_[2]) * root;
            q.z = (elements_[1] - elements_[4]) * root;
        }
        else if (elements_[0] > elements_[5] && elements_[0] > elements_[10])
        {
            root = 2.0f * std::sqrt(1.0f + elements_[0] - elements_[5] - elements_[10]);
            q.w = (elements_[6] - elements_[9]) / root;
            q.x = 0.25f * root;
            q.y = (elements_[4] + elements_[1]) / root;
            q.z = (elements_[8] + elements_[2]) / root;
        }
        else if (elements_[5] > elements_[10])
        {
            root = 2.0f * std::sqrt(1.0f + elements_[5] - elements_[0] - elements_[10]);
            q.w = (elements_[8] - elements_[2]) / root;
            q.x = (elements_[4] + elements_[1]) / root;
            q.y = 0.25f * root;
            q.z = (elements_[9] + elements_[6]) / root;
        }
        else
        {
            root = 2.0f * std::sqrt(1.0f + elements_[10] - elements_[0] - elements_[5]);
            q.w = (elements_[1] - elements_[4]) / root;
            q.x = (elements_[8] + elements_[2]) / root;
            q.y = (elements_[9] + elements_[6]) / root;
            q.z = 0.25f * root;
        }

        return q;
    }

    /**
     *
     *  Static method to convert given 'rotation' matrix to quaternion (expecting that this matrix has extracted translation
     *  and was divided it's own column by it's corresponding scalar).
     *
     *  @param m Matrix what we convert to quaternion.
     *
     *  @return Quaternion.
     *
     */
    static constexpr Quaternion to_quat(const Matrix4 &m)
    {
        return m.to_quat();
    }

    /**
     *
     *  Decompose a matrix to corresponding (translation, rotation, scale).
     *
     *  @return Tuple [Translation, Rotation, Scale].
     *
     */
    constexpr std::tuple<Vector3, Quaternion, Vector3> decompose() const
    {
        Matrix4 copy{*this};

        Vector3 translation{copy[3], copy[7], copy[11]};

        Vector3 scale{Vector3{copy[0], copy[4], copy[8]}.magnitude(),
                      Vector3{copy[1], copy[5], copy[9]}.magnitude(),
                      Vector3{copy[2], copy[6], copy[10]}.magnitude()};

        copy[0] /= scale.x;
        copy[1] /= scale.y;
        copy[2] /= scale.z;
        copy[3] /= 0.0f;
        copy[4] /= scale.x;
        copy[5] /= scale.y;
        copy[6] /= scale.z;
        copy[7] /= 0.0f;
        copy[8] /= scale.x;
        copy[9] /= scale.y;
        copy[10] /= scale.z;
        copy[11] /= 0.0f;
        copy[15] /= 1.0f;

        Quaternion rotation = Matrix4::to_quat(copy);

        return {translation, rotation, scale};
    }

    /**
     *
     *  Static method to decompose a given matrix.
     *
     *  @param m Matrix to decompose.
     *
     *  @return Tuple [Translation, Rotation, Scale].
     *
     */
    static constexpr std::tuple<Vector3, Quaternion, Vector3> decompose(const Matrix4 &m)
    {
        return m.decompose();
    }

    /**
     *
     *  Transpose this matrix (change matrix's ordering).
     *
     */
    constexpr void transpose()
    {
        swap(elements_[1], elements_[4]);
        swap(elements_[2], elements_[8]);
        swap(elements_[3], elements_[12]);
        swap(elements_[6], elements_[9]);
        swap(elements_[7], elements_[13]);
        swap(elements_[11], elements_[14]);
    }

    /**
     *
     *  Static method to transpose supplied matrix.
     *
     *  @param m Matrix to transpose.
     *
     *  @return Transposed version of given matrix.
     *
     */
    static constexpr Matrix4 transpose(const Matrix4 &m)
    {
        auto ret = Matrix4{m};
        ret.transpose();
        return ret;
    }

    /**
     *
     *  Compare two matrices.
     *
     *  @param m Matrix to compare with.
     *
     *  @return True if they're equal, false otherwise.
     *
     */
    bool operator==(const Matrix4 &m) const
    {
        return std::equal(std::cbegin(elements_), std::cend(elements_), std::cbegin(m.elements_), compare);
    }

    /**
     *
     *  Compare two matrices for unequalities.
     *
     *  @param m Matrix to compare with.
     *
     *  @return True if they're unequal, false otherwise.
     *
     */
    bool operator!=(const Matrix4 &m) const
    {
        return !(*this == m);
    }

  private:
    /** Helpers. Just functions and different formulas to compute certain matrices */
    static constexpr Matrix4 perspective_projection_opengl(float fov, float width, float height, float near_plane, float far_plane)
    {
        const auto tan_half_fov = std::tan(to_radians(fov / 2.0f));
        const auto aspect_ratio = width / height;
        const auto range = far_plane - near_plane;

        return Matrix4{1.0f / (tan_half_fov * aspect_ratio), 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
                       0.0f, 0.0f, -(far_plane + near_plane) / range, -(2.0f * near_plane * far_plane) / range,
                       0.0f, 0.0f, -1.0f, 0.0f};
    }

    static constexpr Matrix4 perspective_projection_vulkan(float fov, float width, float height, float near_plane, float far_plane)
    {
        const auto tan_half_fov = std::tan(to_radians(fov / 2.0f));
        const auto aspect_ratio = width / height;
        const auto range = far_plane - near_plane;
        const auto t = tan_half_fov * near_plane;
        const auto b = -t;
        const auto r = t * aspect_ratio;
        const auto l = b * aspect_ratio;

        return Matrix4{2.0f * near_plane / (r - l), 0.0f, -(r + l) / (r - l), 0.0f,
                       0.0f, 2.0f * near_plane / (b - t), -(b + t) / (b - t), 0.0f,
                       0.0f, 0.0f, far_plane / range, -far_plane * near_plane / range,
                       0.0f, 0.0f, 1.0f, 0.0f};
    }

    static constexpr Matrix4 orthographic_projection_opengl(float width, float height, float depth)
    {
        const auto t = height;
        const auto b = -t;
        const auto r = width;
        const auto l = -r;
        const auto far_plane = depth;
        const auto near_plane = -far_plane;

        return Matrix4{2.0f / (r - l), 0.0f, 0.0f, -(r + l) / (r - l),
                       0.0f, 2.0f / (t - b), 0.0f, -(t + b) / (t - b),
                       0.0f, 0.0f, -2.0f / (far_plane - near_plane), -(far_plane + near_plane) / (far_plane - near_plane),
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Matrix4 orthographic_projection_vulkan(float width, float height, float depth)
    {
        const auto t = height;
        const auto b = -t;
        const auto r = width;
        const auto l = -r;
        const auto far_plane = depth;
        const auto near_plane = -far_plane;

        return Matrix4{2.0f / (r - l), 0.0f, 0.0f, -(r + l) / (r - l),
                       0.0f, 2.0f / (b - t), 0.0f, -(b + t) / (b - t),
                       0.0f, 0.0f, 1.0f / (far_plane - near_plane), -near_plane / (far_plane - near_plane),
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Matrix4 look_at_opengl(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
    {
        const auto zaxis = Vector3::normalize(target - eye);
        const auto xaxis = Vector3::normalize(Vector3::cross(zaxis, up));
        const auto yaxis = Vector3::cross(xaxis, zaxis);

        return Matrix4{xaxis.x, xaxis.y, xaxis.z, -Vector3::dot(xaxis, eye),
                       yaxis.x, yaxis.y, yaxis.z, -Vector3::dot(yaxis, eye),
                       -zaxis.x, -zaxis.y, -zaxis.z, Vector3::dot(zaxis, eye),
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Matrix4 look_at_vulkan(const Vector3 &eye, const Vector3 &target, const Vector3 &up)
    {
        const auto zaxis = Vector3::normalize(target - eye);
        const auto xaxis = Vector3::normalize(Vector3::cross(up, zaxis));
        const auto yaxis = Vector3::cross(zaxis, xaxis);

        return Matrix4{xaxis.x, xaxis.y, xaxis.z, -Vector3::dot(xaxis, eye),
                       yaxis.x, yaxis.y, yaxis.z, -Vector3::dot(yaxis, eye),
                       zaxis.x, zaxis.y, zaxis.z, -Vector3::dot(zaxis, eye),
                       0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Matrix4 infinite_perspective_projection_opengl(float fov, float width, float height, float near_plane, float epsilon = std::numeric_limits<float>::epsilon())
    {
        const auto tan_half_fov = std::tan(to_radians(fov / 2.0f));
        const auto aspect_ratio = width / height;

        return Matrix4{1.0f / (tan_half_fov * aspect_ratio), 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
                       0.0f, 0.0f, epsilon - 1.0f, (epsilon - 2.0f) * near_plane,
                       0.0f, 0.0f, -1.0f, 0.0f};
    }

    /** Elements of matrix. */
    std::array<float, 16> elements_;
};

/**
 *
 *  Function to print matrix to supplied stream.
 *
 *  @param os Stream to write into.
 *  @param m Matrix to write.
 *
 *  @return Reference to stream.
 *
 */
inline std::ostream &operator<<(std::ostream &os, const Matrix4 &m)
{
    os << '\n';
    os << '[' << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << ']' << '\n'
       << '[' << m[4] << ", " << m[5] << ", " << m[6] << ", " << m[7] << ']' << '\n'
       << '[' << m[8] << ", " << m[9] << ", " << m[10] << ", " << m[11] << ']' << '\n'
       << '[' << m[12] << ", " << m[13] << ", " << m[14] << ", " << m[15] << ']';

    return os;
}

}