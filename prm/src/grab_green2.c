#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i, k, green;
	float turn_speed=0;
  playerc_client_t *client;
	playerc_position2d_t *position2d;
  playerc_laser_t *laser;
	playerc_blobfinder_t *bf;
	playerc_gripper_t *gripper;

  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
    return -1;

  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  laser = playerc_laser_create(client, 0);
  if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
    return -1;

  bf = playerc_blobfinder_create(client, 0);
  if (playerc_blobfinder_subscribe(bf, PLAYER_OPEN_MODE))
    return -1;

  gripper = playerc_gripper_create(client, 0);
  if (playerc_gripper_subscribe(gripper, PLAYER_OPEN_MODE))
    return -1;

	playerc_position2d_enable(position2d, 1);

	//Anda
  playerc_position2d_set_cmd_vel(position2d, 0.2, 0, turn_speed, 1);

  playerc_client_read(client);
  playerc_client_read(client);

  while(1)
  {
    playerc_client_read(client);
		
    // Encontrar blob verde
		if (bf->blobs_count > 0)
			for(k=0; k < bf->blobs_count; k++)
				if (bf->blobs[k].color==0x00ff00)
					{
					if(bf->blobs[k].x<38)
						turn_speed=0.2;
					else if(bf->blobs[k].x>42)
						turn_speed=-0.2;
					else
						turn_speed=0.0;
			
					printf("\nBlob x:%u, range:%.2f", bf->blobs[k].x, bf->blobs[green].range);
					green=k;
					}
	  playerc_position2d_set_cmd_vel(position2d, 0.25, 0, turn_speed, 1);

	  if(bf->blobs[green].range<0.32) break;
  } 

  

  playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);

	playerc_gripper_open_cmd(gripper);
	playerc_gripper_close_cmd(gripper);

  for (i = 0; i < 50; i++)
    playerc_client_read(client);

  playerc_position2d_set_cmd_vel(position2d, 0.2, 0, 0, 1);

  for (i = 0; i < 100; i++)
  {
    playerc_client_read(client);
    printf("position : %f %f %f\n", position2d->px, position2d->py, position2d->pa);
  } 

  playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);

  playerc_gripper_open_cmd(gripper);

  for (i = 0; i < 50; i++)
    playerc_client_read(client);

  playerc_gripper_unsubscribe(gripper);
  playerc_gripper_destroy(gripper);
  playerc_position2d_unsubscribe(position2d);
  playerc_position2d_destroy(position2d);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
