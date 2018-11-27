
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "ProtocolDissector.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace std;

ProtocolDissector::ProtocolDissector(string jsonFile){
    ProtocolData = rapidjson::Document();
    string json = readFile(jsonFile);
    cout << json << endl;
    ProtocolData.Parse( json.c_str() );
}

string ProtocolDissector::readFile(string filename ){
    string buffer = "";
    ifstream in;
    in.open(filename, ifstream::in);

    if(in.is_open()){
        char C;
        while(!in.fail()){
            C = in.get();
            buffer+= C;
        }
        buffer.resize(buffer.size()-1);
        in.close();

    }else{
        cout << "Cannot open File: " << filename << endl;
        exit(-1);
    }
    return buffer;
}

void ProtocolDissector::toLua(rapidjson::Document proto){
    ofstream out;


    //out.open("",ofstream::out);
}