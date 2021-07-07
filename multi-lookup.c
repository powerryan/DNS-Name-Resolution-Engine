#include "multi-lookup.h"

sem_t empty;
pthread_mutex_t buffer_m;
pthread_mutex_t input_m;
char *buff[80];
char *files[MAX_INPUT_FILES];
int ptfiles[MAX_PARSE_THREADS];
int count = 0;
int tail = 0;
int head = 0;
char *poutput;
char *coutput;
int pthreadcount = 0;
int filecount = 0;

int isFull() {
  if (count >= 80) {
    return 1;
  }
  return 0;
}

int isEmpty() {
  return !count;
}

int addToBuffer(char**buff, char*add) {
  if (isFull(buff)) {
    return 0;
  }
  buff[tail] = add;
  tail = (tail+1)%80;
  count++;
  return 1;
}

char* removeFromBuffer(char**buff){
  if (isEmpty(buff)) {
    return NULL;
  }
  else {
    char* result = buff[head];
    buff[head] = NULL;
    head = (head+1) % 80;
    count--;
    if (isEmpty(buff)) {
      head = 0;
      tail = 0;
    }
    return result;
  }
}

void* parse(void *tnumber){
  FILE *pout;
	FILE *stream;
  int tnum = *((int*)tnumber);
  while(1){
    pout = fopen(poutput, "a");
    pthread_mutex_lock(&input_m);
    if (filecount < MAX_INPUT_FILES){
  	   stream = fopen(files[filecount++], "r");
       if(!stream){
         pthreadcount = pthreadcount-1;
         pthread_mutex_unlock(&input_m);
         fprintf(pout, "Thread <%ld> serviced %d file(s)\n", pthread_self(), ptfiles[tnum]);
         fclose(pout);
         pthread_exit(0);
         return NULL;
       }
       pthread_mutex_unlock(&input_m);
    }
    else {
      pthreadcount = pthreadcount-1;
      pthread_mutex_unlock(&input_m);
      fprintf(pout, "Thread <%ld> serviced %d file(s)\n", pthread_self(), ptfiles[tnum]);
      fclose(pout);
      pthread_exit(0);
      return NULL;
    }
  	if(stream != NULL) {
      if (ptfiles[tnum]){
        ptfiles[tnum]++;
      }
      else{
        ptfiles[tnum]=1;
      }
  	  char* str = malloc(sizeof(char) * MAX_NAME_LENGTH);
  		while(fscanf(stream, "%s", str) == 1){
        char* line = malloc(sizeof(str));
        strcpy(line, str);
        sem_wait(&empty);
        pthread_mutex_lock(&buffer_m);
  			addToBuffer(buff, line);
        pthread_mutex_unlock(&buffer_m);
  		}
  		free(str);
  		fclose(stream);
    }
    fclose(pout);
  }
  pthread_exit(0);
}

void* convert(){
  FILE *stream;
	char* ip = malloc(sizeof(char) * MAX_IP_LENGTH);
	strcpy(ip, "");
  while (1) {
    stream = fopen(coutput, "a");
    pthread_mutex_lock(&buffer_m);
    while (isEmpty()){
        pthread_mutex_unlock(&buffer_m);
        pthread_mutex_lock(&input_m);
        if (!pthreadcount) {
          pthread_mutex_unlock(&input_m);
          pthread_exit(0);
          return NULL;
        }
        pthread_mutex_unlock(&input_m);
        pthread_mutex_lock(&buffer_m);
    }
    pthread_mutex_unlock(&buffer_m);
    char* hostname = removeFromBuffer(buff);
    pthread_mutex_unlock(&buffer_m);
    if(dnslookup(hostname, ip, 46) == 0){
  			fprintf(stream, "%s,%s\n", hostname, ip);
  	}
    else{
        if(hostname) {
  			     fprintf(stream, "%s,\n", hostname);
             fprintf(stderr, "Domain name cannot be resolved: %s\n", hostname);
      }
  	}
    strcpy(ip, "");
    sem_post(&empty);
    fclose(stream);
  }
		free(ip);
    pthread_exit(0);
}

int main(int argc, char *argv[]){
  int parseThreads;
  int convertThreads;
  int threadnum[MAX_PARSE_THREADS];
  struct timeval begin, finish;
  long seconds;
  long ms;
  gettimeofday(&begin, NULL);
  pthread_mutex_init(&buffer_m, NULL);
  pthread_mutex_init(&input_m, NULL);
  sem_init(&empty, 0, 80);
	poutput = argv[3];
  coutput = argv[4];
  parseThreads = atoi(argv[1]);
  convertThreads = atoi(argv[2]);
  pthreadcount = parseThreads;
  pthread_t pro[parseThreads], con[convertThreads];
  int j = 0;
  for (int i = 5; i < argc; i++) {
    files[j] = argv[i];
    j++;
  }
  for (int i = 0; i < MAX_PARSE_THREADS; i++){
    threadnum[i] = i;
  }
  for (int i=0; i < parseThreads; i++) {
    pthread_create(&pro[i], NULL, parse, (void *)&threadnum[i]);
  }
  for (int i=0; i < convertThreads; i++) {
    pthread_create(&con[i], NULL, convert, NULL);
  }
  for (int i=0; i < parseThreads; i++) {
    pthread_join(pro[i], NULL);
  }
  for (int i=0; i < convertThreads; i++) {
    pthread_join(con[i], NULL);
  }
  pthread_mutex_destroy(&buffer_m);
  pthread_mutex_destroy(&input_m);
  sem_destroy(&empty);
  gettimeofday(&finish, NULL);
  seconds = (finish.tv_sec - begin.tv_sec);
  ms = (finish.tv_usec - begin.tv_usec);
  if (ms < 0){
    seconds = seconds - 1;
    ms = 1000000 + ms;
  }
  printf("Runtime: %ld.%ld seconds\n", seconds, ms);
  return 0;
}
