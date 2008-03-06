## Process this file with automake to produce Makefile.in

nobase_pkginclude_HEADERS = \
    common/base/ErrorHandler.h \
    common/base/Log.h \
    common/base/constants.h \
    common/base/errors.h \
    common/base/fscapi.h \
    common/base/md5.h \
    common/base/base64.h \
    common/base/test.h \
    common/base/util/utils.h \
    common/base/util/ArrayElement.h \
    common/base/util/ArrayList.h \
    common/base/util/KeyValuePair.h \
    common/base/util/WKeyValuePair.h \
    common/base/util/XMLProcessor.h \
    common/base/util/StringBuffer.h \
    common/base/util/WString.h \
    common/client/DMTClientConfig.h \
    common/client/SyncClient.h \
    common/event/constants.h \
    common/filter/Clause.h \
    common/filter/LogicalClause.h \
    common/filter/SourceFilter.h \
    common/http/constants.h \
    common/http/Proxy.h \
    common/http/TransportAgent.h \
    common/http/URL.h \
    common/http/errors.h \
    common/spds/AccessConfig.h \
    common/spds/DeviceConfig.h \
    common/spdm/constants.h \
    common/spdm/errors.h \
    common/spdm/DMTree.h \
    common/spdm/ManagementNode.h \
    common/spds/AbstractSyncConfig.h \
    common/spds/AbstractSyncSourceConfig.h \
    common/spds/CredentialHandler.h \
    common/spds/ItemReport.h \
    common/spds/SyncSourceConfig.h \
    common/spds/SyncItem.h \
    common/spds/SyncItemStatus.h \
    common/spds/SyncMLBuilder.h \
    common/spds/SyncMLProcessor.h \
    common/spds/SyncManager.h \
    common/spds/SyncManagerConfig.h \
    common/spds/SyncMap.h \
    common/spds/SyncReport.h \
    common/spds/SyncSource.h \
    common/spds/SyncSourceReport.h \
    common/spds/SyncStatus.h \
    common/spds/spdsutils.h \
    common/spds/constants.h \
    common/spds/errors.h \
    common/syncml/core/CTCap.h \
    common/syncml/core/CTTypeSupported.h \
    common/syncml/core/VerDTD.h \
    common/vocl/VConverter.h \
    common/vocl/VObject.h \
    common/vocl/VProperty.h \
    common/vocl/vCard/Address.h \
    common/vocl/vCard/BusinessDetail.h \
    common/vocl/vCard/Contact30.h \
    common/vocl/vCard/ContactDetail.h \
    common/vocl/vCard/Contact.h \
    common/vocl/vCard/Email.h \
    common/vocl/vCard/Name.h \
    common/vocl/vCard/Note.h \
    common/vocl/vCard/PersonalDetail.h \
    common/vocl/vCard/Phone.h  \
    common/vocl/vCard/Title.h  \
    common/vocl/vCard/TypedProperty.h \
    common/vocl/vCard/vCardConverter.h \
    common/vocl/vCard/vCardProperty.h \
    common/vocl/vCard/WebPage.h \
    common/event/ManageListener.h \
    common/event/SyncListener.h \
    common/event/SyncItemListener.h \
    common/event/SyncStatusListener.h \
    common/event/SyncSourceListener.h \
    common/event/TransportListener.h \
    common/event/BaseEvent.h \
    common/event/SyncEvent.h \
    common/event/SyncItemEvent.h \
    common/event/SyncStatusEvent.h \
    common/event/SyncSourceEvent.h \
    common/event/TransportEvent.h \
    common/event/Listener.h \
    posix/spdm/DeviceManagementNode.h \
    posix/base/posixlog.h

# fix header files for public distribution
$(nobase_pkginclude_HEADERS): % : ../../../src/include/%
	mkdir -p `dirname $@`
	rm -f $@
	if [ "$(@F)" == "Log.h" ]; then echo '#include <stdarg.h>' >>$@; fi
	sed -e 's/WCHAR/char/g' -e 's/= *NULL/= 0/g' -e 's/__declspec([^)]*)//g' -e 's/__cdecl//g' -e 's/EXTRA_SECTION_[0123456789]*//g' $< >>$@

clean : cleancopies
cleancopies :
	for i in $(nobase_pkginclude_HEADERS); do rm -rf `echo $$i | sed -e 's;\([^/]*\).*;\1;'`; done
