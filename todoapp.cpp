// coded by iveseentheworld

#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>


#define DATA_MAX_SIZE 512


const char filename[] = ".todoapp.data";
const char RESET[] = "\u001b[0m";
const char GREEN[] = "\u001b[32m";


struct Comment
{

    char clock[1024];
    char data[DATA_MAX_SIZE];

    Comment(const char* second){
        strcpy(data, second);
        time_t t = time(NULL);
        tm *tm = localtime(&t);
        strcpy(clock, asctime(tm));
    }

    Comment(){};

};


void print_help(){
    printf("It is Note app for different folders\n");
    printf("Use:\n\t+ \"comment\" for set note in this folder\n\t- <number> for delete note \
with this number. Numbering starts from 0\n\tclear - clearing all notes in a given folder\n");
    printf("\n(c) iveseentheworld\n");
}


void print_comment(const Comment& obj){
    static size_t index_of_note = 0;
    printf("%s%s%s %lu:- %s\n\n", GREEN, obj.clock, RESET, index_of_note++, obj.data);
}

void data_remove(){
    if(remove(filename) == 0){
        printf("All data was successfully deleted\n");
    }else{
        printf("INFO: An error occurred during deletion. Perhaps the note was not initialized here\n");
    }
}


int main(int argc, char** argv){
    if(argc!= 1 && argc != 2 && argc != 3){
        print_help();
        return 0;
    }
    
    if(argc == 1){
        FILE* file = fopen(filename, "rb");
        if(file == NULL){
            printf("No records for this directory\n");
            printf("Use -h for more information\n");
            return 0;
        }
        Comment data;
        bool reading = false;
        while(fread(&data, sizeof(Comment), 1, file)){
            print_comment(data);
            reading = true;
        }
        fclose(file);
        if(!reading){
            printf("Empty\n");
            return 0;
        }
        return 0;
    }

    std::string option = std::string(argv[1]);

    if(option != "+" && option != "-" && option != "clear" && option != "-h"){
        printf("ERROR: Choose the correct option\n");
        print_help();
        return 0;
    }

    if( argc == 2 && option == "-h"){
        print_help();
        return 0;
    }

    if(argc == 2 && option == "clear"){
        data_remove();
        return 0;
    }
    
    if(option == "+"){
        std::string data = std::string(argv[2]);
        if(data.size() >= DATA_MAX_SIZE){
            printf("ERROR: Note is too big\n");
            return 0;
        }
        Comment obj(data.c_str());
        FILE* file = fopen(filename, "ab");
        if(file == NULL){
            printf("INFO: Creating a new file with \"wb\" mode %s\n", filename);
            file = fopen(filename, "wb");
        }
        fwrite(&obj, sizeof(Comment), 1, file);
        fclose(file);
        printf("Note added successfully\n");
    }else if(option == "-"){
        long long int number;
        try{
            number = std::stoll(std::string(argv[2]));
        }catch(...){
            printf("ERROR: Please enter the note line number\n");
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
        fclose(file);

        if(number >= (long long)data.size()){
            printf("ERROR: Wrong number of line (too big)\n");
            return 0;
        }    

        file = fopen(filename, "wb");
        if(file == NULL){
            printf("ERROR: %s file cannot be open\n", filename);
            return 0;
        }
        size_t written = 0;
        for(size_t i = 0; i < data.size(); ++i){
            if(i != number){
                written = fwrite(&data[i], sizeof(Comment), 1, file);
                if(written == 0){
                    printf("ERROR: rewrite failed\n");
                    fclose(file);
                    return 0;
                }
            }
        }
        fclose(file);

        printf("Note successfully deleted\n");
    }
    return 0;
}