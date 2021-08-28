#include <iostream>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "simple.pb.h"
#include "simple.grpc.pb.h"

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

void RunServer() {
  std::string server_address("localhost:50051");
  simple::SimpleAddImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
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

