//
// Created by frann on 24/02/2019.
//

#ifndef LAGUNA_FRAME_H
#define LAGUNA_FRAME_H

#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include "series.h"

namespace nsframe{
    template<typename TRow, typename TCol>
    class Frame {
        std::map<TCol,std::vector<void*>> m_data;
        template<typename K, typename V>
        using MapIterator = typename std::map<K,V>::iterator;

        std::vector<TRow> rows_;

    private:
        void alighnRows(const std::vector<TRow> &index0,const TCol& newcol);
        template<typename T>
        Frame<TRow,TCol>& addColumn(TCol name,std::vector<T>& data);

    public:
        Frame() {};

        template<typename T>
        Frame<TRow,TCol>& addColumn(TCol name,std::vector<TRow> index,std::vector<T>& data);
        template<typename T>
        Series<TRow,T> getCol(const TCol& column,const T& fillvalue);

    };

    template<typename TRow, typename TCol>
    template<typename T>
    Frame<TRow,TCol> &Frame<TRow,TCol>::addColumn(TCol name, std::vector<T> &data) {

        std::vector<void*> aux;
        std::transform(data.begin(),data.end(),std::back_inserter(aux),[](auto & x){return static_cast<void*>(new T(x));});
        m_data[name] =std::move(aux);
        return *this;
    }

    template<typename TRow, typename TCol>
    template<typename T>
    Frame<TRow,TCol> &Frame<TRow,TCol>::addColumn(TCol name, std::vector<TRow> index, std::vector<T> &data) {
        if(index.size()!=data.size()){
            throw "Inconsistent index and column sizes";
        }
        if(rows_.empty()){
            rows_ = index;
        }
        addColumn(name,data);
        alighnRows(index,name);

    }

    template<typename TRow, typename TCol>
    void Frame<TRow,TCol>::alighnRows(const std::vector<TRow> &index0,const TCol& newcol) {
        std::vector<TRow> index = index0;
        std::sort(index.begin(),index.end(),[](const TRow& a,const TRow& b){return a<b;});
        int i=0;
        int j = 0;
        std::vector<TRow> newrow;
        std::vector<void*> xaux = m_data[newcol];

        while(i<rows_.size() || j<index.size()){
            if(i>= rows_.size()){
                newrow.push_back(index[j]);
                MapIterator<TCol,std::vector<void*> > iter;
                for(iter=m_data.begin();iter!=m_data.end();++iter){
                    auto a = iter->first;
                    std::vector<void*>& b = iter->second;
                    if(a!=newcol){
                        b.push_back(static_cast<void*>(nullptr));
                    }

                }
                j++;
            }
            else if(j>=index.size()){
                newrow.push_back(rows_[i]);
                xaux.insert(xaux.begin()+j, nullptr);
                ++i;
            }
            else{
                if(rows_[i]<index[j]){
                    newrow.push_back(rows_[i]);
                    xaux.insert(xaux.begin()+j, nullptr);
                    ++i;
                }
                else if(rows_[i]>index[j]){
                    newrow.push_back(index[j]);
                    MapIterator<TCol,std::vector<void*> > iter;
                    for(iter=m_data.begin();iter!=m_data.end();++iter){
                        auto a = iter->first;
                        std::vector<void*>& b = iter->second;
                        b.insert(b.begin()+i, static_cast<void*>(nullptr));
                    }
                    j++;
                }
                else{
                    newrow.push_back(rows_[i]);
                    i++;
                    j++;
                }
            }
        }
        this->rows_ = newrow;
    }

    template<typename TRow, typename TCol>
    template<typename T>
    Series<TRow, T> Frame<TRow,TCol>::getCol(const TCol &column,const T& fillvalue) {
        return Series<TRow,T>(rows_,m_data[column],fillvalue);
    }
}


#endif //LAGUNA_FRAME_H
