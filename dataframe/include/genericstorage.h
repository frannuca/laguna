//
// Created by fran on 3/27/19.
//

#ifndef PROJECT_GENERICVECTOR_H
#define PROJECT_GENERICVECTOR_H
#include <vector>
#include <memory>
#include <utility>

namespace laguna{

    using namespace std;


    class Storage{

        template<typename T>
        static std::unordered_map<Storage *, map<string,vector<T>>> vectors_;

        template<typename T>
        static std::unordered_map<Storage *, map<string,std::function<void(void)>>> clean_f;

        template<typename T>
        static std::unordered_map<Storage *, map<string,std::function<std::string(void)>>> printer_f;

    public:
        template<typename T>
        std::vector<T>& NewColumn(const std::string name);

        template<typename T>
        bool DeleteColumn(const string& name);

        template<typename T>
        std::vector<T>& Column(const std::string name);

        virtual ~Storage();
    };


#ifdef DMS_INCLUDE_SOURCE
#include "../genericstorage.tcc"
#endif // DMS_INCLUDE_SOURCE
}
#endif //PROJECT_GENERICVECTOR_H
