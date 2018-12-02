#include <string>
#include <iostream>
#include "ProtocolDissector.hpp"
#include "rapidjson/document.h"

using namespace std;

int main(){
    ProtocolDissector test = ProtocolDissector("Protocols/RIPv1_Protocol.json");
    test.toLua();
    return 0;
}