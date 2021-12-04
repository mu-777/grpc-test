#include <iostream>
#include <grpcpp/security/credentials.h>
#include <grpcpp/create_channel.h>
#include "reflection.pb.h"
#include "reflection.grpc.pb.h"

#include <tuple>

void printHelp() {
  auto indent = "    ";
  auto exename = "reflection_client";
  std::cout << "" << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "" << std::endl;
  std::cout << indent << exename << " t <baseSide> <height>" << std::endl;
  std::cout << indent << exename << " r <side1> <side2>" << std::endl;
  std::cout << indent << exename << " s <side>" << std::endl;
  std::cout << indent << exename << " c <radius>" << std::endl;
}

int main(int argc, char **argv) {
  std::cout << "Hello, World! I'm reflection client\n" << std::endl;
  if (argc < 2) {
    std::cout << "Wrong args num" << std::endl;
    printHelp();
    return 0;
  }

  auto channel = grpc::CreateChannel("localhost:50051",
                                     grpc::InsecureChannelCredentials());
  auto stub = reflection::Area::NewStub(channel);
  reflection::AreaData res;

  std::tuple<std::string, int, std::function<reflection::ShapeData(char **)>> tuples[] = {
          {"t", 2, [&](char **a) {
            auto tri = new reflection::Triangle();
            tri->set_baseside(atof(a[2]));
            tri->set_height(atof(a[3]));
            reflection::ShapeData req;
            req.set_allocated_triangle(tri);
            return req;
          }},
          {"r", 2, [&](char **a) {
            auto rec = new reflection::Rectangle();
            rec->set_side1(atof(a[2]));
            rec->set_side2(atof(a[3]));
            reflection::ShapeData req;
            req.set_allocated_rectangle(rec);
            return req;
          }},
          {"s", 1, [&](char **a) {
            auto sqr = new reflection::Square();
            sqr->set_side(atof(a[2]));
            reflection::ShapeData req;
            req.set_allocated_square(sqr);
            return req;
          }},
          {"c", 1, [&](char **a) {
            auto cir = new reflection::Circle();
            cir->set_radius(atof(a[2]));
            reflection::ShapeData req;
            req.set_allocated_circle(cir);
            return req;
          }},
  };

  reflection::ShapeData req;
  for (auto t : tuples) {
    if (argv[1] != std::get<0>(t)) {
      continue;
    }
    if (argc != (std::get<1>(t) + 2)) {
      std::cout << "Need " << std::get<1>(t) << " input after " << argv[1] << std::endl;
      printHelp();
      return 0;
    }
    req = std::get<2>(t)(argv);
    break;
  }
  if (req.shape_case() == reflection::ShapeData::ShapeCase::SHAPE_NOT_SET) {
    std::cout << "Not found option: " << argv[1] << std::endl;
    printHelp();
    return 0;
  }

  grpc::ClientContext ctx;
  auto status = stub->CalcArea(&ctx, req, &res);
  if (!status.ok()) {
    std::cout << "RPC failed[" << status.error_code() << "]: " << status.error_message() << std::endl;
    return 0;
  }
  std::cout << "[Client] ReceivedRes: " << res.area() << std::endl;
  return 0;
}
