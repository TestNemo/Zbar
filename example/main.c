/****************************************Copyright (c)****************************************************
**                       		 	   CJS(greyorbit@foxmail.com)
**                          		   COPYRIGHT (C) 2018
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Last modified Date:      2018-01-31
** Last Version:            1.0.0
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2018-01-31
** Version:                 1.0.0
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
** Modified by:				
** Modified date:  			          
** Version:                 
** Descriptions:                        
**
*********************************************************************************************************/
#include "zbar.h"
#include <stdio.h>
#include <string.h>

//#define   WIDTH    128
//#define   HEIGHT   160
//#define   WIDTH    176
//#define   HEIGHT   144
#define   WIDTH    320
#define   HEIGHT   240
int yuv_scan()
{
   int ret;
   unsigned char* framePtr = NULL;
   int width = WIDTH;
   int height = HEIGHT;
   zbar_image_scanner_t *scanner , *scanner_ptr= NULL;
    /* create a reader */
   scanner_ptr = scanner = zbar_image_scanner_create();

    /* configure the reader */
   zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);


   int frameSize = width * height;
   // yuv422p: w * h * 2
   int picSize   = frameSize * 2;;
   framePtr = (unsigned char *) malloc(sizeof(unsigned char) * picSize);
   if (framePtr == NULL)
   {
      printf("malloc failed.\n");
      return -1;
   }
   memset(framePtr, '\0', picSize);
   FILE *fp1 = fopen("test.yuv", "rb"); 
   ret = (int)fread(framePtr, 1, picSize, fp1);
   
   zbar_image_t *gray_img = NULL;
    
   zbar_image_t *image = zbar_image_create();
   zbar_image_set_format(image, *(int*)"YUYV");
   //zbar_image_set_format(image, *(int*)"UYVY");
   zbar_image_set_size(image, width, height);
   zbar_image_set_data(image, framePtr, width * height, zbar_image_free_data);
   
   gray_img = zbar_image_convert(image,*(int*)"Y800");
   
    /* scan the image for barcodes */
   printf("start zbar_scan_image\r\n");
   int n = zbar_scan_image(scanner, gray_img);
   printf("n = %d\r\n", n);
   /* extract results */
   const zbar_symbol_t *symbol = zbar_image_first_symbol(gray_img);
   for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("\r\ndecoded %s symbol \"%s\"\r\n",
               zbar_get_symbol_name(typ), data);
        printf("len = %d\r\n",strlen(data));
   }   
   return 0;
} 

char buf[] = "Hello CJS\r\n";
int main(void)
{	
	printf("%s \r\n", buf);	
	//Zbar_Test((void* )data_buf,280,280);
    yuv_scan();	
}


