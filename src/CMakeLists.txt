cmake_minimum_required(VERSION 3.12)
project(RPC)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(RPC
        Main.cpp

        Basic/Debug.cpp
        Basic/Exception.cpp
        Basic/StringViewUtil.cpp

        Btree/Test/BtreeTest.cpp
        Btree/Test/LiteVectorTest.cpp
        Btree/Test/ElementsTest.cpp

        Json/Json.cpp
        Json/Parser.cpp
        Json/LocationInfo.cpp
        Json/ParseException.cpp
        Json/Test/ParserTest.cpp

        Json/JsonConverter/SerialFunctionGenerator.cpp
        Json/JsonConverter/DeserialFunctionGenerator.cpp
        Json/JsonConverter/StructObject.cpp
        Json/JsonConverter/StructParser.cpp
        Json/JsonConverter/WordEnumerator.cpp
        Json/JsonConverter/JsonConverter.cpp
        Json/JsonConverter/Test/StructParserTest.cpp
        Json/JsonConverter/Test/WordEnumeratorTest.cpp

        FuncLib/FunctionLibrary.cpp
        FuncLib/FuncBinaryLibIndex.cpp
        FuncLib/FuncBinaryLib.cpp
        FuncLib/Store/File.cpp
        FuncLib/Store/FileCache.cpp
        FuncLib/Store/FileReader.cpp
        FuncLib/Store/ObjectRelation/LabelNode.cpp
        FuncLib/Store/ObjectRelation/LabelTree.cpp
        FuncLib/Store/ObjectRelation/ObjectRelationTree.cpp
        FuncLib/Store/StoreInfoPersistence.cpp
        FuncLib/Store/ObjectBytes.cpp
        FuncLib/Store/FakeObjectBytes.cpp
        FuncLib/Store/ObjectBytesQueue.cpp
        FuncLib/Store/StorageAllocator.cpp
        FuncLib/Compile/ParseFunc.cpp
        FuncLib/Compile/FuncType.cpp
        FuncLib/Compile/SharedLibrary.cpp
        FuncLib/Compile/Invoke.cpp
        FuncLib/Compile/Util.cpp
        FuncLib/Compile/FuncsDefReader.cpp
        FuncLib/Compile/CompileProcess.cpp
        FuncLib/Test/Util.cpp
        FuncLib/Test/FileTest.cpp
        FuncLib/Test/ByteConverterTest.cpp
        #FuncLib/Test/ByteConvereterTest_Gen.cpp
        FuncLib/Test/FileReaderObjectBytesTest.cpp
        FuncLib/Test/StorageAllocatorTest.cpp
        FuncLib/Test/FileCacheTest.cpp
        FuncLib/Test/TypeConverterTest.cpp
        FuncLib/Test/CompileTest.cpp
        FuncLib/Test/LabelNodeTest.cpp
        FuncLib/Test/ObjectRelationTreeTest.cpp
        FuncLib/Test/FunctionLibraryTest.cpp

        Network/Request.cpp

        Server/Test/ServerTest.cpp
        Server/Test/CmdFunctionTest.cpp
        Server/Test/FuncLibWorkerTest.cpp
        )

add_executable(cmd
        Cmd/Main.cpp
        # duplicate compile between projects TODO remove
        Basic/Debug.cpp

        Basic/Exception.cpp
        Basic/StringViewUtil.cpp

        Json/Json.cpp
        Json/Parser.cpp
        Json/LocationInfo.cpp
        Json/ParseException.cpp
        Json/JsonConverter/JsonConverter.cpp

        FuncLib/Compile/FuncType.cpp

        Network/Request.cpp
        )
#for *nix platform
target_link_libraries(cmd ncurses)

add_executable(client_demo
        Client/Demo/Main.cpp

        Basic/Debug.cpp
        Basic/Exception.cpp
        Basic/StringViewUtil.cpp
        
        Json/Json.cpp
        Json/Parser.cpp
        Json/LocationInfo.cpp
        Json/ParseException.cpp
        Json/JsonConverter/JsonConverter.cpp

        Network/Request.cpp

        FuncLib/Compile/FuncType.cpp
)