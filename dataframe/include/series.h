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
#include <numeric>
#include "seriesStorage.h"

namespace laguna {

    template<typename A, typename  B> class Series;

    template<typename A,typename B>
    Series<A,B> operator*(const B& l, const Series<A,B>& series);

    template<typename A,typename B>
    Series<A,B> operator*(const Series<A,B>& series,const B& l);

    template<typename A,typename B>
    Series<A,B> operator/(const B& l, const Series<A,B>& series);

    template<typename A,typename B>
    Series<A,B> operator/( const Series<A,B>& series,const B& l);

    template<typename TKey, typename T>
    class Series : protected SeriesStorage<TKey,T>{
    protected:
        Series<TKey,T> applyBinaryOp(const Series<TKey, T> &a, std::function<T(const T &, const T &)> fop) const;
        Series<TKey,T> applyUnaryOp(std::function<T(const T &)> fop) const;
        std::vector<std::pair<TKey,void*>> valuesAllRaw(){return this->SeriesStorage<TKey, T>::valuesAllRaw();}
        std::vector<void*> valuesRaw(){return this->SeriesStorage<TKey, T>::valuesRaw();}
    public:
        Series(){};
        Series(const Series<TKey, T> &that);
        Series(Series<TKey, T> &&that);
        ~Series();
        Series <TKey, T> &withData(const std::vector<std::pair<TKey, T>> &data,Constants::AddDataType adddata=Constants::ErrorOnDuplication);

        Series <TKey, T> &withData(const std::vector<TKey> &x, const std::vector<T> &y,Constants::AddDataType adddata=Constants::ErrorOnDuplication);


        T& operator[](const TKey& key);


        /** operators **/
        Series<TKey,T> operator+(const Series<TKey,T>& that) const;
        Series<TKey,T> operator*(const Series<TKey,T>& that) const;
        Series<TKey,T> operator/(const Series<TKey,T>& that) const;
        Series<TKey,T> operator-() const;
        Series<TKey,T> operator-(const Series<TKey,T>& that) const {return *this + (-that);}

        template<typename A,typename B>
        friend Series<A,B> operator*(const B& l, const Series<A,B>& series);

        template<typename A,typename B>
        friend Series<A,B> operator*(const Series<A,B>& series,const B& l);

        template<typename A,typename B>
        friend Series<A,B> operator/(const B& l, const Series<A,B>& series);

        template<typename A,typename B>
        friend Series<A,B> operator/( const Series<A,B>& series,const B& l);

        /** mapping **/
        Series<TKey,T> mapValues(std::function<T(const T&)> pred) const;
        template<typename A>
        Series<A,T> mapKeys(std::function<A(const TKey&)> pred) const;

        size_t size(){return this->SeriesStorage<TKey,T>::m_data.size();}

        T sum();
        std::vector<std::pair<TKey,T>> valuesAll(){return this->SeriesStorage<TKey, T>::valuesAll();}
        std::vector<T> values(){return this->SeriesStorage<TKey, T>::values();}

        std::vector<TKey> keys(){return this->SeriesStorage<TKey,T>::getKeys();}
        void deleteKey(const TKey& key){this->SeriesStorage<TKey,T>::deleteItem(key);};
    };

    template<typename TKey,typename T>
    Series<TKey,T> operator*(const T& l, const Series<TKey,T>& series);

    template<typename TKey,typename T>
    Series<TKey,T> operator/(const T& l, const Series<TKey,T>& series);

    template<typename TKey, typename T>
    Series<TKey, T> operator*(const T &l, const Series<TKey, T> &series) {
        return series.applyUnaryOp([&l](const T &x) { return l * x; });
    }

    template<typename TKey, typename T>
    Series<TKey, T> operator/(const T &l, const Series<TKey, T> &series) {
        return series.applyUnaryOp([&l](const T &x) { return l / x; });
    }


    template<typename TKey, typename T>
    Series<TKey, T>::Series(const Series<TKey, T> &that) {
        auto& m_data = this->SeriesStorage<TKey,T>::m_data;
        std::transform(that.m_data.begin(),that.m_data.end(),std::back_inserter(m_data),
                [](const std::pair<TKey,void*>& p){return std::make_pair(p.first, getVoidCopy<T>(p.second)); });
    }


