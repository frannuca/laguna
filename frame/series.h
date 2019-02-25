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

namespace nsframe{
    template<typename TKey,typename T>
    class Series{
        std::vector<void*> m_data;
        std::vector<TKey> m_keys;
        std::pair<int,int> locate(const TKey& key);
        Series<TKey, T> applyScalar(std::function<T(const T&)> op);
        Series<TKey, T> getAlignedSeries(const Series<TKey,T>& that,std::function<T(const T&,const T&)> op);
        T& toTypedData(void *x);
        void* toVoidData(const T& x);
        void* getvoidcopy(void* x);
        T fillvalue;
    public:
        Series(){}
        Series(const std::vector<TKey>& index,const std::vector<T>& data,const T& fillvalue);
        Series(const Series<TKey,T>& that);
        Series(const Series<TKey,T>&& that);
        Series(std::vector<TKey>& keys,std::vector<void*>& data,double fillvalue);
        size_t size(){return m_keys.size();}

        std::vector<std::reference_wrapper<T>> getValues();
        const std::vector<TKey>& getKeys();
        std::vector<std::pair<TKey,std::reference_wrapper<T>>> getAllValues();
        T& operator[](const TKey& key);
        Series<TKey,T> operator+(const Series<TKey,T>& that);
        Series<TKey,T> operator-(const Series<TKey,T>& that);
        Series<TKey,T> operator*(const Series<TKey,T>& that);
        Series<TKey,T> operator/(const Series<TKey,T>& that);
        Series<TKey,T> operator*(const T& l);
        Series<TKey,T> operator/(const T& l);
    };

    template<typename TKey, typename T>
    std::pair<int, int> Series<TKey, T>::locate(const TKey &key) {
        int nlow=0;
        int nhigh = m_data.size();
        int nmid = 0.0;

        int maxcount = m_data.size();
        int counter = 0;
        while(++counter<maxcount && (nhigh-nlow)>1){
            nmid = int((nlow+nhigh)/2.0);
            TKey& xmid = m_keys[nmid];
            TKey& xlow = m_keys[nlow];
            TKey& xhigh = m_keys[nhigh];

            if(xmid<key){
                nlow=nmid;
            }
            else if (xmid>key){
                nhigh=xmid;
            }
            else{
                nlow=xmid;
                nhigh=xmid;
                break;
            }

        }

        return std::make_pair(nlow,nhigh);
    }

    template<typename TKey, typename T>
    T &Series<TKey, T>::toTypedData(void *x) {
        return *(static_cast<T*>(x));
    }

