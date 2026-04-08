#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_URL 1024

struct Memory {
    char *data;
    size_t size;
};

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    struct Memory *mem = (struct Memory *)userdata;
    char *tmp = realloc(mem->data, mem->size + total + 1);
    if (!tmp) return 0;
    mem->data = tmp;
    memcpy(mem->data + mem->size, ptr, total);
    mem->size += total;
    mem->data[mem->size] = 0;
    return total;
}

// using a modified main function from search.c as i imagine it as a similar process although not functioning yet
int main(int argc, char *argv[]) {
    char input[1024];
    char url[MAX_URL];

    if (argc > 1) {
        // Use command line argument
        strncpy(input, argv[1], sizeof(input) - 1);
        input[sizeof(input) - 1] = '\0';
    } else {
        // Prompt for input
        printf("Enter wanted package : ");
        if (!fgets(input, sizeof(input), stdin)) return 1;
        input[strcspn(input, "\r\n")] = 0;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "libcurl init failed\n");
        return 1;
    }

    struct Memory mem = {0};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Disable SSL cert verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Disable hostname verification. both of these are some really bad security risks but only on hardware
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; curl/7.68.0)"); // set user-agent to avoid blocking by some sites (mainly wikipedia)

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        free(mem.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    free(mem.data);
    curl_easy_cleanup(curl);
    return 0;
}