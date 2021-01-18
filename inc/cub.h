#ifndef CUB_H
# define CUB_H
# include <stdio.h>
# include "libft.h"
# include "ttslist.h"
# include "ft_maths.h"
# include "ft_simplesdl.h"
# define WINDOW_WIDTH 500
# define WINDOW_HEIGHT 500
# define MAP_W 10
# define MAP_H 10
# define GRID_SIZE 50
# define MAX_VIEW_DISTANCE 10000
# define WALL_HEIGHT_SCALE 1.0
# define WALL_SIDE_X 1
# define WALL_SIDE_Y 2
# define SPRITE_COUNT 1
# define CORRECT_COLOR(c) (c & 0x00FFFFFF)

extern t_sdl_image *g_image;
extern t_sdl_env   g_sdl_env;
extern int g_cub_map[MAP_W][MAP_H];

typedef struct  s_ray
{
    t_vector    origin;
    t_vector    direction;
}               t_ray;

typedef struct  s_intersect
{
    t_vector        pos;
    t_point         grid_pos;
    double          distance;
    int             grid_value;
    int             screen_x;
    int             wall_side;
    unsigned char   is_intersection;
}               t_intersect;

typedef struct  s_dda
{
    t_vector    current_pos;
    t_point     pos_on_grid;
    t_point     next_pos_on_grid;
    int         nextX;
    int         nextY;
    t_vector    d;
    double      final_d;
    t_point     sign;
    t_intersect intersection;
}               t_dda;

typedef struct  s_camera 
{
    t_vector        pos;
    t_vector        direction;
    t_vector        left_plane_half;
    t_vector        iter;
    double          projection_plane_distance;
    double          projection_plane_size;
    t_ray           ray;
    double          ray_size;
    t_intersect     intersection;

}               t_camera;

typedef struct  s_cub_env
{
    t_camera    camera;
}               t_cub_env;

extern t_cub_env   g_cub_env;

int init(void);
int raycast_main(void);

// debug functions
void    debug_draw_camera();
void    ft_debug_draw_ray(t_ray ray);

#endif