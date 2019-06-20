/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ulfius.h>

//#include <hiredis/hiredis.h>

#define PORT 8080
#define CHUNK (10 * 1024 * 1024)

char *readFile() {
    size_t nread;
    FILE *fp;
    long lSize;
    char *buffer;
    size_t result;

    fp = fopen("./file.txt", "r"); // read mode

    fseek (fp , 0 , SEEK_END);
    lSize = ftell (fp);
    rewind (fp);

    buffer = (char*) malloc (sizeof(char)*lSize);
    result = fread (buffer,1,lSize,fp);

    fclose (fp);
    return buffer;
}

/**
 * Callback function for the web application on /helloworld url call
 */
int callback_hello_world (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Hello World!");
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function for the web application on /fibonacci url call
 */
int callback_fibonacci (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int i, n = 10000, t1 = 0, t2 = 1, nextTerm;

  for (i = 1; i <= n; ++i)
  {
    // printf("%d\n", t1);
    nextTerm = t1 + t2;
    t1 = t2;
    t2 = nextTerm;
  }

  ulfius_set_string_body_response(response, 200, "Fibonacci!");
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function for the web application on /readfile url call
 */
int callback_read_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
    char *buffer;

    buffer = readFile();

    printf("File content: %s", buffer);
    free (buffer);

    ulfius_set_string_body_response(response, 200, "File read!");
    return U_CALLBACK_CONTINUE;
}

/**
 * Callback function for the web application on /redis url call
 */
// int callback_redis (const struct _u_request * request, struct _u_response * response, void * user_data) {
//     redisContext *c;
//     redisReply *reply;
//     const char *hostname = "127.0.0.1";
//     int port = 6379;

//     struct timeval timeout = { 1, 500000 }; // 1.5 seconds
//     c = redisConnectWithTimeout(hostname, port, timeout);
//     if (c == NULL || c->err) {
//         if (c) {
//             printf("Connection error: %s\n", c->errstr);
//             redisFree(c);
//         } else {
//             printf("Connection error: can't allocate redis context\n");
//         }
//         exit(1);
//     }

//     /* Set a key */
//     reply = redisCommand(c,"SET %s %s", "foo", "hello world");
//     printf("SET: %s\n", reply->str);
    
//     freeReplyObject(reply);
//     redisFree(c);
  
//     ulfius_set_string_body_response(response, 200, "Redis!");
//     return U_CALLBACK_CONTINUE;
// }

/**
 * Callback function for the web application on /storefile url call
 */
int callback_storefile (const struct _u_request * request, struct _u_response * response, void * user_data) {
    redisContext *c;
    redisReply *reply;
    const char *hostname = "127.0.0.1";
    int port = 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    char *buffer;
    buffer = readFile();

    /* Set a key */
    reply = redisCommand(c,"SET %s %s", "file", buffer);
    printf("SET: %s\n", reply->str);
    
    freeReplyObject(reply);
    redisFree(c);
    free (buffer);
  
    ulfius_set_string_body_response(response, 200, "File stored in Redis!");
    return U_CALLBACK_CONTINUE;
}

/**
 * main function
 */
int main(void) {
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/helloworld", NULL, 0, &callback_hello_world, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", "/readfile", NULL, 0, &callback_read_file, NULL);
  // ulfius_add_endpoint_by_val(&instance, "GET", "/redis", NULL, 0, &callback_redis, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", "/storefile", NULL, 0, &callback_storefile, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", "/fibonacci", NULL, 0, &callback_fibonacci, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    // Wait for the user to press <enter> on the console to quit the application
    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
}