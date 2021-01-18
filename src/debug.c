#include "cub.h"

void    ft_debug_draw_ray(t_ray ray)
{
    t_vector    point_from_ray;

    point_from_ray = ft_add_vector(ray.origin, ft_scale_vector(ray.direction, 50));
    ft_sdl_image_line(g_image, (t_point){ray.origin.x, ray.origin.y}, (t_point){point_from_ray.x, point_from_ray.y}, 0xF0E68C);
}

void    debug_draw_grid()
{
    for (int i = 0; i < 10; i++)
    {
        ft_sdl_image_line(g_image, (t_point){i * GRID_SIZE, 0}, (t_point){i * GRID_SIZE, WINDOW_HEIGHT}, 0x333333);
        ft_sdl_image_line(g_image, (t_point){0, i * GRID_SIZE}, (t_point){WINDOW_WIDTH, i * GRID_SIZE}, 0x333333);
        for (int j = 0; j < 10; j++)
        {
            if (g_cub_map[i][j])
            {
                ft_sdl_image_rect(g_image, (t_rect){
                    .x= j*GRID_SIZE,
                    .y= i*GRID_SIZE,
                    .w=GRID_SIZE,
                    .h=GRID_SIZE
                }, 0x202020);
            }
        }
    }
}

void    debug_draw_camera()
{
    t_camera    cam;
    t_vector    plane_center_pos;

    debug_draw_grid();
    cam = g_cub_env.camera;
    plane_center_pos = ft_add_vector(ft_scale_vector(cam.direction, cam.projection_plane_distance), cam.pos);
    cam = g_cub_env.camera;
    ft_sdl_image_disc(g_image, (t_point){cam.pos.x, cam.pos.y}, 3, 0xFF00FF);
    ft_sdl_image_line(g_image, (t_point){cam.pos.x, cam.pos.y}, (t_point){plane_center_pos.x, plane_center_pos.y}, 0xF0E68C);
}