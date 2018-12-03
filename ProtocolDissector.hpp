#ifndef PROTOCOLDISSECTOR_HPP
#define PROTOCOLDISSECTOR_HPP

#include <string>
#include <iostream>
#include <fstream>

#include "rapidjson/document.h"

using namespace std;

typedef rapidjson::Document JSON;
typedef rapidjson::Value JSONVal;
typedef rapidjson::Value::ConstValueIterator ConstValItr;

class ProtocolDissector{

    private:
        JSON ProtocolData;        

        string readFile(string filename);
        string fields();
        string dissector();
        string dissectorTablePort();
        string CodeDisc();
        bool hasCodeDisc();
        int getMember(const char* key,string* value, const JSONVal& json);
        int getMember(const char* key,string* value, ConstValItr json);
    public:
        static const string FieldsKey;
        static const string ProtocolNameKey;
        static const string DescriptonKey;
        static const string FieldNameKey;
        
        ProtocolDissector(string jsonFile);
        void toLua();
        //JSON getJson()const;
};

#endif