#include "common.h"
#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <stdint.h>

#define PTE vaddr_t
#define PTE_V(item) (item & 0x1)
#define PTE_R(item) (item & 0x2)
#define PTE_W(item) (item & 0x4)
#define PTE_X(item) (item & 0x8)
#define PTE_PPN(item) (item >> 12 & 0xFFFFF)
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  rtlreg_t satp = cpu.csr.satp;
  vaddr_t page_dir_base = satp << 12;

  uint32_t vpn_1 = (vaddr >> 22) & 0x3FF;
  uint32_t vpn_0 = (vaddr >> 12) & 0x3FF;
  uint32_t offset = vaddr & 0xFFF;

  // 一级页表
  PTE page_dir_entry = page_dir_base + vpn_1 * 4;
  word_t page_dir_entry_val = paddr_read(page_dir_entry, 4);
  if(!PTE_V(page_dir_entry_val)) assert(0);

  // 二级页表
  vaddr_t page_table_base = PTE_PPN(page_dir_entry_val) << 12;
  PTE page_table_entry = page_table_base + vpn_0 * 4;
  word_t page_table_entry_val = paddr_read(page_table_entry, 4);
  if(!PTE_V(page_table_entry_val)) assert(0);

  // 根据访存类型检查权限位
  switch(type){
    case MEM_TYPE_IFETCH: if(PTE_X(page_table_entry_val) == 0) assert(0);break;
    case MEM_TYPE_READ: if(PTE_R(page_table_entry_val) == 0) assert(0);break;
    case MEM_TYPE_WRITE: if(PTE_W(page_table_entry_val) == 0) assert(0);break;
    default: assert(0);break;
  }

  // 计算物理地址
  paddr_t ppn = PTE_PPN(page_table_entry_val) << 12;
  paddr_t paddr = ppn | offset;

  // 检查物理地址是否和虚拟地址相等（恒等映射）
  assert(paddr == vaddr);

  return paddr;

}
