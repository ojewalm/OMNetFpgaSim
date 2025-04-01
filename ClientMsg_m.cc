//
// Generated file, do not edit! Created by opp_msgtool 6.0 from ClientMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ClientMsg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(ClientMsg)

ClientMsg::ClientMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ClientMsg::ClientMsg(const ClientMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ClientMsg::~ClientMsg()
{
}

ClientMsg& ClientMsg::operator=(const ClientMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ClientMsg::copy(const ClientMsg& other)
{
    this->streamId = other.streamId;
    this->type = other.type;
    this->size = other.size;
    this->source = other.source;
    this->destination = other.destination;
    this->seqNum = other.seqNum;
    this->totNumPackets = other.totNumPackets;
    this->protocol = other.protocol;
    this->sendTime = other.sendTime;
    this->byteLength = other.byteLength;
}

void ClientMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->streamId);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->size);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->seqNum);
    doParsimPacking(b,this->totNumPackets);
    doParsimPacking(b,this->protocol);
    doParsimPacking(b,this->sendTime);
    doParsimPacking(b,this->byteLength);
}

void ClientMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->streamId);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->size);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->seqNum);
    doParsimUnpacking(b,this->totNumPackets);
    doParsimUnpacking(b,this->protocol);
    doParsimUnpacking(b,this->sendTime);
    doParsimUnpacking(b,this->byteLength);
}

int ClientMsg::getStreamId() const
{
    return this->streamId;
}

void ClientMsg::setStreamId(int streamId)
{
    this->streamId = streamId;
}

int ClientMsg::getType() const
{
    return this->type;
}

void ClientMsg::setType(int type)
{
    this->type = type;
}

int ClientMsg::getSize() const
{
    return this->size;
}

void ClientMsg::setSize(int size)
{
    this->size = size;
}

int ClientMsg::getSource() const
{
    return this->source;
}

void ClientMsg::setSource(int source)
{
    this->source = source;
}

int ClientMsg::getDestination() const
{
    return this->destination;
}

void ClientMsg::setDestination(int destination)
{
    this->destination = destination;
}

int ClientMsg::getSeqNum() const
{
    return this->seqNum;
}

void ClientMsg::setSeqNum(int seqNum)
{
    this->seqNum = seqNum;
}

int ClientMsg::getTotNumPackets() const
{
    return this->totNumPackets;
}

void ClientMsg::setTotNumPackets(int totNumPackets)
{
    this->totNumPackets = totNumPackets;
}

const char * ClientMsg::getProtocol() const
{
    return this->protocol.c_str();
}

void ClientMsg::setProtocol(const char * protocol)
{
    this->protocol = protocol;
}

omnetpp::simtime_t ClientMsg::getSendTime() const
{
    return this->sendTime;
}

void ClientMsg::setSendTime(omnetpp::simtime_t sendTime)
{
    this->sendTime = sendTime;
}

int64_t ClientMsg::getByteLength() const
{
    return this->byteLength;
}

void ClientMsg::setByteLength(int64_t byteLength)
{
    this->byteLength = byteLength;
}

class ClientMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_streamId,
        FIELD_type,
        FIELD_size,
        FIELD_source,
        FIELD_destination,
        FIELD_seqNum,
        FIELD_totNumPackets,
        FIELD_protocol,
        FIELD_sendTime,
        FIELD_byteLength,
    };
  public:
    ClientMsgDescriptor();
    virtual ~ClientMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ClientMsgDescriptor)

ClientMsgDescriptor::ClientMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ClientMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ClientMsgDescriptor::~ClientMsgDescriptor()
{
    delete[] propertyNames;
}

bool ClientMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ClientMsg *>(obj)!=nullptr;
}

const char **ClientMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ClientMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ClientMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 10+base->getFieldCount() : 10;
}

unsigned int ClientMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_streamId
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_size
        FD_ISEDITABLE,    // FIELD_source
        FD_ISEDITABLE,    // FIELD_destination
        FD_ISEDITABLE,    // FIELD_seqNum
        FD_ISEDITABLE,    // FIELD_totNumPackets
        FD_ISEDITABLE,    // FIELD_protocol
        FD_ISEDITABLE,    // FIELD_sendTime
        FD_ISEDITABLE,    // FIELD_byteLength
    };
    return (field >= 0 && field < 10) ? fieldTypeFlags[field] : 0;
}

