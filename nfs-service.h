#pragma once

#include <sys/time.h>
#include "libnfs.h"
#include "libnfs-raw.h"
#include "libnfs-raw-mount.h"
#include "libnfs-raw-nfs.h"
#include "libnfs-raw-portmap.h"

extern struct service_proc nfs3_pt[22];
extern struct service_proc nfs3_mount_pt[6];
