/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <stdlib.h>
#include <ulfius.h>

#define PORT 8080
#define CHUNK (10 * 1024 * 1024)

/**
 * Callback function for the web application on /helloworld url call
 */
int callback_hello_world (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Hello World!");
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function for the web application on /readfile url call
 */
int callback_read_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
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

    //printf("File content: %s", buffer);

    fclose (fp);
    free (buffer);

/* 
    char *buf = malloc(CHUNK);

    if (buf == NULL) {
    }

    while ((nread = fread(buf, 1, CHUNK, fp)) > 0) {
        //fwrite(buf, 1, nread, stdout);
    }

    if (ferror(fp)) {
    }
    fclose(fp);
*/

    ulfius_set_string_body_response(response, 200, "File read!");
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