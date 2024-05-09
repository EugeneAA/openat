#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 1024

int extract_exec_line(const char *desktop_file_path, char *exec_line)
{
    FILE *file = fopen(desktop_file_path, "r");
    if (!file)
    {
        perror("Error opening file");
        return -1;
    }

    char line[LINE_SIZE] = {};
    const char *exec_prefix = "Exec=";
    size_t exec_prefix_len = strlen(exec_prefix);

    while (fgets(line, LINE_SIZE, file) != NULL)
    {
        if (strncmp(line, exec_prefix, exec_prefix_len) == 0)
        {
            size_t exec_line_len = strlen(line);

            strncpy(exec_line, line + exec_prefix_len, exec_line_len - exec_prefix_len);

            if (exec_line[exec_line_len - exec_prefix_len - 1] == '\n')
            {
                exec_line[exec_line_len - exec_prefix_len - 1] = '\0';
            }

            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

void replace_placeholder_with_filepath(char *exec_line, const char *filepath)
{
    const char *placeholder = "%F";
    char *found = strstr(exec_line, placeholder);

    if (found != NULL)
    {
        char buffer[1024] = {};
        strncpy(buffer, exec_line, found - exec_line);
        buffer[found - exec_line] = '\0';

        strcat(buffer, filepath);
        strcat(buffer, found + strlen(placeholder));

        strcpy(exec_line, buffer);
    }
}

void get_mime_type(const char *file_path, char *mime_type)
{
    char cmd[512];
    sprintf(cmd, "file --mime-type -b \"%s\"", file_path);
    FILE *fp = popen(cmd, "r");

    if (fp == NULL)
    {
        printf("Failed to run command\n");
        mime_type[0] = '\0';
        return;
    }

    fgets(mime_type, 128, fp);
    mime_type[strcspn(mime_type, "\n")] = 0;

    pclose(fp);
}

void get_default_application(const char *mime_type, char *app_desktop)
{
    char cmd[512] = {};
    sprintf(cmd, "grep \"^%s=\" $HOME/.config/mimeapps.list | sed -n -e \"s|^%s=||p\" | head -n 1", mime_type, mime_type);
    FILE *fp = popen(cmd, "r");

    if (fp == NULL)
    {
        printf("Failed to run command\n");
        app_desktop[0] = '\0';
        return;
    }

    fgets(app_desktop, 128, fp);
    app_desktop[strcspn(app_desktop, "\n")] = 0;

    pclose(fp);
}

void open_with_editor(const char *file_path, const char *editor)
{
    execlp(editor, editor, file_path, (char *)NULL);
    perror("Failed to exec editor");
}

void editor_exists(const char *editor, int *found)
{
    char cmd[512] = {};
    sprintf(cmd, "which %s > /dev/null 2>&1", editor);
    *found = (system(cmd) == 0);
}

int main(int argc, char *argv[])
{
    char mime_type[128] = {};
    char app_desktop[128] = {};

    if (argc < 2)
    {
        printf("Usage: %s [-t|-e [--nano|--vim|--pico|--vi]] <path-to-file>", argv[0]);
        return 1;
    }

    if (argc == 2)
    {
        char *file_path = argv[1];

        if (access(file_path, F_OK) != 0) {
            printf("Failed to open file\n");
            return 1;
        }

        get_mime_type(file_path, mime_type);

        if (strlen(mime_type) == 0)
        {
            printf("Failed to get MIME type\n");
            return 4;
        }

        get_default_application(mime_type, app_desktop);

        if (strlen(app_desktop) == 0)
        {
            printf("No default application found for MIME type: %s\n", mime_type);
            return 3;
        }

        char exec_line[LINE_SIZE] = {};
        char desktop_file_path[1024] = {};
        strcat(desktop_file_path, "/usr/share/applications/");
        strcat(desktop_file_path, app_desktop);

        if (extract_exec_line(desktop_file_path, exec_line) == 0)
        {
            replace_placeholder_with_filepath(exec_line, file_path);
            strcat(exec_line, " &");
            system(exec_line);
        }
        else
        {
            printf("Exec line not found in .desktop file.\n");
        }

        return 1;
    }

    char *mode = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-e") == 0)
        {
            mode = argv[i];
            break;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            mode = argv[i];
            break;
        }
    }
    if (mode == NULL)
    {
        printf("Usage: %s [-t|-e [--nano|--vim|--pico|--vi]] <path-to-file>", argv[0]);
        return 1;
    }

    char *file_path = argv[argc - 1];

    if (access(file_path, F_OK) != 0) {
        printf("Failed to open file\n");
        return 1;
    }

    if (strcmp(mode, "-e") == 0)
    {
        int nano_found = 0, vim_found = 0, pico_found = 0, vi_found = 0;
        char *editor_choice = NULL;

        editor_exists("nano", &nano_found);
        editor_exists("vim", &vim_found);
        editor_exists("pico", &pico_found);
        editor_exists("vi", &vi_found);

        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--nano") == 0 && nano_found)
            {
                editor_choice = "nano";
                break;
            }
            else if (strcmp(argv[i], "--vim") == 0 && vim_found)
            {
                editor_choice = "vim";
                break;
            }
            else if (strcmp(argv[i], "--pico") == 0 && pico_found)
            {
                editor_choice = "pico";
                break;
            }
            else if (strcmp(argv[i], "--vi") == 0 && vi_found)
            {
                editor_choice = "vi";
                break;
            }
        }

        printf("Editor: %s", editor_choice);

        if (editor_choice == NULL)
        {
            if (nano_found)
                editor_choice = "nano";
            else if (vim_found)
                editor_choice = "vim";
            else if (pico_found)
                editor_choice = "pico";
            else if (vi_found)
                editor_choice = "vi";
        }

        open_with_editor(file_path, editor_choice);
        return 0;
    }

    if (strcmp(mode, "-t") == 0)
    {
        strcpy(mime_type, "text/plain");
        get_default_application(mime_type, app_desktop);

        if (strlen(app_desktop) == 0)
        {
            printf("No default application found for MIME type: %s\n", mime_type);
            return 3;
        }

        char exec_line[LINE_SIZE] = {};
        char desktop_file_path[1024] = {};
        strcat(desktop_file_path, "/usr/share/applications/");
        strcat(desktop_file_path, app_desktop);

        if (extract_exec_line(desktop_file_path, exec_line) == 0)
        {
            replace_placeholder_with_filepath(exec_line, file_path);
            strcat(exec_line, " &");
            system(exec_line);
        }
        else
        {
            printf("Exec line not found in .desktop file.\n");
        }
        return 0;
    }
    return 0;
}

