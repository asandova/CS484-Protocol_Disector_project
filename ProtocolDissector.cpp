
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>

#include "ProtocolDissector.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"

using namespace std;

typedef rapidjson::Value JSONVal;
typedef const rapidjson::Value ConstJSONVal;
typedef rapidjson::SizeType JSONSize;
typedef rapidjson::Value::ConstValueIterator ConstValItr;
typedef rapidjson::Value::ValueIterator ValItr;

const string ProtocolDissector::FieldsKey = "Fields";
const string ProtocolDissector::ProtocolNameKey = "ProtocolName";
const string ProtocolDissector::DescriptonKey = "Description";
const string ProtocolDissector::FieldNameKey = "FieldName";


ProtocolDissector::ProtocolDissector(string jsonFile){
    ProtocolData = rapidjson::Document();
    string json = readFile(jsonFile);
    //cout << json << endl;
    if (ProtocolData.Parse( json.c_str() ).HasParseError()){
        printf("JSON Parse ERROR\n");
        exit(-1);
    }
}

string ProtocolDissector::fields(){
    //printf("in fields\n");
    string out = "";
    string ProtoName;
    string ProtoDisc;
    if(ProtocolData.HasMember(ProtocolNameKey.c_str()) ){
        ProtoName = ProtocolData[ProtocolNameKey.c_str()].GetString();
    }else{
        printf("Error: Cannot Find ProtocolName\n");
        exit(-1);
    }
    if(ProtocolData.HasMember(DescriptonKey.c_str()) ){
        ProtoDisc = ProtocolData[DescriptonKey.c_str()].GetString();
    }else{
        printf("Error: Cannot Find Description\n");
        exit(-1);
    }
    if(!ProtocolData.HasMember(FieldsKey.c_str())){
        printf("Error: Cannot Find Fields\n");
        exit(-1);
    }
    out += ProtoName +"_protocol = Proto(\"" + ProtoName + "\", \"" + ProtoDisc + "\")\n\n";
    //add all proto fields
    ConstJSONVal& Fields = ProtocolData[FieldsKey.c_str()];
    if(!Fields.IsArray()){
        printf("Invalid JSON Protocol");
        exit(-1);
    }
    for(ConstValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){
        if(!itr->IsObject() ) {
            break;
        }
        string fieldName;
        string type;
        string base;
        if(getMember(FieldNameKey.c_str(), &fieldName, itr)!= 0){
            printf("Error: Cannot Find FieldName in protocol field\n");
            exit(-1);
        }
        if(getMember("Type", &type, itr)!= 0){
            printf("Error: Cannot Find Type in protocol field\n");
            exit(-1);
        }
        if(getMember("Base", &base, itr)!= 0){
            printf("Error: Cannot Find Base in protocol field\n");
            exit(-1);
        }
        //printf("FieldName: %s, Type: %s, Base: %s\n", fieldName.c_str(),type.c_str(),base.c_str());
        out += fieldName + "_field = ProtoField." + type + "(\"" + ProtoName + "." + fieldName + "\", \"" + fieldName + "\"";
        if(strcmp(type.c_str() , "NONE") != 0){
            out += ",base." + base;
        }
        out += ")\n";
    }

    //loop through and add to fields
    out += "\n" + ProtoName +"_protocol.fields = {\n";
    for(ConstValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){
        if(!itr->IsObject()){
            continue;
        }
        string fieldName;
        if(itr->HasMember(FieldNameKey.c_str())){
            fieldName = (*itr)[FieldNameKey.c_str()].GetString();
        }else{
            exit(-1);
        }
        ConstValItr itr2 = itr;
        ++itr2;
        out += "\t" + fieldName + "_field";
        if(itr2 != Fields.End() ){
            out += ",\n";
        }
    }
    out += "\n}\n";
    return out;
}

