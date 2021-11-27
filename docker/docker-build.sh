#!/usr/bin/env bash
pushd $(dirname $0)
docker build -t mu-777/grpc-cpp -f ./Dockerfile.grpc .
popd