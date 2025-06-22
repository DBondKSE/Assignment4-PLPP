#include <iostream>
#include <dlfcn.h>
#include <functional>
#include <queue>
#include <sstream>
#include <fstream>
#include <string>

class CaesarCypher {
public:
    std::function<char*(char*, int)> encrypt;
    std::function<char*(char*, int)> decrypt;
    CaesarCypher() {
        void* handle = dlopen("/Users/danielbond/libencryptor.so", RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Error: %s\n", dlerror());
            return;
        }
        this->encrypt = reinterpret_cast<char* (*)(char *, int)>(dlsym(handle, "encrypt"));
        this->decrypt = reinterpret_cast<char* (*)(char *, int)>(dlsym(handle, "decrypt"));
        if (!encrypt || !decrypt) {
            fprintf(stderr, "Error: %s\n", dlerror());
            return;
        }
    }
};

class Line {
public:
    virtual void print(){}
    virtual ~Line(){}
    virtual std::string serialize(){}
};

class TextLine: public Line {
public:
    std::string text;
    explicit TextLine(const std::string& str) : text(str) {}

    void print() override {
        std::cout << "Text: " << text << std::endl;
    }

    std::string serialize() override {
        std::ostringstream serialized;
        serialized << "T,"<< text << std::endl;
        return serialized.str();
    }
};

class ChecklistLine : public Line {
    std::string item;
    bool checked;
public:
    ChecklistLine(std::string& i, bool c) : item(i), checked(c) {}
    void print() override {
        std::cout << "[ " << (checked ? "x" : " ") << " ] " << item << std::endl;
    }

    std::string serialize() override {
        std::ostringstream serialized;
        serialized << "X," << (checked&1) << "," << item << std::endl;
        return serialized.str();
    }
};

class ContactLine : public Line {
    std::string name;
    std::string surname;
    std::string email;
public:
    ContactLine(std::string& _name,std::string& _surname,std::string& _email) : name(_name), surname(_surname), email(_email) {}
    void print() override {
        std::cout << "Contact - " << name << " " << surname << ", E-mail: " << email << std::endl;
    }

    std::string serialize() override {
        std::ostringstream serialized;
        serialized << "C," << name << "," << surname << "," << email << std::endl;
        return serialized.str();
    }
};

class TextEditor {
public:
    std::vector<Line*> lines;
    int row = 0;
    int symbol = 0;

    void newLine();
    void save();
    void load();
    void print();
    void insert(int r, int s);
    void search();
    void _delete(int r, int s, int c);
    void undo();
    void redo();
    void cut(int r, int s, int c);
    void paste(int r, int s);
    void copy(int r, int s, int c);
    void insert_r(int r, int s);
    void encrypt_all();
    void decrypt_all();
    static std::string readInput();
private:
    int memory_allocated = 1;
    std::string clipboard;
    std::vector<std::vector<Line*>> undoStack;
    std::vector<std::vector<Line*>> redoStack;
    void saveAction();
    std::vector<Line*> copyState();

};

