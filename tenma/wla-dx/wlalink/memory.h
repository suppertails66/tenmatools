
int mem_insert_allow_overwrite(int address, unsigned char data, unsigned int allowed_overwrites);
int mem_insert(int address, unsigned char data);
int mem_insert_pc(unsigned char d, int slot_current, int bank_current);
int mem_insert_ref(int address, unsigned char data);
int mem_insert_ref_13bit_high(int address, unsigned char data);

