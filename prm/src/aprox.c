#include <libplayerc/playerc.h>

int main(int argc, const char **argv) {
	int i;
	playerc_client_t *client;
	playerc_laser_t *laser;
	playerc_position2d_t *position2d;


	client = playerc_client_create(NULL, "localhost", 6665);
	if (playerc_client_connect(client) != 0) {
		fprintf(stderr, "error: %s\n", playerc_error_str());	
		return -1;
	}

	laser = playerc_laser_create(client, 0);
	if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE)) {
		fprintf(stderr, "error: %s\n", playerc_error_str());
		return -1;
	}

	 position2d = playerc_position2d_create(client, 0);
	if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0) {
		fprintf(stderr, "error: %s\n", playerc_error_str());
		return -1;
	}

	playerc_client_read(client);
	playerc_client_read(client);

	playerc_position2d_enable(position2d, 1); //ativa motores do motor

	while(1) {
		playerc_client_read(client);


		if(laser->scan[180][0] < 2.5 || laser->scan[225][0] <2.5 || laser->scan[165][0] < 2.5 || laser->scan[140][0] < 2.5 || laser->scan[250][0] < 2.5) {
			playerc_position2d_set_cmd_vel(position2d, 0, 0, 0.4, 1);
			
		} else {
			playerc_position2d_set_cmd_vel(position2d, 2, 0, 0, 1);
		}
	}
	playerc_laser_unsubscribe(laser);
	playerc_laser_destroy(laser);
	playerc_position2d_unsubscribe(position2d);
	playerc_position2d_destroy(position2d);
	playerc_client_disconnect(client);
	playerc_client_destroy(client);

	return 0;
}

		

