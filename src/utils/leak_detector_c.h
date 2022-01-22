
#ifndef  LEAK_DETECTOR_C_H
#define  LEAK_DETECTOR_C_H

#define  FILE_NAME_LENGTH   	   256
#define  OUTPUT_FILE_MASTER			   "./leak_info_master.txt"
#define  OUTPUT_FILE_USERS			   "./leak_info_users.txt"
#define  OUTPUT_FILE_NODES			   "./leak_info_nodes.txt"
#define  malloc(size) 	    	   	xmalloc (size, __FILE__, __LINE__)
#define  calloc(elements, size)  	xcalloc (elements, size, __FILE__, __LINE__)
#define  free(mem_ref) 		  	 	xfree(mem_ref)


/*

THIS NEEDS TO BE ROMED AFTER, IM JUST INCLUDING TO FIND MEMORY LEAKS

*/
struct _MEM_INFO
{
	void			*address;
	unsigned int	size;
	char			file_name[FILE_NAME_LENGTH];
	unsigned int	line;
};
typedef struct _MEM_INFO MEM_INFO;

struct _MEM_LEAK {
	MEM_INFO mem_info;
	struct _MEM_LEAK * next;
};
typedef struct _MEM_LEAK MEM_LEAK;

void add(MEM_INFO alloc_info);
void erase(unsigned pos);
void clear(void);

void * xmalloc(unsigned int size, const char * file, unsigned int line);
void * xcalloc(unsigned int elements, unsigned int size, const char * file, unsigned int line);
void xfree(void * mem_ref);

void add_mem_info (void * mem_ref, unsigned int size,  const char * file, unsigned int line);
void remove_mem_info (void * mem_ref);
void report_mem_leak(void);
void report_mem_leak_nodes(void);
void report_mem_leak_users(void);

#endif
