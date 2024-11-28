#include <mlx.h>
#include <math.h>
#include <stdio.h>

typedef struct s_data {
    void    *mlx;
    void    *win;
    int     prev_mouse_x;
    double  view_angle;
}              t_data;

int mouse_move(int x, int y, t_data *data) {
    (void)y; // Nous n'utilisons pas la coordonnée y dans cet exemple
    
    int delta_x = x - data->prev_mouse_x;
    data->view_angle += delta_x * 0.01; // Ajuster la sensibilité selon vos besoins

    if (data->view_angle > 2 * M_PI)
        data->view_angle -= 2 * M_PI;
    else if (data->view_angle < 0)
        data->view_angle += 2 * M_PI;

    data->prev_mouse_x = x;
    
    // Ici vous pouvez mettre à jour votre rendu en fonction de la nouvelle view_angle
    // Par exemple, une fonction update_view(data);

    printf("View angle: %f\n", data->view_angle); // Pour déboguer

    return (0);
}

int main() {
    t_data  data;

    data.mlx = mlx_init();
    data.win = mlx_new_window(data.mlx, 800, 600, "Rotate View with Mouse");
    data.prev_mouse_x = 400; // Initialiser à la moitié de la largeur de la fenêtre
    data.view_angle = 0.0;

    mlx_hook(data.win, 6, 0, mouse_move, &data); // 6 est l'événement de mouvement de la souris
    mlx_loop(data.mlx);

    return (0);
}

-----------------------------42036930719628624173624826116--