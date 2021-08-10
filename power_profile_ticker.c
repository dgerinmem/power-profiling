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


uint32_t tickPw(struct dpu_rank_t *rank)
{
  struct dpu_response_powermes pw;
  dpu_custom_for_rank(
    rank, DPU_COMMAND_TICK_POWERMES,
    &pw);
  return pw.power_mean_uw;
}


#define TICKPW(rank__)\
  pw = tickPw(rank__);\
  printf("pw %d uW \n", pw )


int main() {
  struct dpu_set_t set, dpu;
  uint32_t checksum;
  uint32_t pw;
  printf("start program\n");
  printf("dpu_alloc\n");
  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  struct dpu_rank_t *rank_ = dpu_rank_from_set(set);
  TICKPW(rank_);
  printf("dpu_load\n");
  TICKPW(rank_);
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  TICKPW(rank_);
  printf("populate_mram\n");
  TICKPW(rank_);
  populate_mram(set);
  TICKPW(rank_);

  printf("dpu_launch\n");
  TICKPW(rank_);
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  TICKPW(rank_);
  DPU_FOREACH(set, dpu) {
    printf("dpu_copy_from\n");
    TICKPW(rank_);
    DPU_ASSERT(dpu_copy_from(dpu, "checksum", 0, (uint8_t *)&checksum,
                             sizeof(checksum)));
    printf("dpu_launch\n");
    TICKPW(rank_);
    printf("Computed checksum = 0x%08x\n", checksum);
  }
  DPU_ASSERT(dpu_free(set));
  return 0;
}
