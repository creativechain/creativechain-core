/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   addr-db.cpp
 * Author: ander
 * 
 * Created on 7 de octubre de 2018, 21:05
 */

#include "addressindex-db.h"
#include "uint256.h"
#include "validation.h"
#include "chainparams.h"
#include "wallet/wallet.h"
#include "script/standard.h"
#include "base58.h"
#include "util.h"
#include "script/standard.h"

#include <stdint.h>
#include <map>
#include <vector>

#include <boost/filesystem.hpp>

using namespace std;

CAddressDB::CAddressDB() {
    boost::filesystem::path path = GetDataDir(false);
    boost::filesystem::path addressPath = path / "address";
    boost::filesystem::create_directories(path);
    db = new CDBWrapper(addressPath, DEFAULT_CACHE_SIZE, false, false, false);
}

COutputStore::COutputStore() : address(""), value(0) {}

COutputStore::COutputStore(string addr, CAmount val) : address(addr), value(val) {}

void CAddressDB::PutOutput(const string hash, const int32_t index, const string address, const CAmount value) {
    string key = hash + ":" + to_string(index);
    
    COutputStore store(address, value);
    db->Write(key, store);
}

COutputStore CAddressDB::GetOutput(const string hash, const int32_t index) {
    string key = hash + ":" + to_string(index);
    COutputStore store;
    db->Read(key, store);
    return store;
}

void CAddressDB::UpdateBalance(const CBlock& block) {
    for (int x = 0; x < block.vtx.size(); x++) {
        const CTransactionRef& tx = block.vtx[x];
        UpdateBalance(*tx);
    }
}

void CAddressDB::UpdateBalance(const CTransaction& tx) {
    //Update inputs first
    
    if (!tx.IsCoinBase()) {
        for (CTxIn input : tx.vin) {
            uint256 hash = input.prevout.hash;
            const int32_t n = input.prevout.n;
            
            COutputStore outVal = GetOutput(hash.ToString(), n);
            
            CBitcoinAddress address(outVal.GetAddress());
            CAmount addressVal;
            
            if (db->Exists(address.ToString())) {
                db->Read(address.ToString(), addressVal);
            } else {
                addressVal = 0;
            }
            
            addressVal -= outVal.GetValue();
            db->Write(address.ToString(), addressVal, true);
        }
    }

    //Update outputs
    
    int index = 0;
    for (CTxOut output : tx.vout) {
        
        if (output.nValue > 0) {
            vector<CTxDestination> destinies;
            txnouttype type;
            int required;
            
            if (ExtractDestinations(output.scriptPubKey, type, destinies, required)) {
                
                //Exclude OP_RETURN outputs
                if (type != TX_NULL_DATA) {
                    CBitcoinAddress address(destinies[0]);
                    PutOutput(tx.GetHash().ToString(), index, address.ToString(), output.nValue);
                    CAmount addressVal;
                    
                    if (db->Exists(address.ToString())) {
                        db->Read(address.ToString(), addressVal);
                    } else {
                        addressVal = 0;
                    }
                    
                    addressVal += output.nValue;
                    db->Write(address.ToString(), addressVal, true);
                 }else {
                    LogPrintf("%s: Unable to extract destination from output %s:%d\n", __func__, tx.GetHash().ToString().c_str(), index);
                }
            } 
        }
        
        index++;
    }
}

void CAddressDB::GetBalances(vector<string> addresses, map<CBitcoinAddress, CAmount>& result) {

    for (string a : addresses) {
        CAmount val;
        if (db->Exists(a)) {
            db->Read(a, val);
        } else {
            val = 0;
        }
        
        result[a] = val;
    }
}



