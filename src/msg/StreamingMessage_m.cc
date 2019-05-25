//
// Generated file, do not edit! Created by nedtool 5.1 from msg/StreamingMessage.msg.
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
#include "StreamingMessage_m.h"

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
    for (int i=0; i<n; i++) {
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
    for (int i=0; i<n; i++) {
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
    for (int i=0; i<n; i++) {
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

namespace ecsnetpp {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(StreamingMessage)

StreamingMessage::StreamingMessage(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->messageId = 0;
    this->isProcessingDelayInCyclesPerEvent = false;
    this->processingDelayPerEvent = 0;
    this->selectivityRatio = 0;
    this->startTime = 0;
    this->operatorIngressTime = 0;
    this->channelIngressTime = 0;
    this->networkDelay = 0;
    this->processingDelay = 0;
    this->edgeProcessingDelay = 0;
}

StreamingMessage::StreamingMessage(const StreamingMessage& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

StreamingMessage::~StreamingMessage()
{
}

StreamingMessage& StreamingMessage::operator=(const StreamingMessage& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void StreamingMessage::copy(const StreamingMessage& other)
{
    this->messageId = other.messageId;
    this->sender = other.sender;
    this->content = other.content;
    this->isProcessingDelayInCyclesPerEvent = other.isProcessingDelayInCyclesPerEvent;
    this->processingDelayPerEvent = other.processingDelayPerEvent;
    this->selectivityRatio = other.selectivityRatio;
    this->startTime = other.startTime;
    this->operatorIngressTime = other.operatorIngressTime;
    this->channelIngressTime = other.channelIngressTime;
    this->networkDelay = other.networkDelay;
    this->processingDelay = other.processingDelay;
    this->edgeProcessingDelay = other.edgeProcessingDelay;
}

void StreamingMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->messageId);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->isProcessingDelayInCyclesPerEvent);
    doParsimPacking(b,this->processingDelayPerEvent);
    doParsimPacking(b,this->selectivityRatio);
    doParsimPacking(b,this->startTime);
    doParsimPacking(b,this->operatorIngressTime);
    doParsimPacking(b,this->channelIngressTime);
    doParsimPacking(b,this->networkDelay);
    doParsimPacking(b,this->processingDelay);
    doParsimPacking(b,this->edgeProcessingDelay);
}

void StreamingMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->messageId);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->isProcessingDelayInCyclesPerEvent);
    doParsimUnpacking(b,this->processingDelayPerEvent);
    doParsimUnpacking(b,this->selectivityRatio);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->operatorIngressTime);
    doParsimUnpacking(b,this->channelIngressTime);
    doParsimUnpacking(b,this->networkDelay);
    doParsimUnpacking(b,this->processingDelay);
    doParsimUnpacking(b,this->edgeProcessingDelay);
}

int StreamingMessage::getMessageId() const
{
    return this->messageId;
}

void StreamingMessage::setMessageId(int messageId)
{
    this->messageId = messageId;
}

const char * StreamingMessage::getSender() const
{
    return this->sender.c_str();
}

void StreamingMessage::setSender(const char * sender)
{
    this->sender = sender;
}

const char * StreamingMessage::getContent() const
{
    return this->content.c_str();
}

void StreamingMessage::setContent(const char * content)
{
    this->content = content;
}

bool StreamingMessage::getIsProcessingDelayInCyclesPerEvent() const
{
    return this->isProcessingDelayInCyclesPerEvent;
}

void StreamingMessage::setIsProcessingDelayInCyclesPerEvent(bool isProcessingDelayInCyclesPerEvent)
{
    this->isProcessingDelayInCyclesPerEvent = isProcessingDelayInCyclesPerEvent;
}

double StreamingMessage::getProcessingDelayPerEvent() const
{
    return this->processingDelayPerEvent;
}

void StreamingMessage::setProcessingDelayPerEvent(double processingDelayPerEvent)
{
    this->processingDelayPerEvent = processingDelayPerEvent;
}

double StreamingMessage::getSelectivityRatio() const
{
    return this->selectivityRatio;
}

void StreamingMessage::setSelectivityRatio(double selectivityRatio)
{
    this->selectivityRatio = selectivityRatio;
}

::omnetpp::simtime_t StreamingMessage::getStartTime() const
{
    return this->startTime;
}

void StreamingMessage::setStartTime(::omnetpp::simtime_t startTime)
{
    this->startTime = startTime;
}

::omnetpp::simtime_t StreamingMessage::getOperatorIngressTime() const
{
    return this->operatorIngressTime;
}

void StreamingMessage::setOperatorIngressTime(::omnetpp::simtime_t operatorIngressTime)
{
    this->operatorIngressTime = operatorIngressTime;
}

::omnetpp::simtime_t StreamingMessage::getChannelIngressTime() const
{
    return this->channelIngressTime;
}

