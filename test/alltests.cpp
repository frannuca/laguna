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

    //testing column generation
    Storage gv;

    cout<<"creating a double column"<<endl;
    auto& v_d = gv.NewColumn<double>("col1_double");

    cout<<"creating a string column"<<endl;
    auto& v_s = gv.NewColumn<std::string>("col2_string");


    //adding data to double and string columns
    v_d.push_back(1.0);
    v_s.push_back("inf");

    //accessing columns
    auto xvd = gv.Column<double>("col1_double");
    auto vxs = gv.Column<string>("col2_string");

    BOOST_CHECK_EQUAL(xvd[0],1.0);
    BOOST_CHECK_EQUAL(vxs[0],"inf");

    BOOST_CHECK(gv.DeleteColumn<string>("col2_string"));

    BOOST_CHECK_EXCEPTION( gv.Column<string>("col2_string"), invalid_argument, [](invalid_argument const& ex){return true;} );
    std::cout<<"finished";
}
