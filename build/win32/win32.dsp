# Microsoft Developer Studio Project File - Name="win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=win32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "win32.mak" CFG="win32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "win32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "win32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "win32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output\win32-rel"
# PROP Intermediate_Dir "output\win32-rel-object"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\\" /I "..\..\src\include\win32" /I "..\..\src\include\common" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "win32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output\win32-dbg"
# PROP Intermediate_Dir "output\win32-dbg-object"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\src\include\common" /I "..\..\src\include\win32" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /D "_LIB" /D "NO_CRIT_MSGBOX" /FD /I /GZ /c
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "win32 - Win32 Release"
# Name "win32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "base"

# PROP Default_Filter ""
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\ArrayElement.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\ArrayList.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\baseutils.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\BasicTime.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\KeyValuePair.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\StringBuffer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\WKeyValuePair.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\WString.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\util\XMLProcessor.cpp"
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\src\c++\common\base\base64.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\error.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\ErrorHandler.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\base\Log.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\LogErrorHandler.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\md5.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\base\quoted-printable.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\base\win32adapter.cpp"
# End Source File
# End Group
# Begin Group "http"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\http\HTTPHeader.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\http\Proxy.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\http\TransportAgent.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\http\TransportAgentFactory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\http\URL.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\http\Win32TransportAgent.cpp"
# End Source File
# End Group
# Begin Group "client"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\client\DMTClientConfig.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\client\Sync4jClient.cpp"
# End Source File
# End Group
# Begin Group "spdm"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\win32\spdm\DeviceManagementNode.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spdm\DMtree.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\spdm\DMTreeFactory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spdm\ManagementNode.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spdm\spdmutils.cpp"
# End Source File
# End Group
# Begin Group "spds"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\spds\AccessConfig.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\B64Decoder.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\B64Encoder.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\BodyPart.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\constants.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\CredentialHandler.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\DataTransformer.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\DataTransformerFactory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\spds\DESDecoder.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\win32\spds\DESEncoder.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\EmailData.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\MailMessage.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\MailSyncSourceConfig.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\spdsutils.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncItem.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncItemStatus.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncManager.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncManagerConfig.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncMap.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncMLBuilder.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncMLProcessor.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncSource.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\spds\SyncSourceConfig.cpp"
# End Source File
# End Group
# Begin Group "syncml"

# PROP Default_Filter ""
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\AbstractCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Add.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Alert.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Anchor.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Atomic.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Authentication.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Chal.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\CmdID.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ComplexData.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ContentTypeInfo.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ContentTypeParameter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Copy.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Cred.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\CTCap.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\CTPropParam.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\CTTypeSupported.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Data.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\DataStore.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Delete.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\DevInf.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\DevInfData.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\DevInfItem.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\DSMem.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\EMI.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Exec.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Ext.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Filter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Get.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Item.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ItemizedCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Map.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\MapItem.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Mem.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Meta.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\MetInf.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ModificationCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\NextNonce.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ObjectDel.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Property.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\PropParam.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Put.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Replace.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\ResponseCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Results.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Search.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Sequence.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SessionID.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Source.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SourceArray.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SourceRef.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Status.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\StringElement.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Sync.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncAlert.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncBody.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncCap.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncHdr.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncML.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncNotification.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncType.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\SyncTypeArray.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\Target.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\TargetRef.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\VerDTD.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\core\VerProto.cpp"
# End Source File
# End Group
# Begin Group "parser"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\parser\Parser.cpp"
# End Source File
# End Group
# Begin Group "formatter"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\syncml\formatter\Formatter.cpp"
# End Source File
# End Group
# End Group
# Begin Group "filter"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\filter\AllClause.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\Clause.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\ClauseUtil.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\FieldClause.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\LogicalClause.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\SourceFilter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\filter\WhereClause.cpp"
# End Source File
# End Group
# Begin Group "vocl"

# PROP Default_Filter ""
# Begin Group "iCalendar"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\iCalendar\Calendar.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\iCalendar\Event.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\iCalendar\iCalConverter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\iCalendar\iCalProperty.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\iCalendar\ToDo.cpp"
# End Source File
# End Group
# Begin Group "vCard"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Address.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\BusinessDetail.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Contact.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Contact30.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\ContactDetail.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Email.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Name.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Note.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\PersonalDetail.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Phone.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\Title.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\TypedProperty.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\vCardConverter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\vCardProperty.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\vCard\WebPage.cpp"
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\VConverter.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\VObject.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\VObjectFactory.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\src\c++\common\vocl\VProperty.cpp"
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
