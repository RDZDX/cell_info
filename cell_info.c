#include "cell_info.h"

static VMINT timer_id = -1;
VMBOOL flightMode = VM_FALSE;
VMBOOL swich = VM_FALSE;
VMUINT8* layer_buf = 0;
VMINT screen_w = 0;
VMINT screen_h = 0;

extern int max_lines;
extern int c_y;
//extern int c_x;
char px[1000] = {0};

VMWCHAR file_pathw[100] = {0};
VMUINT nwrite;

extern VMUINT8* get_buf() {
	return layer_buf;
}

void vm_main(void) {

	layer_hdl[0] = -1;	

	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();
	layer_hdl[0] = vm_graphic_create_layer(0, 0, screen_w, screen_h, -1);
	layer_buf = vm_graphic_get_layer_buffer(layer_hdl[0]);
	vm_graphic_set_clip(0, 0, screen_w, screen_h);
	fill_screen();

	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);

	console_init(screen_w, screen_h, (VMUINT16*)layer_buf);

        if (vm_sim_card_count() == 99) {
           flightMode = VM_TRUE;
	   cprintf("Please turn Flight mode off !\n");
        } else {
	   vm_cell_open();
	   timer_id = vm_create_timer_ex(1000, timer);
        }

}

void handle_sysevt(VMINT message, VMINT param) {

	switch (message) {
	case VM_MSG_CREATE:
	case VM_MSG_ACTIVE:
		break;
		
	case VM_MSG_PAINT:
                vm_switch_power_saving_mode(turn_off_mode);
		break;
		
	case VM_MSG_INACTIVE:
		break;	

	case VM_MSG_QUIT:
		if( layer_hdl[0] != -1 )
			vm_graphic_delete_layer(layer_hdl[0]);
                vm_cell_close();
		break;	
	}
}


extern void flush_layer() {
	vm_graphic_flush_layer(layer_hdl, 1);
}

void handle_keyevt(VMINT event, VMINT keycode) {

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
           if (layer_hdl[0] != -1) {
              vm_graphic_delete_layer(layer_hdl[0]);
              layer_hdl[0] = -1;
           }
        vm_cell_close();
        vm_exit_app();
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_OK) { //VM_KEY_LEFT_SOFTKEY VM_KEY_OK
           if (flightMode == VM_FALSE) { timer_id = vm_create_timer_ex(1000, timer); }
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
           if (flightMode == VM_FALSE) {

              create_app_txt_filenamex(file_pathw, "txt");

              VMFILE f = vm_file_open(file_pathw, MODE_APPEND, FALSE);
              if (f < 0){
                 f = vm_file_open(file_pathw, MODE_CREATE_ALWAYS_WRITE, FALSE);
              }
              vm_file_write(f, px, strlen(px), &nwrite);
              vm_file_write(f, "\n",1, &nwrite);
              vm_file_close(f);

              strcpy(px, "");

              vm_ucs2_to_ascii(px, sizeof(px), file_pathw);

              if (c_y == max_lines - 1 && swich == VM_FALSE) {
                 cprintf("Data saved to %s", px);
                 swich = VM_TRUE;
              } else if(c_y < max_lines - 1) {
                 cprintf("Data saved to %s\n", px);
              } else {
                cprintf("\nData saved to %s", px);
              }

           }
	}

}

