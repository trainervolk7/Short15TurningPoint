#include "main.h"
Motor lift {14};
namespace screen {

lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
int x = 2;

  static lv_res_t btn_click_action(lv_obj_t * btn)
{
  uint8_t id = lv_obj_get_free_num(btn);
  x++;

  char buffer[100];
  double liftPosition = lift.getPosition();
		sprintf(buffer, "Position of lift: %lf ", liftPosition);
		lv_label_set_text(label1, buffer);

  /* The button is released.
   * Make something here */

  return LV_RES_OK; /*Return OK if the button is not deleted*/
}

  void init()
  {




/*Create a title label*/
lv_label_set_text(label1, "Default buttons");
lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

/*Create a normal button*/
lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
lv_cont_set_fit(btn1, true, true); /*Enable resizing horizontally and vertically*/
lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);
lv_obj_set_free_num(btn1, 1);   /*Set a unique number for the button*/
lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, btn_click_action);

/*Add a label to the button*/
//label = lv_label_create(btn1, NULL);
lv_obj_t * label = lv_label_create(btn1, NULL);
lv_label_set_text(label, "Normal");


  }
}
