
//#include "../scaffold.h"
#include <SDL/SDL.h>
#ifdef USE_SDL_IMAGE
#include <SDL/SDL_image.h>
#endif /* USE_SDL_IMAGE */
#include "../font.h"

static
SDL_Color graphics_stock_colors[16] = {   /*  r,   g,   b   */
   /* GRAPHICS_COLOR_TRANSPARENT =  0, */ {   0,   0,   0, 0 },
   /* GRAPHICS_COLOR_DARK_BLUE   =  1, */ {   0,   0, 128, 0 },
   /* GRAPHICS_COLOR_DARK_GREEN  =  2, */ {   0, 128,   0, 0 },
   /* GRAPHICS_COLOR_DARK_CYAN   =  3, */ {   0, 128, 128, 0 },
   /* GRAPHICS_COLOR_DARK_RED    =  4, */ { 128,   0,   0, 0 },
   /* GRAPHICS_COLOR_PURPLE      =  5, */ { 128,   0, 128, 0 },
   /* GRAPHICS_COLOR_BROWN       =  6, */ { 128, 128,   0, 0 },
   /* GRAPHICS_COLOR_GRAY        =  7, */ { 128, 128, 128, 0 },
   /* GRAPHICS_COLOR_DARK_GRAY   =  8, */ {   0,   0,   0, 0 },
   /* GRAPHICS_COLOR_BLUE        =  9, */ {   0,   0, 255, 0 },
   /* GRAPHICS_COLOR_GREEN       = 10, */ {   0, 255,   0, 0 },
   /* GRAPHICS_COLOR_CYAN        = 11, */ {   0, 255, 255, 0 },
   /* GRAPHICS_COLOR_RED         = 12, */ { 255,   0,   0, 0 },
                                          { 255,   0, 255, 0 },
   /* GRAPHICS_COLOR_YELLOW      = 13, */ { 255, 255,   0, 0 },
   /* GRAPHICS_COLOR_WHITE       = 14  */ { 255, 255, 255, 0 }
};

#define GRAPHICS_C
#define graphics_get_color( color_i ) (graphics_stock_colors[color_i])
#define graphics_lock( surface ) SDL_LockSurface( surface )
#define graphics_unlock( surface ) SDL_UnlockSurface( surface )
#include "../graphics.h"

void graphics_set_pixel(
   GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y,
   GRAPHICS_COLOR pixel
) {
   SDL_Surface* surface;
   int bpp;
   uint8_t* p;

   if( NULL == g || NULL == g->surface ) {
      return;
   }

   surface = g->surface;
   bpp = surface->format->BytesPerPixel;
   p = (uint8_t*)surface->pixels + y * surface->pitch + x * bpp;

   if( 0 > x || 0 > y || surface->w <= x || surface->h <= y ) {
      return;
   }

   switch( bpp ) {
   case 1:
      *p = pixel;
      break;

   case 2:
      *(uint16_t*)p = pixel;
      break;

#ifdef USE_HICOLOR
   case 3:
      if( SDL_BYTEORDER == SDL_BIG_ENDIAN ) {
         p[0] = (pixel >> 16) & 0xff;
         p[1] = (pixel >> 8) & 0xff;
         p[2] = pixel & 0xff;
      } else {
         p[0] = pixel & 0xff;
         p[1] = (pixel >> 8) & 0xff;
         p[2] = (pixel >> 16) & 0xff;
      }
      break;
#endif /* USE_HICOLOR */

   case 4:
      *(uint32_t*)p = pixel;
      break;
   }
}

GRAPHICS_COLOR graphics_get_pixel(
   const GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y
) {
   SDL_Surface* surface;
   int bpp;
   uint8_t* p;
   GRAPHICS_COLOR pout;

   if( NULL == g || NULL == g->surface ) {
      return GRAPHICS_COLOR_TRANSPARENT;
   }

   if( 0 > x || g->w <= x || 0 > y || g->h <= y ) {
      return GRAPHICS_COLOR_TRANSPARENT;
   }

   surface = g->surface;
   bpp = surface->format->BytesPerPixel;
   p = (uint8_t*)surface->pixels + y * surface->pitch + x * bpp;

   switch( bpp ) {
   case 1:
      return *p;

   case 2:
      return *(uint16_t*)p;

#ifdef USE_HICOLOR
   case 3:
      if( SDL_BYTEORDER == SDL_BIG_ENDIAN ) {
         pout->r = (pixel >> 16) & 0xff;
         pout->g = (pixel >> 8) & 0xff;
         pout->b = pixel & 0xff;
      } else {
         pout->b = pixel & 0xff;
         pout->g = (pixel >> 8) & 0xff;
         pout->r = (pixel >> 16) & 0xff;
      }
      return pout;
#endif /* USE_HICOLOR */

   case 4:
      return *(uint32_t*)p;
   }
}

void graphics_surface_cleanup( GRAPHICS* g ) {
   if( NULL != g->surface ) {
      SDL_FreeSurface( g->surface );
      g->surface = NULL;
   }
   if( NULL == g->palette ) {
      mem_free( g->palette );
      g->palette = NULL;
   }
}

