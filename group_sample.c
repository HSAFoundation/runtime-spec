 // ... assume setup for component, queue 
 
 // User dynamically requests 3 group allocations of 256, 384, and 500 bytes.
 // These can be specified at launch-time.
 int size1 = 256;
 int size2 = 384;
 int size3 = 500;
 
hsa_aql_dispatch_packet_t aql_packet;
uint64_t* kernel_arguments =
reinterpret_cast<uint64_t *>(malloc(argument_size));

memset(kernel_arguments, 0, argument_size);

// Copy parameters into the AQL packet, computing relative offsets:
kernel_arguments[0] = kernel_object_ptr->workgroup_group_segment_size_bytes;
 
kernel_arguments[1] = kernel_object_ptr->workgroup_group_segment_size_bytes+size1;
 
kernel_arguments[2] = kernel_object_ptr->workgroup_group_segment_size_bytes+size1+size2;

 // Set the total group memory size:
aql_packet.group_segment_size_bytes = kernel.group_memory_usage + size1 +
size2 + size3;
