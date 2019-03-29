//
// Created by fran on 3/25/19.
//

#ifndef PROJECT_FRAME_H
#define PROJECT_FRAME_H
#include <unordered_map>
#include <string>
#include <vector>

namespace laguna{

    template<typename TKey>
    class Frame {
        std::unordered_map<std::string,std::vector<double>> numeric_cols;
        std::unordered_map<std::string,std::vector<std::string>> string_cols;
        std::vector<TKey> _index;

    public:



    };

}


#endif //PROJECT_FRAME_H
