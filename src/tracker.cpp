#include <iostream>
#include <string.h>
#include <curl/curl.h>

namespace tracker {
  size_t write_data(void *ptr, size_t size, size_t nmemb, std::string *res) {
    (*res) += (char*)ptr;
    return size*nmemb;
  }

  std::string getSuggestion(std::string query) {
    CURL *curl;
    CURLcode resCode;
    std::string res;
    curl = curl_easy_init();
    if(curl) {
	  std::string url = "http://uk.cube2.org/findserver/";
	  url += query;
	  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);
	  resCode = curl_easy_perform(curl);
	  curl_easy_cleanup(curl);
	  if (resCode == CURLE_OK) return res;
    }
	return query;
  }
}
