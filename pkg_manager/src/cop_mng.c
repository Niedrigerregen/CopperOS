#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>

struct Memory {
    char *data;
    size_t size;
};

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    struct Memory *mem = (struct Memory *)userdata;
    char *tmp = realloc(mem->data, mem->size + total);
    if (!tmp) return 0;
    mem->data = tmp;
    memcpy(mem->data + mem->size, ptr, total);
    mem->size += total;
    return total;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <package_name> <url>\n", argv[0]);
        return 1;
    }

    char *pkg_name = argv[1];
    char *url = argv[2];

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
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // disable SSL certificate verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // disable SSL hostname verification. I don't know anymore as i deleted all comments by accident but i'll keep it that way
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; curl/7.68.0)"); // set a user agent to avoid bot-blocking

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        free(mem.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    char bin_path[1024];
    snprintf(bin_path, sizeof(bin_path), "/bin/%s", pkg_name); /* install to /bin. Normally you would use an unzipper
                                                                  as packages are usually zipped but mine are binaries as it's simpler */

    FILE *fp = fopen(bin_path, "wb");
    if (!fp) {
        perror("fopen");
        free(mem.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    fwrite(mem.data, 1, mem.size, fp);
    fclose(fp);

    if (chmod(bin_path, 0755) != 0) { // make files executable
        perror("chmod");
    }

    printf("Package '%s' installed to %s\n", pkg_name, bin_path);

    free(mem.data);
    curl_easy_cleanup(curl);

    return 0;
}