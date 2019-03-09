//
// Created by venus on 27/02/2019.
//

#ifndef LAGUNA_CONVERTERS_H
#define LAGUNA_CONVERTERS_H
#include <string>
#include <cstdint>
#include <cstring>
#include <limits>

namespace laguna {


    template<typename T>
    T &voidToTypeReference(void *x) {
        return *(static_cast<T *>(x));
    }

    template<typename T>
    void *TypeReferenceToCopyVoid(const T &x) {
        return (static_cast<void*>(new T(x)));
    }
    template<typename T>
    void *getVoidCopy(void *x) {
        if (x == nullptr) return nullptr;

        int sizeofblock = sizeof(T);
        void *cpy = malloc(sizeofblock);
        std::memcpy(cpy, x, sizeofblock);
        return cpy;
    }


    class Constants{
        public:
        enum AddDataType {WithReplacement=0,ErrorOnDuplication,ClearAndAdd};
        };

}
#endif //LAGUNA_CONVERTERS_H
