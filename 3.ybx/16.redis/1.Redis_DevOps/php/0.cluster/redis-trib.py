#!/usr/bin/env python
# -*- coding: utf-8 -*-

import redistrib.command

# start cluster at node 127.0.0.1:7000
#redistrib.command.start_cluster('127.0.0.1', 7000)

## start cluster on multiple nodes, all the slots will be shared among them
## the argument is a list of (HOST, PORT) tuples
## for example, the following call will start a cluster on 127.0.0.1:7000 and 127.0.0.1:7001
#redistrib.command.start_cluster_on_multi([('127.0.0.1', 7000), ('127.0.0.1', 7001)])
#
## add node 127.0.0.1:7001 to the cluster as a master
#redistrib.command.join_cluster('127.0.0.1', 7000, '127.0.0.1', 7001)
#
## add node 127.0.0.1:7002 to the cluster as a slave to 127.0.0.1:7000
#redistrib.command.replicate('127.0.0.1', 7000, '127.0.0.1', 7002)
#
## just add node 127.0.0.1:7001 to the cluster, not specifying its role
## could call migrate_slot(s) on it later, so that it becomes a master
#redistrib.command.join_no_load('127.0.0.1', 7000, '127.0.0.1', 7001)
#
## remove node 127.0.0.7000 from the cluster
#redistrib.command.quit_cluster('127.0.0.1', 7000)
#
## shut down the cluster
#redistrib.command.shutdown_cluster('127.0.0.1', 7001)
#
## fix a migrating slot in a node
#redistrib.command.fix_migrating('127.0.0.1', 7001)
#
## migrate slots; require source node holding the slots

# migrate slots #1, #2, #3 from 127.0.0.1:7001 to 127.0.0.1:7002
#redistrib.command.migrate_slots('127.0.0.1', 7001, '127.0.0.1', 7002, [1, 2, 3])

slots = []
for x in range(5001, 10000):
    slots.append(x)
redistrib.command.migrate_slots('127.0.0.1', 7002, '127.0.0.1', 7001, slots)

# rescue a failed cluster
# 127.0.0.1:7000 is one of the nodes that is still alive in the cluster
# and 127.0.0.1:8000 is the node that would take care of all failed slots
#redistrib.command.rescue_cluster('127.0.0.1', 7000, '127.0.0.1', 8000)