void StreamingMessage::setChannelIngressTime(::omnetpp::simtime_t channelIngressTime)
{
    this->channelIngressTime = channelIngressTime;
}

double StreamingMessage::getNetworkDelay() const
{
    return this->networkDelay;
}

void StreamingMessage::setNetworkDelay(double networkDelay)
{
    this->networkDelay = networkDelay;
}

double StreamingMessage::getProcessingDelay() const
{
    return this->processingDelay;
}

void StreamingMessage::setProcessingDelay(double processingDelay)
{
    this->processingDelay = processingDelay;
}

double StreamingMessage::getEdgeProcessingDelay() const
{
    return this->edgeProcessingDelay;
}

void StreamingMessage::setEdgeProcessingDelay(double edgeProcessingDelay)
{
    this->edgeProcessingDelay = edgeProcessingDelay;
}

class StreamingMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    StreamingMessageDescriptor();
    virtual ~StreamingMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(StreamingMessageDescriptor)

StreamingMessageDescriptor::StreamingMessageDescriptor() : omnetpp::cClassDescriptor("ecsnetpp::StreamingMessage", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

StreamingMessageDescriptor::~StreamingMessageDescriptor()
{
    delete[] propertynames;
}

bool StreamingMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<StreamingMessage *>(obj)!=nullptr;
}

const char **StreamingMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *StreamingMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int StreamingMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount() : 12;
}

unsigned int StreamingMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *StreamingMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "messageId",
        "sender",
        "content",
        "isProcessingDelayInCyclesPerEvent",
        "processingDelayPerEvent",
        "selectivityRatio",
        "startTime",
        "operatorIngressTime",
        "channelIngressTime",
        "networkDelay",
        "processingDelay",
        "edgeProcessingDelay",
    };
    return (field>=0 && field<12) ? fieldNames[field] : nullptr;
}

int StreamingMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "messageId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sender")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "content")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "isProcessingDelayInCyclesPerEvent")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "processingDelayPerEvent")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "selectivityRatio")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "startTime")==0) return base+6;
    if (fieldName[0]=='o' && strcmp(fieldName, "operatorIngressTime")==0) return base+7;
    if (fieldName[0]=='c' && strcmp(fieldName, "channelIngressTime")==0) return base+8;
    if (fieldName[0]=='n' && strcmp(fieldName, "networkDelay")==0) return base+9;
    if (fieldName[0]=='p' && strcmp(fieldName, "processingDelay")==0) return base+10;
    if (fieldName[0]=='e' && strcmp(fieldName, "edgeProcessingDelay")==0) return base+11;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *StreamingMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "string",
        "string",
        "bool",
        "double",
        "double",
        "simtime_t",
        "simtime_t",
        "simtime_t",
        "double",
        "double",
        "double",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : nullptr;
}

const char **StreamingMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *StreamingMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int StreamingMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    StreamingMessage *pp = (StreamingMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *StreamingMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    StreamingMessage *pp = (StreamingMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string StreamingMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    StreamingMessage *pp = (StreamingMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMessageId());
        case 1: return oppstring2string(pp->getSender());
        case 2: return oppstring2string(pp->getContent());
        case 3: return bool2string(pp->getIsProcessingDelayInCyclesPerEvent());
        case 4: return double2string(pp->getProcessingDelayPerEvent());
        case 5: return double2string(pp->getSelectivityRatio());
        case 6: return simtime2string(pp->getStartTime());
        case 7: return simtime2string(pp->getOperatorIngressTime());
        case 8: return simtime2string(pp->getChannelIngressTime());
        case 9: return double2string(pp->getNetworkDelay());
        case 10: return double2string(pp->getProcessingDelay());
        case 11: return double2string(pp->getEdgeProcessingDelay());
        default: return "";
    }
}

bool StreamingMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    StreamingMessage *pp = (StreamingMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setMessageId(string2long(value)); return true;
        case 1: pp->setSender((value)); return true;
        case 2: pp->setContent((value)); return true;
        case 3: pp->setIsProcessingDelayInCyclesPerEvent(string2bool(value)); return true;
        case 4: pp->setProcessingDelayPerEvent(string2double(value)); return true;
        case 5: pp->setSelectivityRatio(string2double(value)); return true;
        case 6: pp->setStartTime(string2simtime(value)); return true;
        case 7: pp->setOperatorIngressTime(string2simtime(value)); return true;
        case 8: pp->setChannelIngressTime(string2simtime(value)); return true;
        case 9: pp->setNetworkDelay(string2double(value)); return true;
        case 10: pp->setProcessingDelay(string2double(value)); return true;
        case 11: pp->setEdgeProcessingDelay(string2double(value)); return true;
        default: return false;
    }
}

const char *StreamingMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *StreamingMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    StreamingMessage *pp = (StreamingMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace ecsnetpp