const char *ClientMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "streamId",
        "type",
        "size",
        "source",
        "destination",
        "seqNum",
        "totNumPackets",
        "protocol",
        "sendTime",
        "byteLength",
    };
    return (field >= 0 && field < 10) ? fieldNames[field] : nullptr;
}

int ClientMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "streamId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "type") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "size") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "source") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "destination") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "seqNum") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "totNumPackets") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "protocol") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "sendTime") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "byteLength") == 0) return baseIndex + 9;
    return base ? base->findField(fieldName) : -1;
}

const char *ClientMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_streamId
        "int",    // FIELD_type
        "int",    // FIELD_size
        "int",    // FIELD_source
        "int",    // FIELD_destination
        "int",    // FIELD_seqNum
        "int",    // FIELD_totNumPackets
        "string",    // FIELD_protocol
        "omnetpp::simtime_t",    // FIELD_sendTime
        "int64",    // FIELD_byteLength
    };
    return (field >= 0 && field < 10) ? fieldTypeStrings[field] : nullptr;
}

const char **ClientMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ClientMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ClientMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ClientMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ClientMsg'", field);
    }
}

const char *ClientMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ClientMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        case FIELD_streamId: return long2string(pp->getStreamId());
        case FIELD_type: return long2string(pp->getType());
        case FIELD_size: return long2string(pp->getSize());
        case FIELD_source: return long2string(pp->getSource());
        case FIELD_destination: return long2string(pp->getDestination());
        case FIELD_seqNum: return long2string(pp->getSeqNum());
        case FIELD_totNumPackets: return long2string(pp->getTotNumPackets());
        case FIELD_protocol: return oppstring2string(pp->getProtocol());
        case FIELD_sendTime: return simtime2string(pp->getSendTime());
        case FIELD_byteLength: return int642string(pp->getByteLength());
        default: return "";
    }
}

void ClientMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        case FIELD_streamId: pp->setStreamId(string2long(value)); break;
        case FIELD_type: pp->setType(string2long(value)); break;
        case FIELD_size: pp->setSize(string2long(value)); break;
        case FIELD_source: pp->setSource(string2long(value)); break;
        case FIELD_destination: pp->setDestination(string2long(value)); break;
        case FIELD_seqNum: pp->setSeqNum(string2long(value)); break;
        case FIELD_totNumPackets: pp->setTotNumPackets(string2long(value)); break;
        case FIELD_protocol: pp->setProtocol((value)); break;
        case FIELD_sendTime: pp->setSendTime(string2simtime(value)); break;
        case FIELD_byteLength: pp->setByteLength(string2int64(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClientMsg'", field);
    }
}

omnetpp::cValue ClientMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        case FIELD_streamId: return pp->getStreamId();
        case FIELD_type: return pp->getType();
        case FIELD_size: return pp->getSize();
        case FIELD_source: return pp->getSource();
        case FIELD_destination: return pp->getDestination();
        case FIELD_seqNum: return pp->getSeqNum();
        case FIELD_totNumPackets: return pp->getTotNumPackets();
        case FIELD_protocol: return pp->getProtocol();
        case FIELD_sendTime: return pp->getSendTime().dbl();
        case FIELD_byteLength: return (omnetpp::intval_t)(pp->getByteLength());
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ClientMsg' as cValue -- field index out of range?", field);
    }
}

void ClientMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        case FIELD_streamId: pp->setStreamId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_type: pp->setType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_size: pp->setSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_source: pp->setSource(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destination: pp->setDestination(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_seqNum: pp->setSeqNum(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_totNumPackets: pp->setTotNumPackets(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_protocol: pp->setProtocol(value.stringValue()); break;
        case FIELD_sendTime: pp->setSendTime(value.doubleValue()); break;
        case FIELD_byteLength: pp->setByteLength(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClientMsg'", field);
    }
}

const char *ClientMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ClientMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ClientMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ClientMsg *pp = omnetpp::fromAnyPtr<ClientMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ClientMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

