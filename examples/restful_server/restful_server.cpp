/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#include "mongoose.h"
#include <string.h>
#include <iostream>

static const char *s_http_port = "800";
static struct mg_serve_http_opts s_http_server_opts;

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};

void *array_concat(const void *a, size_t an,
                   const void *b, size_t bn, size_t s)
{
  char *p = malloc(s * (an + bn));
  memcpy(p, a, an*s);
  memcpy(p + an*s, b, bn*s);
  return p;
}

static void handle_upload(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;


  switch (ev) {
    case MG_EV_HTTP_PART_BEGIN: {
      if (data == NULL) {
        data = calloc(1, sizeof(struct file_writer_data));
        data->fp = fopen(mp->file_name, "w");
        data->bytes_written = 0;
        if (data->fp == NULL) {
          mg_printf(nc, "%s",
                    "HTTP/1.1 500 Failed to open a file\r\n"
                    "Content-Length: 0\r\n\r\n");
          nc->flags |= MG_F_SEND_AND_CLOSE;
          free(data);
          return;
        }
        nc->user_data = (void *) data;
      }

      break;
    }
    case MG_EV_HTTP_PART_DATA: {
      if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      data->bytes_written += mp->data.len;
     
      break;
    }
    case MG_EV_HTTP_PART_END: {
      char command[50];
  

  char delim[] = ".";

  char *file_name = strtok(mp->file_name, delim);
  char *ext = strtok(NULL, delim);

   int *c = ARRAY_CONCAT(char, a, 5, b, 5);
 
  for(i = 0; i < 10; i++)
    printf("%d\n", c[i]);
  

if (strcmp(ext, "cpp") == 0)
{
  strcpy(command, "g++ -o " + file_name + " " + mp->file_name);
}

system(command);
system(file_name +  ">out.txt");

sleep(2);

char ch, file[1000];
   FILE *fp;
 
   fp = fopen("out.txt", "r"); // read mode
 
   if (fp == NULL)
   {
      exit(EXIT_FAILURE);
   }


  for (int i = 0; i < sizeof(file); ++i)
  {
     file[i] = fgetc(fp);
  }
 
   fclose(fp);
  
  mg_printf(nc,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n\r\n"
                "Written %s of POST data to a temp file\n\n",
                file);
      nc->flags |= MG_F_SEND_AND_CLOSE;
      fclose(data->fp);
      free(data);
      nc->user_data = NULL;
      break;
    }
  }

  




 
}


// static void handle_sum_call(struct mg_connection *nc, struct http_message *hm) {
//   char n1[100], n2[100];
//   double result;

//   /* Get form variables */
//   mg_get_http_var(&hm->body, "n1", n1, sizeof(n1));

//   /* Send headers */
//   mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

//   /* Compute the result and send it back as a JSON object */
//   result = strtod(n1, NULL) + strtod(n2, NULL);
//   mg_printf_http_chunk(nc, "{ \"result\": %lf }", result);
//   mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
// }

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/api/v1/compile") == 0) {
       // handle_sum_call(nc, hm); /* Handle RESTful call */
      } else if (mg_vcmp(&hm->uri, "/upload") == 0) {
         mg_serve_http(nc, hm, s_http_server_opts);
      } 
      else if (mg_vcmp(&hm->uri, "/printcontent") == 0) {
        char buf[100] = {0};
        memcpy(buf, hm->body.p,
               sizeof(buf) - 1 < hm->body.len ? sizeof(buf) - 1 : hm->body.len);
        printf("%s\n", buf);
      } else {
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  int i;
  char *cp;
  const char *err_str;
#if MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  mg_mgr_init(&mgr, NULL);

  /* Use current binary directory as document root */
  if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
    *cp = '\0';
    s_http_server_opts.document_root = argv[0];
  }

  /* Process command line options to customize HTTP server */
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
      mgr.hexdump_file = argv[++i];
    } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
      s_http_server_opts.document_root = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      s_http_port = argv[++i];
    } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
      s_http_server_opts.auth_domain = argv[++i];
    } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
      s_http_server_opts.global_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc) {
      s_http_server_opts.per_directory_auth_file = argv[++i];
    } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
      s_http_server_opts.url_rewrites = argv[++i];
#if MG_ENABLE_HTTP_CGI
    } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      s_http_server_opts.cgi_interpreter = argv[++i];
#endif
#if MG_ENABLE_SSL
    } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
      ssl_cert = argv[++i];
#endif
    } else {
      fprintf(stderr, "Unknown option: [%s]\n", argv[i]);
      exit(1);
    }
  }

  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
  if (ssl_cert != NULL) {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  mg_register_http_endpoint(nc, "/upload", handle_upload MG_UD_ARG(NULL));

  printf("Starting RESTful server on port %s, serving %s\n", s_http_port,
         s_http_server_opts.document_root);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
