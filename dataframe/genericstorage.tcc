//implementations

template<typename T>
std::vector<T>& Storage::newVector(const std::string name,const T& fillingvalue) {
    auto iter = vectors_<T>.find(this);

    if(iter == vectors_<T>.end()){

        map<string,vector<T>> obj;
        obj[name]=vector<T>();
        vectors_<T>[this]=std::move(obj);

        map<string,function<void(void)>> fcleancol;
        fcleancol[name]=[this,name](){
            auto& aa =vectors_<T>[this];
            vectors_<T>[this].erase(name);
        };
        clean_f[this] = std::move(fcleancol);
        clean_object[this] = [this](){
            vectors_<T>.erase(this);
            inserter_f.erase(this);
            clean_f.erase(this);
        };

        map<string,std::function<void(int)>> inserter_obj;
        inserter_obj[name] = [this,name,fillingvalue](int n){
            auto& aa =vectors_<T>[this][name];
            vectors_<T>[this][name].insert(vectors_<T>[this][name].begin()+n,fillingvalue);
        };

        inserter_f[this]=std::move(inserter_obj);
    }

    return vectors_<T>[this][name];
}

template<typename T>
vector<T> &Storage::getVector(const std::string name,bool addifnotexisting){
    auto iter = vectors_<T>.find(this);
    if(iter != vectors_<T>.end() && iter->second.find(name) != iter->second.end()){

        return vectors_<T>[this][name];
    }
    else if(addifnotexisting){
        vectors_<T>[this][name]=std::move(vector<T>());
        return vectors_<T>[this][name];
    }
    else{
        throw invalid_argument("column "+name+ "  is not registered in this vector");
    }
}

Storage::~Storage() {
    clean_object[this]();
    clean_object.erase(this);

}

template<typename T>
bool Storage::deleteVector(const string &name) {
    auto iter = clean_f.find(this);
    if(iter!=clean_f.end()){
        iter->second[name]();
    }
    return iter!=clean_f.end();
}

Storage::Storage() {

}


size_t Storage::insertItemAt(const string name, size_t offset) {
    inserter_f[this][name](offset);
}

vector <string> Storage::ColumnNames() {
    vector<string> cols;
    for(auto x:inserter_f[this]){
        cols.push_back(x.first);
    }
    return std::move(cols);
}


template<typename T>
map<Storage*, map<string,vector<T>>> Storage::vectors_;


map<Storage *, map<string,std::function<void(int)>>> Storage::inserter_f;

map<Storage*, map<string,std::function<void(void)>>> Storage::clean_f;
map<Storage *, function<void(void)>> Storage::clean_object;