void graphics_screen_new(
   GRAPHICS** g, size_t w, size_t h,
   size_t vw, size_t vh, int32_t arg1, void* arg2
) {

   graphics_setup();

   (*g) = mem_alloc( 1, GRAPHICS );
   SDL_Init( SDL_INIT_EVERYTHING );
   (*g)->surface = SDL_SetVideoMode(
      w, h, 0,
#ifdef USE_SDL_IMAGE
      SDL_HWSURFACE
#else
      SDL_SWSURFACE
#endif /* USE_SDL_IMAGE */
      | SDL_DOUBLEBUF
   );
   lgc_null( (*g)->surface );
   graphics_surface_set_h( *g, h );
   graphics_surface_set_w( *g, w );
   (*g)->virtual_x = vw;
   (*g)->virtual_y = vh;
   (*g)->palette = NULL;
   (*g)->font = NULL;
cleanup:
   return;
}

void graphics_set_window_title( GRAPHICS* g, bstring title, void* icon ) {
   SDL_WM_SetCaption( bdata( title ), icon );
}

void graphics_surface_init( GRAPHICS* g, GFX_COORD_PIXEL w, GFX_COORD_PIXEL h ) {
   SDL_Surface* screen = NULL;

   screen = SDL_GetVideoSurface();

   if( 0 < w && 0 < h ) {
      g->surface = SDL_CreateRGBSurface(
#ifdef USE_SDL_IMAGE
         SDL_HWSURFACE,
#else
         SDL_SWSURFACE,
#endif /* USE_SDL_IMAGE */
         w,
         h,
         screen->format->BitsPerPixel,
         screen->format->Rmask,
         screen->format->Gmask,
         screen->format->Bmask,
         screen->format->Amask
      );
      lgc_null( g->surface );
   }
   g->virtual_x = 0;
   g->virtual_y = 0;
   graphics_surface_set_h( g, h );
   graphics_surface_set_w( g, w );
cleanup:
   return;
}

void graphics_flip_screen( GRAPHICS* g ) {
   SDL_Flip( g->surface );
}

void graphics_shutdown( GRAPHICS* g ) {
   SDL_FreeSurface( g->surface );
   mem_free( g );
   SDL_Quit();
}

void graphics_screen_scroll(
   GRAPHICS* g, GFX_COORD_PIXEL offset_x, GFX_COORD_PIXEL offset_y
) {
}

void graphics_set_image_path( GRAPHICS* g, const bstring path ) {
}

void graphics_set_image_data(
   GRAPHICS* g, const unsigned char* data, size_t length
) {
   struct GRAPHICS_BITMAP* bitmap = NULL;
   int sdl_ret,
      bytes_per_pixel;
   int surface_i = 0,
      y,
      i = 0;
   SDL_Surface* surface = NULL,
      * screen = NULL,
      * temp_surface = NULL;
   SDL_Color* pixel = NULL;
   SDL_PixelFormat* format = NULL;
   SDL_RWops* rwop;
   uint16_t temp;
   uint32_t color_key;
   unsigned char* holder = NULL;

   screen = SDL_GetVideoSurface();
   lgc_null( screen );

   if( NULL != g->surface ) {
      SDL_FreeSurface( g->surface );
      g->surface = NULL;
   }

#ifdef USE_SDL_IMAGE

   rwop = SDL_RWFromConstMem( data, length );

   temp_surface = IMG_LoadBMP_RW( rwop );
   lgc_null( temp_surface );

   /* Setup transparency. */
   color_key = SDL_MapRGB( temp_surface->format, 0, 0, 0 );
   SDL_SetColorKey( temp_surface, SDL_RLEACCEL | SDL_SRCCOLORKEY, color_key );
   g->surface = SDL_DisplayFormatAlpha( temp_surface );

   /* Image load. */
   surface = (SDL_Surface*)g->surface;
   g->w = surface->w;
   g->h = surface->h;
   //g->fp_w = graphics_precise( g->w );
   //g->fp_h = graphics_precise( g->h );

   lgc_null( g->surface );

cleanup:
   if( NULL != temp_surface ) {
      SDL_FreeSurface( temp_surface );
   }
   SDL_FreeRW( rwop );
#else
   graphics_bitmap_load( data, length, &bitmap );
   lgc_null( bitmap );
   lgc_null( bitmap->pixels );
   g->surface = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      bitmap->w,
      bitmap->h,
      screen->format->BitsPerPixel,
      screen->format->Rmask,
      screen->format->Gmask,
      screen->format->Bmask,
      screen->format->Amask
   );
   lgc_null( g->surface );
   graphics_surface_set_h( g, bitmap->h );
   graphics_surface_set_w( g, bitmap->w );
   sdl_ret = SDL_LockSurface( g->surface );
   lgc_nonzero( sdl_ret );
   surface = (SDL_Surface*)g->surface;
   lgc_null( surface );
   format = surface->format;
   lgc_null( format );
   assert( 16 == format->BitsPerPixel );
   //holder = mem_alloc( bytes_per_pixel, unsigned char );
   for( y = 0 ; surface->h > y ; y++ ) {
      surface_i = (y / bitmap->w) + (y % bitmap->w);

      pixel = &(graphics_stock_colors[(int)(bitmap->pixels[i++])]);
      ((uint16_t*)surface->pixels)[surface_i] =
         ((pixel->r >> format->Rshift) << format->Rloss);

      assert( ((uint16_t*)surface->pixels)[surface_i] == 0 );

      /*
      holder =
         graphics_stock_colors[(int)(bitmap->pixels[i++])].r;
      holder << format->Rshift;
      *(surface->pixels + (surface_i * bytes_per_pixel)) &= *holder;
      */


//      /* FIXME: Read and use the bitmap format. */
//      ((uint8_t*)(surface->pixels))[surface_i] =;
   }
   SDL_UnlockSurface( g->surface );