    template<typename TKey, typename T>
    T &Series<TKey, T>::operator[](const TKey &key) {
        int nlow,nhigh;
        std::tie(nlow,nhigh) = locate(key);
        if(nlow != nhigh) throw new std::invalid_argument("Invalid key");

        return toTypedData(m_data[nlow]);
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(const std::vector<TKey> &index, const std::vector<T> &data,const T& fillvalue_):m_keys(index),fillvalue(fillvalue_) {
        std::transform(data.begin(),data.end(),std::back_inserter(m_data),[](const T& x){return static_cast<void*>(new T(x));});
    }



    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::getAlignedSeries(const Series<TKey, T> &that,std::function<T(const T&,const T&)> op) {

        int i=0;
        int j = 0;
        std::vector<TKey> newrow;
        std::vector<T> newdata;

        while(i<m_keys.size() || j<that.m_keys.size()){
            if(i>= m_keys.size()){
                newrow.insert(newrow.end(),that.m_keys.begin()+j,that.m_keys.end());
                for(auto piter=that.m_data.begin()+j;piter != that.m_data.end();++piter){
                    if(*piter== nullptr){
                        newdata.push_back(fillvalue);
                    }
                    else{
                        newdata.push_back(toTypedData(*piter));
                    }
                }

                break;
            }
            else if(j>=that.m_keys.size()){
                newrow.insert(newrow.end(),m_keys.begin()+j,m_keys.end());
                for(auto piter=m_data.begin()+j;piter != m_data.end();++piter){
                    if(*piter== nullptr){
                        newdata.push_back(fillvalue);
                    }
                    else{
                        newdata.push_back(toTypedData(*piter));
                    }
                }
                break;
            }
            else{
                if(m_keys[i]<that.m_keys[j]){
                    newrow.push_back(m_keys[i]);
                    newdata.push_back(toTypedData(m_data[i]));
                    ++i;
                }
                else if(m_keys[i]>that.m_keys[j]){
                    newrow.push_back(that.m_keys[j]);
                    newdata.push_back(toTypedData(that.m_data[j]));
                    j++;
                }
                else{
                    newrow.push_back(m_keys[i]);
                    if(m_data[i] == nullptr && that.m_data[j]==nullptr){
                        newdata.push_back(fillvalue);
                    }
                    else if(m_data[i] != nullptr && that.m_data[j]!= nullptr){
                        T a = toTypedData(m_data[i]);
                        T b = toTypedData(that.m_data[j]);
                        newdata.push_back(op(a,b));
                    }
                    else if(m_data[i] != nullptr){
                        newdata.push_back(toTypedData(m_data[i]));
                    }
                    else{
                        newdata.push_back(toTypedData(that.m_data[j]));
                    }
                    i++;
                    j++;
                }
            }
        }

        return Series(newrow,newdata,fillvalue);
    }

    template<typename TKey, typename T>
    void *Series<TKey, T>::getvoidcopy(void *x) {
        size_t sizeofblock = sizeof(T);
        void* cpy = malloc(sizeofblock);
        std::memcpy(cpy,x,sizeofblock);
        return cpy;
    }

    template<typename TKey, typename T>
    void *Series<TKey, T>::toVoidData(const T &x) {
        return static_cast<void*>(new T(x));
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(const Series<TKey, T> &that) {
        m_keys = that.m_keys;
        std::transform(that.m_data.begin(),that.m_data.end(),std::back_inserter(m_data),[this](const void*& p){return getvoidcopy(p);});
        fillvalue = that.fillvalue;
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(const Series<TKey, T> &&that) {
        m_keys = std::move(that.m_keys);
        m_data = std::move(that.m_data);
        fillvalue = that.fillvalue;
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::applyScalar(std::function<T(const T &)> op) {
        std::vector<T> data;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(data),[this,op](void*& p){return op(toTypedData(p));});
        return Series(m_keys,data);
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator+(const Series<TKey, T> &that) {
        return getAlignedSeries(that,[](auto a, auto b){return a+b;});
    }


    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator-(const Series<TKey, T> &that) {
        return getAlignedSeries(that,[](auto a, auto b){return a-b;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator*(const Series<TKey, T> &that) {
        return getAlignedSeries(that,[](auto a, auto b){return a*b;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator/(const Series<TKey, T> &that) {
        return getAlignedSeries(that,[](auto a, auto b){return a/b;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator*(const T &l) {
        return applyScalar([&l](auto a){return a*l;});
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator/(const T &l) {
        return applyScalar([&l](auto a){return a/l;});
    }

    template<typename TKey, typename T>
    std::vector<std::reference_wrapper<T>> Series<TKey, T>::getValues() {
        std::vector<std::reference_wrapper<T>> x;
        std::transform(m_data.begin(),m_data.end(),std::back_inserter(x),[this](void*& p){return std::reference_wrapper<T>(toTypedData(p));});
        return x;
    }

    template<typename TKey, typename T>
    const std::vector<TKey> &Series<TKey, T>::getKeys() {
        return m_keys;
    }

    template<typename TKey, typename T>
    std::vector<std::pair<TKey, std::reference_wrapper<T>>> Series<TKey, T>::getAllValues() {
        std::vector<std::pair<TKey, std::reference_wrapper<T>>> data;
        for(int i=0;i<m_keys.size();++i){
            data.push_back(std::make_pair(m_keys[i],std::reference_wrapper<T>(toTypedData(m_data[i]))));
        }
        return data;
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(std::vector<TKey> &keys, std::vector<void *> &data, double fillvalue_) {
        m_keys=keys;
        m_data=data;

        fillvalue=fillvalue_;
        for(auto& x:m_data){
            if(x == nullptr){
                x = static_cast<void*>(new T(fillvalue));
            }
        }
    }



}

#endif //LAGUNA_SERIES_H