string ProtocolDissector::dissector(){
    //printf("Start of Dissector\n");
    string out = "";
    string ProtoName;
    if(ProtocolData.HasMember(ProtocolNameKey.c_str())){
        ProtoName = ProtocolData[ProtocolNameKey.c_str()].GetString();
    }else{
        printf("Error: Cannot find ProtocolName\n");
        exit(-1);
    }
    out += "\nfunction " + ProtoName + "_protocol.dissector(buffer, pinfo, tree)\n";
    out += "\tlength = buffer:len()\n";
    out += "\tif length == 0 then return end\n\n";
    out += "\tpinfo.cols.protocol = " + ProtoName + "_protocol.name\n\n";
    out += "\tlocal subtree = tree:add(" + ProtoName + "_protocol, buffer(),\""+ ProtoName + " Protocol data\")\n";
    
    JSONVal& Fields = ProtocolData[FieldsKey.c_str()];
    if(!Fields.IsArray()){
        printf("Invalid JOSN Protocol\n");
        exit(-1);
    }
    for(ValItr itr = Fields.Begin(); itr != Fields.End(); ++itr){
        if(!itr->IsObject()){
            continue;
        }
        string endian;
        string fieldName;
        string offset;
        string size;
        string bitByte;
        string bitoffset;
        string bitlength;
        bool hasCodeDisc = false;
        string CodeDisc;
        if(getMember("Big/LittleEndien", &endian, itr)!= 0){
            printf("Warning: Cannot Find Big/LittleEndien in current protocol field\nSkipping..\n");
            continue;
        }
        if(getMember(FieldNameKey.c_str(), &fieldName, itr)!= 0){
            printf("Warning: Cannot find FieidName in current protocol field\nSkipping..\n");
            continue;
        }
        if(getMember("Size", &size, itr)!= 0){
            printf("Warning: Cannot find Size in current Protocol field\nSkipping..\n");
            continue;
        }
        if(getMember("Offset", &offset, itr)!= 0){
            printf("Warning: Cannot find Offset in current Protocol Field\nSkipping..\n");
            continue;
        }
        if(getMember("Bit/Byte", &bitByte, itr) != 0 ){
            printf("Warning: Connot find Bit/Byte in current Protocol Field\nSkipping..\n");
        }else{
            if(!itr->HasMember("BitRange")){
                printf("Warning: Cannot fing BirRange in current Protocol Fields\n");
            }else{
                bitoffset = (*itr)["BitRange"][0].GetString();
                bitlength = (*itr)["BitRange"][1].GetString();
            }
        }

        if(itr->HasMember( "HasCodeDisc" ) ){
            string temp = (*itr)["HasCodeDisc"].GetString();
            if( strcmp("True", temp.c_str()) == 0 ){
                hasCodeDisc = true;
            }
        }

        if(hasCodeDisc){
            out += "\n\tlocal " + fieldName + "_value = ";
            
            if(strcmp(bitByte.c_str(), "Bit") == 0){
                out += "buffer(" +offset+ ", "+ size +"):bitfield( " + bitoffset + ", " + bitlength + " )"; 
            }else{
                out += "buffer( " + offset + ", " + size + " )";
                if( strcmp(endian.c_str(), "Little") == 0){
                    out += "le_";
                }
                out += ":uint()\n";
            }
            
            out += "\n\tlocal " + fieldName + "_description = get_" + fieldName + "( " +fieldName +"_value )\n";
        }

        out += "\tsubtree:add";
        if( strcmp(endian.c_str(), "Little") == 0){
            out += "_le";
        }

        out += "(" + fieldName + "_field, ";
        out += "buffer(" + offset + ", ";
        if(strcmp("END", size.c_str() ) == 0){
            out += "length - " + offset;
        }else{
            out += size;
        }
        out += ")";
        if(strcmp(bitByte.c_str(), "Bit") == 0){
            out += ":bitfield( " + bitoffset + ", " + bitlength +")";
        }
        out += ")";
        if(hasCodeDisc){
            out += ":append_text(\" (\" .. " + fieldName + "_description .. \")\")\n";  
        }
        out += "\n"; 
    }
    out += "end\n\n";
    //printf("End of Dissector\n");
    return out;
}

string ProtocolDissector::dissectorTablePort(){
    //printf("Start of dissector Table Port\n");
    string out = "";
    string port;
    string ProtoName;
    string IPProto;
    if(getMember( ProtocolNameKey.c_str(), &ProtoName, ProtocolData ) != 0){
        printf("Error: Cannot Find ProtocolName\n");
        exit(-1);
    }

    if(getMember("TCP/UDP", &port, ProtocolData) == 0){
        if(strcmp(port.c_str(), "TCP") == 0 ){
            out += "local tcp_port = DissectorTable.get(\"tcp.port\")\n";
            out += "tcp_port:add(";
        }else if(strcmp(port.c_str(), "UDP") == 0){
            out += "local udp_port = DissectorTable.get(\"udp.port\")\n";
            out += "udp_port:add(";
        }
        out += ProtocolData["Port"].GetString();
        out += ", " + ProtoName + "_protocol)\n";
    }
    if(getMember("IPProtocol", &IPProto, ProtocolData) == 0){
        out += "local ip_proto = DissectorTable.get(\"ip.proto\")\n";
        out += "ip_proto:add(";
        out += ProtocolData["IPProtocol"].GetString();
        out += ", " + ProtoName + "_protocol)\n";
    }
    //printf("End of dissector Table Port\n");
    return out; 
}

