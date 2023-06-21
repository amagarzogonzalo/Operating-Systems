#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int my_system(const char* command){
	int s;
	pid_t p = fork(); 

	if(p == -1){ 
		s = -1;
	else if(p != 0){
		while(p != wait(&s));
		if(WIFEXITED(s)) s = 0; 
		else s = -1;
	}
	else{ 
		execl("/bin/bash","bash","-c",  command, (char*)0);
		exit(127);
	}
	return s;
}
int main(int argc, char* argv[]){
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return my_system(argv[1]);
	
}

