//
// Created by venus on 10/03/2019.
//

#ifndef PROJECT_SERIESSTORAGE_H
#define PROJECT_SERIESSTORAGE_H
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include <exception>
#include <stdlib.h>
#include <cstring>
#include <memory>
#include <sstream>
#include "converters.h"
#include "locatortrait.h"
#include "missingtraits.h"

namespace laguna {

    template<typename TKey, typename T>
    class SeriesStorage: public  Locator<TKey>,public MissingTrait<T>{

    protected:
        std::vector<std::pair<TKey, void*>> m_data;
        void deleteData();
        void addData(const std::vector<std::pair<TKey, T>> &data,
                     Constants::AddDataType adddata=Constants::ErrorOnDuplication);
        std::vector<std::pair<TKey,T>> binaryOp(const SeriesStorage<TKey,T>& a,std::function<T (const T&, const T&)> fop) const;
        virtual std::vector<std::pair<TKey,T>> unaryOp(std::function<T(const T &)> fop) const;
        std::vector<std::pair<TKey, void*>> valuesAllRaw() const;
        std::vector<void*> valuesRaw() const;
    public:
        void sortSeries();
        void deleteItem(const TKey& key);
        std::vector<std::pair<TKey, T>> valuesAll() const;
        std::vector<T> values() const;
        virtual const std::vector <TKey> getKeys() const;
        virtual ~SeriesStorage();
    };

    template<typename TKey, typename T>
    void SeriesStorage<TKey, T>::sortSeries() {
        std::sort(
                m_data.begin(),
                m_data.end(),
                [](const std::pair<TKey,void*>& a,
                   const std::pair<TKey,void*>& b){
                    return a.first<b.first;
                });
    }

    template<typename TKey, typename T>
    void SeriesStorage<TKey, T>::deleteData() {
        for(auto x:m_data){
            delete (T*)(x.second);
        }
    }

    template<typename TKey, typename T>
    SeriesStorage<TKey, T>::~SeriesStorage() {
        deleteData();
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;
        m_data.clear();
    }

    template<typename TKey, typename T>
    void SeriesStorage<TKey, T>::addData(const std::vector<std::pair<TKey, T>> &data, Constants::AddDataType adddata) {
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;
        if(!(adddata==Constants::ClearAndAdd)){
            size_t i=0;
            size_t j=0;
            std::vector<std::pair<TKey, void*>> newdata;
            while(i<m_data.size() && j<data.size()){
                if(m_data[i].first<data[j].first){
                    newdata.push_back(std::make_pair(m_data[i].first, getVoidCopy<T>(m_data[i].second)));
                    ++i;
                }
                else if(m_data[i].first>data[j].first){
                    newdata.push_back(std::make_pair(data[j].first,TypeReferenceToCopyVoid(data[j].second)));
                    ++j;
                }
                else if (adddata==Constants::WithReplacement){
                    newdata.push_back(std::make_pair(data[j].first,TypeReferenceToCopyVoid(data[j].second)));
                    ++i;
                    ++j;
                }
                else if(adddata==Constants::ErrorOnDuplication){
                    std::ostringstream msg;
                    msg<<"Duplicated key "<< data[j].first<< " provided in call to Series withData function";
                    throw std::invalid_argument(msg.str().c_str());
                }
            }
            if(i<m_data.size()){
                for(;i<m_data.size();++i){
                    newdata.push_back(std::make_pair(m_data[i].first, getVoidCopy<T>(m_data[i].second)));
                }
            }
            else if(j<data.size()){
                for(;j<data.size();++j)
                    newdata.push_back(std::make_pair(data[j].first,TypeReferenceToCopyVoid(data[j].second)));
            }
            deleteData();
            m_data=std::move(newdata);
        }
        else{
            std::transform(data.begin(),data.end(),std::back_inserter(m_data),[](const std::pair<TKey,T>& p){
                return std::make_pair(p.first,TypeReferenceToCopyVoid(p.second));
            });
        }
        sortSeries();
    }

