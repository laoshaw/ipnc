
#include "alg_aewb_priv.h"

#define AEW_WIN_HZ_CNT	        32//12
#define AEW_WIN_VT_CNT	        12//16
//#define AEWB_PRINTS

unsigned char TI_WEIGHTING_MATRIX[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT];
unsigned char TI_WEIGHTING_SPOT[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT];
unsigned char TI_WEIGHTING_CENTER[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT];
static void generate_windows(int width1, int height1, int h_start2, int v_start2, int width2, int height2, int weight,
          unsigned char *win_coeffs);

void TI_2A_init_tables(int width, int height)
{
#ifdef AEWB_PRINTS
    OSA_printf("TI2A weight matrix: width = %d, height = %d\n", width, height);
#endif
    generate_windows(width, height, 0, 0, width, height/2, 50, TI_WEIGHTING_MATRIX);
    generate_windows(width, height, width/4, height/4, width/2, height/2, 50, TI_WEIGHTING_CENTER);
    generate_windows(width, height, width*3/8, height/3, width/4, height/4, 80, TI_WEIGHTING_SPOT);

}

#if 1
static void generate_windows(int width1, int height1, int h_start2, int v_start2, int width2, int height2, int weight,
          unsigned char *win_coeffs)
{
    int total_pixels_1, total_pixels_2;
    int pixel_weight1, pixel_weight2;
    int max_weight;
    int shift;
    int i, j;
    float total_weight;

    /* calculate weight for each window */
    total_pixels_2 = width2 * height2;
    total_pixels_1 = width1 * height1 - total_pixels_2;

    pixel_weight1 = (100 - weight) * 65536 / total_pixels_1;
    pixel_weight2 = weight * 65536 / total_pixels_2;

#ifdef AEWB_PRINTS
    OSA_printf("weights for each window: %d, %d\n", pixel_weight1, pixel_weight2);
#endif

    /* normalize weights */
    max_weight = pixel_weight1 > pixel_weight2 ? pixel_weight1 : pixel_weight2;
    shift = 0;
    while(max_weight >= 255){
        shift ++;
        max_weight = (max_weight + 1) >> 1;
    }
    pixel_weight1 = (pixel_weight1 + (1<<(shift-1))) >> shift;
    pixel_weight2 = (pixel_weight2 + (1<<(shift-1))) >> shift;

    if(pixel_weight2 > pixel_weight1 ){
        pixel_weight2 = (pixel_weight1 * total_pixels_1  * weight) /
                (total_pixels_2 * (100 - weight));
    } else {
        pixel_weight1 = (pixel_weight2 * total_pixels_2  * (100 - weight)) /
                (total_pixels_1 * weight);
    }
    total_weight = pixel_weight1 * total_pixels_1 + pixel_weight2 * total_pixels_2;

#ifdef AEWB_PRINTS
    OSA_printf("weight = %f, %f\n", pixel_weight1 * total_pixels_1/total_weight,
               pixel_weight2 * total_pixels_2 / total_weight);
    OSA_printf("weights for each window after shifting: %d, %d, %d\n", pixel_weight1, pixel_weight2, shift);
#endif

    for(i = 0; i < height1; i ++){
        for(j = 0; j < width1; j ++) {
            if(i >= v_start2 && i < (v_start2 + height2)
              && j >= h_start2 && j < (h_start2 + width2)){
                win_coeffs[i * width1 + j] = pixel_weight2;
            }else {
                win_coeffs[i * width1 + j] = pixel_weight1;
            }
        }
    }
}

#else

unsigned char TI_WEIGHTING_SPOT[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT]={
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16,0x16, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
};

unsigned char TI_WEIGHTING_CENTER[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT]={
 6, 7, 7, 7, 8, 8, 8, 8,  9, 9, 9,10,10,10,10,10, 10,10,10,10,10,10, 9, 9,  9, 8, 8, 8, 8, 7, 7, 7,
 7, 8, 8, 9, 9, 9,10,10, 11,11,11,11,11,12,12,12, 12,12,12,12,11,11,11,11, 10,10,10, 9, 9, 9, 8, 8,
 8, 9, 9,10,10,11,11,11, 12,12,13,13,13,13,13,13, 13,13,13,13,13,13,13,12, 12,11,11,11,10,10, 9, 9,
 9,10,10,11,11,12,12,12, 13,14,14,14,14,14,15,15, 15,15,15,14,14,14,14,14, 13,12,12,12,11,11,10,10,
10,10,11,11,12,12,13,13, 14,14,15,15,15,15,15,16, 16,16,15,15,15,15,15,14, 14,13,13,12,12,11,11,10,
10,11,11,12,12,13,13,14, 14,15,15,15,16,16,16,16, 16,16,16,16,16,15,15,15, 14,14,13,13,12,12,11,11,
10,10,11,11,12,12,13,13, 14,15,15,15,15,16,16,16, 16,16,16,16,15,15,15,15, 14,13,13,12,12,11,11,10,
 9,10,10,11,11,12,12,13, 14,14,14,14,15,15,15,15, 15,15,15,15,15,14,14,14, 13,13,12,12,11,11,10,10,
 9, 9,10,10,11,11,11,12, 12,13,13,13,13,14,14,14, 14,14,14,14,13,13,13,13, 12,12,11,11,11,10,10, 9,
 8, 8, 9, 9, 9,10,10,10, 11,11,12,12,12,12,12,12, 12,12,12,12,12,12,12,11, 11,10,10,10, 9, 9, 9, 8,
 7, 7, 7, 8, 8, 8, 9, 9,  9,10,10,10,10,10,11,11, 11,11,11,10,10,10,10,10,  9, 9, 9, 8, 8, 8, 7, 7,
 6, 7, 7, 7, 8, 8, 8, 8,  9, 9, 9,10,10,10,10,10, 10,10,10,10,10,10, 9, 9,  9, 8, 8, 8, 8, 7, 7, 7
};

unsigned char TI_WEIGHTING_MATRIX[AEW_WIN_HZ_CNT * AEW_WIN_VT_CNT]={
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#endif
