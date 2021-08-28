cd $(dirname $0)
docker build -t mu-777/grpc-cpp -f ./Dockerfile.grpc .
docker build -t mu-777/grpc-cpp-dev -f ./Dockerfile.grpc-dev .