EXTRA_PROGRAMS = vcardconverter
lib_LTLIBRARIES = libsync4j.la

SOURCES_BASE = \
    Log.cpp \
    autotoolsadapter.cpp \
    ArrayElement.cpp \
    ArrayList.cpp \
    BasicTime.cpp \
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
    ItemContainer.cpp \
    LogErrorHandler.cpp \
    md5.c

SOURCES_EVENT = \
    BaseEvent.cpp \
    FireEvent.cpp \
    Listener.cpp \
    ManageListener.cpp \
    SetListener.cpp \
    SyncEvent.cpp \
    SyncItemEvent.cpp \
    SyncItemListener.cpp \
    SyncListener.cpp \
    SyncSourceEvent.cpp \
    SyncSourceListener.cpp \
    SyncStatusEvent.cpp \
    SyncStatusListener.cpp \
    TransportEvent.cpp \
    TransportListener.cpp

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
    ManagementNode.cpp 

SOURCES_SPDS = \
    AccessConfig.cpp \
    BodyPart.cpp \
    CredentialHandler.cpp \
    DataTransformerFactory.cpp \
    DataTransformer.cpp \
    DeviceConfig.cpp \
    B64Decoder.cpp \
    B64Encoder.cpp \
    DESDecoder.cpp \
    DESEncoder.cpp \
    DefaultConfigFactory.cpp \
    EmailData.cpp \
    FILESyncSource.cpp \
    FileData.cpp \
    ItemReport.cpp \
    MailMessage.cpp \
    RawFILESyncSource.cpp \
    SyncManagerConfig.cpp \
    SyncItem.cpp \
    SyncItemStatus.cpp \
    SyncMLBuilder.cpp \
    SyncMLProcessor.cpp \
    SyncManager.cpp \
    SyncMap.cpp \
    SyncReport.cpp \
    SyncSource.cpp \
    SyncSourceConfig.cpp \
    SyncSourceReport.cpp \
    spdsutils.cpp 

SOURCES_CLIENT = \
    DMTClientConfig.cpp \
    SyncClient.cpp

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

SOURCES_LIBSYNC4J = $(SOURCES_BASE) $(SOURCES_EVENT) $(SOURCES_FRAMEWORK) $(SOURCES_HTTP) $(SOURCES_SPDM) $(SOURCES_SPDS) $(SOURCES_CLIENT) $(SOURCES_VOCL)

libsync4j_la_SOURCES = $(SOURCES_LIBSYNC4J)
libsync4j_la_LDFLAGS = -version-info 3:0

# client_SOURCES = client.cpp TestSyncSource.cpp TestSyncSource2.cpp
# client_LDADD = libsync4j.la

vcardconverter_SOURCES = vcardconverter.cpp
vcardconverter_LDADD = libsync4j.la


#NotifProcessorTest_SOURCES = NotifProcessorTest.cpp
#NotifProcessorTest_LDADD = libsync4j.la



vpath %.cpp $(srcdir)/../../../src/c++/common/base             \
            $(srcdir)/../../../src/c++/common/base/util        \
            $(srcdir)/../../../src/c++/common/client           \
            $(srcdir)/../../../src/c++/common/examples         \
            $(srcdir)/../../../src/c++/common/event            \
            $(srcdir)/../../../src/c++/common/filter           \
            $(srcdir)/../../../src/c++/common/syncml/formatter \
            $(srcdir)/../../../src/c++/common/syncml/core      \
            $(srcdir)/../../../src/c++/common/syncml/parser    \
            $(srcdir)/../../../src/c++/common/http             \
            $(srcdir)/../../../src/c++/common/spdm             \
            $(srcdir)/../../../src/c++/common/spds             \
            $(srcdir)/../../../src/c++/common/test             \
            $(srcdir)/../../../src/c++/common/vocl             \
            $(srcdir)/../../../src/c++/common/vocl/iCalendar   \
            $(srcdir)/../../../src/c++/common/vocl/vCard       \
            $(srcdir)/../../../src/c++/posix/base              \
            $(srcdir)/../../../src/c++/posix/http              \
            $(srcdir)/../../../src/c++/posix/spdm              \
            $(srcdir)/../../../src/c++/posix/spds

vpath %.c   $(srcdir)/../../../src/c++/common/base

AM_LDFLAGS = @CURL_LDFLAGS@ @CPPUNIT_LDFLAGS@
AM_CXXFLAGS = @CURL_CFLAGS@ @CPPUNIT_CXXFLAGS@
AM_CPPFLAGS = -I$(srcdir)/../../../src/include/posix -I$(srcdir)/../../../src/include/common -DAUTOTOOLS
