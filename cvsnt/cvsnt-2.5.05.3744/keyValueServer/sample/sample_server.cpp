#include <stdio.h>
#include <stdlib.h>
#include "../include/blob_server.h"
#include "../sampleImplementation/def_log_printf.cpp"
#include "../include/blob_sockets.h"//move init to out of line

int main(int argc, const char **argv)
{
  const int pc = 1;
  if (argc < 2)
    printf("Usage is sample_server [port](2403) [max_pending_connections](1024)\n");
  if (!blob_init_sockets())
  {
    blob_logmessage(LOG_ERROR, "Can't init sockets, %d", blob_get_last_sock_error());
    return 1;
  }
  int port = argc>pc ? atoi(argv[pc]) : 2403;
  int max_pending = argc>pc+1 ? atoi(argv[pc+1]) : 1024;
  volatile bool shouldStop = false;
  printf("Starting server listening at port %d\n", port);
  const bool result = start_push_server(port, max_pending, nullptr);
  printf("server quit %s", result ? "with error\n" :"normally\n");
  blob_close_sockets();
  return result ? 0 : 1;
}