string ProtocolDissector::CodeDisc(){
    //printf("in Code Disc\n");
    vector<string>FieldsWithCode = vector<string>();
    string out = "";

    if(!ProtocolData.HasMember(FieldsKey.c_str())){
        printf("Error: Cannot Find Fields");
        exit(-1);
    }
    ConstJSONVal& Fields = ProtocolData[FieldsKey.c_str()];

    if(!Fields.IsArray()){
        printf("Invalid JOSN Protocol\n");
        exit(-1);
    }

    for( ConstValItr itr = Fields.Begin(); itr != Fields.End(); ++itr ){
        if(!itr->IsObject()){
            continue;
        }
        if(itr->HasMember("HasCodeDisc")){
            if( strcmp("True", (*itr)["HasCodeDisc"].GetString() ) == 0 ){
                string temp = (*itr)[FieldNameKey.c_str()].GetString();
                FieldsWithCode.push_back(temp);
            }
        }
    }

    for(vector<string>::iterator itr = FieldsWithCode.begin(); itr != FieldsWithCode.end(); ++itr){
        if( !ProtocolData.HasMember(itr->c_str() ) ){
            printf("Warning: %s does not have a corrosponding Code Discriptor\n", itr->c_str());
            continue;
        }
        ConstJSONVal& CodeDisc = ProtocolData[itr->c_str()];
        if(!CodeDisc.IsArray()){
            printf("Error: Invalid JSON Protocol\n");
            exit(-1);
        }
        string name = *itr;
        out += "function get_" + name + "( "+ name +" )\n";
        out += "\tlocal " + name + "_name = \"Unknown\"\n\n";

        for(ConstValItr CodeItr = CodeDisc.Begin(); CodeItr != CodeDisc.End(); ++CodeItr){
            string type;
            string Desc;
            string RangeLow;
            string RangeHigh;
            if( getMember("Type", &type, CodeItr)!= 0 ){

                exit(-1);
            }
            if( getMember("Description", &Desc, CodeItr)!= 0 ){
                exit(-1);
            }
            if(!CodeItr->HasMember("Range")){
                exit(-1);
            }
            ConstJSONVal& range = (*CodeItr)["Range"];
            if(!range.IsArray()){
                exit(-1);
            }
            RangeLow = range[0].GetString();
            RangeHigh = range[1].GetString();

            if(CodeItr == CodeDisc.Begin()){
                out += "\tif ";
            }else{
                out += "\telseif ";
            }
            out += "( " +  name + " >= " + RangeLow + " and " + name + " < " + RangeHigh + " ) then " + name + "_name = \"" + Desc + "\"\n";
        }
        out += "\telse " + name + "_name = \"Unknown\" end\n";
        out += "\treturn " + name + "_name\nend\n";
    }
    return out;
}

bool ProtocolDissector::hasCodeDisc(){
    //printf("in hasCodeDisc\n");
    bool hasDisc = false;

    if(!ProtocolData.HasMember(FieldsKey.c_str()) ){
        printf("Cannot find Fields\n");
        return hasDisc;
    }
    ConstJSONVal& Fields = ProtocolData[FieldsKey.c_str()];
    if(!Fields.IsArray()){
        printf("Invalid JOSN Protocol\n");
        exit(-1);
    }
    for( ConstValItr itr = Fields.Begin(); itr != Fields.End(); ++itr ){
        if(!itr->IsObject()){
            continue;
        }
        if(itr->HasMember("HasCodeDisc") ){
            if( strcmp("True", (*itr)["HasCodeDisc"].GetString() ) == 0 ){
                hasDisc = true;
            }
        }

    }
    return hasDisc;
}

int ProtocolDissector::getMember(const char* key, string* value, const JSONVal& json){

    if( json.HasMember(key) ){
        *value = json[key].GetString();
    }else{
        printf("Warning: Cannot find %s in JSON Protocol\n", key);
        return -1;
    }
    return 0;
}
int ProtocolDissector::getMember(const char* key, string* value, ConstValItr json){

    if( json->HasMember(key) ){
        *value = (*json)[key].GetString();
    }else{
        printf("Warning: Cannot find %s in JSON Protocol\n", key);
        return -1;
    }
    return 0;
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
    //printf("in toLua\n");
    ofstream out;
    if(!ProtocolData.HasMember("ProtocolName")){
        printf("Invalid JSON Protocol: Missing ProtocolName\n");
        exit(-1);
    }
    string luaName = ProtocolData[ProtocolNameKey.c_str()].GetString();
    luaName += + "_dissector.lua";
    out.open(luaName, ofstream::out);

    //Protocol init & fields
    out << fields();
    //dissector functon
    out << dissector();
    if(hasCodeDisc()){
        out << CodeDisc();
    }
    //dissector table and port
    out << dissectorTablePort();
    //close file
    out.close();
    printf("JSON to Lua: Complete\n");
}