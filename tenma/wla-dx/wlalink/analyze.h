
int add_reference(struct reference *r);
int add_stack(struct stack *sta);
int add_label(struct label *l);
int add_section(struct section *s);
int free_section(struct section *s);
int find_label(char *name, struct section *context, struct label **l);
int parse_data_blocks(void);
int obtain_source_file_names(void);
int obtain_memorymap(void);
int obtain_rombankmap(void);
int obtain_rombanks(void);
int collect_dlr(void);
int merge_sections(void);
int clean_up_dlr(void);