    template<typename TKey, typename T>
    Series<TKey, T>::~Series() {

    }

    template<typename TKey, typename T>
    Series <TKey, T> &Series<TKey, T>::withData(const std::vector<std::pair<TKey, T>> &data,Constants::AddDataType adddata) {
        this->SeriesStorage<TKey,T>::addData(data,adddata);
        return *this;
    }


    template<typename TKey, typename T>
    Series <TKey, T> &
    Series<TKey, T>::withData(const std::vector<TKey> &x, const std::vector<T> &y,Constants::AddDataType adddata) {

        if(x.size() != y.size()){
            throw std::invalid_argument("provided key and value vectors do not match sizes");
        }
        std::vector<std::pair<TKey,T>> newv;
        for(size_t i=0;i<x.size();++i){
            newv.push_back(std::make_pair(x[i],y[i]));
        }

        return withData(newv,adddata);
    }

    template<typename TKey, typename T>
    T &Series<TKey, T>::operator[](const TKey& key) {
        int nlow, nhigh;
        std::tie(nlow, nhigh) = this->Locator<TKey>::locate(key);
        if (nlow == nhigh && nlow >= 0) {
            return voidToTypeReference<T>(this->SeriesStorage<TKey,T>::m_data[nlow].second);
        }
        std::ostringstream os;
        os << "Invalid Key " << key;
        throw std::invalid_argument( os.str().c_str());
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator+(const Series<TKey, T> &that) const {
        return applyBinaryOp(that, [](const T &a, const T &b) { return a + b; });
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator-() const {
        return applyUnaryOp([](const T &x) { return -x; });
    }


    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::mapValues(std::function<T(const T &)> pred) const {
        auto pairs = this->SeriesStorage<TKey, T>::valuesAll();
        for(auto& x:pairs){
            x.second=pred(x.second);
        }
        return Series().withData(pairs,Constants::ErrorOnDuplication);
    }

    template<typename TKey, typename T>
    template<typename A>
    Series<A, T> Series<TKey, T>::mapKeys(std::function<A(const TKey &)> pred) const {
        auto pairs = this->SeriesStorage<TKey, T>::valuesAll();
        std::vector<std::pair<A,T>> newpairs;
        std::transform(pairs.begin(),pairs.end(),
                std::back_inserter(newpairs),
                [pred](const std::pair<TKey,T>& x){
            return std::make_pair(pred(x.first),x.second);
        });

        return Series<A, T>().withData(newpairs);
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator*(const Series<TKey, T> &that) const {
        return applyBinaryOp(that, [](const T &a, const T &b) { return a * b; });
    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::operator/(const Series<TKey, T> &that) const {
        return applyBinaryOp(that, [](const T &a, const T &b) { return a / b; });
    }


    template<typename TKey, typename T>
    T Series<TKey, T>::sum() {
        auto values = this->SeriesStorage<TKey, T>::valuesAll();
        return std::accumulate(values.begin(),values.end(),T{},[](T& acc,const std::pair<TKey,T>& x){return acc+x.second;});
    }

    template<typename TKey, typename T>
    Series<TKey, T>::Series(Series<TKey, T> &&that) {
        this->SeriesStorage<TKey,T>::m_data = std::move(that.m_data);
    }

    template<typename TKey, typename T>
    Series<TKey, T>
    Series<TKey, T>::applyBinaryOp(const Series<TKey, T> &a, std::function<T(const T &, const T &)> fop) const {
            return Series<TKey, T>().withData(
                    this->SeriesStorage<TKey, T>::binaryOp(a, fop));

    }

    template<typename TKey, typename T>
    Series<TKey, T> Series<TKey, T>::applyUnaryOp(std::function<T(const T &)> fop) const {
        return Series().withData(this->SeriesStorage<TKey, T>::unaryOp(fop));
    }

    template<typename A, typename B>
    Series<A, B> operator*(const Series<A, B> &series, const B &l) {
        return series.applyUnaryOp([&l](const B &x) { return l * x; });
    }

    template<typename A, typename B>
    Series<A, B> operator/(const Series<A, B> &series, const B &l) {
        return series.applyUnaryOp([&l](const B &x) { return  x/l; });
    }


}

#endif //LAGUNA_SERIES_H
