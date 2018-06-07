#include "robot.h"


int init(Robot * r) {
	r->client = playerc_client_create(NULL, "localhost", 6665);
	if (playerc_client_connect(client) != 0)
    	return -1;

	r->position2d = playerc_position2d_create(client, 0);
  	if (playerc_position2d_subscribe(r->position2d, PLAYERC_OPEN_MODE) != 0) {
    	fprintf(stderr, "error: %s\n", playerc_error_str());
    	return -1;
  	}

	r->laser = playerc_laser_create(r->client, 0);
 	if (playerc_laser_subscribe(r->laser, PLAYERC_OPEN_MODE))
    	return -1;

	r->bf = playerc_blobfinder_create(r->client, 0);
  	if (playerc_blobfinder_subscribe(r->bf, PLAYER_OPEN_MODE))
    	return -1;

	r->gripper = playerc_gripper_create(r->client, 0);
  	if (playerc_gripper_subscribe(r->gripper, PLAYER_OPEN_MODE))
    	return -1;

	playerc_position2d_enable(r->position2d, 1);
}

