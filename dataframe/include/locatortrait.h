//
// Created by venus on 03/03/2019.
//

#ifndef PROJECT_LOCATORTRAIT_H
#define PROJECT_LOCATORTRAIT_H

#include <string>
#include <vector>
#include <utility>
#include <exception>

namespace laguna {


    template<typename TKey>
    class Locator {
    protected:
        virtual const std::vector <TKey> getKeys() const = 0;
    public:

        std::pair<int, int> locate(const TKey &key) {

            auto m_keys = getKeys();

            int nlow = 0;
            int nhigh = m_keys.size();
            int nmid = 0.0;

            int maxcount = m_keys.size();
            int counter = 0;
            while (++counter < maxcount && (nhigh - nlow) > 0) {
                nmid = int((nlow + nhigh) / 2.0);
                TKey &xmid = m_keys[nmid];

                if (xmid < key) {
                    nlow = nmid;
                } else if (xmid > key) {
                    nhigh = nmid;
                } else {
                    nlow = nmid;
                    nhigh = nmid;
                    break;
                }

            }
            if(nlow==nhigh && m_keys[nlow]!=key){
                std::ostringstream msg;
                msg<<"Key not present "<<key;
                throw std::invalid_argument(msg.str().c_str());
            }
            return std::make_pair(nlow, nhigh);
        }
    };
}
#endif //PROJECT_LOCATORTRAIT_H
