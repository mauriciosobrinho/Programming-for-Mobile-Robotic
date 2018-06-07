#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i;

  float vlong=0, vrot=0;

  //Cliente para conexão com o servidor
  playerc_client_t *client;
  
  //Objeto para conexão com o odômetro
  playerc_position2d_t *position2d;

  playerc_laser_t *laser;

  //Conecta ao servidor no endereço “localhost”, na porta 6665 (default)  
  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  //Conecta ao odômetro
  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  laser = playerc_laser_create(client, 0);
  if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
    return -1;

    playerc_client_read(client);
    playerc_client_read(client);

  //Ativa os motores do robô
  playerc_position2d_enable(position2d, 1);

  while (1) {

    playerc_client_read(client);

    vlong = 0.8;
    vrot  = 0;
   
    if (laser->scan[i][270] < 1.0) vrot = 0.75;
    if (laser->scan[i][270] > 1.5) vrot = -0.75;
    if (laser->scan[i][180] < 1.2) vlong = 0.3;
    if (laser->scan[i][180] < 0.8) vrot = 0.75;

    playerc_position2d_set_cmd_vel(position2d, vlong, 0, vrot, 1);
    }

  //Desconecta do odômetro e do servidor
  playerc_position2d_unsubscribe(position2d);
  playerc_position2d_destroy(position2d);
  playerc_laser_unsubscribe(laser);
  playerc_laser_destroy(laser);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
