#include "blob_network_processor.h"

//it is better to switch to libcurl https://curl.se/libcurl/c/http2-download.html
#include "httplib.h"

struct HttpNetworkProcessor:public BlobNetworkProcessor
{
  virtual bool canDownload() {return true;}
  virtual bool canUpload() {return false;}
  virtual bool download(const char *hex_hash, std::function<bool(const char *data, size_t data_length)> cb, std::string &err)
  {
    char buf[128];
    std::snprintf(buf, sizeof(buf),
       "%s%.2s/%.2s/%.64s",
       repo.c_str(),
       hex_hash, hex_hash + 2, hex_hash);
    size_t current = 0;
    auto res = client->Get(buf,
      [&](const char *data, size_t data_length) { return cb(data, data_length); }
    );
    if (!res || res->status != 200)
    {
      err = res ? httplib::detail::status_message(res->status) : "unknown";
      err += " err code";
      err += res ? res->status : -1;
      return false;
    }
    return true;
  }
  virtual bool upload(const char *file, bool compress, char *hex_hash, std::string &err) {
    //it is absolutely simple to implement uploading.
    //However, default nginx can't allow upload to subfolders (nor it will check validity of provided blob).
    //So, we first need to write our own nginx module, that will:
    //  a) create all nesessary subfolders.
    //  b) it also should be atomic, i.e. write to tmpfile, and only on success rename to fial destination
    //  b) (optionally) validate that's it hash is correct, before writing out the file (can be done with separate utility, that validates blob)
    // sample uploading module is available here: https://www.yanxurui.cc/posts/server/2017-03-21-NGINX-as-a-file-server/#upload
    // but it doesn't do any of a, b, c
    return false;
  }
  std::unique_ptr<httplib::Client> client;
  std::string repo;
};

BlobNetworkProcessor *get_http_processor(const char *url, int port, const char *repo, const char *user, const char *passwd)
{
  auto ret =  new HttpNetworkProcessor();
  ret->client.reset(new httplib::Client(url, port));
  ret->repo = repo;
  ret->client->set_keep_alive(true);
  ret->client->set_tcp_nodelay(true);
  if (user && passwd)
    ret->client->set_basic_auth(user, passwd);
  return ret;
}
