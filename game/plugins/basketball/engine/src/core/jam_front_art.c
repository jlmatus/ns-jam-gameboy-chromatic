/* Generated native background art; tiles 0-127 in each VRAM bank. */
#pragma bank 255
#include <gbdk/platform.h>
#include "palette.h"
#include <string.h>
#include "jam_dunk_cam.h"
void jam_front_common(void) BANKED;
void jam_front_frame_0(void) BANKED;
void jam_front_frame_1(void) BANKED;
void jam_front_frame_2(void) BANKED;
void jam_front_frame_3(void) BANKED;
void jam_front_frame_4(void) BANKED;
void jam_front_frame_5(void) BANKED;
void jam_front_frame_6(void) BANKED;
void jam_front_frame_7(void) BANKED;
void jam_front_frame_8(unsigned char buffer) BANKED;
void jam_front_frame_9(unsigned char buffer) BANKED;
void jam_front_frame_10(unsigned char buffer) BANKED;
void jam_front_frame_11(unsigned char buffer) BANKED;
void jam_front_frame_12(unsigned char buffer) BANKED;
void jam_front_frame_13(unsigned char buffer) BANKED;
void jam_front_frame_14(unsigned char buffer) BANKED;
void jam_front_frame_15(unsigned char buffer) BANKED;
void jam_front_frame_16(unsigned char buffer) BANKED;
void jam_front_frame_17(unsigned char buffer) BANKED;
void jam_front_frame_18(unsigned char buffer) BANKED;
void jam_front_frame_19(unsigned char buffer) BANKED;
void jam_front_frame_20(void) BANKED;
void jam_front_frame_21(void) BANKED;
void jam_front_frame_22(void) BANKED;
void jam_front_frame_23(void) BANKED;
static const unsigned short palettes[]={6274,9443,16837,6813,6274,16837,6813,27615,6274,9443,16837,27615,6274,9654,18173,27615,6274,9654,18173,7364,6274,9654,18173,9443,6274,4572,6813,27615};
static const unsigned short home_palettes[]={5217,12647,7869,24511,5217,13701,7869,24511,5217,12647,13775,9686,5217,13701,9686,7869,5217,8386,13701,7869,5217,4334,5627,7869,5217,13701,7869,24511};
static const unsigned short card_palettes[]={6274,16837,6813,27615,6274,9443,16837,6813,9443,7364,12648,9654,7364,9654,18173,7471,7471,9654,18173,14906,9443,9654,18173,7364,9443,9654,18173,7471};
static const unsigned short chompa_palettes[]={6274,16837,6813,27615,6274,9443,16837,6813,9443,7364,9654,18173,7364,9654,18173,27615,9443,7364,7988,9654,7364,7988,9654,18173,9443,7364,7988,18173};
static const unsigned short dark[]={6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274,6274};
void jam_front_art(unsigned char frame) BANKED {unsigned char buffer=jam_cam_upload==1?1:0,intro=frame>=8 && frame<20,transition=!intro || !jam_cam_upload;if(transition){memcpy(BkgPalette,dark,sizeof(dark));set_bkg_palette(0,7,dark);wait_vbl_done();}if(intro && !jam_cam_upload)jam_front_common();switch(frame){
case 0:jam_front_frame_0();break;
case 1:jam_front_frame_1();break;
case 2:jam_front_frame_2();break;
case 3:jam_front_frame_3();break;
case 4:jam_front_frame_4();break;
case 5:jam_front_frame_5();break;
case 6:jam_front_frame_6();break;
case 7:jam_front_frame_7();break;
case 8:jam_front_frame_8(buffer);break;
case 9:jam_front_frame_9(buffer);break;
case 10:jam_front_frame_10(buffer);break;
case 11:jam_front_frame_11(buffer);break;
case 12:jam_front_frame_12(buffer);break;
case 13:jam_front_frame_13(buffer);break;
case 14:jam_front_frame_14(buffer);break;
case 15:jam_front_frame_15(buffer);break;
case 16:jam_front_frame_16(buffer);break;
case 17:jam_front_frame_17(buffer);break;
case 18:jam_front_frame_18(buffer);break;
case 19:jam_front_frame_19(buffer);break;
case 20:jam_front_frame_20();break;
case 21:jam_front_frame_21();break;
case 22:jam_front_frame_22();break;
case 23:jam_front_frame_23();break;
}if(!intro)jam_cam_upload=0;else jam_cam_upload=buffer+1;if(transition){const unsigned short *active=frame<3?home_palettes:(frame>=20?chompa_palettes:(frame>=4 && frame<8?card_palettes:palettes));wait_vbl_done();memcpy(BkgPalette,active,sizeof(palettes));set_bkg_palette(0,7,active);}}
