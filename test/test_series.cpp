//
// Created by venus on 11/03/2019.
//

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SERIESTESTS
#define _CRTDBG_MAP_ALLOC
#define BOOST_TEST_DETECT_MEMORY_LEAK 1

#include <cmath>
#include <boost/test/included/unit_test.hpp> // this work
#include <utility>
#include <random>
#include <chrono>

#include "../data/series.h"
#include <boost/date_time.hpp>
#include <boost/date_time/gregorian_calendar.hpp>

using namespace std;
namespace tt = boost::test_tools;
#include <iostream>
#include <limits>


using namespace laguna;

Series<int,double> GetIntSeries(){
    Series<int,double> series1;
    series1.withData(std::vector<int>{0, 1, 2, 3, 4, 5},
                     std::vector<double>{100.0, 101.0, 102.0, 103.0, 104.0, 105.0});
    series1.withData(std::vector<int>{6, 7, 8, 9, 10, 11},
                     std::vector<double>{1100.0, 1101.0, 1102.0, 1103.0, 1104.0, 1105.0});
    return series1;
}

using namespace boost::gregorian;
using namespace boost::posix_time;

date d0(2019,Jan,01);

Series<date,double> GetDateSeries(){
    Series<date,double> series1;

    series1.withData(std::vector<date>{d0, d0+days(1), d0+days(2), d0+days(3), d0+days(4), d0+days(5)},
                     std::vector<double>{100.0, 101.0, 102.0, 103.0, 104.0, 105.0});
    series1.withData(std::vector<date>{d0+days(6), d0+days(7), d0+days(8), d0+days(9), d0+days(10), d0+days(11)},
                     std::vector<double>{1100.0, 1101.0, 1102.0, 1103.0, 1104.0, 1105.0});
    return series1;
}
BOOST_AUTO_TEST_CASE(SERIESTESTS)
{
    {
        std::cout<<"Starting series<int,double> ctors"<<std::endl;
        auto series1 = GetIntSeries();

        BOOST_CHECK_CLOSE(100.0, series1[0], 1e-3);
        BOOST_CHECK_CLOSE(1105.0, series1[11], 1e-3);

        std::cout<<"Checking  series<int,double>::operator[](int)"<<std::endl;
        series1[11] = 42.0;
        BOOST_CHECK_CLOSE(42.0, series1[11], 1e-3);
    }
    {
        std::cout<<"Starting series<date,double> ctors"<<std::endl;
        auto series1 = GetDateSeries();

        BOOST_CHECK_CLOSE(100.0, series1[d0], 1e-3);
        BOOST_CHECK_CLOSE(1105.0, series1[d0+days(11)], 1e-3);

        std::cout<<"Checking  series<date,double>::operator[](int)"<<std::endl;
        series1[d0+days(11)] = 42.0;
        BOOST_CHECK_CLOSE(42.0, series1[d0+days(11)], 1e-3);
        std::cout<<"OK. access modified values"<<std::endl;
    }

    {
        std::cout<<"Starting series<date,double> ctor"<<std::endl;
        auto series1 = GetDateSeries();
        auto iscritical = [](std::exception) { return true; };

        std::cout<<"deleting date key"<<std::endl;
        series1.deleteKey(d0);
        BOOST_REQUIRE_EXCEPTION(series1[d0], std::exception, iscritical);

        std::cout<<"OK. access exception captured"<<std::endl;
    }
    {
        std::cout<<"Checking operations on series"<<std::endl;
        auto series1 = GetDateSeries();
        std::cout<<"Checking (+) ..."<<std::endl;
        auto seriesx2 = series1 + series1;
        BOOST_CHECK_CLOSE(series1[d0+days(3)]*2.0,seriesx2[d0+days(3)],0.01);

        //back to the original series values:
        std::cout<<"Checking (-) ..."<<std::endl;
        auto seriesorig = seriesx2 - series1;
        BOOST_CHECK_CLOSE(seriesorig[d0+days(3)],series1[d0+days(3)],0.01);

        std::cout<<"Checking unary (-) ..."<<std::endl;
        auto seriesorig_x = seriesx2 + (-series1);
        BOOST_CHECK_CLOSE(seriesorig_x[d0+days(3)],series1[d0+days(3)],0.01);

        std::cout<<"Checking (*) ..."<<std::endl;
        auto series_prod = series1 * series1;
        BOOST_CHECK_CLOSE(series_prod[d0+days(3)],series1[d0+days(3)]*series1[d0+days(3)],0.01);

        std::cout<<"Checking (/) ..."<<std::endl;
        auto series_div = series1 / series1;
        BOOST_CHECK_CLOSE(series_div[d0+days(3)],1.0,0.01);

        std::cout<<"Checking (l * series * l) ..."<<std::endl;
        auto series_scalar_prod = 10.0 * series1 *10.0;
        BOOST_CHECK_CLOSE(series_scalar_prod[d0+days(3)],series1[d0+days(3)]*100.0,0.01);

        std::cout<<"Checking (l / series ) ..."<<std::endl;
        auto series_scalar_div_1 = 1.0 / series1 ;
        BOOST_CHECK_CLOSE(series_scalar_div_1[d0+days(3)],1.0/series1[d0+days(3)],0.01);

        std::cout<<"Checking ( series/10.0 ) ..."<<std::endl;
        auto series_scalar_div_2 = series1 / 10.0;
        BOOST_CHECK_CLOSE(series_scalar_div_2[d0+days(3)],series1[d0+days(3)]/10.0,0.01);

        std::cout<<"Checking  map values pow(x,2)..."<<std::endl;
        auto seriespow2 = series1.mapValues([](const double& x){return x*x;});
        BOOST_CHECK_CLOSE(seriespow2[d0+days(3)],series1[d0+days(3)]*series1[d0+days(3)],0.01);

        std::cout<<"Checking  map values pow(x,2)..."<<std::endl;
        auto seriessqrt2 = seriespow2.mapValues([](const double& x){return std::sqrt(x);});
        BOOST_CHECK_CLOSE(seriessqrt2[d0+days(3)],series1[d0+days(3)],0.01);

        ptime epoch = time_from_string("1970-01-01 00:00:00.000");


        std::cout<<"Checking  map keys int -> date  sqrt(pow(_,2))..."<<std::endl;
        auto seriespow2_key =  series1.mapKeys<long long>([&epoch](const date& x){
            return (ptime(x)-epoch).total_seconds();
        });

        auto seriessqrt2_key = seriespow2_key.mapKeys<long long>([](const int& x){return std::sqrt(x);});

        BOOST_CHECK_CLOSE(std::sqrt(seriespow2_key.keys()[3]),seriessqrt2_key.keys()[3],0.01);
    }
    std::cout<<"finished";
}