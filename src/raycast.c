#include "cub.h"

t_sdl_image g_wall_texture;
t_sdl_image g_sprite_texture;
t_vector    g_sprite_positions[SPRITE_COUNT] = {{100, 100}};
double      ray_sizes[WINDOW_WIDTH];
double      z_buffer[WINDOW_WIDTH];

int g_cub_map[MAP_W][MAP_H] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

double  ft_map(double value, double from_min, double from_max, double to_min, double to_max)
{
    double  scale;

    scale = (to_max-to_min)/(from_max-from_min);
    return (to_min + (value-from_min)*scale);
}

int     get_sprite_screen_x(t_vector pos, int *in_view)
{
    t_vector    last_pixel_ray;
    t_vector    current_pos_ray;
    t_vector    first_pixel_ray;
    double      last_pixel_angle;
    double      current_pos_angle;
    double      first_pixel_angle;
    t_vector    left_plane_half;

    left_plane_half = ft_rotate_vectorz(g_cub_env.camera.direction, M_PI/2);
    left_plane_half = ft_scale_vector(left_plane_half, g_cub_env.camera.projection_plane_size / 2);
    first_pixel_ray = ft_add_vector(ft_scale_vector(g_cub_env.camera.direction, g_cub_env.camera.projection_plane_distance)
        ,left_plane_half);
    last_pixel_ray = g_cub_env.camera.ray.direction;
    current_pos_ray = ft_sub_vector(pos, g_cub_env.camera.pos);
    if (ft_dot_vector(current_pos_ray, g_cub_env.camera.direction) < 0)
    {
        *in_view = 0;
        return (-1); // the sprite is behind
    }
    last_pixel_angle = fmod((atan2(last_pixel_ray.x, last_pixel_ray.y) / M_PI * 180) + 720, 360);
    current_pos_angle = fmod((atan2(current_pos_ray.x, current_pos_ray.y) / M_PI * 180) + 720, 360);
    first_pixel_angle = fmod((atan2(first_pixel_ray.x, first_pixel_ray.y) / M_PI * 180) + 720, 360);
    double temp_value = ft_map(current_pos_angle, last_pixel_angle, first_pixel_angle, 0, WINDOW_WIDTH);
    *in_view = 1;
    return(WINDOW_WIDTH - temp_value);
}

void    draw_sprite_at(int screen_x, int size, double distance)
{
    int start_x, end_x, start_y, end_y;
    int texX, texY;
    int color;

    start_x = screen_x - size/2;
    end_x = start_x + size;
    while(start_x < end_x)
    {
        if (start_x >= 0 && start_x < WINDOW_WIDTH && distance < z_buffer[start_x])
        {
            z_buffer[start_x] = distance;
            printf("%f %f\n", z_buffer[start_x], distance);
            texX = (1.0 - (double)(end_x - start_x) / (double)size) * g_sprite_texture.width;
            start_y = WINDOW_HEIGHT/2 - size/2;
            end_y = start_y + size;
            while(start_y < end_y)
            {
                color = ft_sdl_get_image_pixel(&g_sprite_texture, texX, texY);
                texY = (1.0 - (double)(end_y - start_y) / (double)size) * g_sprite_texture.height;
                if (RGB_A(color))
                    ft_sdl_image_pixel(g_image, start_x, start_y, color);
                start_y++;
            }
        }
        start_x++;
    }
}

int     ft_draw_sprites(void)
{
    int         dist_index;
    int         screen_x;
    int         index;
    int         dimentions;
    int         in_view;
    double      distance;
    t_vector    pos;

    index = 0;
    while(index < SPRITE_COUNT)
    {
        pos = g_sprite_positions[index];
        screen_x = get_sprite_screen_x(pos, &in_view);
        if (in_view)
        {
            dist_index = screen_x;
            if (dist_index < 0)
                dist_index = 0;
            else if (dist_index > WINDOW_WIDTH-1)
                dist_index = WINDOW_WIDTH-1;
            distance = ft_vector_size(ft_sub_vector(pos, g_cub_env.camera.pos)) / ray_sizes[dist_index];
            dimentions = (WINDOW_HEIGHT / distance) * WALL_HEIGHT_SCALE;
            draw_sprite_at(screen_x, dimentions, distance);
        }
        index++;
    }
}

