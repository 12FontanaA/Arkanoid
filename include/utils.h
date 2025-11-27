#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"
#include <vector>

template<typename T>
int FindIndex(const std::vector<T>& vec, const T& value) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) return static_cast<int>(i);
    }
    return -1;
}

template<typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

template<typename T>
T Lerp(T a, T b, float t) {
    return a + (b - a) * t;
}

// NOVOS TEMPLATES - ADICIONADOS
template<typename T>
T Max(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template<typename T>
T Min(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template<typename T>
void Swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

template<typename T>
T Abs(const T& value) {
    return (value < 0) ? -value : value;
}

#endif