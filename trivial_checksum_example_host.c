/* Communication with a DPU via the MRAM. */
/* Populate the MRAM with a collection of bytes and request the DPU to */
/* compute the checksum. */

#include <assert.h>
#include <dpu.h>
#include <dpu_custom.h>
#include <dpu_management.h>
#include <dpu_types.h>
#include <stdint.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "trivial_checksum_example"
#endif

/* Size of the buffer for which we compute the checksum: 64KBytes. */
#define BUFFER_SIZE (1 << 16)

void populate_mram(struct dpu_set_t set) {
  uint8_t buffer[BUFFER_SIZE];

  for (int byte_index = 0; byte_index < BUFFER_SIZE; byte_index++) {
    buffer[byte_index] = (uint8_t)byte_index;
  }
  DPU_ASSERT(dpu_broadcast_to(set, "buffer", 0, buffer, BUFFER_SIZE,
                              DPU_XFER_DEFAULT));
}

int main() {
  struct dpu_set_t set, dpu;
  uint32_t checksum;

  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  populate_mram(set);

  struct dpu_rank_t *rankRef = dpu_rank_from_set(set);

  printf("Retreive Rank Ptr 0x%lx \n", rankRef);
  printf("Rank Id  %d \n", dpu_get_rank_id(rankRef));
  // printf("Rank Type  %d \n", (dpu_type_t)((struct
  // dpu_rank_t*)(rankRef)->type)); printf("Retreive Rank from DPU SET : dpu ID
  // ; %x ,  nr_rank %d \n", dpuRef, 0);
  //dpu_custom_for_rank(
  //    rankRef, DPU_COMMAND_SAMPLE_VDD,
  //    NULL); //(dpu_custom_command_args_t)DPU_EVENT_LOAD_PROGRAM);

  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_copy_from(dpu, "checksum", 0, (uint8_t *)&checksum,
                             sizeof(checksum)));
    printf("Computed checksum = 0x%08x\n", checksum);
  }
  DPU_ASSERT(dpu_free(set));
  return 0;
}
