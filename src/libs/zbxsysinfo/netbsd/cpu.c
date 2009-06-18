/*
** ZABBIX
** Copyright (C) 2000-2005 SIA Zabbix
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**/

#include "common.h"

#include "sysinfo.h"
#include "stats.h"

int	SYSTEM_CPU_NUM(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#ifdef HAVE_FUNCTION_SYSCTL_HW_NCPU
	/* NetBSD 3.1 i386; NetBSD 4.0 i386 */
	size_t	len;
	int	mib[] = {CTL_HW, HW_NCPU}, ncpu;
	char	mode[MAX_STRING_LEN];

	assert(result);

	init_result(result);

	if (num_param(param) > 1)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, mode, sizeof(mode)))
		*mode = '\0';

	if (*mode == '\0')
		zbx_snprintf(mode, sizeof(mode), "online");

	if (0 != strcmp(mode, "online"))
		return SYSINFO_RET_FAIL;

	len = sizeof(ncpu);

	if (0 != sysctl(mib, 2, &ncpu, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, ncpu);

	return SYSINFO_RET_OK;
#else
	return SYSINFO_RET_FAIL;
#endif /* HAVE_FUNCTION_SYSCTL_HW_NCPU */
}

int     SYSTEM_CPU_INTR(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#if defined(HAVE_UVM_UVMEXP2)
	/* NetBSD 3.1 i386; NetBSD 4.0 i386 */
	int			mib[] = {CTL_VM, VM_UVMEXP2};
	size_t			len;
	struct uvmexp_sysctl	v;

	assert(result);

	init_result(result);

	len = sizeof(struct uvmexp_sysctl);

	if (0 != sysctl(mib, 2, &v, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, v.intrs);

	return	SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif /* HAVE_UVM_UVMEXP2 */
}

int     SYSTEM_CPU_SWITCHES(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#if defined(HAVE_UVM_UVMEXP2)
	/* NetBSD 3.1 i386; NetBSD 4.0 i386 */
	int			mib[] = {CTL_VM, VM_UVMEXP2};
	size_t			len;
	struct uvmexp_sysctl	v;

	assert(result);

	init_result(result);

	len = sizeof(struct uvmexp_sysctl);

	if (0 != sysctl(mib, 2, &v, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, v.swtch);

	return	SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif /* HAVE_UVM_UVMEXP2 */
}

int	SYSTEM_CPU_UTIL(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
	char	cpuname[MAX_STRING_LEN],
		type[MAX_STRING_LEN],
		mode[MAX_STRING_LEN];
	int	cpu_num;

	assert(result);

	init_result(result);

	if (num_param(param) > 3)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, cpuname, sizeof(cpuname)))
		*cpuname = '\0';

	/* default parameter */
	if (*cpuname == '\0')
		zbx_snprintf(cpuname, sizeof(cpuname), "all");

	if (0 == strcmp(cpuname, "all"))
		cpu_num = 0;
	else {
		cpu_num = atoi(cpuname) + 1;

		if (cpu_num < 1 || cpu_num > collector->cpus.count)
			return SYSINFO_RET_FAIL;
	}

	if (0 != get_param(param, 2, type, sizeof(type)))
		*type = '\0';

	/* default parameter */
	if (*type == '\0')
		zbx_snprintf(type, sizeof(type), "user");

	if (0 != get_param(param, 3, mode, sizeof(mode)))
		*mode = '\0';

	/* default parameter */
	if (*mode == '\0')
		zbx_snprintf(mode, sizeof(mode), "avg1");

	if (!CPU_COLLECTOR_STARTED(collector)) {
		SET_MSG_RESULT(result, strdup("Collector is not started!"));
		return SYSINFO_RET_OK;
	}

	if (0 == strcmp(type, "idle")) {
		if (0 == strcmp(mode, "avg1"))		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].idle[ZBX_AVG1])
		else if (0 == strcmp(mode, "avg5"))	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].idle[ZBX_AVG5])
		else if (0 == strcmp(mode, "avg15"))	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].idle[ZBX_AVG15])
		else return SYSINFO_RET_FAIL;
	}
	else if (0 == strcmp(type, "nice"))
	{
		if (0 == strcmp(mode, "avg1")) 		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].nice[ZBX_AVG1])
		else if (0 == strcmp(mode, "avg5")) 	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].nice[ZBX_AVG5])
		else if (0 == strcmp(mode, "avg15"))	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].nice[ZBX_AVG15])
		else return SYSINFO_RET_FAIL;
	}
	else if (0 == strcmp(type, "user"))
	{
		if (0 == strcmp(mode, "avg1")) 		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].user[ZBX_AVG1])
		else if (0 == strcmp(mode, "avg5")) 	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].user[ZBX_AVG5])
		else if (0 == strcmp(mode, "avg15"))	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].user[ZBX_AVG15])
		else return SYSINFO_RET_FAIL;
	}
	else if (0 == strcmp(type, "system"))
	{
		if (0 == strcmp(mode, "avg1")) 		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].system[ZBX_AVG1])
		else if (0 == strcmp(mode, "avg5")) 	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].system[ZBX_AVG5])
		else if (0 == strcmp(mode, "avg15"))	SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].system[ZBX_AVG15])
		else return SYSINFO_RET_FAIL;
	}
	else
		return SYSINFO_RET_FAIL;

	return SYSINFO_RET_OK;
}

