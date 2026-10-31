#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <openssl/sha.h>
#include <sys/stat.h>

#define URL_DB_FILE "/data/cop_aliases.txt"
#define MAX_URL 1024

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

// Search Alias in the database (cop_aliases.txt) is it really a database? Well database sounds cooler than registry
// a line looks like this now:  name=url sha256hash
int find_alias(const char *input, char *url_out, char *hash_out) {
    FILE *f = fopen(URL_DB_FILE, "r");
    if (!f) return 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;

        char *alias = strtok(line, "=");
        char *rest = strtok(NULL, "\n");
        if (alias && rest && strcmp(alias, input) == 0) {
            hash_out[0] = 0;
            sscanf(rest, "%1023s %64s", url_out, hash_out); // url, space, hash
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <package_name>\n", argv[0]);
        return 1;
    }

    char *pkg_name = argv[1];
    char url[MAX_URL];
    char hash[65]; // sha256 is 64 characters + the \0

    // Always look up the URL from the alias file
    if (!find_alias(pkg_name, url, hash)) {
        fprintf(stderr, "Error: Package '%s' not found in %s\n", pkg_name, URL_DB_FILE);
        return 1;
    }

    // no hash in the file = no install. no exceptions, not even for me
    if (strlen(hash) != 64) {
        fprintf(stderr, "Error: '%s' has no valid sha256 hash in %s\n", pkg_name, URL_DB_FILE);
        return 1;
    }

    printf("Found package '%s'. Installing\n", pkg_name);

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
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // 404 is an error now. before it would install the error page as a program lol
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // still off. the hash check below is what keeps this safe now
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; curl/7.68.0)"); // set a user agent to avoid bot-blocking

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK || mem.size == 0) {
        fprintf(stderr, "download failed: %s\n", curl_easy_strerror(res));
        free(mem.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)mem.data, mem.size, digest);

    // turn the 32 bytes into text so we can compare it with the one in the file
    char got[65];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(got + i * 2, "%02x", digest[i]);

    if (strcmp(got, hash) != 0) { // hashes in the file have to be lowercase (sha256sum does that anyway)
        fprintf(stderr, "Hash does not match! Not installing.\n");
        fprintf(stderr, "wanted: %s\n", hash);
        fprintf(stderr, "got:    %s\n", got);
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

    printf("Package '%s' installed with 256-SHA verification to %s\n", pkg_name, bin_path);

    free(mem.data);
    curl_easy_cleanup(curl);

    return 0;
}