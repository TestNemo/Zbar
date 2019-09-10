#include "zbar.h"
#include "timer.h"
//zbar.h
/*
int zbar_processor_parse_config (zbar_processor_t *processor,const char *config_string)
{
    zbar_symbol_type_t sym;
    zbar_config_t cfg;
    int val;
    int ret = 0;
    if((ret = zbar_parse_config(config_string, &sym, &cfg, &val)) > 0)
        return ret;
    ret = zbar_processor_set_config(processor, sym, cfg, val);
    return ret;
}

int zbar_processor_error_spew (const zbar_processor_t *processor,int verbosity)
{
    return(_zbar_error_spew(processor, verbosity));
}

char* zbar_processor_error_string (const zbar_processor_t *processor,int verbosity)
{
    return(_zbar_error_string(processor, verbosity));
}

zbar_error_t zbar_processor_get_error_code (const zbar_processor_t *processor)
{
    return(_zbar_get_error_code(processor));
}

int zbar_video_error_spew (const zbar_video_t *video,int verbosity)
{
    return(_zbar_error_spew(video, verbosity));
}

char *zbar_video_error_string (const zbar_video_t *video,int verbosity)
{
    return(_zbar_error_string(video, verbosity));
}

zbar_error_t zbar_video_get_error_code (const zbar_video_t *video)
{
    return(_zbar_get_error_code(video));
}



int zbar_image_scanner_parse_config (zbar_image_scanner_t *scanner,const char *config_string)
{
    zbar_symbol_type_t sym;
    zbar_config_t cfg;
    int val;
    return(zbar_parse_config(config_string, &sym, &cfg, &val) ||
           zbar_image_scanner_set_config(scanner, sym, cfg, val));
}

int zbar_image_scanner_parse_config (zbar_image_scanner_t *scanner,const char *config_string)
{
    zbar_symbol_type_t sym;
    zbar_config_t cfg;
    int val;
    return(zbar_parse_config(config_string, &sym, &cfg, &val) ||
           zbar_decoder_set_config(decoder, sym, cfg, val));
}*/
/*
zbar_symbol_type_t zbar_scan_rgb24 (zbar_scanner_t *scanner,
                                                    unsigned char *rgb)
{
    return(zbar_scan_y(scanner, rgb[0] + rgb[1] + rgb[2]));
}*/

//time.h
#if 1
zbar_timer_t *_zbar_timer_init (zbar_timer_t *timer,int delay)
{
    if(delay < 0)
    return(NULL);

    gettimeofday(timer, NULL);
    timer->tv_usec += (delay % 1000) * 1000;
    timer->tv_sec += (delay / 1000) + (timer->tv_usec / 1000000);
    timer->tv_usec %= 1000000;
    return(timer);
}

int _zbar_timer_check (zbar_timer_t *timer)
{
    struct timeval now;
    if(!timer)
    return(-1);
    gettimeofday(&now, NULL);
    return((timer->tv_sec - now.tv_sec) * 1000 +
    (timer->tv_usec - now.tv_usec) / 1000);
}
#endif

