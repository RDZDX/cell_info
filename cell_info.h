#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vmcell.h"
#include "vmtimer.h"
#include "string.h"
#include <time.h>
#include "console.h"

VMINT layer_hdl[1];

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void timer(int a);
void create_app_txt_path(VMWSTR text, VMSTR extt);
void checkFileExist(void);
static void fill_screen(void);

#endif