int     get_color_from_distance(double distance)
{
    unsigned char r, g, b;

    if (distance < 1)
        distance = 1;
    r = 255 / distance;
    g = 255 / distance;
    b = 255 / distance;

    return (r << 16 | g << 8 | b);
}

int     get_color_from_texture(t_intersect inter, double screen_y)
{
    int texture_x;
    int texture_y;

    texture_y = screen_y * g_wall_texture.height;
    if (inter.wall_side == WALL_SIDE_X)
        texture_x = fmod((inter.pos.y/(double)GRID_SIZE), 1.0) * g_wall_texture.width;
    else
        texture_x = fmod((inter.pos.x/(double)GRID_SIZE), 1.0) * g_wall_texture.width;
    return (ft_sdl_get_image_pixel(&g_wall_texture, texture_x, texture_y));
}

void    draw_wall(void)
{
    double  distance;
    double  plane_distance_size;
    int     wall_height;
    int     top_y, bottom_y;
    int     color;
    
    //dividing to solve for fisheye
    distance = g_cub_env.camera.intersection.distance / g_cub_env.camera.ray_size;
    wall_height = (WINDOW_HEIGHT / distance) * WALL_HEIGHT_SCALE;
    top_y = WINDOW_HEIGHT / 2 - wall_height / 2;
    bottom_y = top_y + wall_height;

    if (top_y < 0)
        top_y = 0;
    while   (top_y <= bottom_y && top_y <= WINDOW_HEIGHT)
    {
        color = get_color_from_texture(g_cub_env.camera.intersection,
            1.0 - (double)(bottom_y-top_y) / (double)wall_height);
        ft_sdl_image_pixel(g_image, g_cub_env.camera.intersection.screen_x, top_y, color);
        top_y++;
    }
}

t_point get_pos_on_grid(t_vector pos)
{
    t_point result;

    result.x = pos.x / GRID_SIZE;
    result.y = pos.y / GRID_SIZE;
    return result;
}

int intersect_with_x(t_dda *dda)
{
    int gridX, gridY;

    gridX = dda->nextX - 1 + dda->sign.x;
    gridY = dda->pos_on_grid.y;

    if (gridX < 0 || gridX >= MAP_W || gridY < 0 || gridY >= MAP_H)
        return (0);
    if (g_cub_map[gridY][gridX])
    {
        dda->intersection.wall_side = WALL_SIDE_X;
        dda->intersection.is_intersection = 1;
        dda->intersection.grid_pos = (t_point){gridX, gridY};
        dda->intersection.grid_value = g_cub_map[gridY][gridX];
        return (1);
    }
    return (0);
}

int intersect_with_y(t_dda *dda)
{
    int gridX, gridY;

    gridX = dda->pos_on_grid.x;
    gridY = dda->nextY - 1 + dda->sign.y;

    if (gridX < 0 || gridX >= MAP_W || gridY < 0 || gridY >= MAP_H)
        return (0);
    if (g_cub_map[gridY][gridX])
    {
        dda->intersection.wall_side = WALL_SIDE_Y;
        dda->intersection.is_intersection = 1;
        dda->intersection.grid_pos = (t_point){gridX, gridY};
        dda->intersection.grid_value = g_cub_map[gridY][gridX];
        return (1);
    }
    return (0);
}

