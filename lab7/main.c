#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <libgen.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

struct _context_ {
    char* src;
    char* dest;
    mode_t mode;
};

struct _descriptors_ {
    int dest_fd;
    int src_fd;
};

typedef struct _context_ context;
typedef struct _descriptors_ descriptors;

pthread_attr_t attr;

void* copy_file(void*);

char* concat_file_path(char* dir_path, char* file_name) {
    unsigned int path_len = strlen(dir_path) + strlen(file_name);
    if (dir_path[strlen(dir_path) - 1] != '/')
        path_len += 1;

    char* path = (char*)malloc(sizeof(dir_path) * path_len);

    strcpy(path, dir_path);
    if (dir_path[strlen(dir_path) - 1] != '/')
        strcat(path, "/");
    strcat(path, file_name);

    return path;
}

void* copy_directory_content(void* context_ptr) {
    context* dir_context = (context*) context_ptr;

    char* source_dir_path = dir_context->src;
    char* dest_dir_path = dir_context->dest;

    DIR* source_dir = opendir(source_dir_path);

    if (source_dir == NULL) {
        //perror("can't open directory");
        while (errno == EMFILE) {
            errno = 0;
            sleep(1);
            source_dir = opendir(source_dir_path);
        }
    }

    struct dirent* curr_file;
    struct dirent* buffer_dir = (struct dirent*)malloc((sizeof(struct dirent) + pathconf(source_dir_path, _PC_NAME_MAX) + 1));
    struct stat file_stat;

    while ((readdir_r(source_dir, buffer_dir,  &curr_file)) == 0 && curr_file != NULL) {
        if (strcmp(curr_file->d_name, ".") == 0 || strcmp(curr_file->d_name, "..") == 0)
            continue;

        char* src_file_path = concat_file_path(source_dir_path, curr_file->d_name);

        stat(src_file_path, &file_stat);

        if (S_ISREG(file_stat.st_mode) || S_ISDIR(file_stat.st_mode)) {
            pthread_t thread;
            context* file_context = (context*)malloc(sizeof(context));

            file_context->src = src_file_path;
            file_context->dest = concat_file_path(dest_dir_path, curr_file->d_name);
            file_context->mode = file_stat.st_mode;

            pthread_create(&thread, &attr, copy_file, file_context);
        }
    }

    closedir(source_dir);

    free(buffer_dir);
    free(curr_file);
    free(dir_context->dest);
    free(dir_context->src);
    free(dir_context);
}

descriptors* open_dest_src_file(char* dest_path, char* src_path, mode_t mode) {
    descriptors* descrs = (descriptors*)malloc(sizeof(descriptors));

    int dest_fd = open(dest_path, O_WRONLY | O_CREAT, mode);
    if (dest_fd == -1 ) {
        //perror("can't open dest file");
        while (errno == EMFILE) {
            sleep(1);
            dest_fd = open(dest_path, O_WRONLY | O_CREAT, mode);
        }
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        //perror("can't open source file");
        do {
            close(dest_fd);
            do {
                sleep(1);
                dest_fd = open(dest_path, O_WRONLY | O_CREAT, mode);
            } while (errno == EMFILE);

            sleep(1);
            src_fd = open(src_path, O_RDONLY);
        } while (errno == EMFILE);
    }

    descrs->dest_fd = dest_fd;
    descrs->src_fd = src_fd;

    return descrs;
}

void* copy_file(void* context_ptr) {
    context* file_context = (context*) context_ptr;
    if (S_ISREG(file_context->mode)) {
        descriptors* descrs = open_dest_src_file(file_context->dest, file_context->src, file_context->mode);
        int dest_fd = descrs->dest_fd;
        int src_fd = descrs->src_fd;

        char* buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
        long bytes_read;
        while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
            write(dest_fd, buffer, bytes_read);
        }

        close(src_fd);
        close(dest_fd);

        free(buffer);
        free(descrs);
        free(file_context->src);
        free(file_context->dest);
        free(file_context);
    }
    else {
        if (S_ISDIR(file_context->mode)) {
            mkdir(file_context->dest, file_context->mode);
            //if (mkdir(file_context->dest, file_context->mode) != 0)
                //perror("mkdir");
            pthread_t thread;
            pthread_create(&thread, &attr, copy_directory_content, file_context);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Invalid arguments");
        return EINVAL;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    char* source_path = argv[1];
    char* destination_path = argv[2];

    struct stat file_stat;
    int status = stat(source_path, &file_stat);
    if (status != 0) {
        perror("stat failed");
        return errno;
    }
    context* file_context = (context*)malloc(sizeof(context));
    file_context->src = strdup(source_path);
    file_context->mode = file_stat.st_mode;

    if (S_ISDIR(file_stat.st_mode)) {
        file_context->dest = strdup(destination_path);
        mkdir(file_context->dest, file_context->mode);
        copy_directory_content(file_context);
    }
    else {
        file_context->dest = concat_file_path(destination_path, basename(source_path));
        copy_file(file_context);
    }

    pthread_exit(NULL);
}
