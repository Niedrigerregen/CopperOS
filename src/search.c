/* The search command allows users to visit the World Wide Web
* by entering either a URL or an alias defined in the aliases.txt file the program will fetch it from the internet and display the text content
* Aliases are defined in the aliases.txt file in the format: alias=url
* Under the hood, it's a big boy html to text converter that uses libcurl to perform HTTP requests. R.E.V.O.L.U.T.I.O.N.A.R.Y.
* SSL Verification is disabled because of some unexplainable errors with qemu
* as QEMU gives out it's own IP address there is no need for driver stuff
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define URL_DB_FILE "/data/srch_aliases.txt"
#define MAX_URL 512

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

//converts html to text by removing everything between html tags and styling stuff for more readability
void html_to_text(char *html) {
    int in_tag = 0;
    int in_braces = 0;
    int in_brackets = 0;
    for (size_t i = 0; html[i]; i++) {
        if (html[i] == '<') in_tag = 1;
        else if (html[i] == '>') in_tag = 0;
        else if (html[i] == '{') in_braces = 1;
        else if (html[i] == '}') in_braces = 0;
        else if (!in_tag && !in_braces) putchar(html[i]);
        else if (html[i] == '\n' && in_tag) putchar('\n'); // add newlines for block elements
        else if (html[i] == '\t' && in_tag) putchar('\t'); // add tabs for block elements
    }
}

// Searches Alias in the database (aliases.txt)
int find_alias(const char *input, char *url_out) {
    FILE *f = fopen(URL_DB_FILE, "r");
    if (!f) return 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // remove trailing newline / carriage return
        line[strcspn(line, "\r\n")] = 0;

        char *alias = strtok(line, "=");
        char *url = strtok(NULL, "\n");
        if (alias && url && strcmp(alias, input) == 0) {
            strncpy(url_out, url, MAX_URL-1);
            url_out[MAX_URL-1] = 0;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    char input[128];
    char url[MAX_URL];

    if (argc > 1) {
        // Use command line argument
        strncpy(input, argv[1], sizeof(input) - 1);
        input[sizeof(input) - 1] = '\0';
    } else {
        // Prompt for input
        printf("reminder: command line arguments are supported for most commands\n");
        printf("(Enter site (alias or full URL): ");
        if (!fgets(input, sizeof(input), stdin)) return 1;
        input[strcspn(input, "\r\n")] = 0;
    }

    if (!find_alias(input, url)) {
        // If not an alias, assume it's a URL
        strncpy(url, input, MAX_URL-1);
        url[MAX_URL-1] = 0;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "libcurl init failed\n");
        return 1;
    }

    // Most disablings are because qemu is handling the network stuff which makes verification not possible
    struct Memory mem = {0};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Disable SSL cert verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  // Disable hostname verification. 
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; curl/7.68.0)"); // set user-agent to avoid blocking by some sites (mainly wikipedia)

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
        free(mem.data);
        curl_easy_cleanup(curl);
        return 1;
    }

    html_to_text(mem.data);

    free(mem.data);
    curl_easy_cleanup(curl);
    return 0;
}