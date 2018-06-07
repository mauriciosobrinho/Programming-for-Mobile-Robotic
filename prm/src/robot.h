#include <stdio.h>
#include <libplayerc/playerc.h>
#include <math.h>



typedef struct Robot {
	// Library vars
	playerc_client_t *client;
	playerc_position2d_t *position2d;
	playerc_laser_t *laser;
	playerc_blobfinder_t *bf;
	playerc_gripper_t *gripper;

	// Robot vars


} Robot;


int init(Robot *);