//
// Created by venus on 28/02/2019.
//
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FRAMETESTS
#define _CRTDBG_MAP_ALLOC
#define BOOST_TEST_DETECT_MEMORY_LEAK 1

#include <cmath>
#include <boost/test/included/unit_test.hpp> // this work
#include <utility>
#include <random>
#include <chrono>

#include "../data/series.h"

using namespace std;
namespace tt = boost::test_tools;
#include <iostream>
#include <limits>
#include "../data/converters.h"


bool correctMessage(const std::invalid_argument& ex)
{
//    BOOST_CHECK_EQUAL(ex.what(), std::string("some error message"));
    return true;
}
BOOST_AUTO_TEST_CASE(FRAMETESTS)
{
    {
        laguna::Series<int,double> series1;
        series1.withData(std::vector<int> {0,1,2,3,4,5}, std::vector<double>{100.0,101.0,102.0,103.0,104.0,105.0},
                         laguna::Constants::ClearAndAdd);
        series1.withData(std::vector<int> {6,7,8,9,10,11}, std::vector<double>{1100.0,1101.0,1102.0,1103.0,1104.0,1105.0},
                         laguna::Constants::WithReplacement);


        auto zz = series1[0];
        BOOST_CHECK_CLOSE(100.0,series1[0],1e-3);
        BOOST_CHECK_CLOSE(1105.0,series1[11],1e-3);

        series1[11] = 42.0;
        BOOST_CHECK_CLOSE(42.0,series1[11],1e-3);

    }
    {
        laguna::Series<int,double> series1;
        series1.withData(std::vector<int> {0,1,2,3,4,5}, std::vector<double>{100.0,101.0,102.0,103.0,104.0,105.0},
                         laguna::Constants::ClearAndAdd);
        series1.withData(std::vector<int> {6,7,8,9,10,11},
                std::vector<double>{1100.0,1101.0,1102.0,1103.0,1104.0,1105.0},
                         laguna::Constants::WithReplacement);


        series1[3]=33.0;
        std::vector<std::pair<int,double>> aa = series1.getValues();
        aa[0] = std::make_pair(0,0.0);
        auto entryor33 = std::find_if(aa.begin(),aa.end(),[](const std::pair<int,double>&p)
        {
            bool y = p.first==3;
            return y;
        });
        if(entryor33 == aa.end()){
            throw "number not found";
        }
        std::cout<<"aa="<<entryor33->first<<","<<entryor33->second<<std::endl;
        BOOST_CHECK_CLOSE(series1[3],entryor33->second,0.01);

        auto iscritical = [](std::exception e){return true;};

        series1.deleteEntry(0);
        BOOST_REQUIRE_EXCEPTION(series1[0],std::exception,iscritical);

        auto seriesx2 = series1 + series1;
        double v1 = series1[3];
        double v2 = seriesx2[3];
        BOOST_CHECK_CLOSE(series1[3]*2.0,seriesx2[3],0.01);

        //back to the original series values:
        auto seriesorig = seriesx2 - series1;
        BOOST_CHECK_CLOSE(seriesorig[3],series1[3],0.01);

        auto seriesorig_x = seriesx2 + (-series1);
        BOOST_CHECK_CLOSE(seriesorig_x[3],series1[3],0.01);


        auto seriespow2 = series1.mapValues([](const double& x){return x*x;});
        auto seriessqrt2 = seriespow2.mapValues([](const double& x){return std::sqrt(x);});

        BOOST_CHECK_CLOSE(seriespow2[3],series1[3]*series1[3],0.01);
        BOOST_CHECK_CLOSE(seriessqrt2[3],series1[3],0.01);

/*
        auto seriespow2_key =  series1.mapValues([](const int& x){return x*x;});
        auto seriessqrt2_key = seriespow2.mapValues([](const int& x){return (int)std::sqrt(x);});
        BOOST_CHECK_CLOSE(seriespow2_key.Keys()[3],series1[3]*series1[3],0.01);
        BOOST_CHECK_CLOSE(seriessqrt2_key.Keys()[3],series1[3],0.01);
*/

    }
    std::cout<<"finished";

}
