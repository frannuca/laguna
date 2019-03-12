//
// Created by venus on 03/03/2019.
//

#ifndef PROJECT_MISSINGTRAITS_H
#define PROJECT_MISSINGTRAITS_H
#include <string>
#include <cstdint>
#include <cstring>
#include <limits>

namespace laguna {

    template<typename T>
    class MissingTrait {
    public:
        const T missing;
        typedef T value_type;
        MissingTrait() {}
    };

    template<>
    class MissingTrait<double> {
    public:
        const double missing;
        typedef double value_type;
        MissingTrait() : missing(std::numeric_limits<double>::quiet_NaN()) {}
    };

    template<>
    class MissingTrait<int> {
    public:
        const int missing;
        typedef int value_type;
        MissingTrait() : missing(std::numeric_limits<int>::quiet_NaN()) {}
    };

    template<>
    class MissingTrait<std::string> {
    public:
        const std::string missing;
        typedef std::string value_type;
        MissingTrait() : missing("NA") {}
    };

    template<>
    class MissingTrait<bool> {
    public:
        const bool missing;
        typedef bool value_type;
        MissingTrait() : missing(false) {}
    };
}
#endif //PROJECT_MISSINGTRAITS_H
