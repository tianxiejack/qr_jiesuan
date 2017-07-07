#ifndef  CLAHE_HEAD
#define  CLAHE_HEAD

#define BYTE_IMAGE 
//#undef BYTE_IMAGE 
 
#ifdef BYTE_IMAGE

typedef unsigned char kz_pixel_t;	 /* for 8 bit-per-pixel images */ 
#define uiNR_OF_GREY (256) 

#else 

typedef unsigned short kz_pixel_t;	 /* for 12 bit-per-pixel images (default) */ 
# define uiNR_OF_GREY (4096) 

#endif 
 
/******** Prototype of CLAHE function. Put this in a separate include file. *****/ 
int CLAHE_CR(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min, 
	  kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY, 
	  unsigned int uiNrBins, float fCliplimit); 

int CLAHE2(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min, 
	  kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY, 
	  unsigned int uiNrBins, float fCliplimit); 
 
int CLAHE_new(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min, 
	  kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY, 
	  unsigned int uiNrBins, float fCliplimit); 
/*********************** Local prototypes ************************/ 
static void ClipHistogram (unsigned long*, unsigned int, unsigned long); 
static void MakeHistogram (kz_pixel_t*, unsigned int, unsigned int, unsigned int, 
		unsigned long*, unsigned int, kz_pixel_t*); 
static void MapHistogram (unsigned long*, kz_pixel_t, kz_pixel_t, 
	       unsigned int, unsigned long); 
static void MakeLut (kz_pixel_t*, kz_pixel_t, kz_pixel_t, unsigned int); 
static void Interpolate (kz_pixel_t*, int, unsigned long*, unsigned long*, 
	unsigned long*, unsigned long*, unsigned int, unsigned int, kz_pixel_t*); 
static void Interpolate2 (kz_pixel_t*, int, unsigned long*, unsigned long*, 
	unsigned long*, unsigned long*, unsigned int, unsigned int, kz_pixel_t*); 
 
 
const unsigned int uiMAX_REG_X = 16;	  /* max. # contextual regions in x-direction */ 
const unsigned int uiMAX_REG_Y = 16;	  /* max. # contextual regions in y-direction */ 

#endif
