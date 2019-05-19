#pragma once

#include "Extern/glm/gtc/matrix_transform.hpp"

struct Mat4x4Storage
{
    float m[4][4];

    void SetColumn(const float col[4], size_t col_index)
    {
        for (size_t i = 0; i < 4; i++)
        {
            m[col_index][i] = col[i];
        }
    }
    void Set(const glm::mat4x4& m)
    {
        for (size_t i = 0; i < 4; i++)
        {
            glm::vec4 glmCol = m[i];

            float col[4];

            for (size_t i =0 ; i<4; i++)
                col[i] = glmCol[i];

            SetColumn(col, i);
        }
    }
};