cleanup:
   if( NULL != holder ) {
      free( holder );
   }
   graphics_free_bitmap( bitmap );
#endif /* USE_SDL_IMAGE */
}

unsigned char* graphics_export_image_data( GRAPHICS* g, size_t* out_len ) {
}

void graphics_draw_rect(
   GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y,
   GFX_COORD_PIXEL w, GFX_COORD_PIXEL h, GRAPHICS_COLOR color_i, bool filled
) {
   SDL_Rect rect;
   SDL_Color* color = &(graphics_stock_colors[color_i]);
   SDL_Surface* surface = g->surface;

   lgc_null( g->surface );

   rect.x = x;
   rect.y = y;
   rect.w = w,
   rect.h = h;

   if( filled ) {
      SDL_FillRect(
         g->surface,
         &rect,
         SDL_MapRGB( surface->format, color->r, color->g, color->b )
      );
   } else {
      /* TODO */
   }

cleanup:
   return;
}

void graphics_draw_triangle(
   GRAPHICS* g,
   GFX_COORD_PIXEL x1, GFX_COORD_PIXEL y1,
   GFX_COORD_PIXEL x2, GFX_COORD_PIXEL y2,
   GFX_COORD_PIXEL x3, GFX_COORD_PIXEL y3,
   GRAPHICS_COLOR color, bool filled
) {
   /* TODO */
}

void graphics_draw_circle(
   GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y,
   GFX_COORD_PIXEL radius, GRAPHICS_COLOR color, bool filled
) {
   /* TODO */
}

void graphics_draw_char(
   GRAPHICS* g, GFX_COORD_PIXEL x_start, GFX_COORD_PIXEL y_start,
   GRAPHICS_COLOR color_i, GRAPHICS_FONT_SIZE size, char c
) {
   GFX_COORD_PIXEL x, y, bit;
   uint8_t* font_char;
   float divisor;
   SDL_Color* color;

   divisor = size / 8.0f;

   lgc_null( g->surface );

   color = &(graphics_get_color( color_i ));

   SDL_LockSurface( g->surface );
   for( y = 0 ; size > y ; y++ ) {
      for( x = 0 ; size > x ; x++ ) {
         if( font8x8_basic[c][(uint8_t)(y / divisor)] & 1 << (uint8_t)(x / divisor) ) {
            graphics_set_pixel( g, x_start + x, y_start + y, *((uint32_t*)color) );
         }
      }
   }
   SDL_UnlockSurface( g->surface );

cleanup:
   return;
}

void graphics_transition( GRAPHICS* g, GRAPHICS_TRANSIT_FX fx ) {
}

void graphics_blit_partial(
   GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y, GFX_COORD_PIXEL s_x,
   GFX_COORD_PIXEL s_y, GFX_COORD_PIXEL s_w, GFX_COORD_PIXEL s_h, const GRAPHICS* src
) {
   SDL_Rect src_rect,
      dest_rect;

   src_rect.x = s_x;
   src_rect.y = s_y;
   src_rect.w = s_w;
   src_rect.h = s_h;

   dest_rect.x = x;
   dest_rect.y = y;
   dest_rect.w = s_w;
   dest_rect.h = s_h;

   SDL_BlitSurface( src->surface, &src_rect, g->surface, &dest_rect );
}

void graphics_blit_stretch(
   GRAPHICS* g, GFX_COORD_PIXEL x, GFX_COORD_PIXEL y,
   GFX_COORD_PIXEL w, GFX_COORD_PIXEL h, const GRAPHICS* src
) {
   SDL_Rect src_rect, dest_rect;

   if( w == 0 || h == 0 ) {
      goto cleanup;
   }

   /* Switch the surfaces so temp can be disposed below. */
   if( NULL != g->surface && NULL != src->surface ) {

      src_rect.w = src->w;
      src_rect.h = src->h;
      src_rect.x = 0;
      src_rect.y = 0;

      dest_rect.w = w;
      dest_rect.h = h;
      dest_rect.x = x;
      dest_rect.y = y;

      SDL_SoftStretch( src->surface, &src_rect, g->surface, &dest_rect );
   }

cleanup:
   return;
}

void graphics_sleep( uint16_t milliseconds ) {
   SDL_Delay( milliseconds );
}

uint32_t graphics_get_ticks() {
   return SDL_GetTicks();
}
