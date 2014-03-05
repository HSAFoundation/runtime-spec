kernel &__vector_copy_kernel(kernarg_u32 %arg_p0, 
    kernarg_u32 %arg_p1,
    kernarg_u32 %arg_p2)
{
  workitemaid $s0, 0;
  shl_u32 $s0, $s0, 2;
  ld_kernarg_u32 $s1, [%arg_p1];
  add_u32 $s1, $s1, $s0;
  ld_kernarg_u32 $s2, [%arg_p0];
  add_u32 $s0, $s2, $s0;
  ld_global_u32 $s0, [$s0];
  st_global_u32 $s0, [$s1];
  ret;
};

//Create an AQL packet.
hsa_aql_dispatch_packet_t aql_packet;
uint64_t* kernel_arguments =
reinterpret_cast<uint64_t *>(malloc(argument_size));

memset(kernel_arguments, 0, argument_size);

//The ABI dictates that all arguments must be aligned to a 64bit boundary,
//hence the typecast.
kernel_arguments[0] = (uint64_t)a;
kernel_arguments[1] = (uint64_t)b;
kernel_arguments[2] = (uint64_t)size;

//Set the address for kernel argument within the AQL packet.
aql_packet.kernarg_address = (uint64_t)kernel_arguments;
