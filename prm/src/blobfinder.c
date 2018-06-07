#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i, k;
  playerc_client_t *client;
  playerc_blobfinder_t *bf;

  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
    return -1;

  bf = playerc_blobfinder_create(client, 0);
  if (playerc_blobfinder_subscribe(bf, PLAYER_OPEN_MODE))
    return -1;

  while(1)
  {
    playerc_client_read(client);
    
    printf("\nNumber of blobs:%u\n",bf->blobs_count);

    for(k = 0; k < bf->blobs_count; k++)
	if (bf->blobs[k].color == 0xff00) 
		printf("Color:%8x, x:%2u range:%.2f\n", bf->blobs[k].color, bf->blobs[k].x, bf->blobs[k].range);
  } 

  playerc_blobfinder_unsubscribe(bf);
  playerc_blobfinder_destroy(bf);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
