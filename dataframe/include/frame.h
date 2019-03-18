//
// Created by venus on 12/03/2019.
//

#ifndef PROJECT_FRAME_H
#define PROJECT_FRAME_H
#include <vector>
#include "series.h"
#include <map>

namespace laguna {
    template<typename TRow, typename TCol>
    class Frame{
    protected:
        std::vector<TRow> _index;
        std::vector<std::pair<TCol,std::vector<void*>>>  m_data;
        std::map<TCol,std::string> column_type;
        std::map<TCol,void*> column_missing;
        std::map<TCol,std::function<void(void*)>> column_deleter;

    public:
        Frame(){}
        ~Frame();

        template<typename T>
        Frame<TRow,TCol>& withColumn(const TCol& name,const Series<TRow,T>& series);
    };

    template<typename TRow, typename TCol>
    Frame<TRow, TCol>::~Frame() {
        for(size_t i=0;i<m_data.size();++i){
            for(size_t j=0;j<m_data[i].second.size();++j){
                column_deleter[m_data[i].first](m_data[i].second[j]);
            }
        }
    }

    template<typename TRow, typename TCol>
    template<typename T>
    Frame<TRow, TCol>& Frame<TRow, TCol>::withColumn(const TCol &name, const Series<TRow, T> &series) {

        T missingx = series.missing;

        if(_index.empty()){

            _index = series.getKeys();
            m_data.push_back(std::make_pair(name, series.valuesRaw()));
            T aux;


            column_type[name]=std::type_info(aux).name();
            column_missing[name] = [&missingx](){return TypeReferenceToCopyVoid(missingx);};
            column_deleter[name] = [](void* p){delete (T*)(p);};

            return *this;
        }
        else{
            size_t  n = _index.size();

            auto otherkeys = series.getKeys();
            auto othervalues = series.values();

            std::vector<TRow> newindex;

            size_t  m = series.size();
            size_t i = 0;
            size_t j = 0;

            while(i<n && j<m)
            {
                if(_index[i]< otherkeys[j]){
                    othervalues.insert(othervalues.begin()+newindex.size(), series.missing);
                    newindex.push_back(_index[i]);
                    ++i;
                }
                else if(_index[i]>otherkeys[j]){
                    for(const std::pair<TCol,std::vector<void*>>& kv:m_data){
                        kv.second.insert(kv.second.begin()+newindex.size(), column_missing[name]());
                    }
                    newindex.push_back(otherkeys[j]);
                    ++j;
                }
                else{
                    ++i;
                    ++j;
                    newindex.push_back(_index[i]);
                }
            }

            for(int k=i;k<n;++k){
                newindex.push_back(_index[k]);
                othervalues.push_back(series.missing);
            }

            for(int k=j;k<m;++k){
                newindex.push_back(otherkeys[k]);
                for(const std::pair<TCol,std::vector<void*>>& kv:m_data){
                    kv.second.push_back(column_missing[kv.first]());
                }
            }

            _index = std::move(newindex);
            m_data.push_back(std::make_pair(name,othervalues));

            column_type[name]    = std::type_info(missingx).name();
            column_missing[name] = [&missingx](){return TypeReferenceToCopyVoid(missingx);};
            column_deleter[name] = [](void* p){delete (T*)(p);};

            return *this;
        }
    }
}
#endif //PROJECT_FRAME_H
