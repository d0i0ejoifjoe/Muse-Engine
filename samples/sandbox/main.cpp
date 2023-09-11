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

    glm::mat4 mat1{0.0f, 0.0f, -1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};


    muse::Transform transform{mat};

    LOG_INFO(MatrixDecompose,  "Matrix: {}\n\n"
                               "Translation: {}\n"
                               "Rotation: {}\n"
                               "Scale: {}\n", mat, transform.translation(), transform.rotation(), transform.scale());

    std::cout << transform.rotation().w << " " << transform.rotation().y << std::endl;

    muse::Transform transform1{mat1};

    LOG_INFO(Interpolation, "\nTranslation: {}\n"
                            "Rotation: {}\n"
                            "Scale: {}\n", transform1.translation(), transform1.rotation(), transform1.scale());    


    auto new_transform = transform.interpolate(transform1, 0.5f);

    LOG_INFO(Interpolation, "\nTranslation: {}\n"
                            "Rotation: {}\n"
                            "Scale: {}\n", new_transform.translation(), new_transform.rotation(), new_transform.scale());

    auto rot_mat = glm::mat4_cast(new_transform.rotation());

    std::cout << rot_mat << std::endl;    

    return 0;
}