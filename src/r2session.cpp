#include "r2session.h"

#include <stdlib.h>
#include <string.h>

// Helper to run r2 commands and get output as string
char* r2_cmd(R2Session* session, const char* cmd) {
  char full_cmd[512];
  // We use radare2 -q (quiet) to execute a command and exit
  sprintf(full_cmd, "radare2 -q -c \"%s\" %s", cmd, session->filename);

  FILE* fp = _popen(full_cmd, "r");  // Use popen on Linux
  if (!fp) return NULL;

  char* response = (char*)malloc(1024 * 512);  // 512KB buffer for JSON
  size_t len = fread(response, 1, 1024 * 512 - 1, fp);
  response[len] = '\0';
  _pclose(fp);
  return response;
}

R2Session* r2_open(const char* filename, bool do_analysis, bool write) {
  R2Session* session = (R2Session*)malloc(sizeof(R2Session));
  strncpy(session->filename, filename, 255);

  // Get info (ij)
  char* info_str = r2_cmd(session, "ij");
  cJSON* info = cJSON_Parse(info_str);

  cJSON* bin = cJSON_GetObjectItem(info, "bin");
  if (!bin) {
    fprintf(stderr, "File type not supported\n");
    return NULL;
  }

  strncpy(session->arch, cJSON_GetObjectItem(bin, "arch")->valuestring, 15);
  session->bits = cJSON_GetObjectItem(bin, "bits")->valueint;

  if (do_analysis) {
    r2_cmd(session, "aaa");  // This might take time
  }

  free(info_str);
  cJSON_Delete(info);
  return session;
}

cJSON* r2_get_functions(R2Session* session) {
  char* fcns_str = r2_cmd(session, "aflj");
  cJSON* json = cJSON_Parse(fcns_str);
  free(fcns_str);
  return json;
}

cJSON* r2_disasm_function(R2Session* session, const char* func_name) {
  char cmd[128];
  sprintf(cmd, "pdfj @ %s", func_name);
  char* dis_str = r2_cmd(session, cmd);
  cJSON* json = cJSON_Parse(dis_str);
  free(dis_str);
  return json;
}

void r2_close(R2Session* session) {
  free(session);
}

// int r2_write_patches(R2Session* session, cJSON* patches) {
//   if (!patches || !cJSON_IsArray(patches)) return 0;

//   int count = 0;
//   cJSON* patch = NULL;

//   cJSON_ArrayForEach(patch, patches) {
//     cJSON* offset_item = cJSON_GetObjectItem(patch, "offset");
//     cJSON* bytes_item = cJSON_GetObjectItem(patch, "newbytes");

//     if (offset_item && bytes_item) {
//       char cmd[512];
//       // Format: wx <hex_bytes> @ <offset>
//       // We use %lld for offset to handle 64-bit addresses
//       sprintf(cmd, "wx %s @ %lld", bytes_item->valuestring,
//               (long long)offset_item->valuedouble);

//       // Execute the write command
//       char* res = r2_cmd(session, cmd);
//       if (res) free(res);

//       count++;
//     }
//   }
//   return count;
// }

int r2_write_patches(R2Session* session, cJSON* patches) {
  if (!patches || !cJSON_IsArray(patches)) return 0;

  int count = 0;
  cJSON* patch = NULL;

  cJSON_ArrayForEach(patch, patches) {
    cJSON* offset_item = cJSON_GetObjectItem(patch, "offset");
    cJSON* bytes_item = cJSON_GetObjectItem(patch, "newbytes");

    if (offset_item && bytes_item) {
      char cmd[512];
      // Format: wx <hex_bytes> @ <offset>
      // We use %lld for offset to handle 64-bit addresses
      sprintf(cmd, "wx %s @ %lld", bytes_item->valuestring,
              (long long)offset_item->valuedouble);

      // Execute the write command
      char* res = r2_cmd(session, cmd);
      if (res) free(res);

      count++;
    }
  }
  return count;
}