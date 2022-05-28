#include "syslib.h"

int sys_schedctl(uint32_t flags, endpoint_t proc_ep, int priority, int quantum,
	int cpu, int bucket)
{
	message m;

	m.m_lsys_krn_schedctl.flags = flags;
	m.m_lsys_krn_schedctl.endpoint = proc_ep;
	m.m_lsys_krn_schedctl.priority = priority;
	m.m_lsys_krn_schedctl.quantum = quantum;
	m.m_lsys_krn_schedctl.cpu = cpu;
	m.m_lsys_krn_schedctl.bucket = bucket;

	return(_kernel_call(SYS_SCHEDCTL, &m));
}
