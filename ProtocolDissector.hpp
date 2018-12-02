#ifndef PROTOCOLDISSECTOR_HPP
#define PROTOCOLDISSECTOR_HPP

#include <string>
#include <iostream>
#include <fstream>

#include "rapidjson/document.h"

using namespace std;

typedef rapidjson::Document JSON;

class ProtocolDissector{

    private:
        JSON ProtocolData;        

        string readFile(string filename);
        string fields();
        string dissector();
        string dissectorTablePort();
    public:
        ProtocolDissector(string jsonFile);
        void toLua();
        //JSON getJson()const;
};

#endif