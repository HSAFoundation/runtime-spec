// read the current queue write offset via intrinsic
write_index = hsa_queue_get_write_index(q);

// wait until the queue is no longer full.
while (write_index == read_index + size);

// atomically bump the WriteOffset via intrinsic
if (hsa_queue_cas_write_index(q, write_index, write_index + 1) == write_index)
{
  // calculate index
  uint32_t index = write_index & (size - 1);

  // copy over the packet, the format field is INVALID
  memcpy(q->base_address + index, pkt);

  // update format field with release semantics
  q->base_address[index].hdr.format.store(DISPATCH, std::memory_order_release);

  // ring doorbell with release semantics (could also amortize over multiple packets)
  ring_doorbell(write_index + 1);
}

