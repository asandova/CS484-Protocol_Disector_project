#include <string>
#include <string.h>
#include <iostream>
#include "ProtocolDissector.hpp"
#include "rapidjson/document.h"

using namespace std;

int main(int argc, char* argv[]){
    string filename;
    if(argc > 1){
        int i = 1;
        while(i < argc){
            if(strcmp("-JSONFILE", argv[i] ) == 0){
                filename = string(argv[i+1]);
                i++;
            }
            i++;
        }
    }else{
        cout << "Enter JSON File path: ";
        cin >> filename;
    }
    if(filename.empty()){
        printf("No File path entered\nExiting\n");
        return 0;
    }
    printf("Selected file: %s\n", filename.c_str());
    ProtocolDissector test = ProtocolDissector(filename);
    test.toLua();
    return 0;
}