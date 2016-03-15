#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "phantomOP.hpp"
int to_wav(char* filepass){
	
	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		return -1;
	} else if (pid == 0){
		execlp("ffmpeg","ffmpeg","-i",filepass,"output.wav",NULL);
		perror("execlp");
		return -1;
	}

	int status;
	pid_t t = waitpid(pid,&status,0);
	if (t < 0){
		perror("waitpid");
		return -1;
	}
	if (WIFEXITED(status)){
		return 0;
	} else {
		return -1;
	}

}

int to_avi(char* filepass){
	
        pid_t pid = fork();
        if(pid < 0) {
                perror("fork");
                return -1;
        } else if (pid == 0){
                execlp("ffmpeg","ffmpeg","-i",filepass,"-i","output.wav","-vcodec","copy","-acodec","copy","output.avi",NULL);
                perror("execlp");
                return -1;
        }

        int status;
        pid_t t = waitpid(pid,&status,0);
        if (t < 0){
                perror("waitpid");
                return -1;
        }
        if (WIFEXITED(status)){
                return 0;
        } else {
                return -1;
        }

}	

