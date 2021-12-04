#include <iostream>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include "simple/simple_server.h"
#include "reflection/reflection_server.h"

void RunServer() {
  std::string server_address("localhost:50051");
  simple::SimpleAddImpl service_simple;
  reflection::ReflectionImpl service_reflection;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service_simple);
  builder.RegisterService(&service_reflection);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char **argv) {
  std::cout << "Hello, World! server" << std::endl;
  RunServer();
  return 0;
}

