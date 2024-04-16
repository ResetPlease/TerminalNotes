// coded by iveseentheworld

#include <filesystem>
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include <string.h>

#define PATH_MAX_SIZE 1024
#define DATA_MAX_SIZE 512


const char filename[] = "/home/beregom/Projects/ITODO/todoapp.data";
const char logo_filename[] = "/home/beregom/Projects/ITODO/logo.txt";
const char RESET[] = "\u001b[0m";
const char GREEN[] = "\u001b[32m";



struct Comment
{

    char clock[1024];
    char path[PATH_MAX_SIZE];
    char data[DATA_MAX_SIZE];

    Comment(const std::string& first, const std::string& second){
        strcpy(path, first.c_str());
        strcpy(data, second.c_str());
        time_t t = time(NULL);
        tm *tm = localtime(&t);
        strcpy(clock, asctime(tm));
    }

    Comment(){};

};


void print_logo(){
    FILE* file = fopen(logo_filename, "rb");
    if(file == NULL){
        printf("ERROR: Logo file cannot be read :( \n");
        return;
    }
    char c;
    printf(GREEN);
    while(fread(&c, sizeof(char), 1, file)){
        printf("%c", c);
    }
    printf("\n");
    printf(RESET);
}

void print_help(){
    print_logo();
    printf("It is TODO app for different folders\n");
    printf("INFO: DELETE IS NOT IMPLEMENTED\n");
    printf("Use:\n\t+ \"comment\" for set todo in this folder\n\t- <number> for delete todo with this number\n");
}

void print_comment(const Comment& obj){
    static size_t index_of_note = 0;
    printf("%s%s%s %lu:- %s\n\n", GREEN, obj.clock, RESET, index_of_note++, obj.data);
}



int main(int argc, char** argv){
    if( (argc == 2 && std::string(argv[1]) == std::string("-h")) 
        || (argc!= 1 && argc != 3)){
        print_help();
        return 0;
    }
    
    std::string path = std::filesystem::current_path();
    printf("Current directory: %s\n\n", path.c_str());

    if(argc == 1){
        FILE* file = fopen(filename, "rb");
        if(file == NULL){
            printf("ERROR: %s file cannot be read\n", filename);
            return 0;
        }
        Comment data;
        while(fread(&data, sizeof(Comment), 1, file)){
            if( std::string(data.path) == path ){
                print_comment(data);
            }
        }
        fclose(file);
        return 0;
    }

    std::string option = std::string(argv[1]);
    
    if(option != std::string("+") && option != std::string("-")){
        printf("ERROR: Choose the correct option\n");
        print_help();
        return 0;
    }
    
    if(path.size() >= PATH_MAX_SIZE){
        printf("ERROR: file path too long %s\n", path.c_str());
        return 0;
    }
    
    if(option == "+"){
        std::string data = std::string(argv[2]);
        if(data.size() >= DATA_MAX_SIZE){
            printf("ERROR: comment is too big\n");
            return 0;
        }
        Comment obj(path.c_str(), data.c_str());
        FILE* file = fopen(filename, "ab"); // дописываем информацию к файлу
        if(file == NULL){
            printf("ERROR: can't write in this file\n.");
            printf("INFO: Crating a new file with \"wb\" mode %s\n", filename);
            file = fopen(filename, "wb");
        }
        fwrite(&obj, sizeof(Comment), 1, file);
        fclose(file);
        printf("Comment added successfully\n");
    }else if(option == "-"){
        long long int number;
        try{
            number = std::stoll(std::string(argv[2]));
        }catch(...){
            printf("ERROR: Please enter the comment line number\n");
            return 0;
        }
        if(number < 0){
            printf("ERROR: Line number can't be negative\n");
            return 0;    
        }
        FILE* file = fopen(filename, "rb");
        if(file == NULL){
            printf("ERROR: %s file cannot be read\n", filename);
            return 0;
        }
        std::vector<Comment> data;
        Comment obj;
        while(fread(&obj, sizeof(Comment), 1, file)){
            data.push_back(obj);
        }

        if(number >= (long long)data.size()){
            printf("ERROR: Wrong number of line (too big)\n");
            return 0;
        }

        size_t number_of_notes = 0;
        bool been_deleted = false;
        std::vector<Comment> without_number;
        for(size_t i = 0; i < data.size(); ++i){
            if(std::string(data[i].path) == path){
                if(number_of_notes == (size_t)number){
                    number_of_notes++;
                    been_deleted = true;
                    continue;
                }
                number_of_notes++;
            }
            without_number.push_back(data[i]);
        }
        fclose(file);

        if(!been_deleted){
            printf("ERROR: Wrong number. There are no comments with this number\n");
            return 0;
        }

        file = fopen(filename, "wb");
        if(file == NULL){
            printf("ERROR: %s file cannot be open\n", filename);
            return 0;
        }
        size_t written = 0;
        for(size_t i = 0; i < without_number.size(); ++i){
            written = fwrite(&without_number[i], sizeof(Comment), 1, file);
            if(written == 0){
                printf("ERROR: rewrite failed\n");
                fclose(file);
                return 0;
            }
        }
        fclose(file);
        printf("Comment successfully deleted\n");
    }
    return 0;
}