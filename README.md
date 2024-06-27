# raft
[WIP] [Raft](https://raft.github.io/raft.pdf) Distributed Consensus in C++

## Installation
```
git clone https://github.com/kyuds/raft.git
cd raft && ./setup.sh
```

### Installing gRPC
This project uses gRPC to communicate messages between each node. In order to install gRPC, refer to this [link](https://grpc.io/docs/languages/cpp/quickstart/). The build involves a `CMAKE_PREFIX_PATH`, meaning that it is necessary to set an environment variable to point to the path in which gRPC is installed (if there is a better way, please let us know).

```
# in ~/.zprofile (or linux/windows equivalent)
export GRPC_CPP_INSTALL=<path to installed grpc directory>
```

## Running Election
Open five new terminal windows and run the following command, one per window. Each will be a separate Raft node that is able to elect leaders.
```
build/bin/raft node1 localhost:10001
build/bin/raft node2 localhost:10002
build/bin/raft node3 localhost:10003
build/bin/raft node4 localhost:10004
build/bin/raft node5 localhost:10005
```

## Development Plan
- [X] v0: leader election mechanism
- [ ] v1: std:string based log replication, fixed quorum
- [ ] v2: single value state machine replication
- [ ] v3: single value SMR + snapshot
- [ ] v4: changing quorum, single value smr
- [ ] v5: interface for generate state machine
- [ ] v6: KV store example?