int main() {
    system("clear");
    TextEditor text = TextEditor();
    while (true) {
        printf("> Choose the command ([h] - help):\n");
        std::string command = TextEditor::readInput();
        system("clear");
        int number = -1;
        try {
            number = std::stoi(command);
        } catch (...) {
            number = -1;
        }
        switch (number) {
            case 0: {
                printf("-----PROGRAM END-----");
                return 0;
            }
            case 1: {
                printf("Enter text to append: ");
                text.insert(text.row, text.symbol);
                break;
            }
            case 2: {
                text.newLine();
                break;
            }
            case 3: {
                printf("Enter the file name for saving: ");
                text.save();
                break;
            }
            case 4: {
                printf("Enter the file name for loading: ");
                text.load();
                break;
            }
            case 5: {
                text.print();
                break;
            }
            case 6: {
                printf("Choose row and index to insert [%%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                if (scanf(" %d %d", &r, &s) == 2) {
                    printf("Enter text to insert: ");
                    text.insert(r, s);
                } else printf("Invalid input, try again\n");
                break;
            }
            case 7: {
                printf("Enter text to search: ");
                text.search();
                break;
            }
            case 8: {
                printf("Choose row, index, and number of symbols to delete [%%d %%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                int c = 0;
                if (scanf(" %d", &r) && scanf(" %d", &s) && scanf(" %d", &c)) {
                    text._delete(r, s, c);
                } else printf("Invalid input, try again\n");
                break;
            }
            case 9: {
                text.undo();
                break;
            }
            case 10: {
                text.redo();
                break;
            }
            case 11: {
                printf("Choose row, index, and number of symbols to cut [%%d %%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                int c = 0;
                if (scanf(" %d", &r) && scanf(" %d", &s) && scanf(" %d", &c)) {
                    text.cut(r, s, c);
                } else printf("Invalid input, try again\n");
                break;
            }
            case 12: {
                printf("Choose row and index to paste [%%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                if (scanf(" %d %d", &r, &s) == 2) {
                    text.paste(r, s);
                } else printf("Invalid input, try again\n");
                break;
            }
            case 13: {
                printf("Choose row, index, and number of symbols to copy [%%d %%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                int c = 0;
                if (scanf(" %d", &r) && scanf(" %d", &s) && scanf(" %d", &c)) {
                    text.copy(r, s, c);
                } else printf("Invalid input, try again\n");
                break;
            }
            case 14: {
                printf("Choose row and index to insert with replacement [%%d %%d]: ");
                int r = text.row;
                int s = text.symbol;
                if (scanf(" %d %d", &r, &s) == 2) {
                    printf("Enter text to insert: ");
                    text.insert_r(r, s);
                } else printf("Invalid input, try again\n");
                break;
            }
            default: {
                printf("Valid command numbers:\n");
                printf("[0] - exit\n");
                printf("[1] - append\n");
                printf("[2] - new line\n");
                printf("[3] - save\n");
                printf("[4] - load\n");
                printf("[5] - print\n");
                printf("[6] - insert\n");
                printf("[7] - search\n");
                // New commands
                printf("[8] - delete\n");
                printf("[9] - undo\n");
                printf("[10] - redo\n");
                printf("[11] - cut\n");
                printf("[12] - paste\n");
                printf("[13] - copy\n");
                printf("[14] - insert replacement\n");
                break;
            }
        }
    }
}

void TextEditor::newLine() {
    saveAction();
    row++;
    symbol = 0;
    std::cout << "Select line type:" << std::endl;
    std::cout << "\t[1] -> Text" << std::endl;
    std::cout << "\t[2] -> Checklist" << std::endl;
    std::cout << "\t[3] -> Contact" << std::endl;
    std::string command = readInput();
    int number = -1;
    try {
        number = std::stoi(command);
    } catch (...) {
        number = -1;
    }
    if (number == -1) {
        std::cout << "Wrong line type, try again" << std::endl;
        return;
    }
    switch (number) {
        case 1: {
            std::cout << "Enter text line:" << std::endl;
            const std::string str = readInput();
            lines.push_back(new TextLine(str));
            break;
        }
        case 2: {
            std::cout << "Enter checklist item:" << std::endl;
            std::string item = readInput();
            std::cout << "Enter checklist status [true/false]:" << std::endl;
            const std::string inp = readInput();
            int stat = inp == "true" ? 1 : inp == "false" ? 0 : -1;
            if (stat != -1) {
                lines.push_back(new ChecklistLine(item, stat == 1));
                break;
            }
            std::cout << "Wrong input, try again" << std::endl;
            break;
        }
        case 3: {
            std::cout << "Enter contact name:" << std::endl;
            std::string name = readInput();
            std::cout << "Enter contact surname:" << std::endl;
            std::string surname = readInput();
            std::cout << "Enter contact email:" << std::endl;
            std::string email = readInput();
            lines.push_back(new ContactLine(name, surname, email));
            break;
        }
        default:
            std::cout << "Wrong action, try again" << std::endl;
            return;
    }
    std::cout << "Line added" << std::endl;
}

void TextEditor::save() {
    std::queue<std::string> whole_text;
    for (int i = 0; i < row; i++) {
        whole_text.push(lines[i]->serialize());
    }

    FILE *file = fopen(readInput().c_str(), "w");
    if (file != nullptr)
    {
        std::cout << "Enter encryption key: ";
        std::string command = readInput();
        system("clear");
        int number = -1;
        try {
            number = std::stoi(command);
        } catch (...) {
            number = -1;
        }
        if (number == -1) {
            std::cout << "Wrong encryption key, try again" << std::endl;
            return;
        }
        CaesarCypher cc = CaesarCypher();
        for (int i = 0; i < row; i++) {
            fputs(cc.encrypt(const_cast<char*>(whole_text.front().c_str()), number), file);
            whole_text.pop();
        }
        fclose(file);
        printf("Text has been saved successfully\n");
    }
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void TextEditor::load() {
    std::ifstream file(readInput().c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }
    std::cout << "Enter decryption key: ";
    std::string command = readInput();
    system("clear");
    int number = -1;
    try {
        number = std::stoi(command);
    } catch (...) {
        number = -1;
    }
    if (number == -1) {
        std::cout << "Wrong decryption key, try again" << std::endl;
        return;
    }
    CaesarCypher cc = CaesarCypher();
    lines.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::string decrypted(cc.decrypt(const_cast<char*>(line.c_str()), number));
        auto sp = split(decrypted, ',');
        if (sp[0] == "T")
            lines.push_back(new TextLine(sp[1]));
        else if (sp[0] == "X")
            lines.push_back(new ChecklistLine(sp[2], sp[1]=="1"));
        else if (sp[0] == "C")
            lines.push_back(new ContactLine(sp[1],sp[2],sp[3]));
        else
            lines.push_back(new TextLine(decrypted));
    }

    file.close();
    // int row_counter = 0;
    // int temp; while ((temp = getchar()) != '\n' && temp != EOF) {}
    // FILE *file = fopen(readInput(), "r");
    // if (file == nullptr)
    // {
    //     printf("Error opening file");
    //     return;
    // }
    //
    // free(text);
    // text = nullptr;
    // int alloc = sizeof(char*);
    // text = (char**)malloc(alloc);
    //
    // int size = 10;
    // int count = 0;
    // char* buf = (char*)malloc(size);
    // char ch;
    // while ((ch = (char)fgetc(file)) != EOF) {
    //     if (count >= size)
    //         buf = (char*)realloc(buf, size*=2);
    //     if (ch == '\n') {
    //         *(buf + count) = '\0';
    //         row = row_counter;
    //         symbol = count;
    //         *(text + row_counter++) = buf;
    //         buf = (char*)malloc(size = 10);
    //         count = 0;
    //         text = (char**)realloc(text, alloc+=sizeof(char*));
    //     } else {
    //         *(buf + count++) = ch;
    //     }
    // }
    // fclose(file);
    // printf("Text has been loaded successfully\n");
    // undoStack.clear();
    // redoStack.clear();
}

void TextEditor::insert(int r, int s) {
    if (typeid(*lines[r]) == typeid(TextLine)) {
        saveAction();
        auto cur = dynamic_cast<TextLine*>(lines[r]);
        cur->text.insert(s, clipboard);
        lines[r] = cur;
        printf("\n");
    }
}

void TextEditor::print() {
    for (int i = 0; i < lines.size(); i++)
        lines[i]->print();
}

void TextEditor::search() {
    // int temp; while ((temp = getchar()) != '\n' && temp != EOF) {}
    // char* prompt = readInput();
    // if (strlen(prompt) == 0)
    //     return;
    // int length = 0; while (*(prompt + length) != '\0') length++;
    // for (int i = 0; i <= row; i++) {
    //     char* whole = *(text + i);
    //     char* found;
    //
    //     while ((found = strstr(whole, prompt)) != nullptr) {
    //         while (whole < found) putchar(*whole++);
    //         printf("[>%.*s<]", length, found);
    //         whole = found + length;
    //     }
    //     printf("%s\n", whole);
    // }
 }

std::string TextEditor::readInput() {
    std::string buffer;
    std::getline(std::cin, buffer);
    return buffer;
}

void TextEditor::_delete(int r, int s, int c) {
    saveAction();
    if (typeid(*lines[r]) == typeid(TextLine)) {
        auto* cur = dynamic_cast<TextLine *>(lines[r]);
        int i = 0;
        while (cur->text[s + i] != '\0') {
            cur->text[s + i] = cur->text[s + i + c];
            i++;
        }
        lines[r] = cur;
    } else {
        lines.erase(lines.begin() + r);
    }
}

void TextEditor::undo() {
    if (undoStack.empty())
        return;
    redoStack.push_back(copyState());
    lines = undoStack.back();
    undoStack.pop_back();
}

void TextEditor::redo() {
    if (redoStack.empty())
        return;
    undoStack.push_back(copyState());
    lines = redoStack.back();
    redoStack.pop_back();
}

void TextEditor::saveAction() {
    undoStack.push_back(copyState());
    redoStack.clear();

    if (undoStack.size() > 10) {
        undoStack.erase(undoStack.begin());
    }
}

std::vector<Line*> TextEditor::copyState() {
    return lines;
}

void TextEditor::cut(int r, int s, int c) {
    if (typeid(*lines[r]) == typeid(TextLine)) {
        clipboard = "";
        auto cur = dynamic_cast<TextLine *>(lines[r]);
        for (int i = 0; i < c; i++) {
            if (cur->text[s + i] == '\0')
                break;
            clipboard.append(reinterpret_cast<const std::string::value_type *>(cur->text[s + i]));
        }
        _delete(r,s,c);
    }
}

void TextEditor::paste(int r, int s) {
    if (typeid(*lines[r]) == typeid(TextLine)) {
        saveAction();
        if (clipboard.empty())
            return;
        auto cur = dynamic_cast<TextLine *>(lines[r]);
        cur->text.insert(s, clipboard);
        lines[r] = cur;
        printf("\n");
    }
}

void TextEditor::copy(int r, int s, int c) {
    if (typeid(*lines[r]) == typeid(TextLine)) {
        clipboard = "";
        auto cur = dynamic_cast<TextLine *>(lines[r]);
        for (int i = 0; i < c; i++) {
            if (cur->text[s + i] == '\0')
                break;
            clipboard.append(reinterpret_cast<const std::string::value_type *>(cur->text[s + i]));
        }
    }
}

void TextEditor::insert_r(int r, int s) {
    if (typeid(*lines[r]) == typeid(TextLine)) {
        saveAction();
        auto cur = dynamic_cast<TextLine *>(lines[r]);
        cur->text.replace(s, clipboard.length(), clipboard);
        lines[r] = cur;
        printf("\n");
    }
}

void TextEditor::encrypt_all() {

}

void TextEditor::decrypt_all() {

}

