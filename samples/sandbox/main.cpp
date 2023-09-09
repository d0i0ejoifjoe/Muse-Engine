#include "src/log/public/logger.h"
#include "src/graphics/public/transform.h"
#include <iostream>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cstdio>

int main()
{
    glm::mat4 mat{0.0f, 0.0f, 1.0f, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                 -1.0f, 0.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f};

    muse::Transform transform{mat};

    LOG_INFO(MatrixDecompose,  "Matrix: {}\n\n"
                               "Translation: {}\n"
                               "Rotation: {}\n"
                               "Scale: {}\n", mat, transform.translation(), transform.rotation(), transform.scale());
    return 0;
}