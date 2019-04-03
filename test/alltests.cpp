//
// Created by venus on 28/02/2019.
//
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ALLTESTS
#define _CRTDBG_MAP_ALLOC
#define BOOST_TEST_DETECT_MEMORY_LEAK 1

#include <cmath>
#include <boost/test/included/unit_test.hpp> // this work
#include <utility>
#include <random>
#include <chrono>
#include "../dataframe/include/Frame.h"
#include "genericstorage.h"

using namespace std;
namespace tt = boost::test_tools;
#include <iostream>
#include <limits>


using namespace laguna;

BOOST_AUTO_TEST_CASE(ALLTESTS)
{

    Frame<int> frame;
    vector<pair<int,double>> series1 = {move(pair<int,double>(1,1.5)),move(pair<int,double>(2,2.5)),move(pair<int,double>(3,3.5))};
    vector<pair<int,double>> series2 = {move(pair<int,double>(10,11.5)),move(pair<int,double>(12,12.5)),move(pair<int,double>(13,13.5))};
    frame.withColumn<double>("col1",series1,numeric_limits<double>::quiet_NaN());

    auto& col_1 = frame.col<double>("col1");
    auto& index = frame.Index();

    frame.withColumn("col2",series2,numeric_limits<double>::quiet_NaN());

    auto& col_1b = frame.col<double>("col1");
    auto& col_2b = frame.col<double>("col2");
    auto& indexb = frame.Index();


    //testing column generation
    Storage gv;

    cout<<"creating a double column"<<endl;
    vector<string> key = {"a","b"};
    vector<double>& v_d = gv.newVector<double>("col1_double",0.0);

    ostringstream os;
    cout<<os.str()<<endl;
    cout<<"creating a string column"<<endl;
    vector<string>& v_s = gv.newVector<std::string>("col2_string","NA");


    //adding data to double and string columns
    v_d.push_back(1.0);
    v_s.push_back("inf");

    //accessing columns
    auto& xvd = gv.getVector<double>("col1_double",true);
    auto& vxs = gv.getVector<string>("col2_string",true);

    BOOST_CHECK_EQUAL(xvd[0],1.0);
    BOOST_CHECK_EQUAL(vxs[0],"inf");

    BOOST_CHECK(gv.hasColumn<double>("col1_double"));
    BOOST_CHECK(gv.hasColumn<string>("col2_string"));

    BOOST_CHECK(gv.deleteVector<string>("col2_string"));

    BOOST_CHECK(!gv.hasColumn<string>("col2_string"));

    BOOST_CHECK_EXCEPTION(gv.getVector<string>("col2_string",false), invalid_argument, [](invalid_argument const& ex){return true;} );
    std::cout<<"finished";
}
