#include <iostream>
#include <fstream>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "simple.pb.h"
#include "simple.grpc.pb.h"
#include "fileio.pb.h"
#include "fileio.grpc.pb.h"

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
  ::grpc::Status RequestSqlite(::grpc::ServerContext *context, const SqliteRequest *req,
                               SqliteResponse *res) override {
    std::cout << "[Server] ReceivedReq: " << req->filepath() << std::endl;

    std::string sqlite;
    if (readFile(sqlite, req->filepath())) {
      res->set_status(0);
      res->set_sqlite(sqlite);
    } else {
      std::cout << "[Server] Not found: " << req->filepath() << std::endl;
      res->set_status(1);
      res->set_allocated_sqlite(nullptr);
    }
    return ::grpc::Status::OK;
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
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service_simple" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service_simple.
  builder.RegisterService(&service_simple);
  builder.RegisterService(&service_fileio);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char **argv) {
  std::cout << "Hello, World! server" << std::endl;
  RunServer();
  return 0;
}

