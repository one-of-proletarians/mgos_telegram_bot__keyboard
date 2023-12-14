#include <fstream>
#include <iostream>

using namespace std;

typedef enum {
    ROW,
    BUTTON,
    WEB_APP
} NodeType;

class Node {
public:
    Node(NodeType type, string text)
    {
        this->text = text;
        this->type = type;
        this->url = "";
    }

    Node(NodeType type, string text, string url)
    {
        this->text = text;
        this->type = type;
        this->url = url;
    }

    Node(NodeType type)
    {
        this->text = "";
        this->url = "";
        this->type = type;
    }

    NodeType type;
    string text;
    string url;
};

class Keyboard {

private:
    vector<Node> stack;
    bool isResized = false;
    string json = "{\"keyboard\": [";

public:
    Keyboard text(string text)
    {
        this->stack.push_back(Node(BUTTON, text));
        return *this;
    }

    Keyboard row(void)
    {
        if (this->stack.back().type != ROW)
            this->stack.push_back(Node(ROW));
        return *this;
    }

    Keyboard webApp(string text, string url)
    {
        this->stack.push_back(Node(WEB_APP, text, url));
        return *this;
    }

    Keyboard resized(void)
    {
        this->isResized = true;
        return *this;
    }

    char* build()
    {
        if (this->stack.back().type != ROW) {
            this->row();
        }

        string row = "[";

        for (Node n : this->stack) {
            switch (n.type) {
            case BUTTON:
                row += "{\"text\":\"" + n.text + "\"},";
                break;
            case WEB_APP:
                row += "{\"text\":\"" + n.text + "\",\"web_app\":{\"url\":\"" + n.url + "\"}" + "},";
                break;
            default:
                row.pop_back();
                this->json += row + "],";
                row = "[";
            }
        }

        this->json.pop_back();
        if (this->isResized) {
            this->json += "],\"resize_keyboard\": true}";
        } else {
            this->json += "]}";
        }

        char* jsonPointer = NULL;
        jsonPointer = (char*)calloc(sizeof(char), this->json.size() + 1);

        if (jsonPointer != NULL) {
            strcpy(jsonPointer, this->json.c_str());
        }

        this->stack.clear();
        this->json.clear();
        this->isResized = false;

        return jsonPointer;
    }
};

int main()
{

    Keyboard keyboard;
    char* k = keyboard
                  .text("Menu")
                  .row()
                  .text("Reset")
                  .webApp("Open web app", "https://www.google.com")
                  .text("Menu2")
                  .row()
                  .row()
                  .text("Plus")
                  .resized()
                  .build();

    fstream json;
    json.open("k.json", ios::out);
    if (json.is_open()) {
        json << k;
        free(k);
        json.close();
    }

    return 0;
}