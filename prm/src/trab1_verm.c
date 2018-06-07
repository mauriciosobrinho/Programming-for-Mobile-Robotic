#include <stdio.h>
#include <libplayerc/playerc.h>
#include <math.h>
#include <string.h>

#define FOLLOW_WAYPOINT 1
#define GRAB_GREEN 2
#define RETURN_TO_DEST 3
#define GO_TO_GREEN 4


int main(int argc, const char **argv)
{
  int i, k, green;
	float turn_speed=0;
  playerc_client_t *client;
	playerc_position2d_t *position2d;
  playerc_laser_t *laser;
	playerc_blobfinder_t *bf;
	playerc_gripper_t *gripper;

  float vlong=0, vrot=0;
  float destx, desty, distobst, angdest, campo_obst, ang_rot;

  int state;

  state = FOLLOW_WAYPOINT;

  int playerIncrement = 0;

  if(argc >= 2){
    if(strcmp(argv[1], "1") == 0)
      playerIncrement = 1;
    else if(strcmp(argv[1], "2") == 0)
      playerIncrement = 2;
    else if(strcmp(argv[1], "3") == 0)
      playerIncrement = 3;    
  }


  client = playerc_client_create(NULL, "localhost", 7665 + playerIncrement);
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

  int WAYPOINTS[3][5][2] = {
    {
      {8, 5},
      {2, 5},
      {-4, 5},
      {-8, 6},
      {-11, 3}
    },
    {
      {0, -3},
      {0, -3},
      {7, -3},
      {7, 0},
      {0, 0}
    },
    {
      {5, -6.7},
      {3, -5},
      {-5, -5},
      {-8.5, -6},
      {-8.5, -3}
    }
  };
  int NUMBER_OF_WAYPOINTS = 5;
  int currentWaypoint = 0;

  while(1)
  {
    playerc_client_read(client);
		
    if(state == FOLLOW_WAYPOINT || state == RETURN_TO_DEST) {
      
      if(state == FOLLOW_WAYPOINT) {
        // Seta o próximo waypoint
        destx = WAYPOINTS[playerIncrement][currentWaypoint][0];
        desty = WAYPOINTS[playerIncrement][currentWaypoint][1];
      } else {
        // Vai para origem
        destx = +11.7;
        // destx = 0;

        desty = 0;
      }


      //Mover até waypoint
      distobst=sqrt((position2d->px-destx)*(position2d->px-destx)+ (position2d->py-desty)*(position2d->py-desty)) ;

      if (distobst > 0.2) vlong=0.5;
      else vlong=0;

      //Calcula força de atração
      angdest=atan2(desty-position2d->py, destx-position2d->px);

      ang_rot=position2d->pa-angdest;
      
      vrot=-ang_rot;
      if (vrot > 3.1415) vrot=vrot-6.283;
      if (vrot < -3.1415) vrot=vrot+6.283;




      //Calcula força de repulsão
      campo_obst=0;

      for(i=200; i<=360; i+=30) 
        if (laser->scan[i][0] < 1.0)
         campo_obst += 2.0-laser->scan[i][0];

      for(i=160; i>=0; i-=30) 
        if (laser->scan[i][0] < 1.0)
         campo_obst -= 2.0-laser->scan[i][0];

      vrot -= 0.5 * campo_obst;

      for(i=170; i<=200; i++){
        if(laser->scan[i][0] < 1){
          vlong = 0;
        }
      }

      if((vlong == 0) && (vrot == 0)){
        vrot = 0.3;
      }




      playerc_position2d_set_cmd_vel(position2d, vlong, 0, vrot, 1);

      // Ir até Blob Verde, exceto quando estiver retornando para origem
      if (bf->blobs_count > 0 && state != RETURN_TO_DEST) {
        for(k=0; k < bf->blobs_count; k++)
          if (bf->blobs[k].color==0xffff00) {
            state = GO_TO_GREEN;
            printf("[STATE] - Changing state to GO_TO_GREEN");
          }
      }

      // Verificar se robô está suficientemente próximo do waypoint
      if(state != GO_TO_GREEN && abs(position2d->px - destx) < 0.2 && abs(position2d->py - desty) < 0.2){

        // Se está no estado FOLLOW_WAYPOINT, vai para o próximo waypoint
        if(state == FOLLOW_WAYPOINT){
          currentWaypoint++;

          // Se já foi até o  último waypoint, vá para o primeiro waypoint
          if(currentWaypoint > NUMBER_OF_WAYPOINTS - 1)
            currentWaypoint = 0;         
        } else if(state == RETURN_TO_DEST) {
          // Se está no state RETURN_TO_DEST, soltar a bolinha verde
          playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);
          playerc_gripper_open_cmd(gripper);
          for (i = 0; i < 25; i++)
            playerc_client_read(client);        
          state = FOLLOW_WAYPOINT;
        }


        printf("[Player %d][WAYPOINT] - Changing WAYPOINT to (%f, %f)\n", playerIncrement, destx, desty);
      }

    } else if (state == GO_TO_GREEN) {




      // Encontrar blob verde
      if (bf->blobs_count > 0) {
        int closest = 0;
        for(k = 0; k < bf->blobs_count; k++){
          if (bf->blobs[k].color==0xffff00) {
            if(bf->blobs[k].range <= bf->blobs[closest].range)
              closest = k;
          }
        }

        if(bf->blobs[closest].x<38)
          turn_speed=0.2;
        else if(bf->blobs[closest].x>42)
          turn_speed=-0.2;
        else
          turn_speed=0.0;
    
        if(bf->blobs[closest].range > 1.0){
          //Calcula força de repulsão
          campo_obst=0;

          for(i=200; i<=360; i+=30) 
            if (laser->scan[i][0] < 1.0)
             campo_obst += 2.0-laser->scan[i][0];

          for(i=160; i>=0; i-=30) 
            if (laser->scan[i][0] < 1.0)
             campo_obst -= 2.0-laser->scan[i][0];

          turn_speed -= 0.5 * campo_obst;
        }

        if(bf->blobs[closest].range < 0.4) {
          state = GRAB_GREEN;
          printf("[STATE] - Changing state to GRAB_GREEN \n");
        }
      } else {
        state = FOLLOW_WAYPOINT;
      }
      
      playerc_position2d_set_cmd_vel(position2d, 0.8, 0, turn_speed, 1);


    } else if (state == GRAB_GREEN) {
      playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);

      playerc_gripper_open_cmd(gripper);
      playerc_gripper_close_cmd(gripper);
      for (i = 0; i < 25; i++)
        playerc_client_read(client);

      state = RETURN_TO_DEST;
      printf("[STATE] - Changing state to RETURN_TO_DEST \n");

    }

  } 

  

  

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
