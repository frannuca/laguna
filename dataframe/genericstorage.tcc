//implementations

template<typename T>
std::vector<T>& Storage::NewColumn(const std::string name) {
    auto iter = vectors_<T>.find(this);
    if(iter == vectors_<T>.end()){
        map<string,vector<T>> obj;
        obj[name]=vector<T>();
        // auto iter = vectors_<T>.emplace (this,obj);


        map<string,function<string(void)>> f_print;

        f_print[name]=[&obj,&name]() {
            ostringstream os;
            vector<T> &m = obj[name];


            os << name <<",";
            for (auto item:m) {
                os << item << ",";
            }

            string line = os.str();
            line.substr(0, line.size() - 1) + "\n";
            return line;
        };
        printer_f<T>.emplace(this,f_print);
//

        map<string,function<void(void)>> f;
        f[name]=[this,&name](){
            vectors_<T>.erase(this);
        };

        clean_f<T>.emplace(this,f);
    }
    return vectors_<T>[this][name];
}

template<typename T>
vector<T> &Storage::Column(const std::string name){
    auto iter = vectors_<T>.find(this);
    if(iter != vectors_<T>.end() && iter->second.find(name) != iter->second.end()){

        return vectors_<T>[this][name];
    }
    throw invalid_argument("invalid column name "+name);
}

Storage::~Storage() {

}

template<typename T>
bool Storage::DeleteColumn(const string &name) {
    auto iter = clean_f<T>.find(this);
    if(iter!=clean_f<T>.end()){
        iter->second[name]();
    }
    return iter!=clean_f<T>.end();
}

template<typename T>
std::unordered_map<Storage*, map<string,vector<T>>> Storage::vectors_;

template<typename T>
std::unordered_map<Storage*, map<string,std::function<void(void)>>> Storage::clean_f;

template<typename T>
std::unordered_map<Storage *, map<string,std::function<std::string(void)>>> Storage::printer_f;