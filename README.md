# raft
[WIP] [Raft](https://raft.github.io/raft.pdf) Distributed Consensus in C++

## Installation
```
git clone https://github.com/kyuds/raft.git
cd raft && ./build.sh
```

### Installing gRPC
This project uses gRPC to communicate messages between each node. In order to install gRPC, refer to this [link](https://grpc.io/docs/languages/cpp/quickstart/). The build involves a `CMAKE_PREFIX_PATH`, meaning that it is necessary to set an environment variable to point to the path in which gRPC is installed (if there is a better way, please let us know).

```
# in ~/.zprofile (or linux/windows equivalent)
export GRPC_CPP_INSTALL=<path to installed grpc directory>
```

## Development Plan
- [ ] v0: leader election mechanism
- [ ] v1: std:string based log replication, fixed quorum
- [ ] v2: single value state machine replication
- [ ] v3: single value SMR + snapshot
- [ ] v4: changing quorum, single value smr
- [ ] v5: interface for generate state machine
- [ ] v6: KV store example?
