cmake_minimum_required(VERSION 3.12)
project(RPC)

set(CMAKE_CXX_STANDARD 20)

add_executable(RPC
        Main.cpp

        Basic/Debug.cpp
        Basic/Exception.cpp

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
        Json/JsonConverter/Test/StructParserTest.cpp
        Json/JsonConverter/Test/WordEnumeratorTest.cpp

        FuncLib/Store/File.cpp
        FuncLib/Store/FileCache.cpp
        FuncLib/Store/FileResource.cpp
        FuncLib/Store/FileReader.cpp
        FuncLib/Store/LabelRelationNode.cpp
        FuncLib/Store/ObjectBytes.cpp
        FuncLib/Store/FakeWriter.cpp
        FuncLib/Store/LabelTree.cpp
        FuncLib/Store/ObjectRelationTree.cpp
        FuncLib/Store/ObjectBytesQueue.cpp
        FuncLib/Store/StorageAllocator.cpp
        FuncLib/Compile/ParseFunc.cpp
        FuncLib/Compile/FuncDefTokenReader.cpp
        FuncLib/Compile/CompileProcess.cpp
        FuncLib/Test/Util.cpp
        FuncLib/Test/FileTest.cpp
        #FuncLib/Test/DiskBtreeTest.cpp
        FuncLib/Test/ByteConverterTest.cpp
        FuncLib/Test/ByteConvereterTest_Gen.cpp
        FuncLib/Test/FileReader&ObjectWriterTest.cpp
        FuncLib/Test/StorageAllocatorTest.cpp
        FuncLib/Test/FileCacheTest.cpp
        FuncLib/Test/TypeConverterTest.cpp
        FuncLib/Test/CompileTest.cpp
        )