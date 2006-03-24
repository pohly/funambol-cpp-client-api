bin_PROGRAMS = client vcardconverter
lib_LTLIBRARIES = libsync4j.la

SOURCES_BASE = \
    Log.cpp \
    autotoolsadapter.cpp \
    ArrayElement.cpp \
    ArrayList.cpp \
    KeyValuePair.cpp \
    WKeyValuePair.cpp \
    StringBuffer.cpp \
    WString.cpp \
    XMLProcessor.cpp \
    baseutils.cpp \
    base64.cpp \
    quoted-printable.cpp \
    error.cpp \
    ErrorHandler.cpp \
    LogErrorHandler.cpp \
    md5.c

SOURCES_FRAMEWORK = \
    Formatter.cpp \
    AbstractCommand.cpp \
    Add.cpp \
    AllClause.cpp \
    Alert.cpp \
    Anchor.cpp \
    Atomic.cpp \
    Authentication.cpp \
    Chal.cpp \
    Clause.cpp \
    ClauseUtil.cpp \
    CmdID.cpp \
    ComplexData.cpp \
    ContentTypeInfo.cpp \
    ContentTypeParameter.cpp \
    Copy.cpp \
    Cred.cpp \
    CTCap.cpp \
    CTPropParam.cpp \
    CTTypeSupported.cpp \
    Data.cpp \
    DataStore.cpp \
    Delete.cpp \
    DevInf.cpp \
    DevInfData.cpp \
    DevInfItem.cpp \
    DSMem.cpp \
    EMI.cpp \
    Exec.cpp \
    Ext.cpp \
    FieldClause.cpp \
    Filter.cpp \
    Get.cpp \
    Item.cpp \
    ItemizedCommand.cpp \
    LogicalClause.cpp \
    Map.cpp \
    MapItem.cpp \
    Mem.cpp \
    Meta.cpp \
    MetInf.cpp \
    ModificationCommand.cpp \
    NextNonce.cpp \
    ObjectDel.cpp \
    Parser.cpp \
    Property.cpp \
    PropParam.cpp \
    Put.cpp \
    Replace.cpp \
    ResponseCommand.cpp \
    Results.cpp \
    Search.cpp \
    Sequence.cpp \
    SessionID.cpp \
    Source.cpp \
    SourceArray.cpp \
    SourceFilter.cpp \
    SourceRef.cpp \
    Status.cpp \
    StringElement.cpp \
    Sync.cpp \
    SyncBody.cpp \
    SyncCap.cpp \
    SyncHdr.cpp \
    SyncML.cpp \
    SyncType.cpp \
    SyncTypeArray.cpp \
    SyncNotification.cpp \
    SyncAlert.cpp \
    Target.cpp \
    TargetRef.cpp \
    VerDTD.cpp \
    VerProto.cpp \
    WhereClause.cpp

SOURCES_HTTP = \
    CurlTransportAgent.cpp \
    TransportAgentFactory.cpp \
    HTTPHeader.cpp \
    Proxy.cpp \
    TransportAgent.cpp \
    URL.cpp

SOURCES_SPDM = \
    DMTree.cpp \
    DMTreeFactory.cpp \
    DeviceManagementNode.cpp \
    ManagementNode.cpp \
    spdmutils.cpp

SOURCES_SPDS = \
    AccessConfig.cpp \
    BodyPart.cpp \
    CredentialHandler.cpp \
    DataTransformerFactory.cpp \
    DataTransformer.cpp \
    B64Decoder.cpp \
    B64Encoder.cpp \
    DESDecoder.cpp \
    DESEncoder.cpp \
    EmailData.cpp \
    FileData.cpp \
    MailMessage.cpp \
    SyncManagerConfig.cpp \
    SyncItem.cpp \
    SyncItemStatus.cpp \
    SyncMLBuilder.cpp \
    SyncMLProcessor.cpp \
    SyncManager.cpp \
    SyncMap.cpp \
    SyncSource.cpp \
    SyncSourceConfig.cpp \
    spdsutils.cpp \
    constants.cpp

SOURCES_CLIENT = \
    DMTClientConfig.cpp \
    Sync4jClient.cpp

SOURCES_VOCL = \
    VConverter.cpp \
    VObject.cpp \
    VObjectFactory.cpp \
    VProperty.cpp \
\
    Calendar.cpp \
    Event.cpp \
    ToDo.cpp \
    iCalConverter.cpp \
    iCalProperty.cpp \
\
    Address.cpp \
    BusinessDetail.cpp \
    Contact.cpp \
    Contact30.cpp \
    ContactDetail.cpp \
    Email.cpp \
    Name.cpp \
    Note.cpp \
    PersonalDetail.cpp \
    Phone.cpp \
    Title.cpp \
    TypedProperty.cpp \
    WebPage.cpp \
    vCardConverter.cpp \
    vCardProperty.cpp

libsync4j_la_SOURCES = $(SOURCES_BASE) $(SOURCES_FRAMEWORK) $(SOURCES_HTTP) $(SOURCES_SPDM) $(SOURCES_SPDS) $(SOURCES_CLIENT) $(SOURCES_VOCL)
libsync4j_la_LDFLAGS = -version-info 3:0

client_SOURCES = client.cpp TestSyncSource.cpp TestSyncSource2.cpp
client_LDADD = libsync4j.la

vcardconverter_SOURCES = vcardconverter.cpp
vcardconverter_LDADD = libsync4j.la

#NotifProcessorTest_SOURCES = NotifProcessorTest.cpp
#NotifProcessorTest_LDADD = libsync4j.la

vpath %.cpp ../../../src/c++/common/base             \
            ../../../src/c++/common/base/util        \
            ../../../src/c++/common/client           \
            ../../../src/c++/common/examples         \
            ../../../src/c++/common/filter           \
            ../../../src/c++/common/syncml/formatter \
            ../../../src/c++/common/syncml/core      \
            ../../../src/c++/common/syncml/parser    \
            ../../../src/c++/common/http             \
            ../../../src/c++/common/spdm             \
            ../../../src/c++/common/spds             \
            ../../../src/c++/common/vocl             \
            ../../../src/c++/common/vocl/iCalendar   \
            ../../../src/c++/common/vocl/vCard       \
            ../../../src/c++/posix/base              \
            ../../../src/c++/posix/http              \
            ../../../src/c++/posix/spdm              \
            ../../../src/c++/posix/spds

vpath %.c   ../../../src/c++/common/base

AM_LDFLAGS = `curl-config --libs`
AM_CPPFLAGS = -I../../../src/include/posix -I../../../src/include/common -DAUTOTOOLS
