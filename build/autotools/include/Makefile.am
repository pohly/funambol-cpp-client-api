## Process this file with automake to produce Makefile.in

nobase_pkginclude_HEADERS = \
    common/base/ErrorHandler.h \
    common/base/Log.h \
    common/base/constants.h \
    common/base/errors.h \
    common/base/fscapi.h \
    common/base/md5.h \
    common/base/base64.h \
    common/base/util/utils.h \
    common/base/util/ArrayElement.h \
    common/base/util/ArrayList.h \
    common/base/util/KeyValuePair.h \
    common/base/util/WKeyValuePair.h \
    common/base/util/XMLProcessor.h \
    common/base/util/WString.h \
    common/client/DMTClientConfig.h \
    common/client/Sync4jClient.h \
    common/filter/Clause.h \
    common/filter/LogicalClause.h \
    common/filter/SourceFilter.h \
    common/http/constants.h \
    common/http/Proxy.h \
    common/http/TransportAgent.h \
    common/http/URL.h \
    common/http/errors.h \
    common/spds/AccessConfig.h \
    common/spdm/constants.h \
    common/spdm/errors.h \
    common/spdm/DMTree.h \
    common/spdm/ManagementNode.h \
    common/spds/CredentialHandler.h \
    common/spds/SyncSourceConfig.h \
    common/spds/SyncItem.h \
    common/spds/SyncItemStatus.h \
    common/spds/SyncMLBuilder.h \
    common/spds/SyncMLProcessor.h \
    common/spds/SyncManager.h \
    common/spds/SyncManagerConfig.h \
    common/spds/SyncMap.h \
    common/spds/SyncSource.h \
    common/spds/SyncStatus.h \
    common/spds/spdsutils.h \
    common/spds/constants.h \
    common/spds/errors.h \
    common/vocl/VConverter.h \
    common/vocl/VObject.h \
    common/vocl/VProperty.h \
    posix/base/posixlog.h

# fix header files for public distribution
$(nobase_pkginclude_HEADERS): % : ../../../src/include/%
	mkdir -p `dirname $@`
	sed -e 's/wchar_t/char/g' -e 's/BCHAR/char/g' -e 's/= *NULL/= 0/g' -e 's/__declspec([^)]*)//g' -e 's/__cdecl//g' -e 's/EXTRA_SECTION_[0123456789]*//g' -e 's/BOOL/int/g' -e 's/FALSE/0/g' -e 's/TRUE/1/g' $< >$@

clean : cleancopies
cleancopies :
	for i in $(nobase_pkginclude_HEADERS); do rm -rf `echo $$i | sed -e 's;\([^/]*\).*;\1;'`; done
