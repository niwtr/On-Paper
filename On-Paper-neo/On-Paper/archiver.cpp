#include "archiver.h"

bool on_paper::Archiver::query(std::string codenum, on_paper::archiv_conf &ac) {
    auto _aciter=_hash.find(codenum);
    if(_aciter==_hash.end())
        return false;
    else
        ac=_aciter->second;
    return true;
}

void on_paper::Archiver::read_storage_file(std::string json_file_path){


    ifstream ifs(json_file_path);
    if(not ifs.is_open())exit(JSON_FILE_NOEXIST);


    int suc_count=0;
    try{
        while(true){
            json j; ifs>>j;
            string name=j["name"].get<string>(), type=j["type"].get<string>(), code=j["code"].get<string>(),
                    image=_prefix+j["image"].get<string>(),conf=_prefix+j["conf"].get<string>();
            archiv_conf ac{name,type,code,image,conf};
            cout<<image<<endl;
            _hash.insert(make_pair(code, ac));
            suc_count++;
        }
    } catch(...){ cout<<"Successfully loaded: "<<suc_count<<endl; }

}
