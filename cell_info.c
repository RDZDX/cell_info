#include "cell_info.h"

VMINT drv;
static VMINT timer_id = -1;
VMBOOL flightMode = VM_FALSE;
VMUINT8* layer_buf = 0;
VMINT screen_w = 0;
VMINT screen_h = 0;

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

        drv = vm_get_removable_driver();
        if (drv < 0) {drv = vm_get_system_driver();}

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
                //vm_cell_close();
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
        vm_exit_app();
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
           if (flightMode == VM_FALSE) { timer_id = vm_create_timer_ex(1000, timer); }
	}
}

void timer(int a){

	VMUINT nwrite;
	VMWCHAR file1[1000];
        int i;
        char t[20] = {0};
        char s[10000] = {0};
        struct vm_time_t curr_time;

        //vm_cell_info_struct* vm_cell_get_cur_cell_info(); //gauti prisijungtos stoties informacija
	vm_cell_info_struct** bs = vm_cell_get_nbr_cell_info(); //gauti kaimyniniu stociu informacija

        vm_get_time(&curr_time);
        cprintf("%04d-%02d-%02d %02d:%02d:%02d\n", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);

	int n = *vm_cell_get_nbr_num();
	for(i = 0; i<n; ++i)
	{
                cprintf("cellid:%d, lac:%d, signal:%d\n", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
	}
	s[strlen(s)-1]=']';
	sprintf(s, "%s}", s);

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

void checkFileExist(void) {

    VMFILE f_read;
    VMUINT nread;
    VMWCHAR file_pathw[100];
    VMCHAR new_data[500];

    create_app_txt_path(file_pathw, (char*)"txt");

    f_read = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f_read < 0) {

       vm_file_close(f_read);
       //missingConfigFile = VM_TRUE;

    } else {

      vm_file_read(f_read, new_data, 500, &nread);
      new_data[nread] = '\0';
      vm_file_close(f_read);

      if (strlen(new_data) > 1) {

         //parseText(new_data);
         //missingConfigFile = VM_FALSE;

      } else {

        //missingConfigFile = VM_TRUE;

      }

    }
}

static void fill_screen(void) {

    vm_graphic_color color;
    color.vm_color_565 = VM_COLOR_BLACK;
    vm_graphic_setcolor(&color);
    vm_graphic_fill_rect_ex(layer_hdl[0], 0, 0, screen_w, screen_h);
    vm_graphic_flush_layer(layer_hdl, 1);
}

