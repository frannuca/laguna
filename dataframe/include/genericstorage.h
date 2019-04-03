//
// Created by fran on 3/27/19.
//

#ifndef PROJECT_GENERICVECTOR_H
#define PROJECT_GENERICVECTOR_H
#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <functional>
#include <sstream>

namespace laguna{

    using namespace std;

    class Storage{

        template<typename T>
        static map<Storage *, map<string,vector<T>>> vectors_;

        static map<Storage *, map<string,std::function<void(void)>>> clean_f;

        static map<Storage *, map<string,std::function<void(int)>>> inserter_f;

        static map<Storage *, function<void(void)>> clean_object;



    public:

        Storage();
        template<typename T>
        std::vector<T>& newVector(const std::string name,const T& fillingvalue);

        template<typename T>
        bool deleteVector(const string &name);


        template<typename T>
        std::vector<T>& getVector(const std::string name,bool addifnotexisting);

        template<typename T>
        bool hasColumn(const string& name){
            return vectors_<T>.find(this)!=vectors_<T>.end() && vectors_<T>[this].find(name) !=  vectors_<T>[this].end();
        }

        vector<string> ColumnNames();
        size_t insertItemAt(const string name, size_t offset);
        virtual ~Storage();
    };


#ifdef DMS_INCLUDE_SOURCE
#include "../genericstorage.tcc"
#endif // DMS_INCLUDE_SOURCE
}
#endif //PROJECT_GENERICVECTOR_H
