#include "cub.h"

t_sdl_image *g_image;
t_sdl_env   g_sdl_env;
t_cub_env   g_cub_env;

int ft_do_exit(void *arg, SDL_Event e)
{
    if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        return (0);
    if (e.key.keysym.scancode == SDL_SCANCODE_LEFT)
    {
        g_cub_env.camera.direction = ft_rotate_vectorz(g_cub_env.camera.direction, M_PI/10);
    }
    else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT)
    {
        g_cub_env.camera.direction = ft_rotate_vectorz(g_cub_env.camera.direction, -M_PI/10);
    }
    else if (e.key.keysym.scancode == SDL_SCANCODE_UP)
    {
        g_cub_env.camera.pos = ft_add_vector(ft_scale_vector(ft_normalise_vector(g_cub_env.camera.direction), 5), g_cub_env.camera.pos);
    }
    else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN)
    {
        g_cub_env.camera.pos = ft_add_vector(ft_scale_vector(ft_normalise_vector(g_cub_env.camera.direction), -5), g_cub_env.camera.pos);
    }
    return (1);
}

// HERE is the code executed at each frame

int frame_function(void *arg)
{
    (void)arg;
    raycast_main();
    ft_sdl_put_image(g_image, &g_sdl_env);
    ft_sdl_render_texture(&g_sdl_env);
    ft_sdl_clear_image(g_image, 0);
    return (1);
}

// Here the main is just for initialisation

int main(int argc, char **argv)
{
    t_sdl_init_config   config;

    config = ft_sdl_new_config();
    config.screen_width = WINDOW_WIDTH;
    config.screen_height = WINDOW_HEIGHT;
    config.window_title = "cub3d";
    if (!(g_image = newimage(WINDOW_WIDTH, WINDOW_HEIGHT)))
        return (2);
    if (!ft_sdl_init(&g_sdl_env, config))
        return (1);
    init();
    ft_sdl_hook(ft_do_exit, &g_sdl_env, SDL_KEYDOWN);
    ft_sdl_loop_hook(frame_function, NULL);
    ft_sdl_loop(&g_sdl_env);
    return (0);
}