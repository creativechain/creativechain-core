/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   addr-db.h
 * Author: ander
 *
 * Created on 7 de octubre de 2018, 21:05
 */

#ifndef ADDR_DB_H
#define ADDR_DB_H

#include "primitives/transaction.h"
#include "dbwrapper.h"
#include "primitives/block.h"
#include "base58.h"
#include "serialize.h"


using namespace std;

static const size_t DEFAULT_CACHE_SIZE = 80 * 1024 * 1024; //80 MB

class COutputStore {
private:
    string address;
    CAmount value;
    
public:
    COutputStore();
    
    COutputStore(string addr, CAmount val);
    
    ADD_SERIALIZE_METHODS;
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(address);
        READWRITE(value);
    }
        
    string GetAddress() const {
        //string addr(address);
        return address;
    }
    
    CAmount GetValue() const {
        return value;
    }
};

class CAddressDB {
private:
    CDBWrapper* db;
    
public:
    CAddressDB();
    
    void PutOutput(const string hash, const int32_t index, const string address, const CAmount value);
    
    COutputStore GetOutput(const string hash, const int32_t index);
    
    void UpdateBalance(const CBlock &block);
    
    void UpdateBalance(const CTransaction &tx);
    
    void GetBalances(vector<string> addresses, map<CBitcoinAddress, CAmount> &result);
};


#endif /* ADDR_DB_H */

