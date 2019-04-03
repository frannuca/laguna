//
// Created by fran on 3/25/19.
//

#ifndef PROJECT_FRAME_H
#define PROJECT_FRAME_H
#include <unordered_map>
#include <string>
#include <vector>
#include "genericstorage.h"
namespace laguna{

    using namespace std;

    template<typename TKey>
    class Frame: private Storage {

        std::vector<TKey> _index;


        template<typename T>
        void alignVectorWithIndex(const vector<pair<TKey,T>>& series0,vector<TKey>& newindex, vector<T>& newvalues,const T& fillingval,const string& name);
    public:
        template<typename T>
        Frame<TKey>& withColumn(const string& name,const vector<pair<TKey,T>>& series,T missingval);

        template<typename T>
        const vector<T> col(string name);

        const vector<TKey>& Index();

    };


    template<typename TKey>
    template<typename T>
    Frame<TKey> &Frame<TKey>::withColumn(const string &name, const vector<pair<TKey, T>> &series,T missingval) {
        if(this->Storage::hasColumn<T>(name)){
            throw invalid_argument("The provided column name "+name+" already is part of the data frame");
        }
        else{
            vector<T>& values = this->Storage::newVector<T>(name,missingval);
            vector<TKey> keys;

            alignVectorWithIndex(series,keys,values,missingval,name);
            _index=keys;

        }
        return *this;
    }

    template<typename TKey>
    template<typename T>
    void Frame<TKey>::alignVectorWithIndex(const vector<pair<TKey, T>> &series0, vector<TKey> &newindex,
                                           vector<T> &newvalues, const T &fillingval,const string& name) {
        int i=0;
        int j=0;
        int offset=0;
        vector<pair<TKey,T>>  series =series0;

        sort(series.begin(),series.end(),[](auto&a, auto&b){return a.first<b.first;});

        vector<string> columns_all = ColumnNames();
        vector<string> columns;
        copy_if(columns_all.begin(),columns_all.end(),back_inserter(columns),[name](const string& c){return c!=name;});

        while(i<_index.size() && j<series.size()){

            if(_index[i]<series[j].first){
                newindex.push_back(_index[i]);
                newvalues.push_back(fillingval);
                ++i;
            }
            else if(_index[i]>series[j].first){
                newindex.push_back(series[j].first);
                newvalues.push_back(series[j].second);
                for(auto c:columns){this->insertItemAt(c,i+offset++);}
                ++j;
            }
            else{
                newindex.push_back(_index[i]);
                newvalues.push_back(series[j].second);
                ++i;++j;
            }
        }

        for(;i<_index.size();++i){
            newindex.push_back(_index[i]);
            newvalues.push_back(fillingval);
        }


        for(;j<series.size();++j){
            newindex.push_back(series[j].first);
            newvalues.push_back(series[j].second);
            for(auto c:columns){this->insertItemAt(c,newindex.size()-1);}
        }
    }

    template<typename TKey>
    template<typename T>
    const vector<T> Frame<TKey>::col(string name) {
        return getVector<T>(name,false);
    }

    template<typename TKey>
    const vector<TKey> &Frame<TKey>::Index() {
        return _index;
    }

}


#endif //PROJECT_FRAME_H
