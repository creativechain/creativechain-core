/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "rpc/server.h"
#include "primitives/transaction.h"
#include "amount.h"
#include "utilstrencodings.h"
#include "validation.h"

#include <univalue.h>

#include <string>
#include <vector>
#include <map>

using namespace std;

UniValue IndexesToJSON(map<CBitcoinAddress, CAmount> &addresses) {
    UniValue json(UniValue::VOBJ);
    
    CBitcoinAddress a;
    for (map<CBitcoinAddress, CAmount>::iterator it = addresses.begin(); it != addresses.end(); ++it) {
        json.pushKV(it->first.ToString(), ValueFromAmount(it->second));
    }
    
    return json;
}

UniValue getbalances(const JSONRPCRequest& request) {
    
    if (request.fHelp || request.params.size() != 1) {
        throw runtime_error(
                "getbalances [\"address1\", \"address2\" ...]\n"
                
                "\nReturn the balances of address provided.\n"
                
                "\nArguments:\n"
                "1. \"addresses\"       (array ,required) Array of Creativecoin addresses strings.\n"
                
                "\nResult:\n"
                "[\"address1\": 00000000, \"addresss2\": 00000000, ...]\n"
                
                "\nExamples:\n"
                + HelpExampleCli("getbalances", "[\"address\"]")
        );
    }
    
    string jsonString = request.params[0].getValStr();
    UniValue uniVal(UniValue::VARR);
    uniVal.read(jsonString);
    
    vector<string> addresses;
    addresses.reserve(uniVal.size());
    
    for (int x = 0; x < uniVal.size(); x++) {
        UniValue addressVal = uniVal[x];
        string addr = addressVal.get_str();
        addresses.push_back(addr);
    }
    
    map<CBitcoinAddress, CAmount> addressMap;
    addressDb->GetBalances(addresses, addressMap);
    
    return IndexesToJSON(addressMap);
}

static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         okSafe argNames
  //  --------------------- ------------------------  -----------------------  ------ ----------
    { "addressindex",       "getbalances",            &getbalances,      true, {} },
};

void RegisterAddressIndexRPCCommands(CRPCTable &tableRPC) {
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++) {
        tableRPC.appendCommand(commands[vcidx].name, &commands[vcidx]);
    }
        
}