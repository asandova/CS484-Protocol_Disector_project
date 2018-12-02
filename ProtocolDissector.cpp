
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "ProtocolDissector.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"

using namespace std;

typedef rapidjson::Value JSONVal;
typedef rapidjson::SizeType JSONSize;
typedef rapidjson::Value::ConstValueIterator ConstValItr;
typedef rapidjson::Value::ValueIterator ValItr;

ProtocolDissector::ProtocolDissector(string jsonFile){
    ProtocolData = rapidjson::Document();
    string json = readFile(jsonFile);
    cout << json << endl;
    if (ProtocolData.Parse( json.c_str() ).HasParseError()){
        printf("JSON Parse ERROR\n");
        exit(-1);
    }
}

string ProtocolDissector::fields(){
    printf("in fields\n");
    string out = "";
    //try{
        //protocol init
        printf("\tProtocol init\n");
        out += ProtocolData["ProtocolName"].GetString();
        printf("\tgetting Protocol Name\n");
        out += "_protocol = Proto(\"";
        out += ProtocolData["ProtocolName"].GetString();
        printf("\tgetting Protocol Name\n");
        out += "\", \"";
        out += ProtocolData["Description"].GetString();
        printf("\tGetting protocol Description\n");
        out += "\")\n";

        //add all proto fields
        JSONVal& Fields = ProtocolData["Fields"];
        if(!Fields.IsArray()){
            printf("Invalid JOSN Protocol");
            exit(-1);
        }
        printf("\tInitialize Fields\n");
        for(ValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){

            if(!itr->IsObject()){break;}
            JSONVal Field = itr->GetObject();
            if(!Field.IsObject()){
                break;
            }
            printf("FieldName: %s\n", Field["FieldName"].GetString());
            out += Field["FieldName"].GetString();
            out += "_field = ProtoField.";
            out += Field["Type"].GetString();
            printf("\tType: %s\n", Field["Type"].GetString());
            out += "(\"";
            out += ProtocolData["ProtocolName"].GetString();
            printf("\tProtocolName: %s\n",ProtocolData["ProtocolName"].GetString());
            out += ".";
            out += Field["FieldName"].GetString();
            out += "\", \"";
            out += Field["FieldName"].GetString();
            out += "base.";
            out += Field["Base"].GetString();
            printf("\tBase: %s\n", Field["Base"].GetString());
            out += ")\n";
        }

        //loop through and add to fields
        printf("\tadd fields to protocol fields");
        out += ProtocolData["ProtocolName"].GetString();
        out += "_protocol.fields = {";
        for(ValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){
            JSONVal Field = itr->GetObject();
            out += Field["FieldName"].GetString();
            out += "_field";
            //need to put a guard to not place coma at end of list
            out += ",";
        }
        out += "}\n";
    //}catch(){
    //    printf("invalid JSON protocol Fields");
   //     exit(-1);
    //}
    return out;
}

string ProtocolDissector::dissector(){
    string out = "";
    string ProtoName = ProtocolData["ProtocolNme"].GetString();
    out += "functon " + ProtoName + "_protocol.dissector(buffer, pinfo, tree)\n";
    out += "\tlength = buffer::len()\n";
    out += "\tif length = 0 then return end\n\n";
    out += "\tpinfo.cols.Protocol = " + ProtoName + "_protocol.name\n\n";
    out += "local subtree = tree::add(" + ProtoName + "_protocol, buffer(),\""+ ProtoName + " Protocol data\")\n";
    
    JSONVal& Fields = ProtocolData["Fields"];
    if(!Fields.IsArray()){
        printf("Invalid JOSN Protocol");
        exit(-1);
    }
    for(ValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){
        if(!itr->IsObject()){break;}
        JSONVal Field = itr->GetObject();
        string endian = Field["Big/LittleEndien"].GetString();
        string fieldName = Field["FieldName"].GetString();
        out += "\tsubtree:add";
        if( strcmp(endian.c_str(), "Little") == 0){
            out += "_le";
        }
        out += "(" + fieldName + "_field, buffer(";
        out += Field["Offsect"].GetString();
        out += ", ";
        out += Field["Size"].GetString();
        out += "))\n";
    }
    out += "end\n";
    return out;
}

string ProtocolDissector::dissectorTablePort(){
    string out = "";
    string port = ProtocolData["TCP/UDP"].GetString();
    string protoName = ProtocolData["ProtocolName"].GetString();
    if(strcmp(port.c_str(), "TCP") == 0 ){
        out += "local tcp_port = dissectorTable.get(\"udp.port\")\n";
        out += "tcp_port:add(";
    }else if(strcmp(port.c_str(), "UDP") == 0){
        out += "local udp_port = dissectorTable.get(\"udp.port\")\n";
        out += "udp_port:add(";
    }
    out += ProtocolData["Port"].GetString();
    out += ", " + protoName + "_protocol)\n";
    return out; 
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

void ProtocolDissector::toLua(){
    printf("in toLua\n");
    ofstream out;
    string luaName = ProtocolData["ProtocolName"].GetString();
    luaName += + "_dissector.lua";
    out.open(luaName, ofstream::out);

    //Protocol init & fields
    out << fields();
    //dissector functon
    out << dissector();
    //dissector table and port
    out << dissectorTablePort();
    //close file
    out.close();
}