#ifndef GRPC_TEST_REFLECTION_SERVER_H
#define GRPC_TEST_REFLECTION_SERVER_H

#include "reflection.pb.h"
#include "reflection.grpc.pb.h"

#include <functional>

namespace reflection {
class ReflectionImpl final : public Area::Service {

  grpc::Status CalcArea(grpc::ServerContext *context,
                        const ShapeData *request,
                        AreaData *response) override {
    std::cout << "Received data" << std::endl;
    auto s = CalcAreaImpl(request);
    assert(s == CalcAreaImpl2(request));
    std::cout << "Calced: " << s << std::endl;
    response->set_area(s);
    return ::grpc::Status::OK;
  }

  float CalcAreaImpl(const ShapeData *sd) {
    switch (sd->shape_case()) {
      case ShapeData::ShapeCase::kTriangle:
        return CalcTriangleArea(sd->triangle());
      case ShapeData::ShapeCase::kRectangle:
        return CalcRectangleArea(sd->rectangle());
      case ShapeData::ShapeCase::kSquare:
        return CalcSquareArea(sd->square());
      case ShapeData::ShapeCase::kCircle:
        return CalcCircleArea(sd->circle());
      case ShapeData::ShapeCase::SHAPE_NOT_SET:
        std::cout << "SHAPE_NOT_SET" << std::endl;
        return 0;
    }
    return 0;
  }

  float CalcAreaImpl2(const ShapeData *sd) {
    std::unordered_map<ShapeData::ShapeCase, std::function<float(const ShapeData *)>> switcher = {
            {ShapeData::ShapeCase::kTriangle,
                    [&](const ShapeData *sd) { return CalcTriangleArea(sd->triangle()); }},
            {ShapeData::ShapeCase::kRectangle,
                    [&](const ShapeData *sd) { return CalcRectangleArea(sd->rectangle()); }},
            {ShapeData::ShapeCase::kSquare,
                    [&](const ShapeData *sd) { return CalcSquareArea(sd->square()); }},
            {ShapeData::ShapeCase::kCircle,
                    [&](const ShapeData *sd) { return CalcCircleArea(sd->circle()); }},
    };
    return switcher.at(sd->shape_case())(sd);
  }

  template<class ProtoMsgType>
  static ProtoMsgType parse(const ShapeData sd) {
    ProtoMsgType ret;
    auto oneof_param_name = "shape";
    auto oneof_desc = sd.GetDescriptor()->FindOneofByName(oneof_param_name);
    auto field_desc = sd.GetReflection()->GetOneofFieldDescriptor(sd, oneof_desc);
    ret.ParseFromString(sd.GetReflection()->GetMessage(sd, field_desc).SerializeAsString());
    return ret;
  }

  std::function<float(Triangle)> CalcTriangleArea = [](Triangle t) {
    return t.baseside() * t.height() * 0.5f;
  };

  std::function<float(Rectangle)> CalcRectangleArea = [](Rectangle r) {
    return r.side1() * r.side2();
  };

  std::function<float(Square)> CalcSquareArea = [](Square s) {
    return s.side() * s.side();
  };

  std::function<float(Circle)> CalcCircleArea = [](Circle c) {
    static const float PI = 3.14159265f;
    return c.radius() * c.radius() * PI;
  };
};
};

#endif //GRPC_TEST_REFLECTION_SERVER_H
