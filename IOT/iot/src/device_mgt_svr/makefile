
CC = g++ -DLINUX
FLAGS = -W -g

BASE_ROOT=/home/xucuiping/sdk/base
JSON_ROOT=../../../../../3rd_party/sdk/rapidjson
TINY_XML_ROOT=../../../../../3rd_party/sdk/tinyxml
MYSQL_ROOT=/home/xucuiping/tool/sdk/mysql
HIREDIS_ROOT=../../../../../3rd_party/sdk/hiredis
MONGO_ROOT=../../../../../3rd_party/sdk/mongo
CURL_ROOT=../../../../../3rd_party/sdk/curl
OPENSSL_ROOT=../../../../../3rd_party/sdk/openssl
CRYPTOPP_ROOT=../../../../../3rd_party/sdk/cryptopp

INCLUDES=-I./ -I../ -I$(BASE_ROOT)/include/ -I$(JSON_ROOT)/include/ \
-I$(TINY_XML_ROOT)/include/ -I$(MYSQL_ROOT)/include/ -I$(HIREDIS_ROOT)/include/ -I$(MONGO_ROOT)/include/ \
-I$(MONGO_ROOT)/include/libbson-1.0/ -I$(MONGO_ROOT)/include/libmongoc-1.0/ -I$(CURL_ROOT)/include/ -I$(CRYPTOPP_ROOT)/include/ -I$(OPENSSL_ROOT)/include/

LIB_PATH=

SHARE_LIBS=$(BASE_ROOT)/lib/libbase.a \
$(TINY_XML_ROOT)/lib/libtinyxml.a $(MYSQL_ROOT)/lib/libmysqlclient_r.a $(HIREDIS_ROOT)/lib/libhiredis.a \
$(MONGO_ROOT)/lib/libmongoc-1.0.a  $(MONGO_ROOT)/lib/libbson-1.0.a $(CURL_ROOT)/lib/libcurl.a \
$(CRYPTOPP_ROOT)/lib/libcryptopp.a $(OPENSSL_ROOT)/lib/libssl.a $(OPENSSL_ROOT)/lib/libcrypto.a \
-lpthread -ldl -lrt -lz

TARGET=device_mgt_svr.bin

OBJS = $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

obj/%.o: %.h
obj/%.o: %.cpp
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(LIB_PATH) $(OBJS) $(SHARE_LIBS);
	cp $(TARGET)  ../../../../../release/bin/$(TARGET)

clean:
	rm obj/*.o
	rm $(TARGET)

init:
	mkdir -p obj
	
