This is the cuting source code for zbar, it used only the source code as bellow:
1. source code in zbar fold (remove jpeg.c and svg.c during the compile error)
2. source code in decoder fold
3. source code in qrcode  fold
4. move the code from processor, video and window
5. the cross compiler is defined in Rules.make, for current we set it as AM5716 which is our Barcode platform, for the future, it will change the cross compiler tool with MT6261.
6. the fold zbar is the orignal code for zbar. 