static int	get_cpuload(double *load1, double *load5, double *load15)
{
#ifdef HAVE_GETLOADAVG
	/* NetBSD 3.1 i386; NetBSD 4.0 i386 */
	double	load[3];

	if (-1 == getloadavg(load, 3))
		return SYSINFO_RET_FAIL;

	if (load1)
		*load1 = load[0];
	if (load5)
		*load5 = load[1];
	if (load15)
		*load15 = load[2];

	return SYSINFO_RET_OK;
#else
	return SYSINFO_RET_FAIL;
#endif /* HAVE_GETLOADAVG */
}

static int	SYSTEM_CPU_LOAD1(AGENT_RESULT *result)
{
	double	value;

	if (SYSINFO_RET_OK != get_cpuload(&value, NULL, NULL))
		return SYSINFO_RET_FAIL;

	SET_DBL_RESULT(result, value);

	return SYSINFO_RET_OK;
}

static int	SYSTEM_CPU_LOAD5(AGENT_RESULT *result)
{
	double	value;

	if (SYSINFO_RET_OK != get_cpuload(NULL, &value, NULL))
		return SYSINFO_RET_FAIL;

	SET_DBL_RESULT(result, value);

	return SYSINFO_RET_OK;
}

static int	SYSTEM_CPU_LOAD15(AGENT_RESULT *result)
{
	double	value;

	if (SYSINFO_RET_OK != get_cpuload(NULL, NULL, &value))
		return SYSINFO_RET_FAIL;

	SET_DBL_RESULT(result, value);

	return SYSINFO_RET_OK;
}

int	SYSTEM_CPU_LOAD(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#define CPU_FNCLIST struct cpu_fnclist_s
CPU_FNCLIST
{
	char	*mode;
	int	(*function)();
};

	CPU_FNCLIST fl[] =
	{
		{"avg1",	SYSTEM_CPU_LOAD1},
		{"avg5",	SYSTEM_CPU_LOAD5},
		{"avg15",	SYSTEM_CPU_LOAD15},
		{0,		0}
	};

	char	cpuname[MAX_STRING_LEN],
		mode[MAX_STRING_LEN];
	int	i;

	assert(result);

	init_result(result);

	if (num_param(param) > 2)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, cpuname, sizeof(cpuname)))
		*cpuname = '\0';

	/* default parameter */
	if (*cpuname == '\0')
		zbx_snprintf(cpuname, sizeof(cpuname), "all");

	if (0 != strncmp(cpuname, "all", sizeof(cpuname)))
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 2, mode, sizeof(mode)))
		*mode = '\0';

	/* default parameter */
	if (*mode == '\0')
		zbx_snprintf(mode, sizeof(mode), "avg1");

	for (i = 0; fl[i].mode != 0; i++)
		if (0 == strncmp(mode, fl[i].mode, MAX_STRING_LEN))
			return (fl[i].function)(result);

	return SYSINFO_RET_FAIL;
}
