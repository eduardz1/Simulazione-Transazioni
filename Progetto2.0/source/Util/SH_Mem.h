#include "../include/Common.h"
#include <sys/types.h>
#include <sys/stat.h>

/*PROTOTYPE'S FUNCTION'S */
void Sh_Mem_Def();
void Sh_CONFIG_Rd();
void Sh_Mem_Wr();
void Sh_Mem_Atc();
void Sh_Mem_Det();

/*
struct SH_Mem_Data
{
unsigned Mem_ind;
pid_t NodePid[];
pid_t UserPid[];
};

*/

struct msqid_dis {
  struct ipc_perm mesg_perm;
  time_t msg_stime;
  time_t msg_rtime;
  time_t msg_ctime;
  msgqnum_t msg_qnum;
  msglen_t msg_qbytes;
  pid_t msg_lspid;
  pid_t msg_lrpid;
};
