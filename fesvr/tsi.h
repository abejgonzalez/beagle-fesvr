#ifndef __SAI_H
#define __SAI_H

#include "htif.h"
#include "context.h"

#include <string>
#include <vector>
#include <deque>
#include <stdint.h>

#define SAI_CMD_READ 0
#define SAI_CMD_WRITE 1

#define SAI_ADDR_CHUNKS 2
#define SAI_LEN_CHUNKS 2

class tsi_t : public htif_t
{
 public:
  tsi_t(int argc, char** argv);
  virtual ~tsi_t();

  bool data_available();
  void send_word(uint32_t word);
  uint32_t recv_word();
  void switch_to_host();

  uint32_t in_bits() { return in_data.front(); }
  bool in_valid() { return !in_data.empty(); }
  bool out_ready() { return true; }
  void tick(bool out_valid, uint32_t out_bits, bool in_ready);

  // helper method to more easily write SCRs
  void write_scr(addr_t taddr, uint32_t value) {
    uint32_t value_loc = value;
    write_chunk(taddr, sizeof(uint32_t), &value_loc);
  }

  uint32_t read_scr(addr_t taddr) {
    uint32_t dest;
    read_chunk(taddr, sizeof(uint32_t), &dest);
    return dest;
  }

 protected:
  void reset() override;
  void start_program() override;
  void read_chunk(addr_t taddr, size_t nbytes, void* dst) override;
  void write_chunk(addr_t taddr, size_t nbytes, const void* src) override;
  void switch_to_target();

  size_t chunk_align() { return 4; }
  size_t chunk_max_size() { return 1024; }

  int get_ipi_addrs(addr_t *addrs);

 private:
  context_t host;
  context_t* target;
  std::deque<uint32_t> in_data;
  std::deque<uint32_t> out_data;

  void push_addr(addr_t addr);
  void push_len(addr_t len);

  static void host_thread(void *tsi);
};

#endif
