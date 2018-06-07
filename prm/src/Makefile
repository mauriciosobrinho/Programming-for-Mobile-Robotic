all: pos sonar laser gripper blob aprox	grab_green pot trab1 trab1verm

pos:	
	gcc -o pos position2d.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

sonar:	
	gcc -o sonar sonar.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

laser:	
	gcc -o laser laser.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

gripper:	
	gcc -o grip gripper.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc 

blob:	
	gcc -o blob blobfinder.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc 

aprox:
	gcc -o aprox aprox.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

grab_green:
	gcc -o grab_green grab_green2.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

pot:
	gcc -o pot campos_pot.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

trab1:
	gcc -g -o trab1 trab1.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc

trab1verm:
	gcc -g -o trab1verm trab1_verm.c -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc