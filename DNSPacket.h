#ifndef DNSPACKET_H
#define DNSPACKET_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <bitset>
#include <arpa/inet.h>

#include "StringUtilities.h"
#include "Record.hpp"
#include "ExtendedRecord.hpp"

class DNSPacket {
public:
    DNSPacket(const std::string &, unsigned short aID = 0);             //Construct a request packet with the domain name (rawName)
    DNSPacket(const char *, const size_t);      //Construct a response packet with the raw data (data)
    DNSPacket(const DNSPacket &);               //Copy constructor

    ~DNSPacket(void) {
        if(data != NULL) {
            free(data);
            data = NULL;
        }
    }

    void Print(void);                           //Print the packet data
    char * GetData(void);                       //Convert the packet into a byte array for transmission
    size_t Size(void);                          //Size of the packet in bytes
    void SwapName(const std::string &);
    void UnsetRecursionFlag();
    void UnsetZFlags();

    void removeAnswerSection();
    void setQuestionSection(const std::vector<QuestionRecord> aQuestionRecords);

    //Getters...
    unsigned short GetID(void) { return id; }
    unsigned short GetFlags(void) { return flags; }
    unsigned short GetQuestionCount(void) { return qdcount; }
    unsigned short GetAnswerCount(void) { return ancount; }
    unsigned short GetNameServerCount(void) { return nscount; }
    unsigned short GetAdditionalRecordCount(void) { return arcount; }
    const std::string & GetDomain(void) const { return questions[0].GetRawName(); }
    const std::vector<QuestionRecord> & GetQuestionSection(void) const { return questions; }
    const std::vector<AnswerRecord> & GetAnswerSection(void) const { return answers; }
    const std::vector<NameServerRecord> & GetNameServerSection(void) const { return nameServers; }
    const std::vector<AdditionalRecord> & GetAdditionalSection(void) const { return additionals; }

    void SetID(unsigned short nid) { this->id = nid; }

private:
    //bool invalid;
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
    std::vector<QuestionRecord> questions;
    std::vector<AnswerRecord> answers;
    std::vector<NameServerRecord> nameServers;
    std::vector<AdditionalRecord> additionals;

    size_t dataLength;
    char * data;
};

#endif//DNSPACKET_H