    template<typename TKey, typename T>
    void SeriesStorage<TKey, T>::deleteItem(const TKey &key) {
        int nlow, nhigh;
        std::tie(nlow, nhigh) = this->Locator<TKey>::locate(key);
        if (nlow == nhigh && nlow >= 0) {
            delete (T*) m_data[nlow].second;
            m_data.erase(m_data.begin()+nlow);
        }
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, T>> SeriesStorage<TKey, T>::binaryOp(const SeriesStorage<TKey, T> &a,
                                                                     std::function<T(const T &, const T &)> fop) const {
        auto data = a.m_data;

        size_t i = 0;
        size_t j = 0;
        TKey xkey;
        std::vector<std::pair<TKey, T>> newdata;
        while (i < m_data.size() && j < data.size()) {

            T x, y;
            if (m_data[i].first < data[j].first) {
                x = this->MissingTrait<T>::missing;
                y = voidToTypeReference<T>(m_data[i].second);
                xkey=m_data[i].first;
                ++i;
            } else if (m_data[i].first > data[j].first) {
                x = this->MissingTrait<T>::missing;
                y = voidToTypeReference<T>(data[j].second);
                xkey=data[j].first;
                ++j;
            } else {
                x = voidToTypeReference<T>(m_data[j].second);
                y = voidToTypeReference<T>(data[j].second);
                xkey=m_data[i].first;
                ++i;
                ++j;
            }
            newdata.push_back(std::make_pair(xkey, fop(x, y)));
        }
        if (i < m_data.size()) {
            T missing = this->MissingTrait<T>::missing;
            for (; i < m_data.size(); ++i) {
                newdata.push_back(
                        std::make_pair(m_data[i].first, fop(voidToTypeReference<T>(m_data[i].second), missing)));
            }
        } else if (j < data.size()) {
            T missing = this->MissingTrait<T>::missing;
            for (; j < data.size(); ++j)
                newdata.push_back(
                        std::make_pair(m_data[j].first, fop(voidToTypeReference<T>(data[j].second), missing)));
        }

        return newdata;
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, T>> SeriesStorage<TKey, T>::unaryOp(std::function<T(const T &)> fop) const {
        auto series = this->valuesAll();

        for(size_t i=0;i<series.size();++i){
            series[i].second = fop(series[i].second);
        }

        return series;
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, T>> SeriesStorage<TKey, T>::valuesAll() const {
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;

        std::vector<std::pair<TKey, T>> x;
        std::transform(m_data.begin(),
                       m_data.end(),
                       std::back_inserter(x),
                       [](auto& y){return std::make_pair(y.first,voidToTypeReference<T>(y.second));});
        return x;
    }

    template<typename TKey, typename T>
    const std::vector<TKey> SeriesStorage<TKey, T>::getKeys() const {
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;
        std::vector<TKey> keys;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(keys),[](auto& p){return p.first;});
        return keys;
    }

    template<typename TKey, typename T>
    std::vector<T> SeriesStorage<TKey, T>::values() const {
        std::vector<T> x;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(x),[](const std::pair<TKey,void*>* z){return voidToTypeReference(z);});
        return x;
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, void*>> SeriesStorage<TKey, T>::valuesAllRaw() const {
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;

        std::vector<std::pair<TKey, void*>> x;
        std::transform(m_data.begin(),
                       m_data.end(),
                       std::back_inserter(x),
                       [](auto& y){return std::make_pair(y.first,getVoidCopy<T>(y.second));});
        return x;
    }

    template<typename TKey, typename T>
    std::vector<void *> SeriesStorage<TKey, T>::valuesRaw() const {
        std::vector<void*> x;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(x),[](const std::pair<TKey,void*>* z){return getVoidCopy<T>(z);});
        return x;
    }
};
#endif //PROJECT_SERIESSTORAGE_H
