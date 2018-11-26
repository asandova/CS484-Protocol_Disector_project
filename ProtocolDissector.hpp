#ifndef PROTOCOLDISSECTOR_HPP
#define PROTOCOLDISSECTOR_HPP

#include <string>
#include <iostream>

#include "include/rapidjson/document.h"

using namespace std;

class ProtocolDissector{

    private:
        rapidjson::Document ProtocolData;        

        string readFile(string filename);
    public:
        ProtocolDissector(string jsonFile);
};
#endif