void timer(int a){

	VMUINT nwrite;
	VMWCHAR file1[1000];
        int i;
        char t[20] = {0};
        struct vm_time_t curr_time;
        char p[1000] = {0};
        char k[1000] = {0};

        vm_cell_info_struct* info = vm_cell_get_cur_cell_info(); //gauti prisijungtos stoties informacija
	vm_cell_info_struct** bs = vm_cell_get_nbr_cell_info(); //gauti kaimyniniu stociu informacija

        vm_get_time(&curr_time);

        strcpy(px, "");

        if(c_y == max_lines - 1 && swich == VM_FALSE) { // 0 <= 29-1 ??
          sprintf(p, "%04d.%02d.%02d %02d:%02d:%02d", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
          swich = VM_TRUE;
        } else if(c_y < max_lines - 1) {
          sprintf(p, "%04d-%02d-%02d %02d:%02d:%02d\n", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
        } else {
          sprintf(p, "\n%04d.%02d.%02d %02d:%02d:%02d", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
        }

        sprintf(k, "%04d-%02d-%02d %02d:%02d:%02d\n", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);

        strcat(px, k);

        cprintf(p);

        if(c_y == max_lines - 1 && swich == VM_FALSE) {
          sprintf(p, "cellid:%d, lac:%d, signal:%d", info->ci, info->lac, info->rxlev-110);
          swich = VM_TRUE;
        } else if(c_y < max_lines - 1){
          sprintf(p, "cellid:%d, lac:%d, signal:%d\n", info->ci, info->lac, info->rxlev-110);
        } else {
          sprintf(p, "\ncellid:%d, lac:%d, signal:%d", info->ci, info->lac, info->rxlev-110);
        }

        //strcat(px, p);

        cprintf(p);

        sprintf(k, "ARFCN:%d, BSIC:%d, Received signal level:%d, MCC:%d, MNC:%d, LAC:%d, CI:%d\n", info->arfcn, info->bsic, info->rxlev, info->mcc, info->mnc, info->lac, info->ci);

        strcat(px, k);

	int n = *vm_cell_get_nbr_num();
	for(i = 0; i<n; ++i) {
           //cprintf("cellid:%d, lac:%d, signal:%d\n", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
           sprintf(k, "ARFCN:%d, BSIC:%d, Received signal level:%d, MCC:%d, MNC:%d, LAC:%d, CI:%d\n", bs[i]->arfcn, bs[i]->bsic, bs[i]->rxlev, bs[i]->mcc, bs[i]->mnc, bs[i]->lac, bs[i]->ci);
           strcat(px, k);

           if(c_y == max_lines - 1 && swich == VM_FALSE) {
             sprintf(p, "cellid:%d, lac:%d, signal:%d", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
             swich = VM_TRUE;
           } else if(c_y < max_lines - 1){
            sprintf(p, "cellid:%d, lac:%d, signal:%d\n", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
           } else {
            sprintf(p, "\ncellid:%d, lac:%d, signal:%d", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
           }

           //strcat(px, p);

           cprintf(p);
	}

	if (timer_id != -1) {
            vm_delete_timer_ex(timer_id);
            timer_id = -1;
        }

        //vm_cell_close();
}

void create_app_txt_path(VMWSTR text, VMSTR extt) {

    VMWCHAR fullPath[100];
    VMWCHAR wfile_extension[8];

    vm_get_exec_filename(fullPath);
    vm_ascii_to_ucs2(wfile_extension, 8, extt);
    vm_wstrncpy(text, fullPath, vm_wstrlen(fullPath) - 3);
    vm_wstrcat(text, wfile_extension);

}

static void fill_screen(void) {

    vm_graphic_color color;
    color.vm_color_565 = VM_COLOR_BLACK;
    vm_graphic_setcolor(&color);
    vm_graphic_fill_rect_ex(layer_hdl[0], 0, 0, screen_w, screen_h);
    vm_graphic_flush_layer(layer_hdl, 1);
}

void create_app_txt_filenamex(VMWSTR text, VMSTR extt) {

    VMINT drv;
    VMWCHAR fullPath[100] = {0};
    VMWCHAR appName[100] = {0};
    VMWCHAR wfile_extension[8] = {0};
    VMCHAR fAutoFileName[100] = {0};
    VMWCHAR wAutoFileName[100] = {0};
    VMWCHAR wProduct[100] = {0};

    vm_ascii_to_ucs2(wfile_extension, 8, extt); // txt

    if ((drv = vm_get_removable_driver()) < 0) {
       drv = vm_get_system_driver();
    }

    sprintf(fAutoFileName, "%c:\\", drv);
    vm_ascii_to_ucs2(wProduct, (strlen(fAutoFileName) + 1) * 2, fAutoFileName); // e:\

    vm_get_exec_filename(fullPath); // e:\home\program.vxp
    vm_get_filename(fullPath, appName); // program.vxp

    vm_wstrncpy(wAutoFileName, appName, vm_wstrlen(appName) - 3); //program.
    vm_wstrcat(wAutoFileName, wfile_extension); // program. + txt
    vm_wstrcat(wProduct, wAutoFileName); //e:\ + program.txt
    vm_wstrcpy(text, wProduct);
}
