#include <iostream>
#include "frame/Frame.h"
#include "frame/series.h"
#include <limits>
int main() {
    std::cout << "Hello, World!" << std::endl;
    nsframe::Frame<int,std::string> a;
    std::vector<double> d {0.0,0.1,0.3};
    std::vector<int>  keyrow {0,1,3};

    std::vector<double> d2 {1.0,1.1,1.3};
    std::vector<int>  keyrow2 {3,4,5};

    a.addColumn("one",keyrow,d);
    a.addColumn("two",keyrow2,d2);

    nsframe::Series<int,double> series1(std::vector<int>{0,1,2,3,4},std::vector<double>{0.0,0.1,0.2,0.3,0.4},std::numeric_limits<double>::quiet_NaN());
    nsframe::Series<int,double> series2(std::vector<int>{0,1,2,3,4},std::vector<double>{0.0,1.1,1.2,1.3,1.4},std::numeric_limits<double>::quiet_NaN());

    nsframe::Series<int,double> series_sum = series1+series2;
    for(auto x:series_sum.getAllValues()){

        std::cout<<x.first<<","<<x.second.get()<<std::endl;
    }

    auto column_one = a.getCol("one",std::numeric_limits<double>::quiet_NaN()).getAllValues();
    return 0;
}