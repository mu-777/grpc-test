#include <iostream>
#include <fstream>
#include <grpcpp/security/credentials.h>
#include <grpcpp/create_channel.h>
#include "fileio.pb.h"
#include "fileio.grpc.pb.h"

int main(int argc, char **argv) {
  std::cout << "Hello, World! I'm  fileio client" << std::endl;
  if (argc < 1){
    std::cout << "Must input 1 args" << std::endl;
    return 0;
  }
  auto path = argv[1];

  auto channel = grpc::CreateChannel("localhost:50051",
                                     grpc::InsecureChannelCredentials());
  auto stub = fileio::FileIO::NewStub(channel);

  fileio::SqliteRequest req;
  req.set_filepath(path);

  grpc::ClientContext ctx;
  fileio::SqliteResponse res;
  auto status = stub->RequestSqlite(&ctx, req, &res);
  if (!status.ok()) {
    std::cout << "RPC failed(" << status.error_code() << "): " << status.error_message() << std::endl;
    return 1;
  }
  std::cout << "[Client] ReceivedRes: " << res.status() << std::endl;
  if (res.status() != 0) {
    std::cout << "Fail to get file from server: " << res.status() << std::endl;
    return 1;
  }
  std::ofstream of("received.sqlite", std::ios::out|std::ios::binary|std::ios::trunc);
  if (!of) {
    std::cout << "Fail to open file to write" << std::endl;
    return 1;
  }
  of.write(res.sqlite().c_str(), res.sqlite().size());
  return 0;
}

