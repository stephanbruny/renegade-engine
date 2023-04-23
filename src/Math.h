//
// Created by Stephan Bruny on 23.04.23.
//

#ifndef RENEGADE_ENGINE_MATH_H
#define RENEGADE_ENGINE_MATH_H

namespace Math {
    static inline float lerp(float a, float b, float f) {
        return a * (1.0 - f) + (b * f);
    }
}


#endif //RENEGADE_ENGINE_MATH_H
