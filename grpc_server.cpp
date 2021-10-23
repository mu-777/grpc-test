#include <iostream>
#include <fstream>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "simple.pb.h"
#include "simple.grpc.pb.h"
#include "fileio.pb.h"
#include "fileio.grpc.pb.h"

#include <thread>
#include <chrono>

namespace simple {
class SimpleAddImpl final : public Add::Service {
  ::grpc::Status Add(::grpc::ServerContext *context, const AddRequest *req,
                     AddReply *res) override {
    std::cout << "[Server] ReceivedReq: " << req->str1() << ", " << req->str2() << std::endl;
    res->set_str(req->str1() + req->str2());
    return ::grpc::Status::OK;
  }
};
};

namespace fileio {
class SqliteFileIOImpl final : public FileIO::Service {

  const int ReadBufferSize = 1048576;

  ::grpc::Status RequestSqlite(::grpc::ServerContext *context,
                               const SqliteRequest *req,
                               grpc::ServerWriter<SqliteResponse> *writer) override {
    std::cout << "[Server] ReceivedReq: " << req->filepath() << std::endl;

    std::ifstream ifs(req->filepath(), std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
      sendMetaRes(writer, 1, 0);
      return ::grpc::Status::CANCELLED;
    }
    for (auto i = 0; i < 10; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      auto m = i % 3;
      std::cout << "loading" << ((m == 0) ? "." : ((m == 1) ? ".." : "...")) << std::endl;
    }
    ifs.seekg(0, std::ios::end);
    sendMetaRes(writer, 0, ifs.tellg());

    ifs.seekg(0, std::ios::beg);
    char *buf = new char[ReadBufferSize];
    while (!ifs.eof()) {
      ifs.read(buf, ReadBufferSize);
      std::cout << "send data chunk" << std::endl;
      sendChunkRes(writer, 1, new std::string(buf, ifs.gcount()));
    }
    delete buf;

    return ::grpc::Status::OK;
  }

  static void sendMetaRes(grpc::ServerWriter<SqliteResponse> *writer, uint32_t status, uint32_t size) {
    auto res = SqliteResponse();
    auto meta = new SqliteMeta();
    meta->set_status(status);
    meta->set_size(size);
    res.set_allocated_meta(meta);
    writer->Write(res);
  }

  static void sendChunkRes(grpc::ServerWriter<SqliteResponse> *writer, uint32_t status, std::string *data) {
    auto res = SqliteResponse();
    auto chunk = new SqliteChunk();
    chunk->set_status(status);
    chunk->set_allocated_data(data);
    res.set_allocated_chunk(chunk);
    writer->Write(res);
  }

  // https://github.com/steplee/lidarRender/blob/fc61c094c2dd173cb3f9d0938516215620ccec55/tdt/tdt/io.hpp
  static bool readFile(std::string &out, const std::string &fname) {
    std::ifstream ifs(fname);
    if (!ifs.is_open()) {
      return false;
    }
    ifs.seekg(0, std::ios::end);
    out.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    out.assign((std::istreambuf_iterator<char>(ifs)),
               std::istreambuf_iterator<char>());
    return true;
  }
};
};

void RunServer() {
  std::string server_address("localhost:50051");
  simple::SimpleAddImpl service_simple;
  fileio::SqliteFileIOImpl service_fileio;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  builder.RegisterService(&service_simple);
  builder.RegisterService(&service_fileio);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char **argv) {
  std::cout << "Hello, World! server" << std::endl;
  RunServer();
  return 0;
}

