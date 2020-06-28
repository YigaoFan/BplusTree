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
        Json/ParseException.cpp
        Json/Test/ParserTest.cpp

        Json/JsonConverter/SerialFunctionGenerator.cpp
        Json/JsonConverter/DeserialFunctionGenerator.cpp
        Json/JsonConverter/StructObject.cpp
        Json/JsonConverter/StructParser.cpp
        Json/JsonConverter/WordEnumerator.cpp
        Json/JsonConverter/Test/StructParserTest.cpp
        Json/JsonConverter/Test/WordEnumeratorTest.cpp
        )