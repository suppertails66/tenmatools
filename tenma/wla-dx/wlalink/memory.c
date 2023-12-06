
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "files.h"



extern unsigned char *rom, *rom_usage;
extern int romsize, banksize, section_overwrite;
extern int pc_bank, pc_full, pc_slot, pc_slot_max;
extern struct object_file *obj_tmp;

char mem_insert_action[MAX_NAME_LENGTH*3 + 1024];
int memory_file_id, memory_file_id_source, memory_line_number;



int mem_insert(int address, unsigned char data) {

  if (rom[address] == data) {
    rom_usage[address]++;
    return SUCCEEDED;
  }
  if (rom_usage[address] != 0) {
    fprintf(stderr, "%s: MEM_INSERT: Overwrite at $%x (old $%.2x new $%.2x).\n", get_file_name(memory_file_id), address, rom[address], data);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
  }

  rom_usage[address]++;
  rom[address] = data;

  return SUCCEEDED;
}


int mem_insert_allow_overwrite(int address, unsigned char data, unsigned int allowed_overwrites) {

  if (rom_usage[address] > allowed_overwrites)
    return mem_insert(address, data);

  rom_usage[address]++;
  rom[address] = data;

  return SUCCEEDED;
}


int mem_insert_ref(int address, unsigned char data) {

  if (address > romsize || address < 0) {
    fprintf(stderr, "%s: %s:%d: MEM_INSERT: Address $%x is out of the output range $0-$%x.\n",
	    get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), memory_line_number, address, romsize);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
    return FAILED;
  }
  if (rom_usage[address] > 1 && rom[address] != data) {
    if (memory_line_number != 0)
      fprintf(stderr, "%s: %s:%d: MEM_INSERT: Overwrite at $%x (old $%.2x new $%.2x).\n",
	      get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), memory_line_number, address, rom[address], data);
    else
      fprintf(stderr, "%s: %s:[WLA]: MEM_INSERT: Overwrite at $%x (old $%.2x new $%.2x).\n",
	      get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), address, rom[address], data);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
  }

  rom_usage[address]++;
  rom[address] = data;

  return SUCCEEDED;
}


int mem_insert_ref_13bit_high(int address, unsigned char data) {

  if (address > romsize || address < 0) {
    fprintf(stderr, "%s: %s:%d: MEM_INSERT: Address $%x is out of the output range $0-$%x.\n",
	    get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), memory_line_number, address, romsize);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
    return FAILED;
  }
  if (rom_usage[address] > 1 && (rom[address] & 0x1F) != data) {
    if (memory_line_number != 0)
      fprintf(stderr, "%s: %s:%d: MEM_INSERT: Overwrite at $%x (old $%.2x new $%.2x).\n",
	      get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), memory_line_number, address, rom[address], data);
    else
      fprintf(stderr, "%s: %s:[WLA]: MEM_INSERT: Overwrite at $%x (old $%.2x new $%.2x).\n",
	      get_file_name(memory_file_id), get_source_file_name(memory_file_id, memory_file_id_source), address, rom[address], data);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
  }

  rom_usage[address]++;
  rom[address] = (rom[address] & 0xE0) | data;

  return SUCCEEDED;
}


int mem_insert_pc(unsigned char d, int slot_current, int bank_current) {

  if (pc_slot >= pc_slot_max) {
    fprintf(stderr, "%s: MEM_INSERT: Overflow from slot %d.\n", get_file_name(memory_file_id), slot_current);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
    return FAILED;
  }
  if (pc_bank >= banksize) {
    fprintf(stderr, "%s: MEM_INSERT: Overflow from bank %d.\n", get_file_name(memory_file_id), bank_current);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
    return FAILED;
  }
  if (rom_usage[pc_full] != 0 && rom[pc_full] != d && section_overwrite == OFF) {
    fprintf(stderr, "%s: MEM_INSERT: Overwrite at $%x, old $%x new $%x\n", get_file_name(memory_file_id), pc_full, rom[pc_full], d);
    if (mem_insert_action[0] != 0)
      fprintf(stderr, "   ^ %s\n", mem_insert_action);
  }
  
  if (section_overwrite == OFF)
    rom_usage[pc_full]++;
  rom[pc_full] = d;
  pc_slot++;
  pc_bank++;
  pc_full++;

  return SUCCEEDED;
}
