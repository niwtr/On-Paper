#ifndef ARCHIVER_H
#define ARCHIVER_H
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#define JSON_FILE_NOEXIST 111
using std::string;using std::ifstream;
using std::make_pair;using std::cout;using std::endl;
using namespace nlohmann;
namespace on_paper {

struct archiv_conf {
    string name;
    string type;
    string codenum;
    string pdf_path;
    string conf_path;
    string source_path;
};
using _Hash = std::unordered_map<string,archiv_conf>;
class Archiver {

private:
    string _prefix;
    _Hash _hash;

public:
    Archiver(){}
    inline void set_prefix(string prefix){ _prefix = prefix; }

    bool query(string codenum, archiv_conf& ac);

    void read_storage_file(string json_file_path);

};


}

#endif // ARCHIVER_H
