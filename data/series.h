//
// Created by frann on 25/02/2019.
//

#ifndef LAGUNA_SERIES_H
#define LAGUNA_SERIES_H

#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include <exception>
#include <stdlib.h>
#include <cstring>
#include "converters.h"
#include <memory>
#include <sstream>
#include "missingtraits.h"
#include "locatortrait.h"
#include <exception>
#include <expat.h>

namespace laguna {

    template<typename TKey, typename T>
    class Series :
            protected Locator<TKey>,
            protected MissingTrait<T>{
    protected:
        std::vector<std::pair<TKey, void*>> m_data;
        virtual const std::vector <TKey> getKeys();
        void sortSeries();
        void deleteData();
        Series<TKey,T> binaryOp(const Series<TKey,T>& a,std::function<T (const T&, const T&)> fop) const;
        Series<TKey,T> unaryOp(std::function<T(const T &)> fop) const;
    public:

        Series(){};
        Series(const Series<TKey, T> &that);
        Series(Series<TKey, T> &&that);
        ~Series();

        Series<TKey, T> &withData(const std::vector<std::pair<TKey, T>> &data,
                                  Constants::AddDataType adddata=Constants::ErrorOnDuplication);

        Series<TKey, T> &withData(const std::vector<TKey>& x,
                                  const std::vector<T>& y,
                                  Constants::AddDataType adddata=Constants::ErrorOnDuplication);

        Series<TKey,T> &deleteEntry(const TKey& key);

        T& operator[](const TKey& key);

        std::vector<std::pair<TKey, T>> getValues() const;

        Series<TKey,T> operator+(const Series<TKey,T>& that) const;
        Series<TKey,T> operator-() const;

        Series<TKey,T> operator-(const Series<TKey,T>& that) const {return *this + (-that);}
        Series<TKey,T> mapValues(std::function<T(const T&)> pred) const;
        Series<TKey,T> mapKeys(std::function<T(const TKey&)> pred) const;
    };

    template<typename TKey, typename T>
    const std::vector<TKey> Series<TKey, T>::getKeys() {
        std::vector<TKey> keys;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(keys),[](auto& p){return p.first;});
        return keys;
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(const Series<TKey, T> &that) {
        std::transform(that.m_data.begin(),that.m_data.end(),std::back_inserter(m_data),
                [](const std::pair<TKey,void*>& p){return std::make_pair(p.first, getVoidCopy<T>(p.second)); });
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(Series<TKey, T> &&that) {
        m_data = std::move(that.m_data);
    }

    template<typename TKey, typename T>
    Series<TKey, T>::~Series() {
        deleteData();
        m_data.clear();
    }

    template<typename TKey, typename T>
    Series<TKey, T> &Series<TKey, T>::withData(const std::vector<std::pair<TKey, T>> &data, Constants::AddDataType adddata) {

        if(!(adddata==Constants::ClearAndAdd)){
            int i=0;
            int j=0;
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
        return *this;
    }

    template<typename TKey, typename T>
    void Series<TKey, T>::sortSeries() {
        std::sort(
                m_data.begin(),
                m_data.end(),
                [](const std::pair<TKey,void*>& a,
                   const std::pair<TKey,void*>& b){
            return a.first<b.first;
        });
    }

    template<typename TKey, typename T>
    Series<TKey, T> &
    Series<TKey, T>::withData(const std::vector<TKey> &x, const std::vector<T> &y, Constants::AddDataType adddata) {

        if(x.size() != y.size()){
            throw std::invalid_argument("provided key and value vectors do not match sizes");
        }
        std::vector<std::pair<TKey,T>> newv;
        for(int i=0;i<x.size();++i){
            newv.push_back(std::make_pair(x[i],y[i]));
        }

        return withData(newv,adddata);
    }

    template<typename TKey, typename T>
    T &Series<TKey, T>::operator[](const TKey& key) {
        int nlow, nhigh;
        std::tie(nlow, nhigh) = this->Locator<TKey>::locate(key);
        if (nlow == nhigh && nlow >= 0) {
            return voidToTypeReference<T>(m_data[nlow].second);
        }
        std::ostringstream os;
        os << "Invalid Key " << key;
        throw std::invalid_argument( os.str().c_str());
    }
    template<typename TKey, typename T>
    Series<TKey, T> &Series<TKey, T>::deleteEntry(const TKey& key) {
        int nlow, nhigh;
        std::tie(nlow, nhigh) = this->Locator<TKey>::locate(key);
        if (nlow == nhigh && nlow >= 0) {
            delete (T*) m_data[nlow].second;
            m_data.erase(m_data.begin()+nlow);
        }
        return *this;
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, T>> Series<TKey, T>::getValues() const {
        std::vector<std::pair<TKey, T>> x;
        std::transform(m_data.begin(),
                m_data.end(),
                std::back_inserter(x),
                [](auto& y){return std::make_pair(y.first,voidToTypeReference<T>(y.second));});
        return x;
    }

    template<typename TKey, typename T>
    void Series<TKey, T>::deleteData() {
        for(auto x:m_data){
            delete (T*)(x.second);
        }
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::binaryOp(const Series<TKey, T> &b,
                                              std::function<T (const T&, const T&)> fop) const {

        auto data = b.m_data;

        int i = 0;
        int j = 0;
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

        return Series<TKey,T>().withData(newdata, Constants::ErrorOnDuplication);

    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator+(const Series<TKey, T> &that) const {
        return binaryOp(that,[](const T& a, const T& b){return a+b;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator-() const {
        return unaryOp([](const T& x){return -x;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::unaryOp(std::function<T(const T &)> fop) const {
        auto series = this->getValues();

        for(int i=0;i<series.size();++i){
            series[i].second = fop(series[i].second);
        }

        return Series<TKey,T>().withData(series);
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::mapValues(std::function<T(const T &)> pred) const {
        auto pairs = getValues();
        for(auto& x:pairs){
            x.second=pred(x.second);
        }
        return Series().withData(pairs);
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::mapKeys(std::function<T(const TKey &)> pred) const {
        auto pairs = getValues();
        for(auto& x:pairs){
            x.first=pred(x.first);
        }
        return Series().withData(pairs);
    }

}

#endif //LAGUNA_SERIES_H
