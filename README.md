# Dynamic-thread-migration

Abstract:
At any point of execution, if a node realises that it cannot serve the
process, the node will migrate this remaining part of process to
another node and compute the result. This applies to a multi-threaded
process as well. If a node cannot serve more threads, it will checkpoint
the entire process, continue execution of the threads that it can serve
and the remaining threads will be scheduled on another node.

dthread implementation: 
When a node sends a checkpoint image of a
process to another node it will create a dthread to do the same. As of
this moment dthread execution takes place in a single machine. Our
implementation would execute a dthread on multiple machines.
