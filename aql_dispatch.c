typedef struct HsaAqlDispatchPacket {
  /**
   * AQL_FORMAT 0: Initial AQL Packet definition
   */
  uint16_t format : 8;

  /** wait on all the packets before this packet in the same queue to complete
   */
  uint16_t wait_on_older_completion : 1;

  /** Flush destination caches on completion of dispatch but prior to final
   * completion synchronization.
   */
  uint16_t flush_destination_cache : 1;

  /**
   * Invalidate source caches on acquire prior to dispatch.
   */
  uint16_t invalidate_source_cache : 1;

  /** 0->Cache actions all but last level cache; 1->Cache operations on last
   * level cache to memory -- Any range-based operations will be
   * implementation-dependent
   */
  uint16_t cache_level_action : 1;

  /**
   * Indicates if system should report time points when kernel dispatch
   * packet was parsed, started and completed its execution. If set info
   * on time points will be available in HsaCompletionObject.
   */
  uint16_t performance_counter_sampling : 1;

  /**
   * 1,2,3 valid dimensions
   */
  uint16_t dimensions : 2;

  uint16_t reserved: 1;

  /**
   * The range (in work-items) for each workgroup.
   */
  uint16_t workgroup_size[3];

  /**
   * Pointer to implementation-specific kernel object information
   */
  uint64_t kernel_object_address;

  /**
   * Pointer to HsaCompletionObject, this object is updated
   * with status of dispatch upon its completion.
   */
  uint64_t completion_object_address;

  /**
   * Pointer to a buffer containing the arguments for the kernel. All
   * arguments must be aligned on a 64bit boundary.
   */
  uint64_t kernel_args_address;

  /**
   * Dispatch ID assigned by user.  Must be unique for each queue.
   */
  uint64_t dispatch_id;

  /**
   * The range (in work-items) for the NDRange or global work dimensions
   */
  uint32_t nd_range_size[3];

  /**
   * Size in bytes of group segment allocation request (per work-group)
   */
  uint32_t work_group_segment_size_bytes;

  /**
   * Size in bytes of private/spill/arg segment (per work-item)
   */
  uint32_t private_segment_size_bytes;

} HsaAqlDispatchPacket;