t_intersect intersect_ray(t_ray ray)
{
    int     iter;
    t_dda   dda;

    ft_bzero(&dda, sizeof(t_dda));
    iter = 0;
    dda.current_pos = ray.origin;
    dda.sign = (t_point){1, 1};
    if (ray.direction.x < 0)
        dda.sign.x = 0;
    if (ray.direction.y < 0)
        dda.sign.y = 0;
    while(iter < MAX_VIEW_DISTANCE)
    {
        dda.pos_on_grid = get_pos_on_grid(dda.current_pos);
        dda.nextX = dda.pos_on_grid.x + dda.sign.x;
        dda.nextY = dda.pos_on_grid.y + dda.sign.y;
        dda.next_pos_on_grid.x = dda.nextX * GRID_SIZE;
        dda.next_pos_on_grid.y = dda.nextY * GRID_SIZE;
        dda.d.x = (dda.next_pos_on_grid.x - dda.current_pos.x) / ray.direction.x;
        dda.d.y = (dda.next_pos_on_grid.y - dda.current_pos.y) / ray.direction.y;
        if (dda.d.x < dda.d.y)
        {
            intersect_with_x(&dda);
            dda.final_d = dda.d.x;
        }
        else
        {
            intersect_with_y(&dda);
            dda.final_d = dda.d.y;
        }
        dda.final_d += 0.00001; // because of the double precision issue
        dda.current_pos.x += dda.final_d * ray.direction.x;
        dda.current_pos.y += dda.final_d * ray.direction.y;
        dda.intersection.distance += dda.final_d;
        dda.intersection.pos = dda.current_pos;
        iter++;
        if (dda.intersection.is_intersection)
            break;
    }
    return dda.intersection;
}

int raycast_camera(t_camera *camera)
{
    int current_screen_x;
    t_vector    current_pos_on_plane;
    t_vector    plane_iter;
    t_ray       current_ray;
    t_vector    pos_to_plane_center;

    current_screen_x = 0;
    pos_to_plane_center = ft_scale_vector(camera->direction, camera->projection_plane_distance);
    camera->left_plane_half = ft_rotate_vectorz(camera->direction, M_PI/2);
    camera->left_plane_half = ft_scale_vector(camera->left_plane_half, camera->projection_plane_size / 2);
    current_pos_on_plane = ft_add_vector(pos_to_plane_center ,camera->left_plane_half);
    plane_iter = ft_scale_vector(camera->left_plane_half, -(2.0/WINDOW_WIDTH));
    current_ray.origin = camera->pos;
    while(current_screen_x < WINDOW_WIDTH)
    {
        g_cub_env.camera.ray_size = ft_vector_size(current_pos_on_plane);
        ray_sizes[current_screen_x] = g_cub_env.camera.ray_size;
        current_ray.direction = ft_normalise_vector(current_pos_on_plane);
        g_cub_env.camera.ray = current_ray;
        g_cub_env.camera.intersection = intersect_ray(current_ray);
        // here you can use the intersection to draw
        g_cub_env.camera.intersection.screen_x = current_screen_x;
        z_buffer[current_screen_x] = g_cub_env.camera.intersection.distance / g_cub_env.camera.ray_size;
        draw_wall();
        current_pos_on_plane = ft_add_vector(current_pos_on_plane, plane_iter);
        current_screen_x++;
    }
    return (1);
}

int init(void)
{
    g_cub_env.camera.pos = (t_vector){250, 250};
    g_cub_env.camera.direction = ft_normalise_vector((t_vector){1, 1});
    g_cub_env.camera.projection_plane_distance = 40;
    g_cub_env.camera.projection_plane_size = 50;
    ft_sdl_load_image("our_wall_texture.tex", &g_wall_texture);
    ft_sdl_load_image("sprite.tex", &g_sprite_texture);
    return (1);
}

int raycast_main(void)
{
    ft_sdl_image_rect(g_image, (t_rect){.x = 0,.y = 0,.w = WINDOW_WIDTH,.h = WINDOW_HEIGHT/2}, 0x212121);
    ft_sdl_image_rect(g_image, (t_rect){.x = 0,.y = WINDOW_HEIGHT/2,.w = WINDOW_WIDTH,.h = WINDOW_HEIGHT/2}, 0x333333);
    raycast_camera(&g_cub_env.camera);
    ft_draw_sprites();
    

    // here we will move the sprite just for fun, as an introduction to bots.
    t_vector    fjla_to_player;
    double      distance;

    fjla_to_player = ft_sub_vector(g_cub_env.camera.pos, g_sprite_positions[0]);
    distance = ft_vector_size(fjla_to_player);
    if (distance > 50)
        g_sprite_positions[0] = ft_add_vector(g_sprite_positions[0], ft_normalise_vector(fjla_to_player));
    return (0);
}